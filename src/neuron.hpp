#ifndef NEURON_HPP
#define NEURON_HPP

#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <vector>

/*
 * The model and terminology used here are based on chapter 6.4 of the textbook
 * "Neuronal Dynamics": https://neuronaldynamics.epfl.ch/online/Ch6.S4.html
 *
 * Terminology:
 *
 * - eta is the form of the neuron action potential and spike afterpotential
 * - kappa is the impulse response of the membrane potential to an input
 *   post-synaptic current
 * - epsilon is the impulse response of the membrane potential to an input
 *   pre-synaptic action potential
 *
 * Note that in our model we are only interested in the spike timings, but not
 * the membrane potential time course. We can therefore remove the spike from
 * eta (and hence save on computation) and only keep the afterpotential.
 *
 * Since our goal is to determine whether a simple spiking neuron model can
 * exhibit interesting behaviour, we should start with the simplest model
 * possible and see if we get any interesting result.
 */

struct Neuron
{
    static constexpr float threshold {1.0f};
    static constexpr float membrane_time_constant {60.0f};
    static constexpr float reset_potential {-0.5f};

    float last_membrane_potential;
    float last_input_time;
    float last_spike_time;

    // Returns true if the neuron spiked
    [[nodiscard]] bool input(float time, float input_voltage)
    {
        const auto time_since_last_input = time - last_input_time;
        last_membrane_potential =
            last_membrane_potential *
                std::exp(-time_since_last_input / membrane_time_constant) +
            input_voltage;
        last_input_time = time;

        if (last_membrane_potential >= threshold)
        {
            last_membrane_potential = reset_potential;
            last_spike_time = time;
            return true;
        }

        return false;
    }
};

struct Network
{
    [[nodiscard]] explicit Network(std::size_t num_neurons);

    void randomize_synapses();
    void update(const std::vector<float> &input);

    std::vector<Neuron> neurons;
    std::vector<float> synapses;
    std::vector<std::size_t> spiking_neurons;
    float time {};
};

#endif // NEURON_HPP
