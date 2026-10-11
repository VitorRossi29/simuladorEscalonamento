#pragma once
#include <vector>
#include "Tarefa.hpp"

struct Historico
{
	int tempo; //tickAtual de tempo
	bool preemptada;
	std::vector<short int> idDeadlinesPerdidas;
	std::vector<SnapshotTarefas> todasTarefas;
	Tarefa tarefaExecutada;
};

struct Metricas
{
	double utilizacaoCPU;
	bool escalabilidade;
	int totalDeadlinesPerdidas;
	int tempoIdleCPU;
};