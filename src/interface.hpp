#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif
#include <SDL.h>

class Interface
{
public:
    [[nodiscard]] Interface();
    ~Interface() noexcept;

    void run();

private:
    SDL_Window *m_window {};
    SDL_Renderer *m_renderer {};
};

#endif // INTERFACE_HPP
