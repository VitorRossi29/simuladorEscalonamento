//Aqui estara o MAIN

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "core/Tarefa.hpp"
#include "core/Metricas.hpp"
#include "core/MotorSimulaçao.hpp"

int main() {
    // Configurações da Janela Nativa (Raylib)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Simulador de Escalonamento - Projeto A");
    SetTargetFPS(60);

    // Inicializa a integração do ImGui
    rlImGuiSetup(true);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Inicia a renderização dos elementos de interface ImGui
        rlImGuiBegin();

        // Janela de Teste Nativa do ImGui (Valida a biblioteca)
        ImGui::ShowDemoWindow();

        // Janela do Simulador (Protótipo para integração com Desenvolvedor A)
        ImGui::Begin("Controle da Simulacao");
        ImGui::Text("Estado da CPU: Parada");
        ImGui::Separator();
        
        if (ImGui::Button("Avancar Tick (+1)")) {
            // Ponto de integração futuro com o motor do Desenvolvedor A
        }
        ImGui::SameLine();
        if (ImGui::Button("Retroceder Tick (-1)")) {
            // Ponto de integração futuro com o histórico do Desenvolvedor A
        }

        ImGui::End();

        // Finaliza o quadro do ImGui
        rlImGuiEnd();

        EndDrawing();
    }

    // Encerrando recursos de forma limpa
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
