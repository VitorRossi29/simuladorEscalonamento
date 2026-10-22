#pragma once
#include "Tarefa.hpp"

#include <vector>

#include "../gui/MockData.hpp"

struct Historico
{
	int tempo; //tickAtual de tempo
	std::vector<int> idTarefasPreemptadas; //guarda ids das tarefas que foram preemptadas
	std::vector<short int> idDeadlinesPerdidas; //guarda ids das tarefas que perderam deadline
	std::vector<SnapshotTarefaMotor> todasTarefas; //guarda os atributos dinamicos de todas as tarefas
	std::vector<Tarefa> tarefas; //guarda todas as tarefas existentes naquele tick
	std::vector<Tarefa> tarefasExecutadas; //guarta todas as tarefas que executaram naquele tick
	std::vector<EventoGantt> eventos; //guarda os eventos para representacao grafica
};

struct Metricas
{
	double utilizacaoCPU; //armazena a utilizacao das CPUs
	bool escalabilidade; //armazena se eh escalonavel ou nao
	int totalDeadlinesPerdidas; //armazena quantidade de deadlines perdidas
	int tempoIdleCPU; //armazena tempo total de idle das CPUs
};

enum class estadoCPU
{
	EXECUTANDO, //possui tarefa
	DESLIGADA //nao possui tarefa
};

class CPU
{
private:
	int idCPU;

	int tempoDesligada; //quantos ticks ficou desligada
	estadoCPU estado; //executando ou desligada
	Tarefa* tarefaAtual; //guarda uma referencia para tarefa que esta executando
	Tarefa* tarefaAnterior; //guarda uma referencia para a tarefa que executou no tick anterior

public:
	CPU(int id):idCPU(id), tempoDesligada(0), tarefaAtual(nullptr), tarefaAnterior(nullptr) {}
	int getId() const { return idCPU; }
	Tarefa* getTarefaAtual() const { return tarefaAtual; }
	Tarefa* getTarefaAnterior() const { return tarefaAnterior; }
	void setTarefaAtual(Tarefa* tarefa) { tarefaAnterior=tarefaAtual; tarefaAtual=tarefa; }
	void atualizaEstado() //atualiza o estado dependendo se tem tarefa ou nao
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