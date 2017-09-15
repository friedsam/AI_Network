#include <iostream>

#include "aicontroller.h"
#include "aigenome.h"
#include "aineuralnet.h"
#include "mainwindow.h"
#include "aitrainingset.h"

AiController::AiController()
{
    // nothing here
}

AiController::AiController(int currentEpochNr, int currentGenerationsNr, int currentPopulationNr, double currentMutationNr, double currentCrossoverNr) :
    nnInputs(4 * 5),            // 4 curves with window size of 5 points
    populationSize(currentPopulationNr),
    mutationRate(currentMutationNr),
    crossoverRate(currentCrossoverNr),
    maxPerturbation(0.3),
    numElite(10),
    numGenerations(currentGenerationsNr),
    numEpochs(currentEpochNr),
    treshold(0.8)
{
    initPopulation();
    quiet = false;

    // set performance curves to NULL
    trainingCurve   = NULL;
    testingCurve    = NULL;
    validationCurve = NULL;

    trainingPerformance   = QVector<QPointF>();
    testingPerformance    = QVector<QPointF>();
    validationPerformance = QVector<QPointF>();

    percentError = 0;
    trainingPercentError   = QVector<double>();
    testingPercentError    = QVector<double>();
    validationPercentError = QVector<double>();

    percentCorrect = 0;
    trainingPercentCorrect   = QVector<double>();
    testingPercentCorrect    = QVector<double>();
    validationPercentCorrect = QVector<double>();
}

// private methods
void AiController::initPopulation()
{
    ga = new AiEvolution(populationSize,
                         mutationRate,
                         crossoverRate,
                         nnInputs,
                         maxPerturbation,
                         numElite);
}

void AiController::setPlot(QwtPlot *aPlot)
{
    plot = aPlot;
}

void AiController::runEpochs(AiTrainingSet *trainingSet, AiTrainingSet *testingSet, AiTrainingSet *validationSet)
{
    int nnOutputs = 1;
    int numHiddenLayers = 0;
    int numNeuronsPerHiddenLayer = 0;
    double bias = -1;
    double activationTreshold = 1;

    updatePerformancePlot();

    DataSet_t *data;

    for (int epoch=0; epoch<numEpochs; epoch++) {
        std::cout << "\n\n==============================================================\n";
        std::cout << "A new age has started... we are counting Epoch No. " << epoch << "\n";

        double bestEpochFitness = -99999999;

        for (int generation=0; generation<numGenerations; generation++) {
//            std::cout << "Generation #" << generation << " is about to be born...\n";
            ga->evolve();

            // iterate over the whole population
            for (int genome=0; genome<populationSize; genome++) {
                // create a new neural net with chromosome of genome X
                AiNeuralNet *net = new AiNeuralNet(nnInputs, nnOutputs, numHiddenLayers, numNeuronsPerHiddenLayer, bias, activationTreshold);
                net->setWeights(ga->population[genome].chromosome);

                trainingSet->reset();
                while ((data = trainingSet->next()) != NULL) {

                    double numOfCorrectPeaks   = 0;
                    double numOfCorrectNoPeaks = 0;
                    double numOfFalsePeaks     = 0;
                    double numOfFalseNoPeaks   = 0;

                    int start, end, pos;

                    end   = data->x.size() - 1;
                    start = end - 5;
                    pos   = 0;

                    while (start >= 0) {
                        QVector<double> inputs = QVector<double>(nnInputs);

                        pos = 0;
                        for (int j=0; j<5; j++) {
                            inputs[pos + 0 ] = data->raw[start + j];
                            inputs[pos + 5 ] = data->derivative[start + j];
                            inputs[pos + 10] = data->smooth_derivative[start + j];
                            inputs[pos + 15] = data->second_derivative[start + j];
                            pos++;
                        }

                        QVector<double> outputs = net->evaluate(inputs);

                        if (outputs.first() > treshold) {       // predicted peak
                            if (data->expected[start] == 1) {   // true positive
                                numOfCorrectPeaks++;
                            } else {                            // false positive
                                numOfFalsePeaks++;
                            }
                        } else {                                // predicted no peak
                            if (data->expected[start] == 0) {   // true negative
                                numOfCorrectNoPeaks++;
                            } else {                            // false negative
                                numOfFalseNoPeaks++;
                            }
                        }

                        end--;
                        start--;
                    }

                    // calculate fitness
                    ga->population[genome].fitness += (numOfCorrectPeaks / data->numOfPeaks) + (numOfCorrectNoPeaks / data->numOfNoPeaks) -
                            (numOfFalsePeaks / data->numOfPeaks) - (numOfFalseNoPeaks / data->numOfNoPeaks);
                    if (numOfFalseNoPeaks == data->numOfPeaks) {
                        ga->population[genome].fitness += -1000.0;
                    }

                }

                if (ga->population[genome].fitness > bestEpochFitness) {
                    bestEpochFitness = ga->population[genome].fitness;
                }
//                std::cout << "Genome #" << genome << " has finished training.\n";
//                std::cout << "\tResulting Fitness: " << ga->population[genome].fitness << "\n";
//                std::cout.flush();
            }
        }

        std::cout << "Best Fitness in Epoch: " << bestEpochFitness << "\n";
        std::cout.flush();

        // start testing this Epoch
        quiet = true;
        startTesting(trainingSet, testingSet, validationSet, epoch);
        quiet = false;

    }

    // output stats
    std::cout << "============ Testing ================\n";
    std::cout << "== Epoch === % Error === % Correct ==\n";
    for (int n=0; n<testingPercentError.size(); n++) {
        std::cout << n << "\t" << testingPercentError.at(n) << "\t" << testingPercentCorrect.at(n) << "\n";
    }
    std::cout << "\n\n";

    std::cout << "============ Training ===============\n";
    std::cout << "== Epoch === % Error === % Correct ==\n";
    for (int n=0; n<trainingPercentError.size(); n++) {
        std::cout << n << "\t" << trainingPercentError.at(n) << "\t" << trainingPercentCorrect.at(n) << "\n";
    }
    std::cout << "\n\n";

    std::cout << "============ Validation =============\n";
    std::cout << "== Epoch === % Error === % Correct ==\n";
    for (int n=0; n<validationPercentError.size(); n++) {
        std::cout << n << "\t" << validationPercentError.at(n) << "\t" << validationPercentCorrect.at(n) << "\n";
    }
    std::cout << "\n\n";
    std::cout.flush();

}

