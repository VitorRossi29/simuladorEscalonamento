cmake_minimum_required(VERSION 3.20)
project(SimuladorEscalonamento CXX C)

# Padrão C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Runtime estático (/MT e /MTd)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(FetchContent)

# 1. Raylib 5.0
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

# 2. Dear ImGui (Branch 'docking')
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

# 3. rlImGui (Branch 'main')
FetchContent_Declare(
    rlImGui
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG main
)
FetchContent_MakeAvailable(rlImGui)

# Biblioteca estática do ImGui
file(GLOB IMGUI_SOURCES ${imgui_SOURCE_DIR}/*.cpp)
add_library(imgui_static STATIC ${IMGUI_SOURCES} "src/core/Tarefa.cpp" "src/core/MotorSimulacao.cpp")
target_include_directories(imgui_static PUBLIC ${imgui_SOURCE_DIR})

# Biblioteca estática do rlImGui
add_library(rlimgui_static STATIC ${rlimgui_SOURCE_DIR}/rlImGui.cpp "src/core/Tarefa.cpp" "src/core/MotorSimulacao.cpp")
target_include_directories(rlimgui_static PUBLIC ${rlimgui_SOURCE_DIR} ${imgui_SOURCE_DIR})
target_link_libraries(rlimgui_static PUBLIC raylib imgui_static)

# Mapeia IsTextureValid para IsTextureReady (Compatibilidade Raylib 5.0 <-> rlImGui)
target_compile_definitions(rlimgui_static PRIVATE IsTextureValid=IsTextureReady)

# Executável Principal
add_executable(SimuladorEscalonamento
    src/main.cpp
 "src/core/Tarefa.cpp" "src/core/MotorSimulacao.cpp")

target_include_directories(SimuladorEscalonamento PRIVATE
    src
    ${imgui_SOURCE_DIR}
    ${rlimgui_SOURCE_DIR}
)

target_link_libraries(SimuladorEscalonamento PRIVATE
    raylib
    imgui_static
    rlimgui_static
    winmm
)