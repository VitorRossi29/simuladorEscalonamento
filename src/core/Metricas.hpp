#pragma once
#include <vector>
#include "Tarefa.hpp"
#include "../gui/StructsTeste.hpp"

struct Historico
{
	int tempo; //tickAtual de tempo
	std::vector<int> idTarefasPreemptadas;
	std::vector<short int> idDeadlinesPerdidas;
	std::vector<SnapshotTarefaMotor> todasTarefas;
	std::vector<Tarefa> tarefas;
	std::vector<Tarefa> tarefasExecutadas;
	std::vector<EventoGantt> eventos;
};

struct Metricas
{
	double utilizacaoCPU;
	bool escalabilidade;
	int totalDeadlinesPerdidas;
	int tempoIdleCPU;
};