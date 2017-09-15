#include <QVector>
#include <cmath>
#include <iostream>

#include "aievolution.h"
#include "aigenome.h"

// AiEvolution
//      implements a simple Genetic Algorithm which will provide the weights for
//      our Neural Network
//
AiEvolution::AiEvolution(int aPopulationSize, double aMutationRate, double aCrossoverRate, int aChromosomeSize, double aMaxPerturbation, int aNumElite) :
    populationSize(aPopulationSize),
    mutationRate(aMutationRate),
    crossoverRate(aCrossoverRate),
    chromosomeSize(aChromosomeSize),
    maxPerturbation(aMaxPerturbation),
    numElite(aNumElite)
{
    population = QVector<AiGenome>();
    population.clear();

    for (int i=0; i<populationSize; i++) {
        population.append(AiGenome(chromosomeSize));
    }
}

// mutate:
//      mutates given chromosome based on mutationRate
//
void AiEvolution::mutate(QVector<double> &chromosome)
{
    for (int i=0; i<chromosomeSize; i++) {
        if (randomFloat() < mutationRate) {
            chromosome[i] += (randomValue() * maxPerturbation);
        }
    }
}

// getRandomGenome:
//      returns a randomly selected Genome from the population
//
AiGenome AiEvolution::getRandomGenome()
{
    int selected = (int)(randomFloat() * (populationSize - 1));

    return population.at(selected);
}

// crossover:
//      takes a mother, father and creates new offspring
//
void AiEvolution::crossover(QVector<double> &mother,
                         QVector<double> &father,
                         QVector<double> &child1,
                         QVector<double> &child2)
{
    if (randomFloat() > crossoverRate || mother == father) {
        child1 = mother;
        child2 = father;

        return;
    }

    int crossoverPoint = randomInt(0, chromosomeSize - 1);

    int i;
    child1.clear();
    child2.clear();

    for (i=0; i<crossoverPoint; i++) {
        child1.append(mother.at(i));
        child2.append(father.at(2));
    }

    for (i=crossoverPoint; i<chromosomeSize; i++) {
        child1.append(father.at(i));
        child2.append(mother.at(i));
    }

    return;
}

// evolve:
//      builds new population from old one by keeping numElite members of the old
//      population and filling up the rest with offsprings of random selected
//      members of the old population
//
void AiEvolution::evolve()
{
    // stort population so we have fittest members at the beginning
    std::sort(population.begin(), population.end());
    currentBest = AiGenome();
    currentBest.chromosome = population.at(0).chromosome;
    currentBest.fitness    = population.at(0).fitness;

    QVector<AiGenome> newPopulation;

    // keep numElite best genomes
    for (int i=0; i<numElite; i++) {
        population[i].fitness = 0;
        newPopulation.append(population[i]);
    }

    // fill up the new population
    while (newPopulation.size() < populationSize) {
        AiGenome mother = getRandomGenome();
        AiGenome father = getRandomGenome();

        QVector<double> child1, child2;

        // crossover mother and father
        crossover(mother.chromosome, father.chromosome, child1, child2);

        // mutatle child chromosomes
        mutate(child1);
        mutate(child2);

        // add new genome to population
        newPopulation.append(AiGenome(child1));
        newPopulation.append(AiGenome(child2));
    }

    // make the new population the new population
    population = newPopulation;
}


// randomValue:
//      returns a random value between -1 and 1
//
double AiEvolution::randomValue()
{
    return randomFloat() - randomFloat();
}

// randomFloat:
//      returns a random value between 0 and 1
//
double AiEvolution::randomFloat()
{
    return ((rand() % 1000)/1000.0);
}

// randomInt:
//      int x:  range start
//      int y:  range stop
//
//      returns a random int value between x and y
//
int AiEvolution::randomInt(int x, int y)
{
    return rand() % (y-x+1) + x;
}
