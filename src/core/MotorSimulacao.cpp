#include "MotorSimulacao.hpp"
#include <algorithm>
#include <random>
#include "../gui/MockData.hpp"
#include "Tarefa.hpp"

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

	//limpa o vetor de CPUs
	todasCPUs.clear(); 

	//cria as CPUs de acordo com a quantidade estabelecida 
	for(int i=0;i<quantCPUs;i++)
	{
		todasCPUs.push_back(CPU(i+1));
	}

	for(Tarefa& tarefa:tarefas)
	{
		tarefa.resetaTarefa();
	}
}

//prepara um vetor com as tarefas mais prioritarias prontas para executar, tamanho maximo eh o numero de CPUs
std::vector<Tarefa*> MotorSimulacao::achaProxima(int quantCPUs)
{
	std::vector<Tarefa*> tarefasProntas;

	//encontra a tarefa com maior prioridade que esteja pronta para executar
	for (Tarefa& tarefa:tarefas)
	{
		//muda o estado da tarefa se estiver pronta para executar
		if (tarefa.getProximaLiberacao()<=tickAtual && tarefa.getTempoRestante()>0)
		{
			tarefa.setEstado(EstadoTarefa::PRONTA);
			tarefasProntas.push_back(&tarefa);
		}
	}

	//ordena com a funcao de comparacao as tarefas mais prioritarias de forma decrescente com base no algoritmo atual utilizando funcao lambda
	if(algoritmoSelecionado==TipoAlgoritmo::RATE_MONOTONIC)
	{
		std::sort(tarefasProntas.begin(), tarefasProntas.end(), [this](const Tarefa* a, const Tarefa* b) 
		{
			return comparaRM(a, b);
		});
	}

	else if(algoritmoSelecionado==TipoAlgoritmo::EARLIEST_DEADLINE_FIRST)
	{
		std::sort(tarefasProntas.begin(), tarefasProntas.end(), [this](const Tarefa* a, const Tarefa* b) 
		{
			return comparaEDF(a, b);
		});
	}

	//limita o vetor para retornar apenas a quantidade maxima de CPUs disponiveis
	if (tarefasProntas.size()>(quantCPUs))
	{
		tarefasProntas.resize(quantCPUs);
	}

	return tarefasProntas;
}

