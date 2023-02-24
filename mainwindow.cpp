#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStateMachine>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    auto * microwave = new QStateMachine( );

    QState *s1 = new QState( );
    microwave->addState(s1);

    QState *stopState = new QState( );
    microwave->addState(stopState);

    QState *cookingState = new QState(stopState);
    QState *hoursState = new QState(stopState);
    QState *minutesState = new QState(stopState);
    QState *powerState = new QState(stopState);
    QState *durationState = new QState(stopState);
    QState *modeState = new QState(stopState);
    QState *defrostState = new QState(stopState);

    // clock
    s1->addTransition(ui->clockButton, SIGNAL(clicked( )), hoursState);
    hoursState->addTransition(ui->clockButton, SIGNAL(clicked( )), minutesState);
    minutesState->addTransition(ui->clockButton, SIGNAL(clicked( )), s1);

    // power
    s1->addTransition(ui->powerButton, SIGNAL(clicked( )), powerState);
    powerState->addTransition(ui->powerButton, SIGNAL(clicked( )), durationState);
    durationState->addTransition(ui->powerButton, SIGNAL(clicked( )), s1);

    // mode
    s1->addTransition(ui->modeButton, SIGNAL(clicked( )), modeState);
    modeState->addTransition(ui->modeButton, SIGNAL(clicked( )), durationState);
    durationState->addTransition(ui->modeButton, SIGNAL(clicked( )), s1);

    // power + mode
    durationState->addTransition(ui->startButton, SIGNAL(clicked( )), cookingState);

    // defrost
    s1->addTransition(ui->defrostButton, SIGNAL(clicked( )), defrostState);
    defrostState->addTransition(ui->startButton, SIGNAL(clicked( )), s1);

//    microwave->addState(cookingState);
//    microwave->addState(hoursState);
//    microwave->addState(minutesState);
//    microwave->addState(powerState);
//    microwave->addState(durationState);
//    microwave->addState(modeState);
//    microwave->addState(defrostState);

    microwave->setInitialState(s1);
    microwave->start();

    delete ui;
}

void MainWindow::on_clockButton_clicked() {
    qDebug() << "On passe au state Hours";
}
