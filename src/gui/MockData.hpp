#pragma once
#include <vector>
#include <string>
#include "StructsTeste.hpp"

//cria tarefas falsas para testar
class GeradorMock
{
private:
    //Pelo jeito nao vai ter
public:
    GeradorMock() = delete;
    //Instancia tarefas iniciais
    static std::vector<TarefaGUI> criarTarefasIniciais();
    //Gera o vetor de snapshots ao longo do tempo
    static std::vector<SnapshotTick> gerarHistoricoInicial(std::vector<TarefaGUI>& tarefas, int maxTicks, int maxCpus, int quantum);
};