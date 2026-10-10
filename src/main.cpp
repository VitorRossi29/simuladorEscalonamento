//Aqui estara o MAIN

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "core/Tarefa.hpp"
#include "core/Metricas.hpp"
#include "core/MotorSimulacao.hpp"

#include "gui/GerenciadorGUI.hpp"

int main() {
    // Configurações da Janela Nativa (Raylib)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Simulador de Escalonamento - Projeto A");
    SetTargetFPS(60);

    // Inicializa a integração do ImGui
    rlImGuiSetup(true);

    GerenciadorGUI GUI;
    GUI.inicializar();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Passo 1
        GUI.atualizar(GetFrameTime());

        // Inicia a renderização dos elementos de interface ImGui
        // Passo 2
        rlImGuiBegin();

        // Janela de Teste Nativa do ImGui (Valida a biblioteca)
        // ImGui::ShowDemoWindow();

        // Janela do Simulador (Prototipo para integrar com Back End)
        
        ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("Simulador de Escalonamento");

        // Passo 3
        GUI.renderizar();

        ImGui::End();

        // Passo 4
        // Finaliza o quadro do ImGui
        rlImGuiEnd();

        EndDrawing();
    }

    // Encerrando recursos de forma limpa
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
