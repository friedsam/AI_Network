#ifndef AIGENOME_H
#define AIGENOME_H

#include <QObject>
#include <QVector>
#include <iostream>

class AiGenome
{
public:
    AiGenome();
    AiGenome(int);
    AiGenome(QVector<double>);

    QVector<double> chromosome;         // our chromosome (weights for Neural net)
    int chromosomeSize;                 // size of our chromosome
    double fitness;                     // fitness for genome

    friend bool operator <(const AiGenome &lhs, const AiGenome &rhs)
    {
        return lhs.fitness > rhs.fitness;
    }

private:

    double randomValue();
};

#endif // AIGENOME_H
