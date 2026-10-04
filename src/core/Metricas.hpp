#pragma once
#include <vector>

struct Historico
{
	int tempo;
	short int idTarefa;
	bool preemptada;
	std::vector<short int> idDeadlinesPerdidas;
};

struct Metricas
{
	double utilizacaoCPU;
	bool escalabilidade;
	int totalDeadlinesPerdidas;
	int tempoIdleCPU;
};