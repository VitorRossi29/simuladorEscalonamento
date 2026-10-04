#pragma once
#include <vector>
#include "Tarefa.hpp"
#include "Metricas.hpp"
#include <cmath>

struct MotorSimulacao
{
private:
	long int tickAtual;

	//Minimo Multiplo Comum (MMC) dos periodos de todas as tarefas.
	int hiperperiodo;
	TipoAlgoritmo algoritmoSelecionado;
	std::vector<Tarefa> tarefas;
	std::vector<Historico> historicoTimeline;

	enum class EstadoSimulacao
	{
		PAUSADA,
		EXECUTANDO,
		PARADA,
		PASSO_A_PASSO
	};

public:
	void addTarefa(const Tarefa& tarefa);
	void resetSimulacao();

	Historico passoSimulacaoRM();
	Historico passoSimulacaoEDF();
	void preparaSimulacaoRM();

	double calculaUtilizacaoCPU();
	bool verificaEscalabilidade();
	bool escalabilidadeRM();

	bool comparaPeriodo(const Tarefa& t1, const Tarefa& t2);
};