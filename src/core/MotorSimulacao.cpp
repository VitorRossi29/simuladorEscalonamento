#include "MotorSimulacao.hpp"

//coloca a tarefa no vetor de tarefas
void MotorSimulacao::addTarefa(const Tarefa& tarefa)
{
	tarefas.push_back(tarefa);
}

//zera o tempo, limpa o historico e reseta todas as tarefas
void MotorSimulacao::resetSimulacao()
{
	tickAtual=0;
	historicoTimeline.clear();

	for(Tarefa& tarefa:tarefas)
	{
		tarefa.resetaTarefa();
	}
}

//executa um passo da simulacao e retorna o historico do tick atual
Historico MotorSimulacao::passoSimulacao()
{
	return Historico();
}

//calcula a utilizacao da CPU com base nas tarefas existentes
double MotorSimulacao::calculaUtilizacaoCPU()
{
	double utilizacao=0.0;

	for (const Tarefa& tarefa:tarefas)
	{
		utilizacao+=static_cast<double>(tarefa.getTempoDeComputacao())/tarefa.getPeriodo();
	}

	return utilizacao;
}

//verifica a escalabilidade do conjunto de tarefas com base no algoritmo selecionado
bool MotorSimulacao::verificaEscalabilidade()
{
	if(algoritmoSelecionado==TipoAlgoritmo::RATE_MONOTONIC)
	{
		return escalabilidadeRM();
	}
	else if(algoritmoSelecionado==TipoAlgoritmo::EARLIEST_DEADLINE_FIRST)
	{
		return calculaUtilizacaoCPU()<=1.0;
	}
	else
	{
		return false;
	}
}

//utiliza a fórmula do Maziero
bool MotorSimulacao::escalabilidadeRM()
{
	int n=tarefas.size();

	if (n==0)
		return true;

	double utilizacao=calculaUtilizacaoCPU();

	double limite=n*(std::pow(2.0, 1.0/n)-1.0);

	return utilizacao<=limite;
}
