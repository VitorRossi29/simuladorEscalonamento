#pragma once
#include <vector>
#include "Tarefa.hpp"

struct Historico
{
	int tempo;
	bool preemptada;
	std::vector<short int> idDeadlinesPerdidas;
	Tarefa tarefaExecutada;
};

struct Metricas
{
	double utilizacaoCPU;
	bool escalabilidade;
	int totalDeadlinesPerdidas;
	int tempoIdleCPU;
};