#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aicontroller.h"
#include "aitrainingset.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentCurveNr(0),
    currentExperimentNr(0),
    mainCurve(0),
    smoothCurve(0),
    secondDerivationCurve(0)
{
    ui->setupUi(this);
    setupGraphs();

    // init some data
    currentInputPath        = "";
    currentTrainingNr       = 100;
    currentTestingNr        = 50;
    currentValidationNr     = 50;
    currentEpochNr          = 20;
    currentGenerationsNr    = 2;
    currentPopulationNr     = 100;
    currentMutationNr       = 0.1;
    currentCrossoverNr      = 0.7;

    // setup controller
    controller = AiController(currentEpochNr, currentGenerationsNr, currentPopulationNr, currentMutationNr, currentCrossoverNr);

    // setup ui elements
    ui->trainingNr->setMaximum(1000);
    ui->trainingNr->setValue(currentTrainingNr);
    ui->trainingNr->setMinimum(1);

    ui->testingNr->setMaximum(1000);
    ui->testingNr->setValue(currentTestingNr);
    ui->testingNr->setMinimum(1);

    ui->validationNr->setMaximum(1000);
    ui->validationNr->setValue(currentValidationNr);
    ui->validationNr->setMinimum(1);

    ui->numEpochsSB->setMaximum(100);
    ui->numEpochsSB->setValue(currentEpochNr);
    ui->numEpochsSB->setMinimum(1);

    ui->numGenerationsSB->setMaximum(100);
    ui->numGenerationsSB->setValue(currentGenerationsNr);
    ui->numGenerationsSB->setMinimum(1);

    ui->numPopulationSB->setMaximum(1000);
    ui->numPopulationSB->setValue(currentPopulationNr);
    ui->numPopulationSB->setMinimum(2);

    ui->mutationSB->setMaximum(1.0);
    ui->mutationSB->setValue(currentMutationNr);
    ui->mutationSB->setMinimum(0.001);
    ui->mutationSB->setSingleStep(0.01);

    ui->crossoverSB->setMaximum(1.0);
    ui->crossoverSB->setValue(currentCrossoverNr);
    ui->crossoverSB->setMinimum(0.1);
    ui->crossoverSB->setSingleStep(0.01);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startTrainingButton_clicked()
{
    controller.setPlot(secondDerivationPlot);
    controller.runEpochs(trainingSet, testingSet, validationSet);
}

void MainWindow::on_loadDataButton_clicked()
{
    currentExperimentNr++;
    currentCurveNr=1;
    ui->experimentNr->setValue(currentExperimentNr);
    ui->experimentNr->repaint();
    ui->curveNr->setValue(currentCurveNr);
    ui->curveNr->repaint();
    plotData();
}

void MainWindow::on_experimentNr_valueChanged(int num)
{
    if (currentExperimentNr != num) {
        currentExperimentNr = num;
        currentCurveNr = 1;
        ui->curveNr->setValue(currentCurveNr);
        ui->curveNr->repaint();
        plotData();

        updateDataSets();
    }
}

void MainWindow::on_curveNr_valueChanged(int num)
{
    if (currentCurveNr != num) {
        currentCurveNr = num;
        plotData();
    }
}

void MainWindow::on_inputPathField_textChanged(QString inputPath)
{
    currentInputPath = inputPath.toStdString();
    updateDataSets();
}

void MainWindow::on_trainingNr_valueChanged(int num)
{
    currentTrainingNr = num;
    updateDataSets();
}

void MainWindow::on_testingNr_valueChanged(int num)
{
    currentTestingNr = num;
    updateDataSets();
}

void MainWindow::on_validationNr_valueChanged(int num)
{
    currentValidationNr = num;
    updateDataSets();
}

void MainWindow::on_numEpochsSB_valueChanged(int num)
{
    currentEpochNr = num;
    controller.updateSettings(currentEpochNr, currentGenerationsNr, currentPopulationNr, currentMutationNr, currentCrossoverNr);
}

void MainWindow::on_numGenerationsSB_valueChanged(int num)
{
    currentGenerationsNr = num;
    controller.updateSettings(currentEpochNr, currentGenerationsNr, currentPopulationNr, currentMutationNr, currentCrossoverNr);
}

void MainWindow::on_numPopulationSB_valueChanged(int num)
{
    currentPopulationNr = num;
    controller.updateSettings(currentEpochNr, currentGenerationsNr, currentPopulationNr, currentMutationNr, currentCrossoverNr);
    controller.resetGA();
}

void MainWindow::on_mutationSB_valueChanged(double num)
{
    currentMutationNr = num;
    controller.updateSettings(currentEpochNr, currentGenerationsNr, currentPopulationNr, currentMutationNr, currentCrossoverNr);
    controller.resetGA();
}

void MainWindow::on_crossoverSB_valueChanged(double num)
{
    currentCrossoverNr = num;
    controller.updateSettings(currentEpochNr, currentGenerationsNr, currentPopulationNr, currentMutationNr, currentCrossoverNr);
    controller.resetGA();
}


void MainWindow::updateDataSets()
{
    // setup training and testing data
    trainingSet   = new AiTrainingSet(currentInputPath, currentExperimentNr, 0, currentTrainingNr);
    testingSet    = new AiTrainingSet(currentInputPath, currentExperimentNr, currentTrainingNr, currentTestingNr);
    validationSet = new AiTrainingSet(currentInputPath, currentExperimentNr, currentTrainingNr + currentTestingNr, currentValidationNr);
}

void MainWindow::resetGA()
{
    controller.resetGA();
}

void MainWindow::plotData()
{
    DataSet_t *AFMData = loadData2();

    if (AFMData->size > 0) {
        int i;
        double deflection[AFMData->size];
        double z[AFMData->size];
        for (i=0; i<AFMData->size; i++) {
            deflection[i] = AFMData->x[i];
            z[i] = AFMData->raw[i];
        }

        // set MainCurve
        QwtPlotCurve *curve = getMainCurve();
        curve->setSamples(deflection, z, AFMData->size);

        QVector<double> *t = controller.getBestResult(AFMData);
        double results[t->size()];
        int pos = 0;
        for (int x=t->size()-1; x>=0; x--) {
            results[x] = t->at(pos++);
        }
        for(int x=AFMData->size-1; x>=t->size(); x--) {
            results[pos++] = 0;
        }

        // set SmoothCurve
        QwtPlotCurve *plot2 = getSmoothCurve();
        plot2->setSamples(deflection, results, AFMData->size);

    } else {
        // reset MainCurve
        getMainCurve()->setSamples(NULL, NULL, 0);

        // reset SmoothCurve
        getSmoothCurve()->setSamples(NULL, NULL, 0);
    }
}

DataSet_t *MainWindow::loadData2()
{
    using namespace std;

    ifstream afmDataFile;
    string line;
    int i=1, j=0;
    double a;
    istringstream strm;
    DataSet_t *afmData = new DataSet_t();

    stringstream fileNum;
    fileNum << currentInputPath << currentExperimentNr << "/AFMData_" << currentCurveNr << ".txt";
    string tmp = fileNum.str();
    const char *fileName = tmp.c_str();

    afmDataFile.open(fileName);
    if (afmDataFile.fail()) {
        cout << "Failed to open file: " << fileName << "\n";
        afmData->size = 0;
        return afmData;
    }

    while( getline(afmDataFile, line)){
        if (i > 52){
            strm.str(line);
            for (int k = 1; k<8; k++){
                strm >> a ;
                if (k == 1) { afmData->x.push_back(a); }
                if (k == 2) { afmData->raw.push_back(a); }
                if (k == 3) { afmData->derivative.push_back(a); }
                if (k == 4) { afmData->smooth_derivative.push_back(a); }
                if (k == 5) { afmData->second_derivative.push_back(a); }
                if (k == 7) { afmData->expected.push_back(a); }
                strm.clear();
            }
            j++;
        }
        i++ ;
    }
    afmData->size = j;

    return afmData;
}

void MainWindow::setupGraphs()
{
    mainPlot = new QwtPlot();
    mainPlot->setAxisTitle(QwtPlot::yLeft, "Force in pN");
    mainPlot->setAxisTitle(QwtPlot::xBottom, "z in nm");
    mainPlot->setCanvasBackground(QColor(Qt::white));
    mainPlot->setAutoReplot(true);
    ui->mainGraph->addWidget(mainPlot);

    smoothPlot = new QwtPlot();
    smoothPlot->setAxisTitle(QwtPlot::yLeft, "NN Output");
    smoothPlot->setAxisTitle(QwtPlot::xBottom, "z in nm");
    smoothPlot->setCanvasBackground(QColor(Qt::white));
    smoothPlot->setAutoReplot(true);
    ui->smoothGraph->addWidget(smoothPlot);

    secondDerivationPlot = new QwtPlot();
    secondDerivationPlot->setAxisTitle(QwtPlot::yLeft, "MSError");
    secondDerivationPlot->setAxisTitle(QwtPlot::xBottom, "Epoch");
    secondDerivationPlot->setCanvasBackground(QColor(Qt::white));
    secondDerivationPlot->setAutoReplot(true);
    ui->secondDerivation->addWidget(secondDerivationPlot);
}

QwtPlotCurve* MainWindow::getMainCurve()
{
    if (mainCurve == 0) {
        mainCurve = new QwtPlotCurve();
        mainCurve->setStyle(QwtPlotCurve::Lines);
        mainCurve->attach(mainPlot);
    }
    return mainCurve;
}

QwtPlotCurve* MainWindow::getSmoothCurve()
{
    if (smoothCurve == 0) {
        smoothCurve = new QwtPlotCurve();
        smoothCurve->setStyle(QwtPlotCurve::Lines);
        smoothCurve->attach(smoothPlot);
    }
    return smoothCurve;
}

QwtPlotCurve* MainWindow::getSecondDerivationCurve()
{
    if (secondDerivationCurve == 0) {
        QColor color = QColor(255, 0, 0);
        secondDerivationCurve = new QwtPlotCurve();
        secondDerivationCurve->setStyle(QwtPlotCurve::Lines);
        secondDerivationCurve->setPen(QPen(color));
        secondDerivationCurve->attach(secondDerivationPlot);
    }
    return secondDerivationCurve;
}
