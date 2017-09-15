#ifndef AITRAININGSET_H
#define AITRAININGSET_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

struct dataSet {
    std::vector<double> x;
    std::vector<double> raw;
    std::vector<double> derivative;
    std::vector<double> smooth_derivative;
    std::vector<double> second_derivative;
    std::vector<int> expected;
    int size;
    int numOfPeaks;
    int numOfNoPeaks;
};
typedef dataSet DataSet_t;

class AiTrainingSet
{
public:
    AiTrainingSet(std::string aInputPath, int aExperimentNr, int aStartCurveNr, int trainingSetSize);

    DataSet_t *next();
    void reset();

protected:
    std::string inputPath;              // holds the path from where we load our data
    int currentExperimentNr;            // holds the experiment number we are currently looking at
    int startCurveNr;                   // curve number to start to data set
    int currentCurveNr;                 // current curve number we loaded last
    int size;                           // size of the dataset

    std::vector<DataSet_t> trainingData;
};

#endif // AITRAININGSET_H
