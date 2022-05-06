#include "client.h"

Client::Client(QObject *parent) : QObject(parent) //конструктор класса "клиент"
{

}

void Client::setTcpSocket(QTcpSocket *newTcpSocket) //сеттер TCP-сокета
{
    if(newTcpSocket != nullptr)
    {
        tcpSocket = newTcpSocket;
        connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::read_data);
        connect(tcpSocket, &QTcpSocket::disconnected, this, &Client::ClientClosedConnection);
        isActive = true; //только в этом случае клиент становится активным
    }
}

void Client::sendMessage(QString sendMessageText) //отправить сообщение
{
    char * date;
    int size;
    size = sendMessageText.length();
    QByteArray ba = sendMessageText.toLocal8Bit();

    date = ba.data();

    if(tcpSocket != nullptr && isActive)
    {
        tcpSocket->write(date, size);
    }
}

void Client::closeConnection() //функция закрытия соединения со стороны сервера
{
   if (tcpSocket != nullptr)
   {
       tcpSocket->close();
   }
}

int Client::getClientID() const //возвращает значение ID
{
    return clientID;
}

void Client::setClientID(int NewClientID) //устанавливает значение ID
{
    clientID = NewClientID;
}

void Client::setNick(QString newNick) //устанавливаем ник
{
    this->nick = newNick;
}

QString Client::getNick() //возвращает значение ника
{
    return nick;
}

void Client::setClientsNames(QList<Client *> *users) //сеттер списка клиентов
{
    clientNames = users;
}

bool Client::getIngnore() const //геттер фильтрации отключения клиента про дубликате имени
{
    return ingnore;
}

void Client::setIngnore(bool value) //сеттер фильтрации отключения клиента про дубликате имени
{
    ingnore = value;
}

void Client::read_data() //метод получения значения сокета
{
    QString stroka = QString::fromLocal8Bit(tcpSocket->readAll());
    lastMessage = stroka;

    if(!savedNick)
    {
        QString newName = stroka.left(stroka.indexOf(" ",0));

        for(int i = 0;i<clientNames->size();i++)
        {
            if(newName == clientNames->at(i)->getNick())
            {
                sendMessage("#nickerror_0");
                setIngnore(true);
                closeConnection();
                return;
            }
        }

        savedNick = true;
        setNick(newName);
    }

    emit incomingMessage(lastMessage); //отправляем сигнал о новом сообщении
}

void Client::ClientClosedConnection() //слот добровольного отключения клиента
{
    isActive = false;
    emit clientDisconnected(this);
}
