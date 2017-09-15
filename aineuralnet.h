#ifndef AINEURALNET_H
#define AINEURALNET_H

#include <QVector>
#include <cmath>

struct AiNeuron
{
    int numInputs;
    QVector<double> weights;

    AiNeuron();
    AiNeuron(int);
};

struct AiNeuronLayer
{
    int numNeurons;
    QVector<AiNeuron> neurons;

    AiNeuronLayer();
    AiNeuronLayer(int, int);
};

class AiNeuralNet
{
public:
    AiNeuralNet(int, int, int, int, double, double);

    int numWeights() const;
    QVector<double> weights() const;
    QVector<double> evaluate(QVector<double> &);

    void create();
    void setWeights(QVector<double> &);

    inline double Sigmoid(double, double);

private:
    int numInputs;                      // holds the number of inputs
    int numOutputs;                     // holds the number of outputs YES/NO currently
    int numHiddenLayers;                // number of hidden layers
    int numNeuronsPerHiddenLayer;       // number of neurons per hidden layer
    double bias;
    double activationTreshold;

    QVector<AiNeuronLayer> layers;
};

#endif // AINEURALNET_H
