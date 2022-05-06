#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QTimer> // подключение класса таймера

#include <QCloseEvent>

#include "client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_radioButton_Client_clicked();

    void on_radioButton_Server_clicked();

    void on_pushButton_Create_clicked();

    void read_data(void);

    void new_connection(void);

    void on_timer(void); // слот обработки события таймера

    void on_pushButton_Connect_clicked();

    void on_pushButton_SendMes_clicked();

    void on_pushButton_Show_Adress_clicked();

    void connectedSuccess(); //успешное соединение с сервером

    void closedConnection(); //соединение закрыто

    void on_pushButton_Spam_clicked();

    void on_pushButton_StopSpam_clicked();

    void on_pushButton_Disconnect_clicked();

    void on_pushButton_turnOfServer_clicked();

    void on_pushButton_clearAll_clicked();

    void clearElements();

    void clearRight();

    void incomingMessage(QString messageText);

    void clientDisconnected(Client *disconnnectedClient);

    void on_pushButton_saveNick_clicked();

    void setNick(QString nick);

    QString getNick();

    void sendToServer(QString msg);

    void closeEvent(QCloseEvent *event);

    bool getError() const;

    void setError(bool value);

private:
    Ui::MainWindow *ui;

    QTcpServer * TCPServer = nullptr;

    QTcpSocket * TCPSocket = nullptr;

    QTimer * timer; // указатель на класс Таймер

    bool connectFl = false; //флаг подключения

    QList<Client*> users; //список клиентов

    bool savedNick = false; //флаг сохранения ника

    QString nick = "";

    bool error = false;

};

#endif // MAINWINDOW_H
