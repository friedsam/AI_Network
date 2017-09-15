#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include "aicontroller.h"

struct data{
    std::vector<double> time;
    std::vector<double> Defl;
    std::vector<double> z;
    int size;
};
typedef data data_t;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    data_t loadData();
    DataSet_t *loadData2();
    void plotData();

private:
    // all the GUI stuff
    Ui::MainWindow *ui;
    QwtPlot *mainPlot;
    QwtPlot *smoothPlot;
    QwtPlot *secondDerivationPlot;
    QwtPlotCurve *mainCurve;
    QwtPlotCurve *smoothCurve;
    QwtPlotCurve *secondDerivationCurve;

    // all the GUI parameters
    std::string currentInputPath;
    int currentTrainingNr;
    int currentTestingNr;
    int currentValidationNr;
    int currentEpochNr;
    int currentGenerationsNr;
    int currentPopulationNr;
    double currentMutationNr;
    double currentCrossoverNr;

    int currentCurveNr;
    int currentExperimentNr;

    // all the backend processing stuff
    AiController controller;
    AiTrainingSet *trainingSet;
    AiTrainingSet *testingSet;
    AiTrainingSet *validationSet;
    void setupGraphs();
    QwtPlotCurve *getMainCurve();
    QwtPlotCurve *getSmoothCurve();
    QwtPlotCurve *getSecondDerivationCurve();

    void updateDataSets();
    void resetGA();

private Q_SLOTS:
    // listeners for GUI events
    void on_experimentNr_valueChanged(int);
    void on_curveNr_valueChanged(int);
    void on_inputPathField_textChanged(QString);
    void on_trainingNr_valueChanged(int);
    void on_testingNr_valueChanged(int);
    void on_validationNr_valueChanged(int);

    void on_numEpochsSB_valueChanged(int);
    void on_numGenerationsSB_valueChanged(int);
    void on_numPopulationSB_valueChanged(int);

    void on_mutationSB_valueChanged(double);
    void on_crossoverSB_valueChanged(double);

    void on_loadDataButton_clicked();
    void on_startTrainingButton_clicked();
};

#endif // MAINWINDOW_H
