#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_finish->setEnabled(false);
    ui->pushButton_b->setVisible(false);
    ui->pushButton_f->setVisible(false);
    ui->radioButton_1->setVisible(false);
    ui->radioButton_2->setVisible(false);
    ui->radioButton_3->setVisible(false);
    ui->radioButton_4->setVisible(false);
    ui->label_question->setVisible(false);
    ui->label_result->setVisible(false);
    ui->tableWidget->setVisible(false);
    ui->tableWidget->setEnabled(false);
    ui->lcdNumber->display("00:00");
    ui->lcdNumber_2->display("00:00");


    setWindowTitle("Тест");

    timer=new QTimer(this); //создаем экземпляр таймера
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timer())); //соединили событие с таймером
    timer->setInterval(1000);
    counter=0;
    down_timer = new QTimer(this);
    connect(down_timer,SIGNAL(timeout()),this,SLOT(m_down_timer())); //соединили событие с таймером
    down_timer->setInterval(1000); //интервал счета
    //down_counter=60;//кол-во секунд на обратный отсчёт

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_timer() //то, что будет вызываться по таймеру потраченного времени
{
   counter++; //увеличили счетчик
   QString str;
   int h, m, sec; //часы, минуты, секунды

   sec=counter%60;
   m=sec/60;
   sec=sec%60;
   h=m/60;
   m=m%60;

   if(h<10){
       str.append("0");
   }
    str.append(QString::number(h));
    str.append(":");

   if(m<10){
       str.append("0");
   }
    str.append(QString::number(m));
    str.append(":");

   if(sec<10){
       str.append("0");
    }
    str.append(QString::number(sec));


    ui->lcdNumber->display(str);

}

void MainWindow::m_down_timer() //таймер оставшегося времени
{
    down_counter--;
    QString str;
    int h, m, sec; //часы, минуты, секунды

    sec=down_counter%60;
    m=down_counter/60;
    //m=sec/60;
    sec=sec%60;
    h=m/60;
    m=m%60;

    if(h<10)
    {
        str.append("0");
    }
    str.append(QString::number(h));
    str.append(":");

    if(m<10)
    {
        str.append("0");
    }
    str.append(QString::number(m));
    str.append(":");

    if(sec<10)
    {
        str.append("0");
    }
    str.append(QString::number(sec));

    ui->lcdNumber_2->display(str);

    if (down_counter==0)
    {
        on_pushButton_finish_clicked();
    }
}


void MainWindow::on_pushButton_start_clicked()
{
    ui->radioButton_1->setVisible(true);
    ui->radioButton_2->setVisible(true);
    ui->radioButton_3->setVisible(true);
    ui->radioButton_4->setVisible(true);
    ui->pushButton_b->setVisible(true);
    ui->pushButton_f->setVisible(true);
    ui->label_question->setVisible(true);

    ui->pushButton_finish->setEnabled(true);
    ui->pushButton_start->setEnabled(false);
    ui->pushButton_b->setEnabled(false);
    on_pushButton_f_clicked();

    counter=0;
    timer->start(); //10 мсек (0,01 сек)
    down_timer->start();
    down_counter=30;//кол-во секунд на обратный отсчёт

}

void MainWindow::on_pushButton_b_clicked()
{
    num_question--;
    choice();

}

void MainWindow::on_pushButton_f_clicked()
{
    num_question++;
    choice();
}

int MainWindow::radio_choice() //возвращает значение выбранного radiobutton
{
    if(ui->radioButton_1->isChecked()){ return 1; }
    if(ui->radioButton_2->isChecked()){ return 2; }
    if(ui->radioButton_3->isChecked()){ return 3; }
    if(ui->radioButton_4->isChecked()){ return 4; }
    return 0;
}

void MainWindow::resetRadio()
{
    ui->radioButton_1->setAutoExclusive(false);
    ui->radioButton_1->setChecked(false);
    ui->radioButton_1->setAutoExclusive(true);

    ui->radioButton_2->setAutoExclusive(false);
    ui->radioButton_2->setChecked(false);
    ui->radioButton_2->setAutoExclusive(true);

    ui->radioButton_3->setAutoExclusive(false);
    ui->radioButton_3->setChecked(false);
    ui->radioButton_3->setAutoExclusive(true);

    ui->radioButton_4->setAutoExclusive(false);
    ui->radioButton_4->setChecked(false);
    ui->radioButton_4->setAutoExclusive(true);
}

void MainWindow::mark_radio() //выделить сохраненный ответ
{
    switch (answers[num_question])
    {
        default: //в случае без case
            resetRadio();
            break;
        case 1:
            ui->radioButton_1->setChecked(true);
            break;
        case 2:
            ui->radioButton_2->setChecked(true);
            break;
        case 3:
            ui->radioButton_3->setChecked(true);
            break;
        case 4:
            ui->radioButton_4->setChecked(true);
            break;
    }
}

