#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork/QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

    void setTcpSocket(QTcpSocket * newTcpSocket);

    void sendMessage(QString sendMessageText); //сообщение, которое будет отправлено пользователю

    void closeConnection();

    int getClientID() const;

    void setClientID(int NewClientID);

    void setNick(QString newNick);

    QString getNick();

    void setClientsNames(QList<Client*> *users);

    bool getIngnore() const;

    void setIngnore(bool value);

private:
    QList<Client*> *clientNames = nullptr;

    QTcpSocket *tcpSocket = nullptr;

    QString lastMessage = ""; //последнее сообщение

    bool isActive = false;

    int clientID = -1;

    QString nick = "";

    bool savedNick = false;

    bool ingnore = false;

private slots:
    void read_data(); //чтение данных

    void ClientClosedConnection(); //клиент закрыл соединение

signals:
    void incomingMessage(QString messageText);

    void clientDisconnected(Client * disconnectedClient);

};

#endif // CLIENT_H
