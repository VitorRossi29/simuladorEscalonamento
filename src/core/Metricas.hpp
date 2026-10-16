#pragma once
#include <vector>
#include "Tarefa.hpp"

struct Historico
{
	int tempo; //tickAtual de tempo
	std::vector<int> idTarefasPreemptadas;
	std::vector<short int> idDeadlinesPerdidas;
	std::vector<SnapshotTarefas> todasTarefas;
	std::vector<Tarefa> tarefas;
	std::vector<Tarefa> tarefasExecutadas;
};

struct Metricas
{
	double utilizacaoCPU;
	bool escalabilidade;
	int totalDeadlinesPerdidas;
	int tempoIdleCPU;
};