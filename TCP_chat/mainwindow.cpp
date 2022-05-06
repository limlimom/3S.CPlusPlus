#include <QNetworkInterface>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"

// Конструктор
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Скрываем все элементы
    ui->labelPort->hide();
    ui->labelAddress->hide();
    ui->lineEditPort->hide();
    ui->lineEditAddress->hide();
    ui->pushButtonConnect->hide();
    ui->pushButtonCreate->hide();
    ui->pushButtonAddresses->hide();


    ui->lineEditMessage->setEnabled(false);
    ui->pushButtonSend->setEnabled(false);
    ui->checkBoxSpam->setEnabled(false);

    // Соединение сигнала нажатия на Enter с слотом отправки сообщения
    connect(ui->lineEditMessage, &QLineEdit::returnPressed, this, &MainWindow::on_pushButtonSend_clicked);

    // Создание, запуск и подключение таймера для спама
    spamTimer = new QTimer(this);
    spamTimer->setInterval(1000);
    connect(spamTimer, &QTimer::timeout, this, &MainWindow::onSpamTimeout);
    spamTimer->start();
}

// Деструктор
MainWindow::~MainWindow()
{
    // Очищаем сокеты при отключении
    clearServer();
    clearSockets();

    delete ui;
}

// Выбор режима сервера
void MainWindow::on_pushButtonServer_clicked()
{
    // Переключаемся в режим сервера
    ui->labelPort->show();
    ui->labelAddress->hide();
    ui->lineEditPort->show();
    ui->lineEditAddress->hide();
    ui->pushButtonConnect->hide();
    ui->pushButtonCreate->show();
    ui->pushButtonAddresses->show();
}

// Выбор режима клиента
void MainWindow::on_pushButtonClient_clicked()
{
    // Переключаемся в режим клиента
    ui->labelPort->show();
    ui->labelAddress->show();
    ui->lineEditPort->show();
    ui->lineEditAddress->show();
    ui->pushButtonConnect->show();
    ui->pushButtonCreate->hide();
    ui->pushButtonAddresses->hide();
}

// Открытие сервера
void MainWindow::on_pushButtonCreate_clicked()
{
    if (server == nullptr)
    {
        // Берём порт для подключения с формы
        int port = ui->lineEditPort->text().toInt();

        if (port > 0 && port < 0x10000)
        {
            // Порт правильный

            // Закрываем все существующие соединения (если есть) и удаляем оставшиеся классы
            clearServer();
            clearSockets();

            // Создаём сервер
            server = new QTcpServer(this);
            connect(server, &QTcpServer::newConnection, this, &MainWindow::onServerConnection);
            // Открываем сервер
            if (server->listen(QHostAddress::Any, quint16(port)))
            {
                // Если удалось открыть
                ui->textEditInput->append("Сервер открыт на порте " + QString::number(port));


                ui->pushButtonCreate->setText("Закрыть");
            }
            else
            {
                // Если не удалось открыть
                ui->textEditInput->append("Не удалось запустить сервер");
                // Очищаем уже не нужный сервер
                server->deleteLater();
                server = nullptr;
            }
        }
        else
        {
            // Порт неправильный
            ui->textEditInput->append("ВВЕДЁН НЕПРАВИЛЬНЫЙ ПОРТ!");
        }
    }
    else
    {
        // Закрываем все существующие соединения (если есть) и удаляем оставшиеся классы
        clearServer();
        clearSockets();
    }
}

