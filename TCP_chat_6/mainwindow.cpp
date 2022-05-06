#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : //конструктор
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*ui->textEdit_output->setVisible(false);*/ //скрываем поле IP-адреса до нажатия на кнопку

    ui->pushButton_SendMes->setShortcut(Qt::Key_Return); //отправка сообщения кнопкой Enter

    //устанавливаем textEdit'ы только на чтение
    ui->textEdit_input->setReadOnly(true);
    ui->textEdit_output->setReadOnly(true);

    clearElements();

    ui->pushButton_StopSpam->setEnabled(false);

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()), this, SLOT(on_timer())); //соединение события с таймером
    timer->setInterval(1000);
}

MainWindow::~MainWindow() //деструктор
{
    delete ui;
}

void MainWindow::on_radioButton_Client_clicked() //режим клиента
{
    clearElements();

    ui->label_Adress->show();
    ui->lineEdit_Adress->show();
    ui->label_Port->show();
    ui->lineEdit_Port->show();
    ui->pushButton_Show_Adress->show();

    ui->pushButton_Connect->show();

    ui->label->show();
    ui->lineEdit_nick->show();
    ui->pushButton_saveNick->show();
}

void MainWindow::on_pushButton_Connect_clicked() //подключаемся
{
    if(!savedNick) //если имя пользователя не сохранено
    {
        QMessageBox::warning(this, "Ошибка", "Имя пользователя не сохранено!");
        return;
    }

    if(connectFl) //проверка соединения клиента
    {
        QMessageBox::warning(this, "Ошибка", "Клиент уже подключен!");
        return;
    }

    int port;

    bool flag = false;

    port=ui->lineEdit_Port->text().toInt(&flag);

    if(ui->lineEdit_Port->text().isEmpty()) //проверка поля порта
    {
        QMessageBox::warning(this, "Ошибка", "Поле порта пустое!");
        return;
    }

    if( (!flag)||(port < 0 || port > 65535) ) //проверка значения порта
    {
        QMessageBox::warning(this, "Ошибка", "Введено некорректное значение порта!");
        return;
    }

    if(ui->lineEdit_Adress->text().isEmpty()) //проверка поля адреса
    {
        QMessageBox::warning(this, "Ошибка", "Поле адреса пустое!");
        return;
    }

    //проверка корректности IP-адреса
    QString socket_adress = ui->lineEdit_Adress->text();

    QStringList addressList = socket_adress.split("."); // разбиваем на список значений между точками

    if(addressList.size() != 4) // если в списке не 4 значения, значит это не IPv4 адресс
    {
        QMessageBox::warning(this, "Ошибка", "Введённый адрес не соответствует формату IPv4!");
        return;
    }

    for (int i=0; i<addressList.size(); i++)
    {
        bool ok = false;
        int value = addressList[i].toInt(&ok);

        if(!ok || value < 0 || value > 255)
        {
            QMessageBox::warning(this, "Ошибка", "Введённый адрес не соответствует формату IPv4!");
            return;
        }
    }

    setError(false);

    TCPSocket = new QTcpSocket(this);
    connect(TCPSocket,SIGNAL(readyRead()),this,SLOT(read_data()));

    QHostAddress  *adres;
    adres = new QHostAddress();
    adres->setAddress(socket_adress);

    connect(TCPSocket, &QTcpSocket::connected, this, &MainWindow::connectedSuccess);
    connect(TCPSocket, &QTcpSocket::disconnected, this, &MainWindow::closedConnection);

    TCPSocket->connectToHost(*adres,port);

    ui->pushButton_SendMes->setEnabled(true);
}

void MainWindow::on_pushButton_Disconnect_clicked() //кнопка "отсоединиться"
{
    if(ui->radioButton_Client->isChecked())
    {
        if(TCPSocket == nullptr) //если соединение не было создано
        {
            QMessageBox::warning(this, "Ошибка", "Соединение не создано!");
            return;
        }
    }
    if(ui->radioButton_Server->isChecked())
    {
        if(TCPServer == nullptr) //если соединение не было создано
        {
            QMessageBox::warning(this, "Ошибка", "Соединение не создано!");
            return;
        }
    }

    ui->lineEdit_Port->clear();
    ui->lineEdit_Adress->clear();
    ui->textEdit_input->clear();
    ui->textEdit_output->clear();
    ui->lineEdit_Adress->clear();
    ui->lineEdit_Mes->clear();

    if(TCPSocket != nullptr)
    {
        TCPSocket->close();
        TCPSocket = nullptr;
    }

    ui->groupBox->setEnabled(true);

    clearRight();

    savedNick = false;

    //ui->pushButton_Connect->setEnabled(false);

    //ui->pushButton_Disconnect->setEnabled(true);
}

void MainWindow::on_radioButton_Server_clicked() //режим сервера
{
    clearElements();

    ui->label_Port->show();
    ui->lineEdit_Port->show();
    ui->pushButton_Show_Adress->show();

    ui->pushButton_Create->show();

    ui->pushButton_turnOfServer->show();
    ui->pushButton_turnOfServer->setEnabled(false);
}