void MainWindow::on_pushButton_finish_clicked()
{

    timer->stop();
    down_timer->stop();
    ui->label_result->setVisible(true);
    ui->tableWidget->setVisible(true);
    ui->pushButton_b->setEnabled(false);
    ui->pushButton_f->setEnabled(false);


    if(num_question == 4)
    {
        answers[num_question] = radio_choice();
    }

    for(int i{};i<5;i++)
    {
        if(answers[i] == answers_right[i])
        {
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem("правильно"));
            ui->tableWidget->item(i,0)->setBackground(Qt::green);

        }
        else if(answers[i] == 0)
        {
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem("нет ответа"));
            ui->tableWidget->item(i,0)->setBackground(Qt::yellow);
        }
        else
        {
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem("неправильно"));
            ui->tableWidget->item(i,0)->setBackground(Qt::red);
        }
    }

}

void MainWindow::on_radioButton_1_clicked() //возможность запоминания выбранного ответа
{
    answers[num_question] = radio_choice();
}

void MainWindow::on_radioButton_2_clicked() //возможность запоминания выбранного ответа
{
    answers[num_question] = radio_choice();
}

void MainWindow::on_radioButton_3_clicked() //возможность запоминания выбранного ответа
{
    answers[num_question] = radio_choice();
}

void MainWindow::on_radioButton_4_clicked() //возможность запоминания выбранного ответа
{
    answers[num_question] = radio_choice();
}


void MainWindow::choice()
{
    ui->radioButton_1->setAutoExclusive(false);
    ui->radioButton_1->setChecked(false);
    ui->radioButton_1->setAutoExclusive(true);

    ui->radioButton_2->setAutoExclusive(false);
    ui->radioButton_2->setChecked(false);
    ui->radioButton_2->setAutoExclusive(true);

    ui->radioButton_3->setAutoExclusive(false);
    ui->radioButton_3->setChecked(false);
    ui->radioButton_3->setAutoExclusive(true);

    ui->radioButton_4->setAutoExclusive(false);
    ui->radioButton_4->setChecked(false);
    ui->radioButton_4->setAutoExclusive(true);

    switch (num_question)
    {
    case 0:
        ui->pushButton_b->setEnabled(false);
        ui->label_question->setText("1.Какой породы крыс не существвует?"); //1 вопрос

        //варианты ответа
        ui->radioButton_1->setText("Сфинкс");
        ui->radioButton_2->setText("Вислоухая");//
        ui->radioButton_3->setText("Хаски");
        ui->radioButton_4->setText("Сиамская");

        break;
    case 1:

        ui->pushButton_b->setEnabled(true);
        ui->label_question->setText("2.Как долго протекает беременность у крыс?"); //2 вопрос

        //варианты ответа
        ui->radioButton_1->setText("20-24 дней");//
        ui->radioButton_2->setText("32-38 дней");
        ui->radioButton_3->setText("10-14 дней");
        ui->radioButton_4->setText("40-46 дней");

        break;
    case 2:
        ui->pushButton_b->setEnabled(true);
        ui->label_question->setText("3.Что из этого ложь?"); //3 вопрос

        //варианты ответа
        ui->radioButton_1->setText("Крысы - замечательные пловцы, они могут проплывать до 30 км и находиться в воде до 3-х суток.");
        ui->radioButton_2->setText("Крысы умеют смеятся, но их смех, к сожалению, мы не можем устышать.");
        ui->radioButton_3->setText("За один год особь потребляет до 58 кг продуктов.");//
        ui->radioButton_4->setText("Чтобы распознать запах и определить его местонахождение крысе нужно всего 0.02 сек.");


        break;
    case 3:
        ui->pushButton_b->setEnabled(true);
        ui->label_question->setText("4.Что НЕ умеют крысы?"); //4 вопрос

        //варианты ответа
        ui->radioButton_1->setText("Поддаваться дрессеровкию");
        ui->radioButton_2->setText("Прыгат больше,чем на 1 метр в высоту.");
        ui->radioButton_3->setText("Испытывать радость, грусть, обиду и другие эмоции.  ");
        ui->radioButton_4->setText("Жить без воды и еды более трех дней.");//


        break;
    case 4:
        ui->pushButton_b->setEnabled(true);
        ui->label_question->setText("5.Как зовут моих крыс?"); //5 вопрос

        //варианты ответа
        ui->radioButton_1->setText("Шерачка и Машерочка");
        ui->radioButton_2->setText("Луи и Виттон");//
        ui->radioButton_3->setText("Биба и Боба");
        ui->radioButton_4->setText("Кошк и Собэк");

        ui->pushButton_f->setEnabled(false);
        break;
    }


}
