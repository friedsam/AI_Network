#include <iostream>

#include "aineuralnet.h"


AiNeuron::AiNeuron()
{
}

AiNeuron::AiNeuron(int aNumInputs) :
    numInputs(aNumInputs + 1)
{
    for (int i=0; i<numInputs; i++) {
        weights.append(0.0);
    }
}

AiNeuronLayer::AiNeuronLayer()
{
}

AiNeuronLayer::AiNeuronLayer(int aNumNeurons, int numInputsPerNeuron) :
    numNeurons(aNumNeurons)
{
    for (int i=0; i<numNeurons; i++) {
        neurons.append(AiNeuron(numInputsPerNeuron));
    }
}

AiNeuralNet::AiNeuralNet(int aNumInputs, int aNumOutputs, int aNumHiddenLayers, int aNumNeuronsPerHiddenLayer, double aBias, double aActivationTreshold) :
    numInputs(aNumInputs),
    numOutputs(aNumOutputs),
    numHiddenLayers(aNumHiddenLayers),
    numNeuronsPerHiddenLayer(aNumNeuronsPerHiddenLayer),
    bias(aBias),
    activationTreshold(aActivationTreshold)
{
    create();
}

void AiNeuralNet::create()
{
    if (numHiddenLayers > 0) {
        layers.append(AiNeuronLayer(numNeuronsPerHiddenLayer, numInputs));

        for (int i=0; i<numHiddenLayers-1; i++) {
            layers.append(AiNeuronLayer(numNeuronsPerHiddenLayer, numNeuronsPerHiddenLayer));
        }

        layers.append(AiNeuronLayer(numOutputs, numNeuronsPerHiddenLayer));
    } else {
        layers.append(AiNeuronLayer(numOutputs, numInputs));
    }
}

QVector<double> AiNeuralNet::weights() const
{
    QVector<double> weights;

    // each layer
    for (int i=0; i<numHiddenLayers + 1; i++) {
        // each neuron
        for (int j=0; j<layers.at(i).numNeurons; j++) {
            // each weight
            for (int k=0; k<layers.at(i).neurons.at(j).numInputs; k++) {
                weights.append(layers.at(i).neurons.at(j).weights.at(k));
            }
        }
    }

    return weights;
}

void AiNeuralNet::setWeights(QVector<double> &newWeights)
{
    int pos = 0;

    // each layer
    for (int i=0; i<numHiddenLayers + 1; i++) {
        // each neuron
        for (int j=0; j<layers.at(i).numNeurons; j++) {
            // each weight
            for (int k=0; k<layers.at(i).neurons.at(j).numInputs; k++) {
                layers[i].neurons[j].weights[k] = newWeights.at(pos);
                pos++;
            }
        }
    }
}

int AiNeuralNet::numWeights() const
{
    int weights = 0;

    // each layer
    for (int i=0; i<numHiddenLayers + 1; i++) {
        // each neuron
        for (int j=0; j<layers.at(i).numNeurons; j++) {
            // each weight
            for (int k=0; k<layers.at(i).neurons.at(j).numInputs; k++) {
                weights++;
            }
        }
    }

    return weights;
}

QVector<double> AiNeuralNet::evaluate(QVector<double> &inputs)
{
    QVector<double> outputs;

    int pos = 0;

    // each layer
    for (int i=0; i<numHiddenLayers + 1; i++) {

        if (i > 0) {
            inputs = outputs;
        }
        outputs.clear();

        pos = 0;

        // each neuron
        for (int j=0; j<layers.at(i).numNeurons; j++) {

            double netinput = 0;

            int currentInputs = layers.at(i).neurons.at(j).numInputs;

            // each weight
            for (int k=0; k<currentInputs - 1; k++) {
                netinput += layers.at(i).neurons.at(j).weights.at(k) * inputs.at(pos);
                pos++;
            }

            netinput += layers.at(i).neurons.at(j).weights.at(currentInputs - 1) * bias;

            outputs.append(Sigmoid(netinput, activationTreshold));

            pos = 0;
        }
    }

    return outputs;
}

double AiNeuralNet::Sigmoid(double netinput, double response)
{
    return ( 1 / ( 1 + exp(-netinput / response)));
}
