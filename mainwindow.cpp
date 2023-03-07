#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStateMachine>
#include <QLCDNumber>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto * microwave = new QStateMachine();


    // Idle
    QState *s1 = new QState();
    microwave->addState(s1);


    // stop
    QState *stopState = new QState();
    microwave->addState(stopState);
    stopState->addTransition(ui->stopButton, SIGNAL(clicked()), s1);

    QState *cookingState = new QState(stopState);
    QState *hoursState = new QState(stopState);
    QState *minutesState = new QState(stopState);
    QState *powerState = new QState(stopState);
    QState *durationState = new QState(stopState);
    QState *modeState = new QState(stopState);
    QState *defrostState = new QState(stopState);

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

    QObject::connect(s1, SIGNAL(entered()), this, SLOT(changeDisplay()));
    QObject::connect(stopState, SIGNAL(entered()), this, SLOT(changeDisplay()));
    QObject::connect(hoursState, SIGNAL(entered()), this, SLOT(changeDisplay()));
    QObject::connect(minutesState, SIGNAL(entered()), this, SLOT(changeDisplay()));
    QObject::connect(powerState, SIGNAL(entered()), this, SLOT(changeDisplay()));
    QObject::connect(durationState, SIGNAL(entered()), this, SLOT(changeDisplay()));
    QObject::connect(modeState, SIGNAL(entered()), this, SLOT(changeDisplay()));
    QObject::connect(cookingState, SIGNAL(entered()), this, SLOT(changeDisplay()));
    QObject::connect(defrostState, SIGNAL(entered()), this, SLOT(changeDisplay()));
    QObject::connect(hoursState, SIGNAL(entered()), this, SLOT(changeDisplay()));


    microwave->setInitialState(s1);
    microwave->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_clockButton_clicked() {
    qDebug() << "On passe au state Hours";
}

void MainWindow::changeDisplay(){
    ui->lcdDisplay->display("14:05");
}
