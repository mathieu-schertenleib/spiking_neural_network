#ifndef NEURON_HPP
#define NEURON_HPP

#include <cmath>
#include <limits>

struct Leaky_integrate_and_fire_neuron
{
    // tau_m * dV_m(t)/dt = R_m * I(t) - V_m(t)

    float membrane_potential {};
    float elapsed_time_since_last_spike {std::numeric_limits<float>::max()};

    static inline constexpr float membrane_time_constant {1.0f};
    static inline constexpr float membrane_resistance {1.0f};
    static inline constexpr float threshold_potential {1.0f};
    static inline constexpr float refractory_period {1.0f};

    constexpr void input(float input_current) noexcept
    {
        if (elapsed_time_since_last_spike > refractory_period)
        {
            membrane_potential += membrane_resistance * input_current;
        }
    }

    void update(float elapsed_time)
    {
        if (membrane_potential >= threshold_potential)
        {
            membrane_potential = 0.0f;
            elapsed_time_since_last_spike = 0.0f;
        }
        else
        {
            membrane_potential *=
                std::exp(-elapsed_time / membrane_time_constant);
            elapsed_time_since_last_spike += elapsed_time;
        }
    }
};

#endif // NEURON_HPP
