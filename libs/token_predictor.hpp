#pragma once
#include <vector>

struct Neuron
{
    double bias
    std::vector<double> inputWeights;
    std::vector<double> outputWeights;
};

// make an "openNetwork" function in file_handler which reads a file containing all weights and connections