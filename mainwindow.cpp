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
    QObject::connect(s1, &QState::entered, [=]{this->ui->mwDisplay->setText("");});


    // stop state
    stopState = new QState();
    microwave->addState(stopState);
    stopState->addTransition(ui->stopButton, SIGNAL(clicked()), s1);

    // initializing the other states
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

    QObject::connect(hoursState, SIGNAL(entered()), this, SLOT(saveTime()));
    QObject::connect(hoursState, &QState::entered, [=]{this->ui->mwDisplay->setText("Set the hours");});
    QObject::connect(minutesState, &QState::entered, [=]{this->ui->mwDisplay->setText("Set the minutes");});


    // power
    s1->addTransition(ui->powerButton, SIGNAL(clicked()), powerState);
    powerState->addTransition(ui->powerButton, SIGNAL(clicked()), durationState);
    durationState->addTransition(ui->powerButton, SIGNAL(clicked()), s1);

    QObject::connect(powerState, SIGNAL(entered()), this, SLOT(resetLabels()));
    QObject::connect(powerState, &QState::entered, [=]{this->ui->mwDisplay->setText("Set the power");});

    // mode
    s1->addTransition(ui->modeButton, SIGNAL(clicked()), modeState);
    modeState->addTransition(ui->modeButton, SIGNAL(clicked()), durationState);
    durationState->addTransition(ui->modeButton, SIGNAL(clicked()), s1);

    // duration power + mode
    durationState->addTransition(ui->startButton, SIGNAL(clicked()), cookingState);

    QObject::connect(durationState, SIGNAL(entered()), this, SLOT(resetLabels()));
    QObject::connect(durationState, &QState::entered, [=]{this->ui->mwDisplay->setText("Set the cooking duration");});

    // defrost
    s1->addTransition(ui->defrostButton, SIGNAL(clicked()), defrostState);
    defrostState->addTransition(ui->startButton, SIGNAL(clicked()), s1);



//    QObject::connect(s1, SIGNAL(entered()), this, SLOT(changeDisplay()));

//    addTrans(s1, hoursState, ui->clockButton, SIGNAL(clicked()), this, SLOT(changeDisplay()));
//    QObject::connect(s1, SIGNAL(entered()), this, SLOT(changeDisplay()));

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

}

void MainWindow::on_defrostButton_clicked() {

}

void MainWindow::changeDisplay(){
    qDebug() << "changeDisplay: On passe au state s1" << microwave->configuration().contains(s1);
    qDebug() << "changeDisplay: On passe au state stop" << microwave->configuration().contains(stopState);
    qDebug() << "changeDisplay: On passe au state hours" << microwave->configuration().contains(hoursState);
    qDebug() << "changeDisplay: On passe au state hours" << hoursState->active();
}

void MainWindow::showTime(){

    // checks that we are not in clock mode
    if( !(microwave->configuration().contains(hoursState) || microwave->configuration().contains(minutesState)) ) {
        QTime t = QTime::currentTime();
        currentTime= t.addSecs(offsetTime);
    }

    // updates the field only if in idle state
    if(microwave->configuration().contains(s1)){
        QString doubleD = ":";
        if((currentTime.second() % 2) == 0)
            doubleD = "";
        ui->doubleDot->setText(doubleD);
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
        offsetTime = currentTime.secsTo(temp);
    }

    if(microwave->configuration().contains(powerState)){
        ui->dial->setRange(0, 100);
        ui->minutesLabel->setText(QString::number(value));
        power=value;
    }

    if(microwave->configuration().contains(durationState)){
        ui->dial->setRange(0, (30*60)); // for the total number of seconds up to 59 minutes and 59 seconds
        QTime t0 = QTime(0, 0, 0);
        int stepDuration = (value / 30) * 30;

        QTime t1 = t0.addSecs(stepDuration);
        ui->hoursLabel->setText(t1.toString("mm"));
        ui->minutesLabel->setText(t1.toString("ss"));
        cookingDuration=stepDuration;
    }

}

void MainWindow::saveTime(){
    currentTime = QTime::currentTime();
}

void MainWindow::resetLabels(){
    ui->hoursLabel->setText("");

    if(microwave->configuration().contains(powerState)){
        ui->doubleDot->setText("");
        ui->minutesLabel->setText(QString::number(power));
    }

    if(microwave->configuration().contains(durationState)){
        ui->doubleDot->setText(":");
        QTime t0 = QTime(0, 0, 0);
        QTime t1 = t0.addSecs(cookingDuration);
        ui->hoursLabel->setText(t1.toString("mm"));
        ui->minutesLabel->setText(t1.toString("ss"));
    }

}
