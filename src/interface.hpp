#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "neuron.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <chrono>
#include <random>
#include <vector>

//#define SINGLE_NEURON

class Interface
{
public:
    [[nodiscard]] Interface();
    ~Interface();

    void run();

private:
    void update_ui();

    SDL_Window *m_window {};
    SDL_Renderer *m_renderer {};
#ifdef SINGLE_NEURON
    Neuron m_neuron {};
#else
    static constexpr std::size_t num_neurons {10};
    Network m_network {num_neurons};
#endif
    float m_input_current {};
    float m_input_probability {};
    std::default_random_engine m_rng {std::random_device {}()};
    std::vector<float> m_time_data;
#ifdef SINGLE_NEURON
    // std::vector<float> m_kappa_data;
    // std::vector<float> m_eta_data;
    std::vector<float> m_membrane_potential_data;
#else
    std::vector<float> m_membrane_potential_data[num_neurons];
    std::vector<float> m_synapse_data[num_neurons * num_neurons];
#endif

    static constexpr float m_viewed_seconds {10.0f};
};

#endif // INTERFACE_HPP