Historico MotorSimulacao::passoSimulacao(std::vector<Tarefa*> tarefasMaiorPrioridade)
{
	bool numIgual=true;

	for (Tarefa& t:tarefas)
	{
		t.geraNumeroAleatorio();
	}

	//garante numeros aleatorios diferentes para todas as tarefas
	while (numIgual)
	{
		numIgual=false;
		for (Tarefa& t:tarefas)
		{
			for (Tarefa& t2:tarefas)
			{
				if (&t!=&t2 && t.getNumAleatorio()==t2.getNumAleatorio()) //duas tarefas diferentes com num igual
				{
					t2.geraNumeroAleatorio();
					numIgual=true;
				}
			}
		}
	}

	Historico historicoAtual;
	//joga os eventos de desempate no historico e reseta depois
	historicoAtual.eventos=std::move(eventosDesempatePendentes);
	eventosDesempatePendentes.clear();

	//tarefa ingressou
	for (const Tarefa& tarefa:tarefas)
	{
		if (tarefa.getEstado()!=EstadoTarefa::TERMINADA && tarefa.getProximaLiberacao()==tickAtual)
		{
			EventoGantt ingresso;
			ingresso.tipo=TipoEvento::INGRESSO;
			ingresso.idTarefa=tarefa.getId();
			historicoAtual.eventos.push_back(ingresso);
		}
	}

	//atribuicao de CPUs
	if (tarefasMaiorPrioridade.empty())
	{
		for (int i=0;i<quantCPUs; i++)
		{
			todasCPUs[i].setTarefaAtual(nullptr); //tarefa nula na CPU
			todasCPUs[i].atualizaEstado(); //desliga CPU
		}
		historicoAtual.tarefasExecutadas.push_back(Tarefa()); //tarefa padrao para indicar que nada executou
	}
	else
	{
		for (int i=0;i<quantCPUs; i++)
		{
			//protege caso haja mais CPUs que tarefa
			if (i<tarefasMaiorPrioridade.size())
			{
				todasCPUs[i].setTarefaAtual(tarefasMaiorPrioridade[i]);
				todasCPUs[i].getTarefaAtual()->setEstado(EstadoTarefa::EXECUTANDO);
				(*todasCPUs[i].getTarefaAtual())--; //decrementa tempo restante

				historicoAtual.tarefasExecutadas.push_back(*todasCPUs[i].getTarefaAtual());

				//ve se houve preempcao
				if (todasCPUs[i].getTarefaAnterior()!=nullptr &&
					todasCPUs[i].getTarefaAtual()!=todasCPUs[i].getTarefaAnterior() &&
					todasCPUs[i].getTarefaAnterior()->getTempoRestante()>0)
				{
					historicoAtual.idTarefasPreemptadas.push_back(todasCPUs[i].getTarefaAnterior()->getId());
				}
			}
			//mais CPUs que tarefas
			else
			{
				todasCPUs[i].setTarefaAtual(nullptr);
				historicoAtual.tarefasExecutadas.push_back(Tarefa());
			}

			todasCPUs[i].atualizaEstado(); //liga ou desliga a CPU
		}
	}

	//atualiza o estado das tarefas e trata dos eventos quando necessario
	for (Tarefa& tarefa:tarefas)
	{
		//ignora tarefas terminadas
		if (tarefa.getEstado()==EstadoTarefa::TERMINADA)
		{
			continue;
		}

		//verifica deadlines perdidas
		if (tarefa.getProximoDeadline()==tickAtual && tarefa.getTempoRestante()>0)
		{
			tarefa.setEstado(EstadoTarefa::DEADLINE_PERDIDA);
			historicoAtual.idDeadlinesPerdidas.push_back(tarefa.getId());

			EventoGantt stouro;
			stouro.tipo=TipoEvento::DEADLINE_STOURO;
			stouro.idTarefa=tarefa.getId();
			historicoAtual.eventos.push_back(stouro);

			++tarefa; //incrementa numero de deadlines perdidas
		}
		//verifica conclusao do período/instancia atual
		else if (tarefa.getTempoRestante()==0)
		{
			tarefa++; //incrementa quantidade de execucoes concluidas

			// Seta azul: Registra o evento de conclusao a cada ocorrência/instância finalizada de uma tarefa
			EventoGantt conclusao;
			conclusao.tipo=TipoEvento::CONCLUSAO;
			conclusao.idTarefa=tarefa.getId();
			historicoAtual.eventos.push_back(conclusao);

			if (tarefa.getQuantidadeExecucoes()<10)
			{
				tarefa.setEstado(EstadoTarefa::SUSPENSA); //fica suspensa ate a chegada do proximo periodo
			}
			else
			{
				tarefa.setEstado(EstadoTarefa::TERMINADA);
			}
		}
		//incrementa tempo de espera se a tarefa estiver pronta mas nao foi executada neste tick, ou seja nao esta no vetor das tarefas
		else if (tarefa.getEstado()==EstadoTarefa::PRONTA &&
			std::find(tarefasMaiorPrioridade.begin(), tarefasMaiorPrioridade.end(), &tarefa)==tarefasMaiorPrioridade.end())
		{
			tarefa+=1;
		}
	}

	//registro do historico
	historicoAtual.tempo=tickAtual;
	for (const Tarefa& tarefa:tarefas)
	{
		historicoAtual.todasTarefas.push_back(tarefa.getSnapshot());
	}

	historicoAtual.tarefas=tarefas;
	historicoTimeline.push_back(historicoAtual);

	//atualiza a tarefa quando terminar um ciclo
	for (Tarefa& tarefa:tarefas)
	{
		if (tarefa.getEstado()==EstadoTarefa::SUSPENSA && tarefa.getTempoRestante()==0)
		{
			tarefa.preparaProximaExecucao(tickAtual);
		}
	}

	tickAtual++;

	return historicoAtual;
}

//atribui as prioridades estaticas para as tarefas
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

	for(const Tarefa& tarefa:tarefas)
	{
		utilizacao+=static_cast<double>(tarefa.getDuracao())/tarefa.getPeriodo();
	}

	return utilizacao;
}

//verifica a escalabilidade do conjunto de tarefas com base no algoritmo selecionado
bool MotorSimulacao::verificaEscalabilidade()
{
	double utilizacao=calculaUtilizacaoCPU();
	int n=tarefas.size();

	if(n==0) 
		return true;

	if(algoritmoSelecionado==TipoAlgoritmo::RATE_MONOTONIC)
	{
		//para mais que uma CPU, nao se aplica a formula de Liu & Layland
		if(quantCPUs>1)
		{
			return utilizacao<=static_cast<double>(quantCPUs);
		}

		//condicao suficiente do RM U<=n*(2^(1/n-1) para uma CPU
		double limite=n*(std::pow(2.0, 1.0/n)-1.0);
		return utilizacao<=limite;
	}
	else if(algoritmoSelecionado==TipoAlgoritmo::EARLIEST_DEADLINE_FIRST)
	{
		return utilizacao<=static_cast<double>(quantCPUs);
	}

	return false;
}