void AiController::startTesting(AiTrainingSet *trainingSet, AiTrainingSet *testingSet, AiTrainingSet *validationSet, int epoch)
{
    double error;
    epoch = trainingPerformance.size();

    // add error to performance curve for testing
    trainingSet->reset();
    error = getMSError(trainingSet);
    trainingPerformance.push_back(QPointF(epoch, error));
    trainingPercentError.push_back(percentError);
    trainingPercentCorrect.push_back(percentCorrect);

    // add error to performance curve for testing
    testingSet->reset();
    error = getMSError(testingSet);
    testingPerformance.push_back(QPointF(epoch, error));
    testingPercentError.push_back(percentError);
    testingPercentCorrect.push_back(percentCorrect);

    // add error to performance curve for testing
    validationSet->reset();
    error = getMSError(validationSet);
    validationPerformance.push_back(QPointF(epoch, error));
    validationPercentError.push_back(percentError);
    validationPercentCorrect.push_back(percentCorrect);

    // update performance plot
    std::cout << "Supposed to update performance plot MSE " << error << "\n";
    std::cout.flush();
    updatePerformancePlot();
}

double AiController::getMSError(AiTrainingSet *dataset)
{
    double error;
    double sumOfErrors = 0.0;
    int totalPoints = 0;

    DataSet_t *data;
    QVector<double> *results;

    double numOfCorrectPeaks   = 0;
    double numOfCorrectNoPeaks = 0;
    double numOfFalsePeaks     = 0;
    double numOfFalseNoPeaks   = 0;

    while ((data = dataset->next()) != NULL) {
        // get results
        results = getBestResult(data);

        // calculate error
        for (int i=0; i<results->size(); i++) {
            error = data->expected[i] - results->at(i);
            sumOfErrors += sqrt(error * error);

            if (results->at(i) > treshold) {        // predicted peak
                if (data->expected[i] == 1) {       // true positive
                    numOfCorrectPeaks++;
                } else {                            // false positive
                    numOfFalsePeaks++;
                }
            } else {                                // predicted no peak
                if (data->expected[i] == 0) {   // true negative
                    numOfCorrectNoPeaks++;
                } else {                            // false negative
                    numOfFalseNoPeaks++;
                }
            }
        }

        // update total points
        totalPoints += data->size;
    }

    // divide sumOfErrors by total number of points
    error = sumOfErrors / totalPoints;

    percentError   = (numOfFalsePeaks + numOfFalseNoPeaks) / totalPoints;
    percentCorrect = (numOfCorrectPeaks + numOfCorrectNoPeaks) / totalPoints;

    return error;
}

