#ifndef NEURON_HPP
#define NEURON_HPP

#include <cmath>

struct Neuron
{
    float membrane_potential {};
    float time_since_last_spike {default_time_since_last_spike};

    static inline constexpr float threshold_potential {1.0f};
    static inline constexpr float reset_potential {-0.5f};
    static inline constexpr float refractory_period {1.0f};
    static inline constexpr float default_time_since_last_spike {-1.0f};
    static inline constexpr float time_constant {1.0f};
    static inline constexpr float delta_time {0.02f};
    static inline const float decay_constant {
        std::exp(-delta_time / time_constant)};

    // V(t) = V(t0) * exp(-(t - t0) / tau)
    // V(t + dt) = exp(-dt / tau) * V(t)

    constexpr void add(float input) noexcept
    {
        if (time_since_last_spike == default_time_since_last_spike)
        {
            membrane_potential += input;
        }
    }

    constexpr void update() noexcept
    {
        if (membrane_potential >= threshold_potential)
        {
            membrane_potential = reset_potential;
            time_since_last_spike = 0;
        }
        else
        {
            membrane_potential *= decay_constant;

            if (time_since_last_spike != default_time_since_last_spike)
            {
                time_since_last_spike += delta_time;
            }

            if (time_since_last_spike >= refractory_period)
            {
                time_since_last_spike = default_time_since_last_spike;
            }
        }
    }
};

#endif // NEURON_HPP
