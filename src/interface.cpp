#include "interface.hpp"

#include "neuron.hpp"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "implot.h"

#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

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
        throw std::runtime_error(SDL_GetError());
    }

    m_renderer = SDL_CreateRenderer(
        m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer)
    {
        throw std::runtime_error(SDL_GetError());
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer_Init(m_renderer);

    ImFontConfig font_config {};
    font_config.SizePixels = 32.0f;
    ImGui::GetIO().Fonts->AddFontDefault(&font_config);

    ImPlot::CreateContext();
}

Interface::~Interface() noexcept
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
    std::size_t x {};
    const std::size_t data_size {1000};
    std::vector<float> x_data;
    std::vector<float> y_data;
    std::vector<float> y2_data;
    x_data.reserve(data_size);
    y_data.reserve(data_size);
    y2_data.reserve(data_size);

    Neuron neuron {};

    std::random_device rd;
    std::default_random_engine e(rd());
    std::bernoulli_distribution d(0.1);

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

        neuron.update();

        if (d(e))
        {
            neuron.add(0.2f);
        }

        if (x_data.size() >= data_size)
        {
            x_data.clear();
            y_data.clear();
            y2_data.clear();
            x_data.reserve(data_size);
            y_data.reserve(data_size);
            y2_data.reserve(data_size);
        }
        x_data.push_back(static_cast<float>(x));
        y_data.push_back(neuron.membrane_potential);
        y2_data.push_back((std::sin(static_cast<float>(x) * 0.02f) + 0.5f) *
                          0.5f);
        ++x;

        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        if (SDL_RenderClear(m_renderer) != 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", SDL_GetError());
        }

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({0, 0});
        int width, height;
        SDL_GetWindowSize(m_window, &width, &height);
        ImGui::SetNextWindowSize(
            {static_cast<float>(width), static_cast<float>(height)});
        if (ImGui::Begin("UI",
                         nullptr,
                         ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoCollapse))
        {
            if (ImPlot::BeginPlot("My plot",
                                  {static_cast<float>(width) * 0.98f,
                                   static_cast<float>(height) * 0.98f},
                                  ImPlotFlags_NoTitle))
            {
                const std::size_t axis_x_min {x / data_size * data_size};
                const std::size_t axis_x_max {(x / data_size + 1) * data_size};
                ImPlot::SetupAxisLimits(ImAxis_X1,
                                        static_cast<double>(axis_x_min),
                                        static_cast<double>(axis_x_max),
                                        ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -0.7, 1.2, ImGuiCond_Always);
                ImPlot::PlotLine("Membrane potential",
                                 x_data.data(),
                                 y_data.data(),
                                 static_cast<int>(x_data.size()));
                ImPlot::PlotLine("Test second data",
                                 x_data.data(),
                                 y2_data.data(),
                                 static_cast<int>(x_data.size()));
                ImPlot::EndPlot();
            }
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

        SDL_RenderPresent(m_renderer);
    }
}
