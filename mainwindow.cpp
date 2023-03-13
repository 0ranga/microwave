#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLCDNumber>
#include <QString>
#include <QTimer>
#include <QRandomGenerator>
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

    // Idle to cooking
    addTrans(s1, cookingState, ui->startButton, SIGNAL(clicked()), this, SLOT(setDefaultTimeNPower()));


    // clock
    s1->addTransition(ui->clockButton, SIGNAL(clicked()), hoursState);
    hoursState->addTransition(ui->clockButton, SIGNAL(clicked()), minutesState);
//    minutesState->addTransition(ui->clockButton, SIGNAL(clicked()), s1);
    addTrans(minutesState, s1, ui->clockButton, SIGNAL(clicked()), this, SLOT(setOffsetTime()));

    QObject::connect(hoursState, SIGNAL(entered()), this, SLOT(saveTime()));
    QObject::connect(hoursState, &QState::entered, [=]{this->ui->mwDisplay->setText("Set the hours");});
    QObject::connect(minutesState, &QState::entered, [=]{this->ui->mwDisplay->setText("Set the minutes");});


    // power
    s1->addTransition(ui->powerButton, SIGNAL(clicked()), powerState);
//    powerState->addTransition(ui->powerButton, SIGNAL(clicked()), durationState);
//    durationState->addTransition(ui->powerButton, SIGNAL(clicked()), s1);
    addTrans(powerState, durationState, ui->powerButton, SIGNAL(clicked()), this, SLOT(setPower()));
    addTrans(durationState, s1, ui->powerButton, SIGNAL(clicked()), this, SLOT(setCookingDuration()));

    QObject::connect(powerState, SIGNAL(entered()), this, SLOT(resetLabels()));
    QObject::connect(powerState, &QState::entered, [=]{this->ui->mwDisplay->setText("Set the power");});

    // mode
    s1->addTransition(ui->modeButton, SIGNAL(clicked()), modeState);
//    modeState->addTransition(ui->modeButton, SIGNAL(clicked()), durationState);
//    durationState->addTransition(ui->modeButton, SIGNAL(clicked()), s1);
    addTrans(modeState, durationState, ui->modeButton, SIGNAL(clicked()), this, SLOT(setCurrentMode()));
    addTrans(durationState, s1, ui->modeButton, SIGNAL(clicked()), this, SLOT(setCookingDuration()));

    QObject::connect(modeState, SIGNAL(entered()), this, SLOT(resetLabels()));
    QObject::connect(modeState, &QState::entered, [=]{this->ui->mwDisplay->setText("Select the mode");});

    // duration power + mode
//    durationState->addTransition(ui->startButton, SIGNAL(clicked()), cookingState);
    addTrans(durationState, cookingState, ui->startButton, SIGNAL(clicked()), this, SLOT(setCookingDuration()));

    QObject::connect(durationState, SIGNAL(entered()), this, SLOT(resetLabels()));
    QObject::connect(cookingState, SIGNAL(entered()), this, SLOT(startCooking()));
    QObject::connect(durationState, &QState::entered, [=]{this->ui->mwDisplay->setText("Set the cooking duration");});
    QObject::connect(cookingState, &QState::entered, [=]{
        QString stringCooking = "Cooking with a power of " + QString::number(power) + "...";
        this->ui->mwDisplay->setText(stringCooking);
    });

    // defrost
    s1->addTransition(ui->defrostButton, SIGNAL(clicked()), defrostState);
    defrostState->addTransition(ui->defrostButton, SIGNAL(clicked()), s1);
    defrostState->addTransition(ui->startButton, SIGNAL(clicked()), cookingState);

    QObject::connect(defrostState, SIGNAL(entered()), this, SLOT(resetLabels()));
    QObject::connect(defrostState, &QState::entered, [=]{this->ui->mwDisplay->setText("Defrost mode");});


    // start the state machine
    microwave->setInitialState(s1);
    microwave->start();




    // Timers
    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start();


    // Dial
    QObject::connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT(slide(int)));

    // Stop Button
    QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopButtonClicked()));

    // Start Button
    QObject::connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startButtonClicked()));

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
        ui->dial->setRange(0, 23);

        if(value >= 0 && value <= 23){
            QTime temp = QTime(value, ui->minutesLabel->text().toInt());


            ui->hoursLabel->setText(temp.toString("hh"));
//            offsetTime = currentTime.secsTo(temp);
//            newOffsetTime = currentTime.secsTo(temp);
        }

    }

    if(microwave->configuration().contains(minutesState)){
        ui->dial->setRange(0, 59);

        if(value >= 0 && value <= 59){
            QTime temp = QTime(ui->hoursLabel->text().toInt(), value);

            ui->minutesLabel->setText(temp.toString("mm"));
//            offsetTime = currentTime.secsTo(temp);
            newOffsetTime = currentTime.secsTo(temp);

        }

    }

    if(microwave->configuration().contains(powerState)){
        ui->dial->setRange(0, 100);

        if (value >= 0 && value <= 100){
            ui->minutesLabel->setText(QString::number(value));
//            power=value;
            newPower=value;
        }

    }

    if(microwave->configuration().contains(durationState)){
        ui->dial->setRange(0, (30*60)); // for the total number of seconds up to 59 minutes and 59 seconds

        if(value >= 0 && value <= (30*60)){
            QTime t0 = QTime(0, 0, 0);
            int stepDuration = (value / 30) * 30;

            QTime t1 = t0.addSecs(stepDuration);
            ui->hoursLabel->setText(t1.toString("mm"));
            ui->minutesLabel->setText(t1.toString("ss"));
//            cookingDuration=stepDuration;
            newCookingDuration=stepDuration;
        }

    }

    if(microwave->configuration().contains(modeState)){
        ui->dial->setRange(0, 2);

        if(value >= 0 && value <= 2){
            ui->minutesLabel->setText(modes[value]);
//            currentMode=value;
            newCurrentMode=value;
        }

    }

}

