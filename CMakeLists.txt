cmake_minimum_required(VERSION 3.20)
project(CluxeEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(CluxeEditor
    src/editor/main.cpp
    src/core/window/CLX_Window.cpp
    src/core/window/CLX_Window.h
    src/core/input/CLX_Input.cpp
    src/core/input/CLX_Input.h
    src/core/time/CLX_Time.cpp
    src/core/time/CLX_Time.h
    src/core/renderer/CLX_Renderer2D.cpp
    src/core/renderer/CLX_Renderer2D.h
    src/core/audio/miniaudio.c
    src/core/audio/AudioManager.cpp
    src/core/audio/AudioManager.h
    src/editor/ui/AudioPanel.cpp
    src/editor/ui/AudioPanel.h
	src/editor/ui/2DGizmo.cpp
	src/editor/ui/2DGizmo.h
	src/editor/ui/2DGameObject.cpp
	src/editor/ui/2DGameObject.h
	src/editor/ui/SelectionRect.cpp
	src/editor/ui/SelectionRect.h
	src/editor/ui/PopUpMenu.h
    src/editor/ui/UITheme.h
    src/game/Game.cpp
    src/game/Game.h
    src/core/font/FontManager.cpp
    src/core/font/FontManager.h
    include/imgui/imgui.cpp
    include/imgui/imgui_demo.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/backends/imgui_impl_sdl3.cpp
    include/imgui/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(CluxeEditor PRIVATE include include/imgui include/imgui/backends include/miniaudio include/SDL3_ttf)

if(WIN32)
    target_link_libraries(CluxeEditor PRIVATE
        "${CMAKE_SOURCE_DIR}/lib/SDL3/x64/SDL3.lib"
        "${CMAKE_SOURCE_DIR}/lib/ttf/x64/SDL3_ttf.lib"
    )
else()
    target_link_libraries(CluxeEditor PRIVATE SDL3)

    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_LIBRARIES m)
    check_cxx_source_compiles("#include <cmath>\nint main() { return static_cast<int>(std::abs(-1.0)); }" NEEDS_MATH)
    if(NEEDS_MATH)
        target_link_libraries(CluxeEditor m)
    endif()

    set(CMAKE_REQUIRED_LIBRARIES pthread)
    check_cxx_source_compiles("#include <pthread.h>\nint main() { pthread_t t; pthread_create(&t, nullptr, nullptr, nullptr); return 0; }" NEEDS_PTHREAD)
    if(NEEDS_PTHREAD)
        target_link_libraries(CluxeEditor pthread)
    endif()
endif()
