#ifndef AICONTROLLER_H
#define AICONTROLLER_H

#include <QObject>
#include <QVector>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include "aievolution.h"
#include "aitrainingset.h"

class AiController
{
public:
    AiController();
    AiController(int currentEpochNr, int currentGenerationsNr, int currentPopulationNr, double currentMutationNr, double currentCrossoverNr);

    void setPlot(QwtPlot *);
    void runEpochs(AiTrainingSet *trainingSet, AiTrainingSet *testingSet, AiTrainingSet *validationSet);
    void evaluate(DataSet_t *);
    void resetGA();
    void updateSettings(int currentEpochNr, int currentGenerationsNr, int currentPopulationNr, double currentMutationNr, double currentCrossoverNr);
    QVector<double> *getBestResult(DataSet_t *);

private:
    QVector<AiGenome> population;               // storgae for the current population

    AiEvolution *ga;                            // pointer to our GA
    AiGenome currentBest;                       // best genome we have right now
    QwtPlot *plot;                              // plot we are going to draw in
    QVector<QPointF> trainingPerformance;       // keeps track of our current performance
    QVector<QPointF> testingPerformance;
    QVector<QPointF> validationPerformance;
    QwtPlotCurve *trainingCurve;                // plotted in plot when done with running through epochs
    QwtPlotCurve *testingCurve;
    QwtPlotCurve *validationCurve;

    double percentError;                        // used to pass error along
    QVector<double> trainingPercentError;       // keeps track of our current error
    QVector<double> testingPercentError;
    QVector<double> validationPercentError;

    double percentCorrect;                      // used to pass correct classification percentage along
    QVector<double> trainingPercentCorrect;     // keeps track of our current success rate
    QVector<double> testingPercentCorrect;
    QVector<double> validationPercentCorrect;

    int nnInputs;                               // number of inputs for Neural Network
    int populationSize;                         // size of population to train
    double mutationRate;                        // mutation rate for genome
    double crossoverRate;                       // crossover rate between genomes
    double maxPerturbation;
    int numElite;                               // defines how many members of a population we transfer into new population
    int numGenerations;                         // defines how many generations we run before one epoch ends
    int numEpochs;                              // defines how many epochs we run for one training run
    double treshold;                            // threshold at which we consider our neural net output to be ON
    bool quiet;                                 // silences some output when testing

    void initPopulation();
    void startTesting(AiTrainingSet *trainingSet, AiTrainingSet *testingSet, AiTrainingSet *validationSet, int);
    double getMSError(AiTrainingSet *dataset);
    void updatePerformancePlot();
};

#endif // AICONTROLLER_H
