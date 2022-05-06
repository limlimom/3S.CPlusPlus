#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QTimer>
#include <QCloseEvent>
#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonServer_clicked();

    void on_pushButtonClient_clicked();

    void on_pushButtonCreate_clicked();

    void on_pushButtonConnect_clicked();

    void on_pushButtonSend_clicked();

    void onServerConnection();

    void onSocketDisconneted();

    void onSocketReadyRead();

    void on_pushButtonAddresses_clicked();

    void onSpamTimeout();

    void new_connection(void);

    void read_data(void);

    void incomingMessage(QString messageText);

    void clientDisconnected(Client *disconnnectedClient);

    void on_pushButton_saveNick_clicked();

    void setNick(QString nick);

    QString getNick();

    void sendToServer(QString msg);

    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    QTcpServer *server = nullptr;
    QTcpSocket * TCPSocket = nullptr;
    QVector<QTcpSocket *> sockets;

    bool socketClearing = false;

    QTimer *spamTimer = nullptr;

    bool connectFl = false; //флаг подключения

    QList<Client*> users; //список клиентов

    bool savedNick = false; //флаг сохранения ника

    QString nick = "";

    bool error = false;

    void clearServer();
    void clearSockets();
};
#endif // MAINWINDOW_H
