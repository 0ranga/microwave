#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLCDNumber>
#include <QString>
#include <QTimer>
#include "Transitions.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    microwave = new QStateMachine();


    // Idle
    s1 = new QState();
    microwave->addState(s1);


    // stop
    stopState = new QState();
    microwave->addState(stopState);
    stopState->addTransition(ui->stopButton, SIGNAL(clicked()), s1);

    cookingState = new QState(stopState);
    hoursState = new QState(stopState);
    minutesState = new QState(stopState);
    powerState = new QState(stopState);
    durationState = new QState(stopState);
    modeState = new QState(stopState);
    defrostState = new QState(stopState);

    // clock
    s1->addTransition(ui->clockButton, SIGNAL(clicked()), hoursState);
    hoursState->addTransition(ui->clockButton, SIGNAL(clicked()), minutesState);
    minutesState->addTransition(ui->clockButton, SIGNAL(clicked()), s1);

    // power
    s1->addTransition(ui->powerButton, SIGNAL(clicked()), powerState);
    powerState->addTransition(ui->powerButton, SIGNAL(clicked()), durationState);
    durationState->addTransition(ui->powerButton, SIGNAL(clicked()), s1);

    // mode
    s1->addTransition(ui->modeButton, SIGNAL(clicked()), modeState);
    modeState->addTransition(ui->modeButton, SIGNAL(clicked()), durationState);
    durationState->addTransition(ui->modeButton, SIGNAL(clicked()), s1);

    // power + mode
    durationState->addTransition(ui->startButton, SIGNAL(clicked()), cookingState);

    // defrost
    s1->addTransition(ui->defrostButton, SIGNAL(clicked()), defrostState);
    defrostState->addTransition(ui->startButton, SIGNAL(clicked()), s1);



//    QObject::connect(s1, SIGNAL(entered()), this, SLOT(changeDisplay()));

//    addTrans(s1, hoursState, ui->clockButton, SIGNAL(clicked()), this, SLOT(changeDisplay()));

////    QObject::connect(s1, &QState::entered, this, &MainWindow::changeDisplay);
////    QObject::connect(stopState, SIGNAL(entered()), this, SLOT(changeDisplay()));
////    QObject::connect(hoursState, SIGNAL(entered()), this, SLOT(changeDisplay()));
////    QObject::connect(minutesState, SIGNAL(entered()), this, SLOT(changeDisplay()));
////    QObject::connect(powerState, SIGNAL(entered()), this, SLOT(changeDisplay()));
////    QObject::connect(durationState, SIGNAL(entered()), this, SLOT(changeDisplay()));
////    QObject::connect(modeState, SIGNAL(entered()), this, SLOT(changeDisplay()));
////    QObject::connect(cookingState, SIGNAL(entered()), this, SLOT(changeDisplay()));
////    QObject::connect(defrostState, SIGNAL(entered()), this, SLOT(changeDisplay()));
////    QObject::connect(hoursState, SIGNAL(entered()), this, SLOT(changeDisplay()));

    microwave->setInitialState(s1);
    microwave->start();

//    currentTime = QTime::currentTime();
//    QString s = QString::number(currentTime.hour());

    ui->lcdDisplay->display("OK");

    QObject::connect(s1, SIGNAL(entered()), this, SLOT(changeDisplay()));
    QObject::connect(hoursState, SIGNAL(entered()), this, SLOT(saveTime()));


    // Time
    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start();

    // Dial
    QObject::connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT(slide(int)));


}

MainWindow::~MainWindow()
{
    delete ui;

    delete microwave;
}

void MainWindow::on_clockButton_clicked() {
    qDebug() << "clockButton: On passe au state s1" << microwave->configuration().contains(s1);
    qDebug() << "clockButton: On passe au state stop" << microwave->configuration().contains(stopState);
    qDebug() << "clockButton: On passe au state hours" << microwave->configuration().contains(hoursState);
    qDebug() << "clockButton: On passe au state defrost" << microwave->configuration().contains(defrostState);

}

void MainWindow::on_defrostButton_clicked() {
    qDebug() << "defrostButton: On passe au state s1" << s1->active();
    qDebug() << "defrostButton: On passe au state stop" << stopState->active();
    qDebug() << "defrostButton: On passe au state hours" << hoursState->active();
    qDebug() << "defrostButton: On passe au state defrost" << defrostState->active();

//    if(hoursState->active()){
//        ui->lcdDisplay->display("true");
//    }
}

void MainWindow::changeDisplay(){
    qDebug() << "changeDisplay: On passe au state s1" << microwave->configuration().contains(s1);
    qDebug() << "changeDisplay: On passe au state stop" << microwave->configuration().contains(stopState);
    qDebug() << "changeDisplay: On passe au state hours" << microwave->configuration().contains(hoursState);
    qDebug() << "changeDisplay: On passe au state hours" << hoursState->active();
}

void MainWindow::showTime(){
    if( !(microwave->configuration().contains(hoursState) || microwave->configuration().contains(minutesState)) ) {
        QTime t = QTime::currentTime();
        currentTime= t.addSecs(offsetTime);
        ui->hoursLabel->setText(currentTime.toString("hh"));
        ui->minutesLabel->setText(currentTime.toString("mm"));
    }
}

void MainWindow::slide(int value){

    if(microwave->configuration().contains(hoursState)){
        QTime temp = QTime(value, ui->minutesLabel->text().toInt());
        ui->dial->setRange(0, 23);
        ui->hoursLabel->setText(temp.toString("hh"));
        offsetTime = currentTime.secsTo(temp);
    }

    if(microwave->configuration().contains(minutesState)){
        QTime temp = QTime(ui->hoursLabel->text().toInt(), value);
        ui->dial->setRange(0, 59);
        ui->minutesLabel->setText(temp.toString("mm"));
        qDebug() << temp.toString();
        offsetTime = currentTime.secsTo(temp);
    }

}

void MainWindow::saveTime(){
    currentTime = QTime::currentTime();
}
