#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "neuron.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <random>
#include <chrono>
#include <vector>

class Interface
{
public:
    [[nodiscard]] Interface();
    ~Interface();

    void run();

private:
    void update_model();
    void update_ui();

    SDL_Window *m_window {};
    SDL_Renderer *m_renderer {};
    Neuron m_neuron {};
    float m_input_current {};
    float m_input_probability {};
    std::default_random_engine m_rng;
    std::chrono::high_resolution_clock::time_point m_start_time;
    std::chrono::high_resolution_clock::time_point m_current_time;
    std::chrono::high_resolution_clock::time_point m_last_time;
    std::vector<float> m_time_data;
    std::vector<float> m_membrane_potential_data;
    std::vector<float> m_threshold_potential_data;
    std::vector<float> m_eta_data;
    std::vector<float> m_kappa_data;

    static constexpr float m_viewed_seconds {10.0f};
};

#endif // INTERFACE_HPP
