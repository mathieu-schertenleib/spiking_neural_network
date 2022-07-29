#include "neuron.hpp"

#include <iostream>
#include <random>

Network::Network(std::size_t num_neurons)
{
    neurons.resize(num_neurons);
    synapses.resize(num_neurons * num_neurons);
    spiking_neurons.resize(num_neurons);

    randomize_synapses();
}

void Network::randomize_synapses()
{
    assert(synapses.size() == neurons.size() * neurons.size());

    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_real_distribution<float> d(-1.0f, 1.0f);
    for (std::size_t i {}; i < neurons.size(); ++i)
    {
        for (std::size_t j {}; j < neurons.size(); ++j)
        {
            if (i == j)
            {
                synapses[i * neurons.size() + j] = 0.0f;
            }
            else
            {
                synapses[i * neurons.size() + j] = d(e);
            }
        }
    }
}

void Network::update(const std::vector<float> &input)
{
    assert(synapses.size() == neurons.size() * neurons.size());
    assert(input.size() == neurons.size());

    // Accumulate inputs
    std::vector<float> neuron_inputs(neurons.size());
    for (std::size_t i {}; i < neurons.size(); ++i)
    {
        neuron_inputs[i] = input[i];
        for (const auto spiking_neuron : spiking_neurons)
        {
            neuron_inputs[i] += synapses[spiking_neuron * neurons.size() + i];
        }
    }

    // Update neurons
    spiking_neurons.clear();
    for (std::size_t i {}; i < neurons.size(); ++i)
    {
        if (neurons[i].input(time, neuron_inputs[i]))
        {
            spiking_neurons.push_back(i);
        }
    }

    // Update synapses
    for (const auto spiking_neuron : spiking_neurons)
    {
        for (std::size_t i {}; i < neurons.size(); ++i)
        {
            if (spiking_neuron == i)
            {
                continue;
            }

            const auto spike_delta_time = time - neurons[i].last_spike_time;
            const float characteristic_delta_time {1.0f};
            const float characteristic_delta_weight {0.2f};
            const auto delta_weight =
                2.0f * characteristic_delta_weight * spike_delta_time /
                characteristic_delta_time /
                (1.0f +
                 spike_delta_time * spike_delta_time /
                     (characteristic_delta_time * characteristic_delta_time));
            synapses[i * neurons.size() + spiking_neuron] += delta_weight;
            synapses[spiking_neuron * neurons.size() + i] -= delta_weight;
        }
    }

    ++time;
}
