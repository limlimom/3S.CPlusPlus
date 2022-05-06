#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : //конструктор
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
     ui->setupUi(this);

     timer = new QTimer(this);
     connect(timer,SIGNAL(timeout()), this, SLOT(on_timer())); //соединение события с таймером
     timer->setInterval(1000);

     ui->textEdit_Show_Adress->setVisible(false); //скрываем поле IP-адреса до нажатия на кнопку

     //устанавливаем textEdit только на чтение
     ui->textEdit_Input->setReadOnly(true);
     ui->textEdit_Output->setReadOnly(true);
     ui->textEdit_Show_Adress->setReadOnly(true);
     ui->pushButton_Spam->setVisible(false);
     ui->pushButton_StopSpam->setVisible(false);

}

MainWindow::~MainWindow() //деструктор
{
    delete ui;
}

void MainWindow::read_data() //считывание данных
{
    char * date;
    long int size;
    size=long(udpsocket->pendingDatagramSize());
   // qDebug() << "size = " << size;
    if(size>0)
    {
        date = new char [size+1];
        udpsocket->readDatagram(date, size);
        date[size]=0; //завершающий 0 - признак конца
        QString stroka;
        stroka=QString::fromLocal8Bit(date, size);
        inputMessage.append(stroka);
       // qDebug() << "stroka = " << stroka;
        ui->textEdit_Input->append(stroka);
        delete [] date;
    }
}

void MainWindow::on_pushButton_Create_clicked() //кнопка "создать"
{
    bool ok1 = false; //флаги
    bool ok2 = false;

    port=ui->lineEdit_Port->text().toInt(&ok1);
    our_port=ui->lineEdit_Our_Port->text().toInt(&ok2);

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

    if (udpsocket != nullptr) //проверяем на существование сокета
    {
        QMessageBox::warning(this, "Ошибка", "Сокет уже создан! Для создания необходимо нажать кнопку Отсоединиться");
    }

    else if(!ok1 || !ok2) //проверка на корректность введенных данных
    {
        QMessageBox::warning(this, "Ошибка", "Введите корректное значение портов");
    }

    else if(our_port <= 0 || our_port >= 65534 ) //проверка валидности нашего порта
    {
        QMessageBox::warning(this, "Ошибка", "Введено недопустимое значение порта для сокета");
        ui->lineEdit_Our_Port->clear();
    }

    else if(port <= 0 || port >= 65534 ) //проверка валидности внешнего порта
    {
        QMessageBox::warning(this, "Ошибка", "Введено недопустимое значение внешнего порта для сокета");
        ui->lineEdit_Port->clear();
    }

    else if(socket_adress == "") //проверка наличия адреса сокета
    {
        QMessageBox::warning(this, "Ошибка", "Не введен адрес сокета");
    }

    else //если все хорошо
    {
        udpsocket = new QUdpSocket(this);
        udpsocket->bind(QHostAddress::Any, quint16(our_port));

        connect(udpsocket, &QUdpSocket::readyRead, this, &MainWindow::read_data);
        adress = new QHostAddress();
        //adress->setAddress(ui->lineEdit_Adress->text());
        adress->setAddress(socket_adress);

        //udpsocket->writeDatagram(" ", 1, *adress, quint16(our_port));

        //ui->lineEdit_Mes->setText(" ");
        //on_pushButton_Send_Mes_clicked();

        //ui->lineEdit_Mes->clear();
        //ui->textEdit_Input->clear();
        //ui->textEdit_Output->clear();

        QMessageBox::information(this, "Ура!", "Сокет создан!");
    }
}

void MainWindow::on_pushButton_Send_Mes_clicked() //кнопка "отправить сообщение"
{
    if(udpsocket == nullptr) //если не создан сокет
    {
        QMessageBox::warning(this, "Ошибка", "Сокет не создан!");
        ui->lineEdit_Mes->clear();
    }

    else if (ui->lineEdit_Mes->text() == "") //если пустое поле
    {
        QMessageBox::warning(this, "Ошибка", "Введите сообщение!");
    }

    else //если все хорошо
    {
        QString message = ui->lineEdit_Mes->text();
        QStringList message_list = getMessagesList(message, 54000);//54000

        for(int i = 0; i < message_list.size(); ++i)
        {
           QString text = message_list.at(i);
           //qDebug()<<text;
           QByteArray ba= text.toLocal8Bit();
           char * date;
           long size = text.length();
           //size=ui->lineEdit_Mes->text().length();
           date = ba.data();
           //qDebug() << "size send = " << size;
           udpsocket->writeDatagram(date, size, *adress, quint16(port));
        }
        ui->textEdit_Output->append(ui->lineEdit_Mes->text());
        ui->lineEdit_Mes->clear();
        ui->pushButton_Spam->setVisible(true);
        ui->pushButton_StopSpam->setVisible(true);
    }
}

