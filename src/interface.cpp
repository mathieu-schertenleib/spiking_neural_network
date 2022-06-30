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

    std::random_device rd;
    m_rng.seed(rd());
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
    m_start_time = std::chrono::high_resolution_clock::now();
    m_last_time = m_start_time;

    while (true)
    {
        SDL_Event event {};
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
            {
                return;
            }
        }

        m_current_time = std::chrono::high_resolution_clock::now();
        update_model();
        m_last_time = m_current_time;

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

void Interface::update_model()
{
    const auto total_time =
        std::chrono::duration<float, std::chrono::seconds::period>(
            m_current_time - m_start_time)
            .count();

    const auto delta_time =
        std::chrono::duration<float, std::chrono::seconds::period>(
            m_current_time - m_last_time)
            .count();

    m_neuron.update(delta_time);

    std::bernoulli_distribution d(0.1);
    if (d(m_rng))
    {
        m_neuron.input(0.15f);
    }

    if (!m_x_data.empty() &&
        m_x_data.back() - m_x_data.front() > m_viewed_seconds)
    {
        m_x_data.clear();
        m_y_data.clear();
    }
    m_x_data.push_back(total_time);
    m_y_data.push_back(m_neuron.membrane_potential);
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
        if (ImPlot::BeginPlot(
                "Membrane potential plot",
                {static_cast<float>(width), static_cast<float>(height)},
                ImPlotFlags_NoTitle))
        {
            const auto axis_x_min = m_x_data.front();
            const auto axis_x_max = m_x_data.front() + m_viewed_seconds;
            ImPlot::SetupAxisLimits(ImAxis_X1,
                                    static_cast<double>(axis_x_min),
                                    static_cast<double>(axis_x_max),
                                    ImGuiCond_Always);
            ImPlot::SetupAxisLimits(
                ImAxis_Y1,
                -0.2,
                static_cast<double>(
                    Leaky_integrate_and_fire_neuron::threshold_potential) +
                    0.2,
                ImGuiCond_Always);
            ImPlot::PlotLine("Membrane potential",
                             m_x_data.data(),
                             m_y_data.data(),
                             static_cast<int>(m_x_data.size()));
            ImPlot::EndPlot();
        }
        ImGui::End();
    }
}