// Обработка нового подключения
void MainWindow::onServerConnection()
{
    // Получаем новое соединение с клиентом
    QTcpSocket *newSocket = server->nextPendingConnection();
    if (newSocket != nullptr)
    {
        // Соединяем слоты, добавляем сокет в вектор
        connect(newSocket, &QTcpSocket::disconnected, this, &MainWindow::onSocketDisconneted);
        connect(newSocket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
        sockets.append(newSocket);

        // Приводим адрес к строке
        QString addressStr = newSocket->localAddress().toString().split(':').last();
        ui->textEditInput->append( nick + " подключился");

        ui->lineEditMessage->setEnabled(true);
        ui->pushButtonSend->setEnabled(true);
        ui->checkBoxSpam->setEnabled(true);

    }

}


// Обработка отключения от сокета
void MainWindow::onSocketDisconneted()
{
    if (server == nullptr)
    {
        if (!socketClearing)
        {
            // Сейчас режим клиента
            ui->textEditInput->append("Вы отключились от сервера");
            ui->textEditInput->clear();
            ui->textEditOutput->clear();

            // Удаляем единственный сокет подключения
            clearSockets();
        }

        ui->lineEditMessage->setEnabled(false);
        ui->pushButtonSend->setEnabled(false);
        ui->checkBoxSpam->setEnabled(false);
    }
    else
    {
        // Сейчас режим сервера
        QTcpSocket *senderSocket = static_cast<QTcpSocket *>(sender());
        // Приводим адрес к строке
        QString addressStr = senderSocket->localAddress().toString().split(':').last();
        ui->textEditInput->append( nick + addressStr + " отключился");

        // Удаляем сокет, от которого отключились
        sockets.removeOne(senderSocket);

        if (sockets.length() == 0)
        {
            // Если уже все клиенты отключились
            ui->lineEditMessage->setEnabled(false);
            ui->pushButtonSend->setEnabled(false);
            ui->checkBoxSpam->setEnabled(false);
        }
    }
}

// Обработка чтения данных из сокета
void MainWindow::onSocketReadyRead()
{
    // Получаем указатель на сокет, который вызвал сигнал
    QTcpSocket *senderSocket = static_cast<QTcpSocket *>(sender());
    if (senderSocket != nullptr)
    {
        // Считываем данные из сокета
        QByteArray data = senderSocket->readAll();
        if (data.length() > 0)
        {
            // Превращаем байты в строку
            QString text = QString::fromLocal8Bit(data);
            ui->textEditInput->append(text);
        }
    }
}

// Подключение к серверу
void MainWindow::on_pushButtonConnect_clicked()
{
    if (sockets.length() == 0)
    {
        // Берём порт для подключения с формы
        int port = ui->lineEditPort->text().toInt();

        if (port > 0 && port < 0x10000)
        {
            // Порт правильный

            // Закрываем все существующие соединения (если есть) и удаляем оставшиеся классы
            clearServer();
            clearSockets();

            // Создаём сокет
            QTcpSocket *newSocket = new QTcpSocket(this);
            // Пробуем подключиться
            newSocket->connectToHost(ui->lineEditAddress->text(), quint16(port));
            if (newSocket->waitForConnected(5000))
            {
                // Если удалось подключиться
                ui->textEditInput->append("Подключение установлено");
                // Соединяем сигналы
                connect(newSocket, &QTcpSocket::disconnected, this, &MainWindow::onSocketDisconneted);
                connect(newSocket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
                sockets.append(newSocket);

                ui->lineEditMessage->setEnabled(true);
                ui->pushButtonSend->setEnabled(true);
                ui->checkBoxSpam->setEnabled(true);
                ui->textEditOutput->clear();
                ui->textEditInput->clear();

                ui->pushButtonConnect->setText("Отключиться");
            }
            else
            {
                // Если не удалось подключиться
                ui->textEditInput->append("Ошибка подключения");
                // Удаляем сокет (так как он больше не нужен)
                newSocket->deleteLater();
            }
        }
        else
        {
            ui->textEditInput->append("ВВЕДЁН НЕПРАВИЛЬНЫЙ ПОРТ!");
        }
    }
    else
    {
        // Закрываем все существующие соединения (если есть) и удаляем оставшиеся классы
        clearServer();
        clearSockets();
    }
}

void MainWindow::read_data(void) //пришли данные
{
    QString stroka = QString::fromLocal8Bit(TCPSocket->readAll());
    if(stroka == "#nickerror_0")
    {
        ui->textEditInput->append("Ошибка: Этот ник уже занят!");
    }
    else
    {
        ui->textEditInput->append(stroka);
    }
}

// Отправка сообщения
void MainWindow::on_pushButtonSend_clicked()
{

    if(ui->pushButtonServer->isChecked())
    {
        for(int i = 0;i<users.size();i++)
        {
            users[i]->sendMessage("Сервер:" + ui->lineEditMessage->text());
            ui->textEditInput->append("Сервер:" + ui->lineEditMessage->text());
        }
    }
    else
    {
        sendToServer(getNick() + ":" + ui->lineEditMessage->text());
    }

    ui->textEditOutput->append(ui->lineEditMessage->text());

    ui->lineEditMessage->clear();

    }


// Закрытие и удаление сервера
void MainWindow::clearServer()
{
    if (server != nullptr)
    {
        // Если сервер слушает порт, то закрываем его
        if (server->isListening())
            server->close();
        // и отправляем класс сервера на удаление
        server->deleteLater();
        server = nullptr;
    }

    ui->pushButtonCreate->setText("Создать");
}

// Закрытие и удаление сокетоа
void MainWindow::clearSockets()
{
    socketClearing = true; // Включаем флаг
    for (int i = 0; i < sockets.length(); i++)
    {
        // Для каждого сокета в векторе:
        // Если сокет открыт, то закрываем соединение
        if (sockets[i]->isOpen())
        {
            sockets[i]->close();
            sockets[i]->disconnect();
        }
        // и отправляет класс на удаление
        sockets[i]->deleteLater();
    }
    socketClearing = false; // Отключаем флаг
    // Удаляем оставшиеся указатели
    sockets.clear();

    ui->pushButtonConnect->setText("Подключиться");
}

// Вывод адресов компьютера
void MainWindow::on_pushButtonAddresses_clicked()
{
    // Получаем все адреса, которые принадлежат компьютеру
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();

    ui->textEditInput->append("Адреса компьютера:");
    for (int i = 0; i < addresses.length(); i++)
    {
        // Выводим только IPv4 адреса (в "более красивой" форме, только сам адрес)
        if (addresses[i].protocol() == QAbstractSocket::IPv4Protocol)
            ui->textEditInput->append(addresses[i].toString().split(':').last());
    }
}

// Отправка спама в чат
void MainWindow::onSpamTimeout()
{
    // Проверяем, что чекбокс активен и нажат
    if (ui->checkBoxSpam->isEnabled() && ui->checkBoxSpam->isChecked())
    {
        // Текст спама
        QString text = "SPAM!";
        // Приводим текст в байты
        QByteArray data = text.toUtf8();
        // Если есть активные подключения
        for (int i = 0; i < sockets.length(); i++)
        {
            // то отправляем данные
            sockets[i]->write(data);
        }

        ui->textEditOutput->append(text);
    }
}

void MainWindow::new_connection() //к нам подключились
{
    QTcpSocket *tcpSocket = server->nextPendingConnection(); //соединяем два сокета

    Client *newClient = new Client(this);
    newClient->setTcpSocket(tcpSocket);

    connect(newClient, &Client::incomingMessage, this, &MainWindow::incomingMessage);
    connect(newClient, &Client::clientDisconnected, this, &MainWindow::clientDisconnected);

    newClient->setClientID(users.size());

    users.append(newClient);

    newClient->setClientsNames(&users);

}

void MainWindow::incomingMessage(QString msg) //рассылка получаемого сообщения от клиента всем клиентам и серверу
{
    ui->textEditInput->append(msg);
    for(int i = 0;i<users.size();i++)
    {
        users[i]->sendMessage(msg);
    }
}

void MainWindow::clientDisconnected(Client *disconnnectedClient) //метод отсоединения определенного клиента
{
    if(users.empty())
    {
        return;
    }

    if(!disconnnectedClient->getIngnore())
    {
        incomingMessage(nick + disconnnectedClient->getNick() + " отключился");
    }

    users.removeAt(disconnnectedClient->getClientID());

    for(int i = 0;i<users.size();i++)
    {
        users[i]->setClientID(i);
    }
}

void MainWindow::on_pushButton_saveNick_clicked()
{
    int sizeNick = ui->lineEdit_nick->text().length();

        if( (sizeNick < 3)||(sizeNick > 10) )
        {
            ui->textEditInput->append("Ошибка: Длина имени должна быть не меньше 3 и не больше 10 символов!");
        }
        else
        {
            ui->pushButton_saveNick->setEnabled(false);
        }

        if(ui->lineEdit_nick->text().isEmpty())
        {
            ui->textEditInput->append("Ошибка: Вы не ввели ник!");
        }
        else
        {
            ui->pushButton_saveNick->setEnabled(false);
        }


        ui->lineEdit_nick->setReadOnly(true);
        savedNick = true;
}

void MainWindow::setNick(QString nick) //сеттер ника
{
    this->nick = nick;
}

QString MainWindow::getNick() //геттер ника
{
    return this->nick;
}

void MainWindow::sendToServer(QString msg) //отправка на сервер
{
    if(TCPSocket == nullptr)
    {
        return;
    }

    char *date;
    int size;
    size = msg.length();

    QByteArray ba = msg.toLocal8Bit();
    date = ba.data();

    TCPSocket->write(date,size);
}

void MainWindow::closeEvent(QCloseEvent *event) //событие отсоединения при закрытии окна
{
    if(!users.empty())
    {
        for(int i = users.size() - 1; i>=0; i--)
        {
            users[i]->closeConnection();
        }
        users.clear();
    }

    if(spamTimer->isActive())
    {
        spamTimer->stop();
    }

    if(server != nullptr)
    {
        server->close();
        server = nullptr;
    }
    if(TCPSocket!= nullptr)
    {
        TCPSocket->close();
        TCPSocket = nullptr;
    }

    event->accept();
}