void AiController::updatePerformancePlot()
{
    if (trainingCurve == NULL) {
        QColor black = QColor(0,0,0);
        trainingCurve = new QwtPlotCurve();
        trainingCurve->setStyle(QwtPlotCurve::Lines);
        trainingCurve->setPen(QPen(black));
        trainingCurve->attach(plot);
    }
    if (testingCurve == NULL) {
        QColor green = QColor(0,255,0);
        testingCurve = new QwtPlotCurve();
        testingCurve->setStyle(QwtPlotCurve::Lines);
        testingCurve->setPen(QPen(green));
        testingCurve->attach(plot);
    }
    if (validationCurve == NULL) {
        QColor red = QColor(255,0,0);
        validationCurve = new QwtPlotCurve();
        validationCurve->setStyle(QwtPlotCurve::Lines);
        validationCurve->setPen(QPen(red));
        validationCurve->attach(plot);
    }

    trainingCurve->setSamples(trainingPerformance);
    testingCurve->setSamples(testingPerformance);
    validationCurve->setSamples(validationPerformance);
}

QVector<double> *AiController::getBestResult(DataSet_t *data)
{
    int nnOutputs = 1;
    int numHiddenLayers = 0;
    int numNeuronsPerHiddenLayer = 0;
    double bias = -1;
    double activationTreshold = 1;

    double numOfNoPeaks = 0;
    double numOfPeaks   = 0;
    for (int i=0; i<data->size; i++) {
        if (data->expected[i] == 0) {
            numOfNoPeaks++;
        } else {
            numOfPeaks++;
        }
    }
    if (numOfNoPeaks == 0) numOfNoPeaks = 1;
    if (numOfPeaks == 0) numOfPeaks = 1;

    QVector<double> *result = new QVector<double>();

    int start, end, pos;

    AiNeuralNet *net = new AiNeuralNet(nnInputs, nnOutputs, numHiddenLayers, numNeuronsPerHiddenLayer, bias, activationTreshold);
    net->setWeights(ga->currentBest.chromosome);

    if (!quiet)
        std::cout << "Using Genome with fitness: " << ga->currentBest.fitness << " for prediction\n";

    end   = data->x.size() - 1;
    start = end - 5;
    pos   = 0;

    double numOfCorrectPeaks   = 0;
    double numOfCorrectNoPeaks = 0;
    double numOfFalsePeaks     = 0;
    double numOfFalseNoPeaks   = 0;

    while (start >= 0) {
        QVector<double> inputs = QVector<double>(nnInputs);

        pos = 0;
        for (int j=0; j<5; j++) {
            inputs[pos + 0 ] = data->raw[start + j];
            inputs[pos + 5 ] = data->derivative[start + j];
            inputs[pos + 10] = data->smooth_derivative[start + j];
            inputs[pos + 15] = data->second_derivative[start + j];
            pos++;
        }

        QVector<double> outputs = net->evaluate(inputs);

        if (outputs.first() > treshold) {            // predicted peak
            if (data->expected[start] == 1) {   // true positive
                numOfCorrectPeaks++;
                if (!quiet)
                    std::cout << "Correct Peak at: " << data->x[start] << "\n";
            } else {                            // false positive
                numOfFalsePeaks++;
                if (!quiet)
                    std::cout << "False Peak at:   " << data->x[start] << "\n";
            }
        } else {                                // predicted no peak
            if (data->expected[start] == 0) {   // true negative
                numOfCorrectNoPeaks++;
            } else {                            // false negative
                numOfFalseNoPeaks++;
            }
        }

        result->push_back(outputs[0]);

        end--;
        start--;
    }

    if (!quiet) {
        std::cout << "==========================================\n";
        std::cout << "NumOfPeaks:          " << numOfPeaks << "\n";
        std::cout << "NumOfNoPeaks:        " << numOfNoPeaks << "\n";
        std::cout << "NumOfCorrectPeaks:   " << numOfCorrectPeaks << "\n";
        std::cout << "NumOfCorrectNoPeaks: " << numOfCorrectNoPeaks << "\n";
        std::cout << "NumOfFalsePeaks:     " << numOfFalsePeaks << "\n";
        std::cout << "NumOfFalseNoPeaks:   " << numOfFalseNoPeaks << "\n";

        std::cout << "Fitness: " << ga->currentBest.fitness << "\n";
    }

    double theoreticalFitness = -999999999;
    theoreticalFitness = (numOfCorrectPeaks / numOfPeaks) + (numOfCorrectNoPeaks / numOfNoPeaks) -
            (numOfFalsePeaks / numOfPeaks) - (numOfFalseNoPeaks / numOfNoPeaks);
    if (numOfFalseNoPeaks == numOfPeaks) {
        theoreticalFitness = -1000.0;
    }

    if (!quiet) {
        std::cout << "Theoretical Fitness: " << theoreticalFitness << "\n";
        std::cout.flush();
    }

    return result;
}

