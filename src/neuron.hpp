#ifndef NEURON_HPP
#define NEURON_HPP

#include <cmath>
#include <limits>

/*
 *  The model and terminology used here are based on chapter 6.4 of the textbook
 * "Neuronal Dynamics": https://neuronaldynamics.epfl.ch/online/Ch6.S4.html
 *
 *  Terminology:
 *  - eta is the form of the neuron action potential and spike afterpotential
 *  - kappa is the impulse response of the membrane potential to an input
 *    post-synaptic current
 *  - epsilon is the impulse response of the membrane potential to an input
 *    pre-synaptic action potential
 */

struct Neuron
{
    float membrane_potential {};
    float eta_exponential_term {};
    float kappa_exponential_term {};
    float threshold_potential {};
    float threshold_exponential_term {};

    static constexpr float resting_membrane_potential {-0.4f};
    static constexpr float membrane_time_constant {1.0f};
    static constexpr float reset_potential {-0.7f};
    static constexpr float refractory_time_constant {5.0f};
    static constexpr float resting_threshold_potential {1.0f};
    static constexpr float threshold_potential_increase {0.3f};
    static constexpr float threshold_time_constant {5.0f};

    constexpr void input(float weighted_input) noexcept
    {
        kappa_exponential_term += weighted_input;
    }

    void update(float elapsed_time)
    {
        kappa_exponential_term *=
            std::exp(-elapsed_time / membrane_time_constant);
        eta_exponential_term *=
            std::exp(-elapsed_time / refractory_time_constant);
        membrane_potential = eta_exponential_term + kappa_exponential_term +
                             resting_membrane_potential;

        threshold_exponential_term *=
            std::exp(-elapsed_time / threshold_time_constant);
        threshold_potential =
            threshold_exponential_term + resting_threshold_potential;

        if (membrane_potential >= threshold_potential)
        {
            eta_exponential_term -= threshold_potential - reset_potential;
            threshold_exponential_term += threshold_potential_increase;
        }
    }
};

struct Leaky_integrate_and_fire_neuron
{
    float membrane_potential {};
    float time_since_last_spike {std::numeric_limits<float>::max()};

    static inline constexpr float membrane_time_constant {1.0f};
    static inline constexpr float membrane_resistance {1.0f};
    static inline constexpr float threshold_potential {1.0f};
    static inline constexpr float refractory_period {1.0f};

    constexpr void input(float input_current) noexcept
    {
        if (time_since_last_spike > refractory_period)
        {
            membrane_potential += membrane_resistance * input_current;
        }
    }

    void update(float elapsed_time)
    {
        if (membrane_potential >= threshold_potential)
        {
            membrane_potential = 0.0f;
            time_since_last_spike = 0.0f;
        }
        else
        {
            membrane_potential *=
                std::exp(-elapsed_time / membrane_time_constant);
            time_since_last_spike += elapsed_time;
        }
    }
};

#endif // NEURON_HPP