void MainWindow::on_pushButton_Create_clicked() //создали соединение
{
    int port;

    bool flag = false;

    if(ui->lineEdit_Port->text().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Поле порта пусто!");
        return;
    }

    port=ui->lineEdit_Port->text().toInt(&flag);

    if(!flag)
    {
        QMessageBox::warning(this, "Ошибка", "Введено некорректное значение порта!");
        return;
    }
    if(port < 0 || port > 65535)
    {
        QMessageBox::warning(this, "Ошибка", "Введено недействительное значение порта!");
        return;
    }
    if(TCPServer == nullptr)
    {
        //QMessageBox::information(this, "Ура!", "Сервер создан!");
    }
    else
    {
        QMessageBox::warning(this, "Ошибка!", "Сервер уже создан!");
        return;
    }

    TCPServer = new QTcpServer(this);

    if(!TCPServer->listen(QHostAddress::Any,port))
    {
        QMessageBox::warning(this, "Ошибка", "Сервер с таким портом уже имеется!");
        TCPServer->close();
        TCPServer = nullptr;
        return;
    }
    else
    {
        QMessageBox::information(this, "Ура!", "Сервер создан!");
    }

    connect(TCPServer,SIGNAL(newConnection()),this,SLOT(new_connection()));

    ui->pushButton_Disconnect->setEnabled(true);
    ui->pushButton_turnOfServer->setEnabled(true);

    ui->groupBox->setEnabled(false);
}

void MainWindow::new_connection() //к нам подключились
{
    QTcpSocket *tcpSocket = TCPServer->nextPendingConnection(); //соединяем два сокета

    Client *newClient = new Client(this);
    newClient->setTcpSocket(tcpSocket);

    connect(newClient, &Client::incomingMessage, this, &MainWindow::incomingMessage);
    connect(newClient, &Client::clientDisconnected, this, &MainWindow::clientDisconnected);

    newClient->setClientID(users.size());

    users.append(newClient);

    newClient->setClientsNames(&users);

    ui->label_chat->show();
    ui->label_yourMessage->show();
    ui->textEdit_input->show();
    ui->textEdit_output->show();
    ui->lineEdit_Mes->show();
    ui->pushButton_SendMes->show();
    ui->pushButton_StopSpam->show();
    ui->pushButton_Spam->show();
    ui->pushButton_clearAll->show();
}

void MainWindow::on_pushButton_turnOfServer_clicked() //кнопка "выключить сервер"
{
    for(int i = users.size() - 1; i>=0; i--)
    {
        users[i]->closeConnection();
    }
    users.clear();

    if(TCPServer != nullptr)
    {
        TCPServer->close();
        TCPServer = nullptr;
    }
    if(TCPSocket!= nullptr)
    {
        TCPSocket->close();
        TCPSocket = nullptr;
    }

    on_pushButton_clearAll_clicked();

    ui->lineEdit_Port->clear();
    ui->lineEdit_Adress->clear();

    ui->groupBox->setEnabled(true);

    clearRight();

    ui->pushButton_turnOfServer->setEnabled(false);
}

void MainWindow::incomingMessage(QString msg) //рассылка получаемого сообщения от клиента всем клиентам и серверу
{
    ui->textEdit_input->append(msg);
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
        incomingMessage("Клиент " + disconnnectedClient->getNick() + " отключился");
    }

    users.removeAt(disconnnectedClient->getClientID());

    for(int i = 0;i<users.size();i++)
    {
        users[i]->setClientID(i);
    }
}

void MainWindow::read_data(void) //пришли данные
{    
    QString stroka = QString::fromLocal8Bit(TCPSocket->readAll());
    if(stroka == "#nickerror_0")
    {
        setError(true);
        QMessageBox::warning(this,"Ошибка", "Данный ник занят");
    }
    else
    {
        ui->textEdit_input->append(stroka);
    }
}

void MainWindow::on_pushButton_SendMes_clicked() //кнопка "отправить сообщение"
{
    if(ui->lineEdit_Mes->text().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Текстовое поле пусто!");
        return;
    }

    if(ui->radioButton_Server->isChecked())
    {
        for(int i = 0;i<users.size();i++)
        {
            users[i]->sendMessage("Сервер:" + ui->lineEdit_Mes->text());
            ui->textEdit_input->append("Сервер:" + ui->lineEdit_Mes->text());
        }
    }
    else
    {
        sendToServer(getNick() + ":" + ui->lineEdit_Mes->text());
    }

    ui->textEdit_output->append(ui->lineEdit_Mes->text());

    ui->lineEdit_Mes->clear();
}

void MainWindow::on_pushButton_Show_Adress_clicked() //кнопка "показать мой IP-адрес"
{
    ui->textEdit_output->setVisible(true);
//    ui->pushButton_Show_Adress->setEnabled(false);
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            ui->textEdit_output->append(address.toString());
    }
}

