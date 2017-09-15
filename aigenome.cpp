#include <QVector>
#include <cmath>
#include <iostream>

#include "aigenome.h"


AiGenome::AiGenome() :
    chromosomeSize(0),
    fitness(0)
{
}

AiGenome::AiGenome(int aChromosomeSize) :
    chromosomeSize(aChromosomeSize),
    fitness(0)
{
    chromosome = QVector<double>(chromosomeSize);
    chromosome.clear();

    for (int i=0; i<chromosomeSize; i++) {
        chromosome.append(randomValue());
    }
}

AiGenome::AiGenome(QVector<double> aChromosome) :
    chromosomeSize(aChromosome.size()),
    fitness(0)
{
    chromosome = aChromosome;
}

// private methods

// randomValue: returns random value between -1 and 1
double AiGenome::randomValue()
{
    return ((rand() % 1000)/1000.0) - ((rand() % 1000)/1000.0);
}
