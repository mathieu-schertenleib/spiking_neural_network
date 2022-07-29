cmake_minimum_required(VERSION 3.19)

project(spiking_neural_network LANGUAGES CXX)

set(CLANG_OPTIONS
        -march=native
        -ffast-math
        -Wfatal-errors
        -Wall
        -Wextra
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        )

set(GCC_OPTIONS
        ${CLANG_OPTIONS}
        -Wmisleading-indentation
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
        -Wswitch-enum
        )

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(PROJECT_OPTIONS ${CLANG_OPTIONS})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(PROJECT_OPTIONS ${GCC_OPTIONS})
else ()
    message(WARNING "No warnings set for compiler '${CMAKE_CXX_COMPILER_ID}'")
endif ()

find_package(SDL2 REQUIRED)

add_executable(snn
        src/main.cpp
        src/interface.cpp src/interface.hpp
        src/neuron.cpp src/neuron.hpp
        external/imgui/backends/imgui_impl_sdl.cpp
        external/imgui/backends/imgui_impl_sdlrenderer.cpp
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/implot/implot.cpp external/implot/implot.h
        external/implot/implot_items.cpp
        external/implot/implot_internal.h
        )
target_compile_options(snn PRIVATE ${PROJECT_OPTIONS})
target_compile_features(snn PRIVATE cxx_std_20)
target_include_directories(snn PRIVATE external/imgui external/imgui/backends external/implot ${SDL2_INCLUDE_DIRS})
target_link_libraries(snn PRIVATE ${SDL2_LIBRARIES})


add_executable(tests tests/main.cpp src/neuron.cpp src/neuron.hpp)
target_compile_options(tests PRIVATE ${PROJECT_OPTIONS})
target_compile_features(tests PRIVATE cxx_std_20)
target_include_directories(tests PRIVATE src)
