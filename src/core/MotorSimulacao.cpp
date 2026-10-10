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

Tarefa* MotorSimulacao::achaProximaRM()
{
	Tarefa* tarefaMaiorPrioridade=nullptr;

	//encontra a tarefa com maior prioridade que esteja pronta para executar
	for(Tarefa& tarefa:tarefas)
	{
		//muda o estado da tarefa se estiver pronta para executar
		if (tarefa.getProximaLiberacao()<=tickAtual && tarefa.getTempoRestante()>0)
		{
			tarefa.setEstado(EstadoTarefa::PRONTA);

			//escolhe a tarefa com o maior valor numerico de prioridade das PRONTAS
			if(tarefaMaiorPrioridade==nullptr || tarefa.getPrioridade()>tarefaMaiorPrioridade->getPrioridade())
			{
				tarefaMaiorPrioridade=&tarefa;
			}
		}
	}

	return tarefaMaiorPrioridade;
}

Tarefa* MotorSimulacao::achaProximaEDF()
{
	Tarefa* tarefaMaiorPrioridade=nullptr;

	//encontra a tarefa com maior prioridade que esteja pronta para executar
	for (Tarefa& tarefa:tarefas)
	{
		//muda o estado da tarefa se estiver pronta para executar
		if (tarefa.getProximaLiberacao()<=tickAtual && tarefa.getTempoRestante()>0)
		{
			tarefa.setEstado(EstadoTarefa::PRONTA);

			//escolhe a tarefa com o menor deadline ja PRONTA
			if(tarefaMaiorPrioridade==nullptr || tarefa.getProximoDeadline()<tarefaMaiorPrioridade->getProximoDeadline())
			{
				tarefaMaiorPrioridade=&tarefa;
			}
		}
	}
	
	return tarefaMaiorPrioridade;
}

Historico MotorSimulacao::passoSimulacao(Tarefa* tarefaMaiorPrioridade)
{
	Historico historicoAtual;

	if(tarefaMaiorPrioridade==nullptr)
	{
		//nenhuma tarefa pronta para executar
		historicoAtual.tempo=tickAtual;
		historicoAtual.preemptada=false;
		//cria uma tarefa com a construtora sem parametro para indicar que nenhuma tarefa foi executada nesse tick
		historicoAtual.tarefaExecutada=Tarefa();
	}
	else
	{
		//armazena a tarefa depois de decrementar o tempo restante
		tarefaMaiorPrioridade->setEstado(EstadoTarefa::EXECUTANDO);
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

		for(Tarefa& tarefa:tarefas)
		{
			//verifica deadlines perdidas
			if (tarefa.getProximoDeadline()==tickAtual && tarefa.getTempoRestante()>0)
			{
				tarefa.setEstado(EstadoTarefa::DEADLINE_PERDIDA);
				historicoAtual.idDeadlinesPerdidas.push_back(tarefa.getId());
				++tarefa; //incrementa o numero de deadlines perdidas
			}
			//verifica tarefas terminadas
			else if(tarefa.getTempoRestante()==0)
			{
				if(tarefa.getQuantidadeExecucoes()<10)
				{
					tarefa++;
					tarefa.resetaTarefa();
				}
				else
					tarefa.setEstado(EstadoTarefa::TERMINADA);
			}
			//incrementa o tempo de espera da tarefa
			else if(tarefa.getEstado()==EstadoTarefa::PRONTA && &tarefa!=tarefaMaiorPrioridade)
			{
				tarefa+=1;
			}
		}
		historicoAtual.tempo=tickAtual;
	}

	historicoTimeline.push_back(historicoAtual);
	tickAtual++;
	return historicoAtual;
}

//Atribui as prioridades estaticas para as tarefas
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

//calcula a utilizacao da CPU com base nas tarefas existentes
double MotorSimulacao::calculaUtilizacaoCPU()
{
	double utilizacao=0.0;

	for (const Tarefa& tarefa:tarefas)
	{
		utilizacao+=static_cast<double>(tarefa.getDuracao())/tarefa.getPeriodo();
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

	if(n==0)
		return true;

	double utilizacao=calculaUtilizacaoCPU();

	//2^(1/n)-1
	double limite=n*(std::pow(2.0, 1.0/n)-1.0);

	return utilizacao<=limite;
}

bool MotorSimulacao::comparaPeriodo(const Tarefa& a, const Tarefa& b)
{
	return a.getPeriodo()<b.getPeriodo();
}

void MotorSimulacao::executaTempoReal()
{
	bool executando=true;
	if(algoritmoSelecionado==TipoAlgoritmo::RATE_MONOTONIC)
	{
		preparaSimulacaoRM();

		while(executando)
		{
			//executa um passo da simulacao
			Historico historico=passoSimulacao(achaProximaRM());
			//verifica se todas as tarefas terminaram
			executando=false;
			for (const Tarefa& tarefa:tarefas)
			{
				if (tarefa.getEstado()!=EstadoTarefa::TERMINADA)
				{
					executando=true;
					break;
				}
			}
		}
	}
	
	else if(algoritmoSelecionado==TipoAlgoritmo::EARLIEST_DEADLINE_FIRST)
	{
		while(executando)
		{
			//executa um passo da simulacao
			Historico historico=passoSimulacao(achaProximaEDF());
			//verifica se todas as tarefas terminaram
			executando=false;
			for (const Tarefa& tarefa:tarefas)
			{
				if (tarefa.getEstado()!=EstadoTarefa::TERMINADA)
				{
					executando=true;
					break;
				}
			}
		}
	}
}