#pragma once
#include "Tarefa.hpp"

enum class estadoCPU
{
	EXECUTANDO,
	OCIOSA
};

static int idCPUGlobal=1;

class CPU
{
private:
	int idCPU;

	int tempoOciosa;
	estadoCPU estado;
	Tarefa* tarefaAtual;
	Tarefa* tarefaAnterior;

public:
	CPU():idCPU(idCPUGlobal++), tempoOciosa(0), tarefaAtual(nullptr), tarefaAnterior(nullptr) {}
	int getId() const { return idCPU; }
	Tarefa* getTarefaAtual() const { return tarefaAtual; }
	Tarefa* getTarefaAnterior() const { return tarefaAnterior; }
	void setTarefaAtual(Tarefa* tarefa) { tarefaAnterior=tarefaAtual; tarefaAtual=tarefa; }
	void atualizaEstado() 
	{ 
		if (tarefaAtual==nullptr)
		{
			estado=estadoCPU::OCIOSA;
			tempoOciosa++;
		}
		else
			estado=estadoCPU::EXECUTANDO;
	}
};