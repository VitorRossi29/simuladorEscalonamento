#pragma once
#include "imgui.h"
#include "StructsTeste.hpp"
#include "raylib.h"

class PainelGraficoGantt
{
private:
    const std::vector<TarefaGUI>* m_tarefasRef;
    const std::vector<SnapshotTick>* m_historicoRef;
    const int* m_tickAtualRef;
    const int* m_qtdeCpusRef;
    const int* m_modoExecucao;

    // Configuracões de dimensao e zoom da grade
    float m_larguraTick;  // Largura em pixels de 1 tick (ex: 30.0f)
    float m_alturaLinha;  // Altura em pixels da linha de cada tarefa (ex: 40.0f)
    float m_margemEsquerda; // Espaco para os rohtulos do eixo Y (ex: 100.0f)
    float m_margemTopo;     // Espaco para a régua do eixo X (ex: 40.0f)

    // Converte string Hex ("FF4500") para ImU32
    ImU32 hexParaImU32(const char* hex, float alpha = 1.0f);
    Color hexParaRaylibColor(const char* hex, float alpha = 1.0f);

    // Métodos auxiliares de desenho
    void desenharReguaEixoX(ImDrawList* listaDesenhos, ImVec2 inicio, int totalTicks);
    void desenharAgulhaTempoAtual(ImDrawList* listaDesenhos, ImVec2 inicio, size_t totalTarefas);
    void desenharEixoYELinhasBase(ImDrawList* listaDesenhos, ImVec2 inicio, const std::vector<TarefaGUI>& tarefasOrdenadas);
    void desenharBlocosExecucao(ImDrawList* listaDesenhos, ImVec2 inicio, const std::vector<TarefaGUI>& tarefasOrdenadas);
    void desenharEventosESinais(ImDrawList* listaDesenhos, ImVec2 inicio, const std::vector<TarefaGUI>& tarefasOrdenadas);

public:
    PainelGraficoGantt(const std::vector<TarefaGUI>* tarefas,
        const std::vector<SnapshotTick>* historico,
        const int* tickAtual,
        const int* qtdeCpus, 
        int* modoExecucao);

    void renderizar();
    ImVec2 dimensoesTotais();
    void desenharExportacaoRaylib(int larguraCanvas, int alturaCanvas);
};