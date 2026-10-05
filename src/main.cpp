//Aqui estara o MAIN

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "core/Tarefa.hpp"
#include "core/Metricas.hpp"
#include "core/MotorSimulacao.hpp"

#include "gui/PainelConfiguracao.hpp"

int main() {
    // Configurações da Janela Nativa (Raylib)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Simulador de Escalonamento - Projeto A");
    SetTargetFPS(60);

    // Inicializa a integração do ImGui
    rlImGuiSetup(true);

    PainelConfiguracao painelConfig;

    bool exibirPainelConfig = false;
    bool exibirPainelMetricas = false;

    int deadlinesPerdidos = 0;
    float tempoOciosoPercentual = 0.67f;

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
       
        // O botão apenas inverte a variável
        if (ImGui::Button("Painel de Configuracao")) {
            exibirPainelConfig = !exibirPainelConfig;
        }

        if (exibirPainelConfig) {
            painelConfig.renderizar();
        }

        if (ImGui::Button("Metricas")) {
            exibirPainelMetricas = !exibirPainelMetricas;
        }

        if (exibirPainelConfig) {
            painelConfig.renderizarMetricas(, , tempoOciosoPercentual);
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