void MainWindow::connectedSuccess() //успешное соединение
{
    this->nick = ui->lineEdit_nick->text();

    sendToServer(nick + " подключился");

    ui->groupBox->setEnabled(false);

    ui->pushButton_StopSpam->show();
    ui->pushButton_Spam->show();
    ui->pushButton_Disconnect->show();

    ui->label_chat->show();
    ui->label_yourMessage->show();
    ui->textEdit_input->show();
    ui->textEdit_output->show();
    ui->lineEdit_Mes->show();
    ui->pushButton_SendMes->show();

    ui->pushButton_clearAll->show();

    connectFl = true;

    ui->pushButton_Connect->setEnabled(false);

    ui->pushButton_Disconnect->setEnabled(true);
}

void MainWindow::closedConnection() //соединение закрыто
{
    connectFl = false;

    if(timer->isActive())
    {
        timer->stop();
        ui->pushButton_Spam->setEnabled(true);
        ui->pushButton_StopSpam->setEnabled(false);
    }

    if(TCPSocket != nullptr)
    {
        TCPSocket->close();
        TCPSocket = nullptr;
    }

    ui->groupBox->setEnabled(true);

    savedNick = false;


    ui->pushButton_Connect->setEnabled(true);

    ui->pushButton_Disconnect->setEnabled(false);

    clearRight();

    if(!error)
    {
        QMessageBox::information(this, "Всё!", "Соединение оборвано!");
    }
}

void MainWindow::clearElements() //возврат в изначальное состояние (не выбрано)
{
    ui->pushButton_StopSpam->hide();
    ui->pushButton_Spam->hide();
    ui->pushButton_Disconnect->hide();

    ui->pushButton_Connect->hide();
    ui->pushButton_Create->hide();
    ui->pushButton_SendMes->hide();

    ui->label_Adress->hide();
    ui->label_Port->hide();

    ui->lineEdit_Adress->hide();
    ui->lineEdit_Mes->hide();
    ui->lineEdit_Port->hide();

//    ui->label_chat->hide();
//    ui->label_yourMessage->hide();

//    ui->textEdit_input->hide();
//    ui->textEdit_output->hide();

    ui->pushButton_turnOfServer->hide();

    ui->pushButton_clearAll->hide();

    ui->label->hide();
    ui->pushButton_Show_Adress->hide();

    ui->lineEdit_nick->hide();
    ui->lineEdit_nick->setReadOnly(false);

    ui->pushButton_saveNick->hide();
    ui->pushButton_saveNick->setEnabled(true);
}

void MainWindow::on_pushButton_clearAll_clicked() //кнопка "очистить всё"
{
    ui->textEdit_input->clear();
    ui->textEdit_output->clear();
}

void MainWindow::clearRight() //очистить справа
{
    ui->label_chat->hide();
    ui->label_yourMessage->hide();

    ui->textEdit_input->hide();
    ui->textEdit_output->hide();

    ui->lineEdit_Mes->hide();

    ui->pushButton_SendMes->hide();

    ui->pushButton_StopSpam->hide();
    ui->pushButton_Spam->hide();

    ui->pushButton_clearAll->hide();

    ui->pushButton_saveNick->setEnabled(true);

    ui->lineEdit_nick->setReadOnly(false);
    ui->lineEdit_nick->clear();
}

void MainWindow::on_timer() //то, что будет вызываться по таймеру для спам-бота
{
    if(ui->radioButton_Client->isChecked())
    {
        sendToServer(nick + ": spam");
    }
    else
    {
        incomingMessage("Сервер: spam");
    }
    ui->textEdit_output->append("spam");
}

void MainWindow::on_pushButton_Spam_clicked() //кнопка "начать спам"
{
    if((users.size() == 0)&&(ui->radioButton_Server->isChecked()))
    {
        QMessageBox::warning(this, "Ошибка", "Сервер не соединён с клиентами!");
        return;
    }

    timer->start();

    ui->pushButton_StopSpam->setEnabled(true);
    ui->pushButton_Spam->setEnabled(false);
}

void MainWindow::on_pushButton_StopSpam_clicked() //кнопка "закончить спам"
{
    timer->stop();

    ui->pushButton_StopSpam->setEnabled(false);
    ui->pushButton_Spam->setEnabled(true);
}

void MainWindow::on_pushButton_saveNick_clicked() //кнопка "сохранить имя"
{
    int sizeNick = ui->lineEdit_nick->text().length();

    if( (sizeNick < 3)||(sizeNick > 10) )
    {
        QMessageBox::warning(this, "Ошибка", "Длина имени должна быть\nне меньше 3 и не больше 10 символов!");
        return;
    }

    if(ui->lineEdit_nick->text().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Поле имени пусто!");
        return;
    }

    ui->pushButton_saveNick->setEnabled(false);
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

    if(timer->isActive())
    {
        timer->stop();
    }

    if(TCPServer != nullptr)
    {
        TCPServer->close();
        TCPServer = nullptr;
    }
    if(TCPSocket!= nullptr)
    {
        TCPSocket->close();
        TCPSocket = nullptr;
    }

    event->accept();
}

bool MainWindow::getError() const //геттер ошибки
{
    return error;
}

void MainWindow::setError(bool value) //сеттер ошибки
{
    error = value;
}
