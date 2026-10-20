#include "MotorSimulacao.hpp"
#include <algorithm>
#include <random>
#include "../gui/StructsTeste.hpp"

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

	// Limpa o vetor de CPUS para cada reset
	todasCPUs.clear(); // Vitor inseriu

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

	//ordena com a funcao de comparacao as tarefas mais prioritarias de forma decrescente com base no algoritmo atual
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
	bool numIgual = true;

	for (Tarefa& t : tarefas)
	{
		t.geraNumeroAleatorio();
	}

	while (numIgual)
	{
		numIgual = false;
		for (Tarefa& t : tarefas)
		{
			for (Tarefa& t2 : tarefas)
			{
				if (&t != &t2 && t.getNumAleatorio() == t2.getNumAleatorio()) //duas tarefas diferentes com num igual
				{
					t2.geraNumeroAleatorio();
					numIgual = true;
				}
			}
		}
	}

	////nenhuma tarefa pronta para executar
	//if(tarefasMaiorPrioridade.empty())
	//{
	//	historicoAtual.tempo=tickAtual;
	//	for (int i=0;i<quantCPUs;i++)
	//	{
	//		todasCPUs[i].setTarefaAtual(nullptr); //indica que nenhuma tarefa esta sendo executada na CPU
	//		todasCPUs[i].atualizaEstado(); //muda para DESLIGADA e incrementa tempo desligada
	//	}

	//	//cria uma tarefa com a construtora sem parametro para indicar que nenhuma tarefa foi executada nesse tick
	//	historicoAtual.tarefasExecutadas.push_back(Tarefa());
	//}
	//else
	//{	
	//	//atribui as tarefas para as CPUs, ve se foram preemptadas e incrementa tempo desligada
	//	for(int i=0;i<quantCPUs;i++)
	//	{
	//		if(i<tarefasMaiorPrioridade.size())
	//		{
	//			todasCPUs[i].setTarefaAtual(tarefasMaiorPrioridade[i]); //atribui a tarefa
	//			todasCPUs[i].getTarefaAtual()->setEstado(EstadoTarefa::EXECUTANDO); //muda o estado da tarefa para EXECUTANDO
	//			(*todasCPUs[i].getTarefaAtual())--; //decrementa o tempo restante da tarefa

	//			historicoAtual.tarefasExecutadas.push_back(*todasCPUs[i].getTarefaAtual()); //armazena a tarefa no historico

	//			if(todasCPUs[i].getTarefaAnterior()!=nullptr && //tarefa anterior nao nula
	//				todasCPUs[i].getTarefaAtual()!=todasCPUs[i].getTarefaAnterior() && //tarefa atual diferente da anterior
	//				todasCPUs[i].getTarefaAnterior()->getTempoRestante()>0) //tarefa anterior ainda nao terminou
	//			{
	//				historicoAtual.idTarefasPreemptadas.push_back(todasCPUs[i].getTarefaAnterior()->getId()); //foi preemptada
	//			}
	//		}
	//		
	//		//tem mais CPUs que tarefas
	//		else
	//		{ 
	//			todasCPUs[i].setTarefaAtual(nullptr);
	//			historicoAtual.tarefasExecutadas.push_back(Tarefa()); //coloca tarefa vazia
	//		}

	//		todasCPUs[i].atualizaEstado(); //muda para EXECUTANDO ou DESLIGADA e incrementa tempo desligada se preciso
	//	}

	//	for(Tarefa& tarefa:tarefas)
	//	{
	//		//verifica deadlines perdidas
	//		if(tarefa.getProximoDeadline()==tickAtual && tarefa.getTempoRestante()>0)
	//		{
	//			tarefa.setEstado(EstadoTarefa::DEADLINE_PERDIDA);
	//			historicoAtual.idDeadlinesPerdidas.push_back(tarefa.getId());

	//			// Registro do evento de estouro
	//			EventoGantt stouro;
	//			stouro.tipo = TipoEvento::DEADLINE_STOURO;
	//			stouro.idTarefa = tarefa.getId();
	//			historicoAtual.eventos.push_back(stouro);

	//			++tarefa; //incrementa o numero de deadlines perdidas
	//		}
	//		//verifica tarefas terminadas
	//		else if(tarefa.getTempoRestante()==0)
	//		{
	//			if(tarefa.getQuantidadeExecucoes()<10)
	//			{
	//				tarefa++; //incrementa quantidade de execucoes
	//				tarefa.preparaProximaExecucao();
	//			}
	//			else
	//			{
	//				// Registro do evento de conclusao
	//				EventoGantt conclusao;
	//				conclusao.tipo = TipoEvento::CONCLUSAO;
	//				conclusao.idTarefa = tarefa.getId();
	//				historicoAtual.eventos.push_back(conclusao);

	//				tarefa.setEstado(EstadoTarefa::TERMINADA);
	//			}
	//				
	//		}
	//		//find procura a tarefa no vetor, se nao achar retorna o final do vetor, se achar retorna o ponteiro para a tarefa
	//		else if(tarefa.getEstado()==EstadoTarefa::PRONTA &&
	//			std::find(tarefasMaiorPrioridade.begin(), tarefasMaiorPrioridade.end(), &tarefa)==tarefasMaiorPrioridade.end())
	//		{
	//			tarefa+=1; //incrementa o tempo de espera da tarefa
	//		}
	//	}
	//}
	//historicoAtual.tempo=tickAtual;
	//for(const Tarefa& tarefa:tarefas)
	//{
	//	historicoAtual.todasTarefas.push_back(tarefa.getSnapshot());

	//	// Sinaliza o ingresso da tarefa no momento adequado
	//	if (tarefa.getIngresso() == tickAtual)
	//	{
	//		EventoGantt ingresso;
	//		ingresso.tipo = TipoEvento::INGRESSO;
	//		ingresso.idTarefa = tarefa.getId();
	//		historicoAtual.eventos.push_back(ingresso);
	//	}
	//}

	//historicoAtual.tarefas=tarefas;
	//historicoTimeline.push_back(historicoAtual);
	//tickAtual++;
	//return historicoAtual;

	Historico historicoAtual;
	historicoAtual.eventos = std::move(eventosDesempatePendentes);
	eventosDesempatePendentes.clear();

	// ALTERAcaO/CORREcaO (Seta Verde - Registro Antecipado):
	// O registro do ingresso (seta verde) DEVE ser feito AQUI, no início do tick, ANTES que a CPU execute.
	// Se a tarefa tiver duracao 1, ela sera concluída no mesmo tick e a funcao preparaProximaExecucao()
	// alterara a proximaLiberacao. Registrar o ingresso aqui garante que tarefas com C=1 recebam a seta verde.
	for (const Tarefa& tarefa : tarefas)
	{
		if (tarefa.getEstado() != EstadoTarefa::TERMINADA && tarefa.getProximaLiberacao() == tickAtual)
		{
			EventoGantt ingresso;
			ingresso.tipo = TipoEvento::INGRESSO;
			ingresso.idTarefa = tarefa.getId();
			historicoAtual.eventos.push_back(ingresso);
		}
	}

	// 1. Atribuicao de CPUs
	if (tarefasMaiorPrioridade.empty())
	{
		for (int i = 0; i < quantCPUs; i++)
		{
			todasCPUs[i].setTarefaAtual(nullptr);
			todasCPUs[i].atualizaEstado();
		}
		historicoAtual.tarefasExecutadas.push_back(Tarefa());
	}
	else
	{
		for (int i = 0; i < quantCPUs; i++)
		{
			if (i < tarefasMaiorPrioridade.size())
			{
				todasCPUs[i].setTarefaAtual(tarefasMaiorPrioridade[i]);
				todasCPUs[i].getTarefaAtual()->setEstado(EstadoTarefa::EXECUTANDO);
				(*todasCPUs[i].getTarefaAtual())--; // Decrementa tempo restante

				historicoAtual.tarefasExecutadas.push_back(*todasCPUs[i].getTarefaAtual());

				if (todasCPUs[i].getTarefaAnterior() != nullptr &&
					todasCPUs[i].getTarefaAtual() != todasCPUs[i].getTarefaAnterior() &&
					todasCPUs[i].getTarefaAnterior()->getTempoRestante() > 0)
				{
					historicoAtual.idTarefasPreemptadas.push_back(todasCPUs[i].getTarefaAnterior()->getId());
				}
			}
			else
			{
				todasCPUs[i].setTarefaAtual(nullptr);
				historicoAtual.tarefasExecutadas.push_back(Tarefa());
			}

			todasCPUs[i].atualizaEstado();
		}
	}

	// 2. Atualizacao de Ciclo das Tarefas (RODA EM TODOS OS TICKS)
	for (Tarefa& tarefa : tarefas)
	{
		// Ignora tarefas que ja encerraram seu ciclo de vida
		if (tarefa.getEstado() == EstadoTarefa::TERMINADA)
		{
			continue;
		}

		// Verifica deadlines perdidas
		if (tarefa.getProximoDeadline() == tickAtual && tarefa.getTempoRestante() > 0)
		{
			tarefa.setEstado(EstadoTarefa::DEADLINE_PERDIDA);
			historicoAtual.idDeadlinesPerdidas.push_back(tarefa.getId());

			EventoGantt stouro;
			stouro.tipo = TipoEvento::DEADLINE_STOURO;
			stouro.idTarefa = tarefa.getId();
			historicoAtual.eventos.push_back(stouro);

			++tarefa; // Incrementa número de deadlines perdidas
		}
		// Verifica conclusao do período/instância atual
		else if (tarefa.getTempoRestante() == 0)
		{
			tarefa++; // Incrementa quantidade de execucões concluídas

			// Seta azul: Registra o evento de conclusao a cada ocorrência/instância finalizada de uma tarefa
			EventoGantt conclusao;
			conclusao.tipo = TipoEvento::CONCLUSAO;
			conclusao.idTarefa = tarefa.getId();
			historicoAtual.eventos.push_back(conclusao);

			if (tarefa.getQuantidadeExecucoes() < 10)
			{
				tarefa.preparaProximaExecucao(tickAtual); // Prepara prohximo ciclo (soma período e atualiza proximaLiberacao)
				tarefa.setEstado(EstadoTarefa::SUSPENSA); // Fica suspensa ate a chegada do proximo periodo
			}
			else
			{
				tarefa.setEstado(EstadoTarefa::TERMINADA);
			}
		}
		// Incrementa tempo de espera se a tarefa estiver pronta mas nao foi executada neste tick
		else if (tarefa.getEstado() == EstadoTarefa::PRONTA &&
			std::find(tarefasMaiorPrioridade.begin(), tarefasMaiorPrioridade.end(), &tarefa) == tarefasMaiorPrioridade.end())
		{
			tarefa += 1;
		}
	}

	// 3. Registro do Histohrico
	historicoAtual.tempo = tickAtual;
	for (const Tarefa& tarefa : tarefas)
	{
		historicoAtual.todasTarefas.push_back(tarefa.getSnapshot());
		// NOTA: A verificacao de ingresso que ficava aqui foi movida para o início do método (Passo 0)
	}

	historicoAtual.tarefas = tarefas;
	historicoTimeline.push_back(historicoAtual);
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

		//condicao suficiente do RM U<=n*(2^(1/n -1) para uma CPU
		double limite=n*(std::pow(2.0, 1.0/n)-1.0);
		return utilizacao<=limite;
	}
	else if(algoritmoSelecionado==TipoAlgoritmo::EARLIEST_DEADLINE_FIRST)
	{
		return utilizacao<=static_cast<double>(quantCPUs);
	}

	return false;
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
		if(getTickAtual()>1000) //interrompe se demorar demais
		{
			executando=false;
		}
	}
}

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

	//remove do historico tudo o que esta depois do tick
	historicoTimeline.erase(historicoTimeline.begin()+tick, historicoTimeline.end());

	//altera o relogio global para o tick
	tickAtual=tick;
}

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

bool MotorSimulacao::comparaRM(const Tarefa* a, const Tarefa* b)
{
	//quem tiver o periodo mais curto vem primeiro (prioridade ja depende do periodo no RM)
	if(a->getPrioridade()!=b->getPrioridade())
	{
		return a->getPrioridade()>b->getPrioridade();
	}

	//desempate
	return desempate(a, b);
}

//sort precisa de uma funcao que diz se a deve vir antes de b
//logo se a eh escolhida retorna true, se b retorna false
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

	//condicao 5 aleatorio, precisa ter elemento grafico
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