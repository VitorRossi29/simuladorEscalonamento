//Aqui estara o MAIN

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "core/Tarefa.hpp"
#include "core/Metricas.hpp"
#include "core/MotorSimulacao.hpp"

#include "gui/GerenciadorGUI.hpp"

int main() {
    // Configuracões da Janela Nativa (Raylib)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Simulador de Escalonamento - Projeto A");
    SetTargetFPS(60);

    // Inicializa a integracao do ImGui
    rlImGuiSetup(true);

    MotorSimulacao motorReal;

    GerenciadorGUI GUI(&motorReal); // Injetar o motor na GUI
    GUI.inicializar();

    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        
        GUI.atualizar(GetFrameTime());

        ClearBackground(DARKGRAY);

        rlImGuiBegin();

        GUI.renderizar();

        rlImGuiEnd();

        EndDrawing();
    }

    // Encerrando recursos de forma limpa
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
