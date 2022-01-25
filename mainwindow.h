#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gclib.h"
#include "gclibo.h"
#include "gclib_errors.h"
#include "gclib_record.h"
#include "progwindow.h"
#include "outputwindow.h"
#include "powdersetupwidget.h"

class Printer;
class PrintThread;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();
    void setup(Printer *printerPtr, PrintThread *printerThread);

private slots:
    void on_yPositive_clicked();
    void on_xPositive_clicked();
    void on_yNegative_clicked();
    void on_xNegative_clicked();
    void on_xHome_clicked();
    void on_yHome_clicked();
    void on_zStepSize_valueChanged(int arg1);
    void on_zMax_clicked();
    void on_zUp_clicked();
    void on_zDown_clicked();
    void on_zMin_clicked();
    void on_activateHopper_stateChanged(int arg1);
    void on_connect_clicked();
    void on_OpenProgramWindow_clicked();
    void on_saveDefault_clicked();
    void on_revertDefault_clicked();
    void on_spreadNewLayer_clicked();
    void on_activateRoller1_toggled(bool checked);
    void on_activateRoller2_toggled(bool checked);
    void on_activateJet_stateChanged(int arg1);
    void allow_user_input(bool allowed);
    void thread_ended();
    void disable_user_input();
    void connected_to_motion_controller();

    void on_removeBuildBox_clicked();

    void on_actionShow_Hide_Console_triggered();

private:
    Ui::MainWindow *ui;
    int mZPosition{100};
    int mDeltaX{0};
    int mDeltaY{0};
    int mDeltaZ{0};

    Printer *printer{nullptr};
    PrintThread *mPrinterThread{nullptr};
    progWindow *sWindow{nullptr};
    QDockWidget *mDockWidget{nullptr};
    OutputWindow *mOutputWindow{nullptr};
    PowderSetupWidget *mPowderSetupWidget{nullptr};
    void e(GReturn rc);
};
#endif // MAINWINDOW_H
