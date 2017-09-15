#include "aitrainingset.h"


AiTrainingSet::AiTrainingSet(std::string aInputPath, int aExperimentNr, int aStartCurveNr, int trainingSetSize) :
    inputPath(aInputPath),
    currentExperimentNr(aExperimentNr),
    startCurveNr(aStartCurveNr),
    size(trainingSetSize)
{
    reset();
}

void AiTrainingSet::reset()
{
    currentCurveNr = startCurveNr;
}

DataSet_t *AiTrainingSet::next()
{
    using namespace std;

    // increment curveNr
    currentCurveNr++;

    // stop if we went through enough training data
    if (currentCurveNr - startCurveNr > size)
        return NULL;

    DataSet_t *afmData;

    if (trainingData.size() <= size) {
        ifstream afmDataFile;
        string line;
        int i=1, j=0;
        double a;
        istringstream strm;
        afmData = new DataSet_t();

        stringstream fileNum;
        fileNum << inputPath << currentExperimentNr << "/AFMData_" << currentCurveNr << ".txt";
        string tmp = fileNum.str();
        const char *fileName = tmp.c_str();

        afmDataFile.open(fileName);
        if (afmDataFile.fail()) {
            cout << "Failed to open file: " << fileName << "\n";
            return NULL;
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
                if (afmData->expected[j] == 0) {
                    afmData->numOfNoPeaks++;
                } else {
                    afmData->numOfPeaks++;
                }
                j++;
            }
            i++ ;
        }
        afmData->size = j;
        if (afmData->numOfNoPeaks == 0) afmData->numOfNoPeaks = 1;
        if (afmData->numOfPeaks   == 0) afmData->numOfPeaks   = 1;

        trainingData.push_back(*afmData);
    } else {
        afmData = &trainingData[currentCurveNr-startCurveNr-1];
    }
    return afmData;
}
