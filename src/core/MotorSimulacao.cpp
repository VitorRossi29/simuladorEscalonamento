#include "MotorSimulacao.hpp"
#include <algorithm>

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
Historico MotorSimulacao::passoSimulacaoRM()
{
	Historico historicoAtual;
	Tarefa* tarefaMaiorPrioridade=nullptr;

	//encontra a tarefa com maior prioridade que esteja pronta para executar
	for(Tarefa& tarefa:tarefas)
	{
		if(tarefaMaiorPrioridade==nullptr || tarefa.getPrioridade()>tarefaMaiorPrioridade->getPrioridade())
		{
			if(tarefa.getIngresso()<=tickAtual && tarefa.getTempoRestante()>0)
				tarefaMaiorPrioridade=&tarefa;
		}
	}

	if(tarefaMaiorPrioridade==nullptr)
	{
		//nenhuma tarefa pronta para executar
		//VER OQ TEM QUE FAZER AQ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		return historicoAtual;
	}
	
	//armazena a tarefa depois de decrementar o tempo restante
	(*tarefaMaiorPrioridade)--;
	historicoAtual.tarefaExecutada=*tarefaMaiorPrioridade; //tarefa que sera executada nesse tick
	
	
	//para o primeiro tick nunca eh preemptada
	if(tickAtual==0)
	{
		historicoAtual.preemptada=false;
	}
	//compara se eh igual a tarefa anterior e se a mesma ja terminou
	else
	{
		if(historicoTimeline.back().tarefaExecutada.getId()!=tarefaMaiorPrioridade->getId() && //tarefa diferente da anterior
			historicoTimeline.back().tarefaExecutada.getTempoRestante()>0) //tarefa anterior ainda nao terminou
			historicoAtual.preemptada=true;
		else
			historicoAtual.preemptada=false;
	}
	
	//verifica deadlines perdidas
	//TALVEZ SEJA <= TICK ATUAL
	for (Tarefa& tarefa:tarefas)
	{
		if (tarefa.getProximoDeadline()==tickAtual && tarefa.getTempoRestante()>0)
		{
			historicoAtual.idDeadlinesPerdidas.push_back(tarefa.getId());
		}
	}

	historicoAtual.tempo=tickAtual++;
	historicoTimeline.push_back(historicoAtual);

	return historicoAtual;
}

void MotorSimulacao::preparaSimulacaoRM()
{
	//ordena as tarefas do menor pro maior periodo
	std::sort(tarefas.begin(), tarefas.end(), comparaPeriodo);

	//atribui a maior prioridade para as tarefas com menor periodo
	short int i=tarefas.size();
	for(Tarefa& tarefa:tarefas)
	{
		tarefa.setPrioridade(i--);
	}
}

//executa um passo da simulacao e retorna o historico do tick atual
//FAZER DPS
Historico MotorSimulacao::passoSimulacaoEDF()
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

bool MotorSimulacao::comparaPeriodo(const Tarefa& a, const Tarefa& b)
{
	return a.getPeriodo()<b.getPeriodo();
}