void MainWindow::saveTime(){
    currentTime = QTime::currentTime();
    ui->dial->setRange(0, 23);
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

    if(microwave->configuration().contains(modeState)){

        ui->doubleDot->setText("");
        ui->minutesLabel->setText(modes[currentMode]);
    }

    if(microwave->configuration().contains(defrostState)){
        ui->doubleDot->setText("");
        ui->minutesLabel->setText("Calculating...");

        int stepDuration = ( QRandomGenerator::global()->bounded(30*60) / 30) * 30;

        QTime t0 = QTime(0, 0, 0);
        QTime t1 = t0.addSecs(stepDuration);

        // Timer to simulate calculation
        defrostTimer = new QTimer(this);
        defrostTimer->setSingleShot(true);

        QObject::connect(defrostTimer, &QTimer::timeout, [=]{
            this->ui->hoursLabel->setText(t1.toString("mm"));
            this->ui->doubleDot->setText(":");
            this->ui->minutesLabel->setText(t1.toString("ss"));
            cookingDuration=stepDuration;
        });
//        QObject::connect(defrostTimer, &QTimer::timeout, [=]{this->ui->doubleDot->setText(":");});
//        QObject::connect(defrostTimer, &QTimer::timeout, [=]{this->ui->minutesLabel->setText(t1.toString("ss"));});

        defrostTimer->start(2000);

//        cookingDuration=stepDuration;
    }

}

void MainWindow::startCooking(){

    seconds = new QTimer(this);


    cookingTimer = new QTimer(this);
    cookingTimer->setSingleShot(true);

    QTime t0 = QTime(0, 0, 0);
    QTime t1 = t0.addSecs(cookingDuration);

    QString doubleD = ":";
    if((t1.second() % 2) == 0)
        doubleD = "";
    ui->doubleDot->setText(doubleD);

    ui->hoursLabel->setText(t1.toString("mm"));
    ui->minutesLabel->setText(t1.toString("ss"));


    seconds->start(1000);
    cookingTimer->start(1000*cookingDuration);

    QObject::connect(seconds, &QTimer::timeout, [=]{

        cookingDuration = cookingDuration - 1;

        QTime t0 = QTime(0, 0, 0);
        QTime t1 = t0.addSecs(cookingDuration);

        QString doubleD = ":";
        if((t1.second() % 2) == 0)
            doubleD = "";
        ui->doubleDot->setText(doubleD);



        ui->hoursLabel->setText(t1.toString("mm"));
        ui->minutesLabel->setText(t1.toString("ss"));

    });

    QObject::connect(cookingTimer, &QTimer::timeout, [=]{
        seconds->stop();
        cookingDuration = 60;
    });
    cookingState->addTransition(cookingTimer, SIGNAL(timeout()), s1);

}

void MainWindow::setOffsetTime(){
    offsetTime = newOffsetTime;
}

void MainWindow::setPower(){
    power = newPower;
}

void MainWindow::setCookingDuration(){
    cookingDuration = newCookingDuration;
}

void MainWindow::setCurrentMode(){
    currentMode = newCurrentMode;
}

void MainWindow::setDefaultTimeNPower(){
    cookingDuration = 60;
    power = 100;
}

void MainWindow::stopButtonClicked(){

    if(microwave->configuration().contains(cookingState)){
        cookingTimer->stop();
        seconds->stop();
        cookingDuration = 60;
    }

    if(microwave->configuration().contains(defrostState)){
        defrostTimer->stop();
    }

}

void MainWindow::startButtonClicked(){

    if(microwave->configuration().contains(cookingState)){
        cookingDuration = cookingDuration + 60;
        QTime t0 = QTime(0, 0, 0);
        QTime t1 = t0.addSecs(cookingDuration);

        QString doubleD = ":";
        if((t1.second() % 2) == 0)
            doubleD = "";
        ui->doubleDot->setText(doubleD);

        ui->hoursLabel->setText(t1.toString("mm"));
        ui->minutesLabel->setText(t1.toString("ss"));

    }

}
