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
    int cookingTime;
    QTime currentTime;
    int offsetTime{};


private slots:
    void showTime();
    void slide(int value);
    void saveTime();

    void on_clockButton_clicked();
    void on_defrostButton_clicked();
    void changeDisplay();


};
#endif // MAINWINDOW_H
