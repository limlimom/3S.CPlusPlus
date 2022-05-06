#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtNetwork/QUdpSocket> //библиотека работы с UDP
#include <QNetworkInterface>
#include <QTimer> // подключение класса таймера


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
    void on_pushButton_Create_clicked();

    void on_timer(void); // слот обработки события таймера

    void read_data(void); //слот для обработки поступивших данных

    void on_pushButton_Send_Mes_clicked();

    void on_pushButton_Show_Adress_clicked();

    void on_pushButton_Clear_All_clicked();

    void on_pushButton_Disconnect_clicked();

    QStringList getMessagesList(QString message, int split_size = 60000);

    void on_pushButton_Spam_clicked();

    void on_pushButton_StopSpam_clicked();

private:
    Ui::MainWindow *ui;

    QUdpSocket * udpsocket = nullptr; //сам UDP сокет (указатель на него)

    QHostAddress * adress = nullptr; //для адреса

    int port, our_port; //порты

    QString inputMessage;

    QTimer * timer; // указатель на класс Таймер

    int counter; //счетчик
};

#endif // MAINWINDOW_H
