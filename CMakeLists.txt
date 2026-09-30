cmake_minimum_required(VERSION 3.20)
project(SimuladorEscalonamento CXX C)

# Padrão C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Força o runtime estático do Visual Studio (/MT e /MTd)
# Evita a dependência de DLLs extras como VCRUNTIME140.dll
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(FetchContent)

# 1. Raylib (Compilação Estática)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

# 2. Dear ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.4
)
FetchContent_MakeAvailable(imgui)

# 3. rlImGui (Bridge entre Raylib e Dear ImGui)
FetchContent_Declare(
    rlImGui
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG main
)
FetchContent_MakeAvailable(rlImGui)

# Empacota os fontes do ImGui em uma biblioteca estática
file(GLOB IMGUI_SOURCES ${imgui_SOURCE_DIR}/*.cpp)
add_library(imgui_static STATIC ${IMGUI_SOURCES})
target_include_directories(imgui_static PUBLIC ${imgui_SOURCE_DIR})

# Empacota o rlImGui em uma biblioteca estática
add_library(rlimgui_static STATIC ${rlimgui_SOURCE_DIR}/rlImGui.cpp)
target_include_directories(rlimgui_static PUBLIC ${rlimgui_SOURCE_DIR} ${imgui_SOURCE_DIR})
target_link_libraries(rlimgui_static PUBLIC raylib imgui_static)

# Definição do Executável Principal
add_executable(SimuladorEscalonamento
    src/main.cpp
)

# Inclusão dos diretórios de cabeçalho
target_include_directories(SimuladorEscalonamento PRIVATE
    src
    ${imgui_SOURCE_DIR}
    ${rlimgui_SOURCE_DIR}
)

# Linkagem Estática dos Módulos no Executável Único
target_link_libraries(SimuladorEscalonamento PRIVATE
    raylib
    imgui_static
    rlimgui_static
    winmm # Biblioteca do Windows necessária para o Timer da Raylib
)
