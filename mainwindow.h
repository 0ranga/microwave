#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStateMachine>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // state Machine
    QStateMachine * microwave;

    // Idle
    QState *s1;

    // stop
    QState *stopState;

    //other state
    QState *cookingState;
    QState *hoursState;
    QState *minutesState;
    QState *powerState;
    QState *durationState;
    QState *modeState;
    QState *defrostState;

    // microwave variables
    QTime currentTime;
    int offsetTime{};
    int newOffsetTime{};

    int power = 100;
    int newPower = 100;

    int cookingDuration = 60;
    int newCookingDuration = 60;

    QString modes[3] = {"Microwave", "Grill", "Microwave + Grill"};
    int currentMode = 0;
    int newCurrentMode;

    QTimer *seconds;
    QTimer *cookingTimer;

    QTimer *defrostTimer;


private slots:
    void showTime();
    void slide(int value);
    void saveTime();
    void resetLabels();
    void startCooking();

    void setOffsetTime();
    void setPower();
    void setCookingDuration();
    void setCurrentMode();
    void setDefaultTimeNPower();

    void stopButtonClicked();
    void startButtonClicked();
    void changeDisplay();


};
#endif // MAINWINDOW_H