void MainWindow::on_pushButton_Show_Adress_clicked() //кнопка "показать мой IP-адрес"
{
    ui->textEdit_Show_Adress->setVisible(true);
    ui->pushButton_Show_Adress->setEnabled(false);
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            ui->textEdit_Show_Adress->append(address.toString());
    }
}

void MainWindow::on_pushButton_Clear_All_clicked() //кнопка "очистить всё"
{
    ui->textEdit_Input->clear();
    ui->textEdit_Output->clear();
    ui->lineEdit_Mes->clear();
}

void MainWindow::on_pushButton_Disconnect_clicked() //кнопка разрыва соединения
{
    if(udpsocket == nullptr) //если соединение не было создано
    {
        QMessageBox::warning(this, "Ошибка", "Сокет не создан!");
        return;
    }

    ui->lineEdit_Port->clear();
    ui->lineEdit_Our_Port->clear();
    ui->textEdit_Input->clear();
    ui->textEdit_Output->clear();
    ui->lineEdit_Adress->clear();
    ui->lineEdit_Mes->clear();

    ui->pushButton_Spam->setVisible(false);
    ui->pushButton_StopSpam->setVisible(false);

    adress->clear();
    adress = nullptr;

    udpsocket->close();
    udpsocket = nullptr;

    if(udpsocket == nullptr)
    {
        QMessageBox::information(this, "Ура!", "Вы отсоединились!");
        ui->pushButton_Spam->setVisible(false);
        ui->pushButton_StopSpam->setVisible(false);
        timer->stop();
        counter = 1;

    }
}

QStringList MainWindow::getMessagesList(QString message, int split_size) //метод обработки разделения сообщений на части
{
    QStringList messageList, bufStr;
    if(message.size()<=split_size)
    {
        messageList.append(message);
        return messageList;
    }
    int iter = 0;
    for (int i = 0; i < message.size(); ++i)
    {
        if(iter == split_size)
        {
            messageList.append(message.mid(i - iter,iter));
            iter = 0;
        }
        if(i == (message.size() - 1))
        {
            messageList.append(message.mid(i - iter,iter + 1));
        }
        iter++;
    }
    return messageList;
}
void MainWindow::on_pushButton_Spam_clicked() //кнопка "начать спам"
{
    QString socket_adress = ui->lineEdit_Adress->text();

    if(socket_adress == "") //проверка наличия адреса сокета
    {
        QMessageBox::warning(this, "Ошибка", "Не введен адрес сокета, вы не можете начать спам!");
    }

    else //если все хорошо
    {
    timer->start();
    counter = 1;

    ui->pushButton_StopSpam->setEnabled(true);
    ui->pushButton_Spam->setEnabled(false);
    }
}


void MainWindow::on_timer() // то что будет вызываться по таймеру
{
    counter--;

    QString str;
    int h,m,sec; //часы, минуты. секунду, сотые секунд

    sec=counter%60;
    m=counter/60;
    sec=sec%60;
    h=m/60;
    m=m%60;

    if (h<10)
        str.append("0");
    str.append(QString::number(h));
    str.append(":");
    if (m<10)
        str.append("0");
    str.append(QString::number(m));
    str.append(":");
    if (sec<10)
        str.append("0");
    str.append(QString::number(sec));

//    ui->lcdNumber->display(str);

     if(counter==0)
     {
         QString message = "spam";
         QStringList message_list = getMessagesList(message, 54000);
         for (int i = 0; i < message_list.size(); ++i)
         {
             QString text = message_list.at(i);
             QByteArray ba=text.toLocal8Bit(); // кодировка
             char * date; // данные
             long size = text.length(); // размер
             date= ba.data();
             //qDebug() << "size send = " << size;
             udpsocket->writeDatagram(date,size,*adress,quint16(port));
         }

         ui->textEdit_Output->append("spam"); // вывод сообщения
         counter=1;
     }
}


void MainWindow::on_pushButton_StopSpam_clicked() //кнопка "закончить спам"
{
    timer->stop();

    ui->lineEdit_Mes->setText(" ");
    ui->pushButton_StopSpam->setEnabled(false);
    ui->pushButton_Spam->setEnabled(true);
}

