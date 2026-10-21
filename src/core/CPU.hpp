#pragma once
#include "Tarefa.hpp"

#include <vector>

#include "../gui/MockData.hpp"

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

enum class estadoCPU
{
	EXECUTANDO,
	DESLIGADA
};

class CPU
{
private:
	int idCPU;

	int tempoDesligada;
	estadoCPU estado;
	Tarefa* tarefaAtual;
	Tarefa* tarefaAnterior;

public:
	CPU(int id):idCPU(id), tempoDesligada(0), tarefaAtual(nullptr), tarefaAnterior(nullptr) {}
	int getId() const { return idCPU; }
	Tarefa* getTarefaAtual() const { return tarefaAtual; }
	Tarefa* getTarefaAnterior() const { return tarefaAnterior; }
	void setTarefaAtual(Tarefa* tarefa) { tarefaAnterior=tarefaAtual; tarefaAtual=tarefa; }
	void atualizaEstado() 
	{ 
		if (tarefaAtual==nullptr)
		{
			estado=estadoCPU::DESLIGADA;
			tempoDesligada++;
		}
		else
			estado=estadoCPU::EXECUTANDO;
	}
};