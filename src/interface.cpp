#include "interface.hpp"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "implot.h"

#include <chrono>
#include <iostream>
#include <stdexcept>

Interface::Interface()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    m_window = SDL_CreateWindow("SNN",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                640,
                                480,
                                SDL_WINDOW_RESIZABLE);
    if (!m_window)
    {
        SDL_Quit();
        throw std::runtime_error(SDL_GetError());
    }

    m_renderer = SDL_CreateRenderer(
        m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer)
    {
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        throw std::runtime_error(SDL_GetError());
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer_Init(m_renderer);

    /*ImFontConfig font_config {};
    font_config.SizePixels = 32.0f;
    ImGui::GetIO().Fonts->AddFontDefault(&font_config);*/

    ImPlot::CreateContext();
}

Interface::~Interface()
{
    ImPlot::DestroyContext();

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Interface::run()
{
    const auto start_time = std::chrono::high_resolution_clock::now();

    while (true)
    {
        SDL_Event event {};
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_QUIT: return;
            default: break;
            }
        }

        const auto now = std::chrono::high_resolution_clock::now();
        const auto total_time =
            std::chrono::duration<float> {now - start_time}.count();

        std::bernoulli_distribution d(static_cast<double>(m_input_probability));
#ifdef SINGLE_NEURON
        m_neuron.input(total_time, d(m_rng) ? m_input_current : 0.0f);
#else
        std::vector<float> inputs(num_neurons);
        inputs[0] = d(m_rng) ? m_input_current : 0.0f;
        m_network.update(inputs);
#endif

        if (!m_time_data.empty() &&
            m_time_data.back() - m_time_data.front() > m_viewed_seconds)
        {
            m_time_data.clear();
#ifdef SINGLE_NEURON
            // m_kappa_data.clear();
            // m_eta_data.clear();
            m_membrane_potential_data.clear();
#else
            for (std::size_t i {}; i < num_neurons; ++i)
            {
                m_membrane_potential_data[i].clear();
            }
            for (std::size_t i {}; i < num_neurons * num_neurons; ++i)
            {
                m_synapse_data[i].clear();
            }
#endif
        }

        m_time_data.push_back(total_time);
#ifdef SINGLE_NEURON
        // m_kappa_data.push_back(m_neuron.kappa);
        // m_eta_data.push_back(m_neuron.eta);
        m_membrane_potential_data.push_back(m_neuron.membrane_potential);
#else
        for (std::size_t i {}; i < num_neurons; ++i)
        {
            m_membrane_potential_data[i].push_back(
                m_network.neurons[i].last_membrane_potential);
        }
        for (std::size_t i {}; i < num_neurons * num_neurons; ++i)
        {
            m_synapse_data[i].push_back(m_network.synapses[i]);
        }
#endif

        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        if (SDL_RenderClear(m_renderer) != 0)
        {
            throw std::runtime_error(SDL_GetError());
        }

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        update_ui();

        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

        SDL_RenderPresent(m_renderer);
    }
}

void Interface::update_ui()
{
    ImGui::SetNextWindowPos({0, 0});
    int width, height;
    SDL_GetWindowSize(m_window, &width, &height);
    ImGui::SetNextWindowSize(
        {static_cast<float>(width), static_cast<float>(height)});
    if (ImGui::Begin("UI",
                     nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::SliderFloat("Input current", &m_input_current, 0.0f, 2.0f);
        ImGui::SliderFloat(
            "Input probability", &m_input_probability, 0.0f, 1.0f);

        if (ImPlot::BeginPlot(
                "Membrane potential",
                {static_cast<float>(width), static_cast<float>(height) * 0.4f},
                ImPlotFlags_NoTitle))
        {
            const auto axis_x_min = m_time_data.front();
            const auto axis_x_max = m_time_data.front() + m_viewed_seconds;
            ImPlot::SetupAxisLimits(ImAxis_X1,
                                    static_cast<double>(axis_x_min),
                                    static_cast<double>(axis_x_max),
                                    ImGuiCond_Always);
#ifdef SINGLE_NEURON
            /*ImPlot::PlotLine("Kappa",
                             m_time_data.data(),
                             m_kappa_data.data(),
                             static_cast<int>(m_time_data.size()));
            ImPlot::PlotLine("Eta",
                             m_time_data.data(),
                             m_eta_data.data(),
                             static_cast<int>(m_time_data.size()));*/
            ImPlot::PlotLine("Membrane potential",
                             m_time_data.data(),
                             m_membrane_potential_data.data(),
                             static_cast<int>(m_time_data.size()));
#else
            for (std::size_t i {}; i < num_neurons; ++i)
            {
                const auto label = "Membrane potential " + std::to_string(i);
                ImPlot::PlotLine(label.c_str(),
                                 m_time_data.data(),
                                 m_membrane_potential_data[i].data(),
                                 static_cast<int>(m_time_data.size()));
            }
#endif
            ImPlot::EndPlot();
        }

#ifndef SINGLE_NEURON
        if (ImPlot::BeginPlot(
                "Synapses",
                {static_cast<float>(width), static_cast<float>(height) * 0.4f},
                ImPlotFlags_NoTitle))
        {
            const auto axis_x_min = m_time_data.front();
            const auto axis_x_max = m_time_data.front() + m_viewed_seconds;
            ImPlot::SetupAxisLimits(ImAxis_X1,
                                    static_cast<double>(axis_x_min),
                                    static_cast<double>(axis_x_max),
                                    ImGuiCond_Always);
            for (std::size_t i {}; i < num_neurons * num_neurons; ++i)
            {
                const auto label = "Synapse " + std::to_string(i);
                ImPlot::PlotLine(label.c_str(),
                                 m_time_data.data(),
                                 m_synapse_data[i].data(),
                                 static_cast<int>(m_time_data.size()));
            }
            ImPlot::EndPlot();
        }
#endif
    }
    ImGui::End();
}