void AiController::evaluate(DataSet_t *data)
{
    int nnOutputs = 1;
    int numHiddenLayers = 0;
    int numNeuronsPerHiddenLayer = 0;
    double bias = -1;
    double activationTreshold = 1;

    double numOfNoPeaks = 0;
    double numOfPeaks   = 0;
    for (int i=0; i<data->size; i++) {
        if (data->expected[i] == 0) {
            numOfNoPeaks++;
        } else {
            numOfPeaks++;
        }
    }
    if (numOfNoPeaks == 0) numOfNoPeaks = 1;
    if (numOfPeaks == 0) numOfPeaks = 1;

    int g;
    for (g=0; g<numGenerations; g++) {
        std::cout << "Generation #" << g << "\n";
        ga->evolve();

        for (int i=0; i<populationSize; i++) {

            double numOfCorrectPeaks   = 0;
            double numOfCorrectNoPeaks = 0;
            double numOfFalsePeaks     = 0;
            double numOfFalseNoPeaks   = 0;

            AiNeuralNet *net = new AiNeuralNet(nnInputs, nnOutputs, numHiddenLayers, numNeuronsPerHiddenLayer, bias, activationTreshold);
            net->setWeights(ga->population[i].chromosome);

            int start, end, pos;

            end   = data->x.size() - 1;
            start = end - 5;
            pos   = 0;

            while (start >= 0) {
                QVector<double> inputs = QVector<double>(nnInputs);

                pos = 0;
                for (int j=0; j<5; j++) {
                    inputs[pos + 0 ] = data->raw[start + j];
                    inputs[pos + 5 ] = data->derivative[start + j];
                    inputs[pos + 10] = data->smooth_derivative[start + j];
                    inputs[pos + 15] = data->second_derivative[start + j];
                    pos++;
                }

                QVector<double> outputs = net->evaluate(inputs);

                if (outputs.first() > treshold) {            // predicted peak
                    if (data->expected[start] == 1) {   // true positive
                        numOfCorrectPeaks++;
                    } else {                            // false positive
                        numOfFalsePeaks++;
                    }
                } else {                                // predicted no peak
                    if (data->expected[start] == 0) {   // true negative
                        numOfCorrectNoPeaks++;
                    } else {                            // false negative
                        numOfFalseNoPeaks++;
                    }
                }

                end--;
                start--;
            }

//            std::cout << "==========================================\n";
//            std::cout << "NumOfPeaks:        " << numOfPeaks << "\n";
//            std::cout << "NumOfNoPeaks:      " << numOfNoPeaks << "\n";
//            std::cout << "NumOfCorrectPeaks: " << numOfCorrectPeaks << "\n";
//            std::cout << "NumOfCorrectNoPeaks: " << numOfCorrectNoPeaks << "\n";
//            std::cout << "NumOfFalsePeaks:   " << numOfFalsePeaks << "\n";
//            std::cout << "NumOfFalseNoPeaks: " << numOfFalseNoPeaks << "\n";


            // calculate fitness
            ga->population[i].fitness = (numOfCorrectPeaks / numOfPeaks) + (numOfCorrectNoPeaks / numOfNoPeaks) -
                    (numOfFalsePeaks / numOfPeaks) - (numOfFalseNoPeaks / numOfNoPeaks);
            if (numOfFalseNoPeaks == numOfPeaks) {
                ga->population[i].fitness = -1000.0;
            }

//            std::cout << "Fitness: " << ga->population[i].fitness << "\n\n";
        }
    }
    std::cout << "Generation #" << g << "\n";
    //ga->evolve();

}

void AiController::updateSettings(int currentEpochNr, int currentGenerationsNr, int currentPopulationNr, double currentMutationNr, double currentCrossoverNr)
{
    populationSize  = currentPopulationNr;
    mutationRate    = currentMutationNr;
    crossoverRate   = currentCrossoverNr;
    numGenerations  = currentGenerationsNr;
    numEpochs       = currentEpochNr;
}

void AiController::resetGA()
{
    initPopulation();
}