//usado no sort em preparaSimulacaoRM
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
	}

	while(executando)
	{
		//executa um passo da simulacao
		Historico historico=passoSimulacao(achaProxima(quantCPUs));
		//verifica se todas as tarefas terminaram
		executando=false;
		for(const Tarefa& tarefa:tarefas)
		{
			if(tarefa.getEstado()!=EstadoTarefa::TERMINADA)
			{
				executando=true;
				break;
			}
		}
		if(getTickAtual()>10000) //interrompe se demorar demais
		{
			executando=false;
		}
	}
}

//atualmente nao utilizada, possivelmente na entrega B
//avanca ou retrocede um determinado numero de passos
void MotorSimulacao::executaPassoAPasso(int instrucao)
{
	if(instrucao>0)
	{
		//avanca um determinado numero de passos
		for(int i=0;i<instrucao;i++)
		{
			Historico historico=passoSimulacao(achaProxima(quantCPUs));
		}
	}
	else if(instrucao<0)
	{
		//retrocede um determinado numero de passos
	}
}

//reseta a simulacao a partir de um tempo especifico
void MotorSimulacao::resetaAPartirDe(int tick)
{
	//reseta completamente por ser menor igual o comeco da simulacao
	if(tick<=0)
	{
		resetSimulacao();
		return;
	}
	//tick fora do limite atual
	if(tick>=historicoTimeline.size())
	{
		return;
	}

	//restaura o estado das tarefas usando o vetor salvo no historico daquele tick
	tarefas=historicoTimeline[tick].tarefas;

	//remove do historico tudo depois do tick
	historicoTimeline.erase(historicoTimeline.begin()+tick, historicoTimeline.end());

	//altera o relogio global para o tick
	tickAtual=tick;
}

//utilizado para ordenar as tarefas em achaProxima
bool MotorSimulacao::comparaEDF(const Tarefa* a, const Tarefa* b)
{
	//quem tiver o deadline mais proximo vem primeiro
	if(a->getProximoDeadline()!=b->getProximoDeadline())
	{
		return a->getProximoDeadline()<b->getProximoDeadline();
	}

	//desempate
	return desempate(a, b);
}

//utilizado para ordenar as tarefas em achaProxima
bool MotorSimulacao::comparaRM(const Tarefa* a, const Tarefa* b)
{
	//quem tiver maior prioridade vem primeiro
	if(a->getPrioridade()!=b->getPrioridade())
	{
		return a->getPrioridade()>b->getPrioridade();
	}

	//desempate
	return desempate(a, b);
}

//utilizada quando tarefas sao igualmente prioritarias
bool MotorSimulacao::desempate(const Tarefa* a, const Tarefa* b)
{
	//condicao 1 evita preemptar mantendo a tarefa que ja esta executando
	if(a->getEstado()==EstadoTarefa::EXECUTANDO && b->getEstado()!=EstadoTarefa::EXECUTANDO)
		return true;
	else if(b->getEstado()==EstadoTarefa::EXECUTANDO && a->getEstado()!=EstadoTarefa::EXECUTANDO)
		return false;

	//condicao 2 tarefa com o menor prazo
	if(a->getPrazo()!=b->getPrazo())
	{
		if(a->getPrazo()<b->getPrazo())
			return true;
		else
			return false;
	}

	//condicao 3 tarefa com o menor ingresso
	if(a->getIngresso()!=b->getIngresso())
	{
		if(a->getIngresso()<b->getIngresso())
			return true;
		else
			return false;
	}

	//condicao 4 tarefa com a menor duracao
	if(a->getDuracao()!=b->getDuracao())
	{
		if(a->getDuracao()<b->getDuracao())
			return true;
		else
			return false;
	}

	//condicao 5 aleatorio, armazena o evento que eventualmente eh passado
	EventoGantt desempate;
	desempate.tipo=TipoEvento::SORTEIO_DESEMPATE;

	if(a->getNumAleatorio()>b->getNumAleatorio())
	{
		desempate.idTarefa=a->getId();
		eventosDesempatePendentes.push_back(desempate);
		return true;
	}
	else
	{
		desempate.idTarefa=b->getId();
		eventosDesempatePendentes.push_back(desempate);
		return false;
	}
		
}