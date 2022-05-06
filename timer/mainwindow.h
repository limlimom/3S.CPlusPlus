#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMessageBox>

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
    void timer_tick(); //Слот для обработки сигнала таймера

    void on_pushButton_start_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_pause_clicked();

    void down_timer_tick();

    void on_pushButton_clicked();

    void third_timer_tick();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer; //Указатель на класс таймер
    int counter;
    QTimer *down_timer;
    int down_counter;
    QTimer *third_timer;
    int firstcounter, secondcounter;
    int n;
    bool fl=true;

};

#endif // MAINWINDOW_H
