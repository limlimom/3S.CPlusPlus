#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QRadioButton>
#include <QTimer>

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
    void on_pushButton_start_clicked();

    void on_pushButton_b_clicked();

    void on_pushButton_f_clicked();

    void on_pushButton_finish_clicked();

    void choice();

    int radio_choice(); //метод возвращения значения выбранного radiobutton

    void resetRadio();

    void mark_radio();

    void on_radioButton_1_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_4_clicked();

    void on_timer(void); //слот обработки события таймера

    void m_down_timer(void); //слот обработки события таймера




private:
    Ui::MainWindow *ui;
    QTimer * timer; //указатель на класс Таймер
    QTimer * down_timer; //указатель на класс Таймер

    int num_question = -1;

    int counter; //счетчик
    int down_counter; //счетчик

    int answers[5] = {0, 0, 0, 0, 0}; //массив с выбранными ответами
    int answers_right[5] = {2, 1, 3, 4, 2}; //правильные ответы
};

#endif // MAINWINDOW_H
