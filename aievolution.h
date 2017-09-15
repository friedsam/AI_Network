#ifndef AIEVOLUTION_H
#define AIEVOLUTION_H

#include <QObject>

#include "aigenome.h"

class AiEvolution : public QObject
{
public:
    AiEvolution(int, double, double, int, double, int);

    void evolve();
    QVector<AiGenome> population;
    AiGenome currentBest;

private:
    int populationSize;             // size of population
    double mutationRate;            // mutation rate when creating new population
    double crossoverRate;           // crossover rate when creating new population
    int chromosomeSize;             // size of our chromosome must be the same as number of weights needed for neural net
    double maxPerturbation;         // max changes to chromosome when mutating
    int numElite;                   // how many members we transfer into new population without modification

    void mutate(QVector<double> &);
    void crossover(QVector<double> &, QVector<double> &, QVector<double> &, QVector<double> &);
    AiGenome getRandomGenome();

    double randomValue();
    double randomFloat();
    int randomInt(int, int);
};

#endif // AIEVOLUTION_H
