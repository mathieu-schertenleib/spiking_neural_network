#ifndef NEURON_HPP
#define NEURON_HPP

#include <cmath>
#include <limits>

/*
 * The model and terminology used here are based on chapter 6.4 of the textbook
 * "Neuronal Dynamics": https://neuronaldynamics.epfl.ch/online/Ch6.S4.html
 *
 *  Terminology:
 *  - eta is the form of the neuron action potential and spike afterpotential
 *  - kappa is the impulse response of the membrane potential to an input
 *    post-synaptic current
 *  - epsilon is the impulse response of the membrane potential to an input
 *    pre-synaptic action potential
 *
 * Note that for a model where we are only interested in the spike timings, but
 * not the membrane potential time course, we could remove the spike from eta
 * (and hence save on computation) and only keep the afterpotential
 */

struct Complex_spike_response_model_neuron
{
    static constexpr float resting_membrane_potential {-0.5f};
    static constexpr float membrane_time_constant {1.0f};
    static constexpr float threshold_potential {0.5f};
    static constexpr std::size_t num_adaptation_variables {5};
    static constexpr float adaptation_time_constants[num_adaptation_variables] {
        0.02f, 0.05f, 0.2f, 0.6f, 1.0f};
    static constexpr float adaptation_amplitudes[num_adaptation_variables] {
        -8.0f, 10.0f, -2.8f, 2.0f, -1.2f};

    float membrane_potential {resting_membrane_potential};
    float eta_exponential_terms[num_adaptation_variables] {};
    float eta_term {};
    float kappa_exponential_term {};
    bool spiked {};

    constexpr void reset() noexcept
    {
        membrane_potential = resting_membrane_potential;
        for (auto &value : eta_exponential_terms)
            value = 0.0f;
        eta_term = 0.0f;
        kappa_exponential_term = 0.0f;
        spiked = false;
    }

    constexpr void input(float weighted_input) noexcept
    {
        kappa_exponential_term += weighted_input;
    }

    void update(float elapsed_time)
    {
        kappa_exponential_term *=
            std::exp(-elapsed_time / membrane_time_constant);

        eta_term = 0.0f;
        for (std::size_t i {}; i < num_adaptation_variables; ++i)
        {
            eta_exponential_terms[i] *=
                std::exp(-elapsed_time / adaptation_time_constants[i]);
            eta_term += eta_exponential_terms[i];
        }

        membrane_potential =
            eta_term + kappa_exponential_term + resting_membrane_potential;

        if (membrane_potential >= threshold_potential && !spiked)
        {
            spiked = true;
            for (std::size_t i {}; i < num_adaptation_variables; ++i)
            {
                eta_exponential_terms[i] += adaptation_amplitudes[i];
            }
        }
        else if (membrane_potential < threshold_potential && spiked)
        {
            spiked = false;
        }
    }
};

struct Simple_spike_response_model_neuron
{
    static constexpr float membrane_time_constant {1.0f};
    static constexpr float threshold_potential {0.5f};
    static constexpr float eta_time_constant {5.0f};
    static constexpr float eta_amplitude {0.4f};

    float membrane_potential {};
    float eta_exponential_term {};
    float kappa_exponential_term {};

    constexpr void reset() noexcept
    {
        membrane_potential = 0.0f;
        eta_exponential_term = 0.0f;
        kappa_exponential_term = 0.0f;
    }

    constexpr void input(float weighted_input) noexcept
    {
        kappa_exponential_term += weighted_input;
    }

    void update(float elapsed_time)
    {
        kappa_exponential_term *=
            std::exp(-elapsed_time / membrane_time_constant);
        eta_exponential_term *= std::exp(-elapsed_time / eta_time_constant);
        membrane_potential = eta_exponential_term + kappa_exponential_term;
        // NOTE: it is not strictly necessary to store membrane_potential if we
        // do not need it from the outside

        if (membrane_potential >= threshold_potential)
        {
            eta_exponential_term -= eta_amplitude;
        }
    }
};

struct Discrete_time_neuron
{
    static constexpr float membrane_time_constant {1.0f};
    static constexpr float threshold_potential {0.5f};
    static constexpr float eta_time_constant {5.0f};
    static constexpr float eta_amplitude {0.4f};
    static inline const float kappa_constant {
        std::exp(-1.0f / membrane_time_constant)};
    static inline const float eta_constant {
        std::exp(-1.0f / eta_time_constant)};

    float eta_exponential_term {};
    float kappa_exponential_term {};

    constexpr void reset() noexcept
    {
        eta_exponential_term = 0.0f;
        kappa_exponential_term = 0.0f;
    }

    constexpr void input(float weighted_input) noexcept
    {
        kappa_exponential_term += weighted_input;
    }

    void update() noexcept
    {
        kappa_exponential_term *= kappa_constant;
        eta_exponential_term *= eta_constant;
        const auto membrane_potential =
            eta_exponential_term + kappa_exponential_term;

        if (membrane_potential >= threshold_potential)
        {
            eta_exponential_term -= eta_amplitude;
        }
    }
};

#endif // NEURON_HPP
