#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : //
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer=new QTimer(this); //Создаем экземпляр класса
    connect(timer,SIGNAL(timeout()),this, SLOT(timer_tick()));
    counter=0;
    down_timer=new QTimer(this); //Создаем экземпляр класса
    connect(down_timer,SIGNAL(timeout()),this, SLOT(down_timer_tick()));
    down_counter=60;
    ui->label->hide();
    down_timer->setInterval(1000);
    third_timer= new QTimer(this);
    connect(third_timer, SIGNAL(timeout()), this, SLOT(third_timer_tick()));
    firstcounter=0;
    third_timer->setInterval(1000);

}

MainWindow::~MainWindow()//
{
    delete ui;
}

void MainWindow::timer_tick()// таймер 1
{ //то, что будет вызываться по таймеру
    counter++;
    QString str;
    int hours,min,sec,s_sec; //часы,минуты,секунды, сотые секунды
    s_sec=counter% 100;
    sec=counter/100;
    min=sec/60;
    sec%=60;
    hours=min/60;
    min%=60;
    if(hours<10)
        str.append("0");
    str.append(QString::number(hours));
    str.append(":");
    if(min<10)
        str.append("0");
    str.append(QString::number(min));
    str.append(":");
    if(sec<10)
        str.append("0");
    str.append(QString::number(sec));
    str.append(".");
    if(s_sec<10)
        str.append("0");
    str.append(QString::number(s_sec));
    ui->lcdNumber_out->display(str);

}
void MainWindow::on_pushButton_start_clicked() //Кнопка старт
{
    counter=0;
    timer->start(10);
    ui->pushButton_pause->setEnabled(true);
}

void MainWindow::on_pushButton_stop_clicked() //Кнопка стоп
{
    timer->stop();
    ui->pushButton_pause->setEnabled(false);
}

void MainWindow::on_pushButton_pause_clicked() //Кнопка пауза
{
    if(timer->isActive()){
        timer->stop();
    } else{
        timer->start(10);
    }

    if(third_timer->isActive()){
        third_timer->stop();
    } else{
        third_timer->start(10);
    }
}

void MainWindow::on_pushButton_clicked()// армагедон
{
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_pause->setEnabled(false);
    ui->pushButton_start->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
    timer->stop();
    ui->spinBox_start->setEnabled(false);
    ui->spinBox_stop->setEnabled(false);

    down_counter=10;
    down_timer->start();
}

void MainWindow::down_timer_tick()// таймер отсчета
{
    down_counter--;
    QString str;
    int hours,min,sec; //часы,минуты,секунды

    sec=down_counter%60;
    min=down_counter/60;
    sec%=60;
    hours=min/60;
    min%=60;
    if(hours<10)
        str.append("0");
    str.append(QString::number(hours));
    str.append(":");
    if(min<10)
        str.append("0");
    str.append(QString::number(min));
    str.append(":");
    if(sec<10)
        str.append("0");
    str.append(QString::number(sec));

    ui->lcdNumber_down->display(str);

    if(down_counter==0){
        down_timer->stop();
        third_timer->stop();
        ui->label->setText("BOOM!");
        ui->label->setVisible(true);
        ui->pushButton_stop->click();
        ui->pushButton_stop->setEnabled(false);
        ui->pushButton_pause->setEnabled(false);
        ui->pushButton_start->setEnabled(false);
        ui->spinBox_start->setEnabled(false);
        ui->spinBox_stop->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
    }

}

void MainWindow::on_pushButton_2_clicked()// отсчет и запуск
{

    firstcounter=0;
    secondcounter= ui->spinBox_stop->value()+1;
    n=ui->spinBox_start->value();
    ui->spinBox_start->setEnabled(true);
    ui->spinBox_stop->setEnabled(true);
    third_timer->start();
}

void MainWindow::third_timer_tick()// таймер запуска со спинов
{

    firstcounter++;

    if(firstcounter>=n){
        if(fl){
            ui->pushButton_start->click();
            fl=false;
        }
        secondcounter--;
        if(secondcounter<=0){
            ui->pushButton_stop->click();
            third_timer->stop();
            fl=true;
        }
    }
}
