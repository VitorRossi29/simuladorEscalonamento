#include "MotorSimulacao.hpp"
#include <algorithm>
#include <random>

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

	//cria as CPUs de acordo com a quantidade estabelecida 
	todasCPUs.assign(quantCPUs, CPU());

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
	Historico historicoAtual;

	//nenhuma tarefa pronta para executar
	if(tarefasMaiorPrioridade.empty())
	{
		historicoAtual.tempo=tickAtual;
		for (int i=0;i<quantCPUs;i++)
		{
			todasCPUs[i].setTarefaAtual(nullptr); //indica que nenhuma tarefa esta sendo executada na CPU
			todasCPUs[i].atualizaEstado(); //muda para OCIOSA e incrementa tempo ociosa
		}

		//cria uma tarefa com a construtora sem parametro para indicar que nenhuma tarefa foi executada nesse tick
		historicoAtual.tarefasExecutadas.push_back(Tarefa());
	}
	else
	{
		//armazena a tarefa depois de decrementar o tempo restante
		for (Tarefa* t:tarefasMaiorPrioridade)
		{
			t->setEstado(EstadoTarefa::EXECUTANDO);
			(*t)--; //Decrementa o tempo restante
			historicoAtual.tarefasExecutadas.push_back(*t); //tarefas que serao executadas nesse tick
		}
		
		//atribui as tarefas para as CPUs, ve se foram preemptadas e incrementa tempo ocioso
		for(int i=0;i<quantCPUs;i++)
		{
			if (i<tarefasMaiorPrioridade.size())
			{
				todasCPUs[i].setTarefaAtual(tarefasMaiorPrioridade[i]); //atribui as tarefas

				if (todasCPUs[i].getTarefaAnterior()!=nullptr && //tarefa anterior nao nula
					todasCPUs[i].getTarefaAtual()!=todasCPUs[i].getTarefaAnterior() && //tarefa atual diferente da anterior
					todasCPUs[i].getTarefaAnterior()->getTempoRestante()>0) //tarefa anterior ainda nao terminou
				{
					historicoAtual.idTarefasPreemptadas.push_back(todasCPUs[i].getTarefaAnterior()->getId()); //foi preemptada
				}
			}
			
			else
			{ 
				todasCPUs[i].setTarefaAtual(nullptr);
			}

			todasCPUs[i].atualizaEstado(); //muda para EXECUTANDO ou OCIOSA e incrementa tempo ociosa se preciso
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
			//find procura a tarefa no vetor, se nao achar retorna o final do vetor, se achar retorna o ponteiro para a tarefa
			else if(tarefa.getEstado()==EstadoTarefa::PRONTA &&
				std::find(tarefasMaiorPrioridade.begin(), tarefasMaiorPrioridade.end(), &tarefa)==tarefasMaiorPrioridade.end())
			{
				tarefa+=1;
			}
		}
	}
	historicoAtual.tempo=tickAtual;
	for(const Tarefa& tarefa:tarefas)
	{
		historicoAtual.todasTarefas.push_back(tarefa.getSnapshot());
	}

	historicoAtual.tarefas=tarefas;
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
	}

	while(executando)
	{
		//executa um passo da simulacao
		Historico historico=passoSimulacao(achaProxima(quantCPUs));
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
		if(getTickAtual()>1000)
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
	//Tick fora do limite atual
	if(tick>=static_cast<int>(historicoTimeline.size()))
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
	if (a->getProximoDeadline()!=b->getProximoDeadline())
	{
		return a->getProximoDeadline()<b->getProximoDeadline();
	}

	//desempate
	return desempate(a, b);
}

bool MotorSimulacao::comparaRM(const Tarefa* a, const Tarefa* b)
{
	//quem tiver o periodo mais curto vem primeiro (prioridade ja depende do periodo no RM)
	if (a->getPrioridade()!=b->getPrioridade())
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
	if (a->getPrazo()!=b->getPrazo())
	{
		if(a->getPrazo()<b->getPrazo())
			return true;
		else
			return false;
	}

	//condicao 3 tarefa com o menor ingresso
	if (a->getIngresso()!=b->getIngresso())
	{
		if(a->getIngresso()<b->getIngresso())
			return true;
		else
			return false;
	}

	//condicao 4 tarefa com a menor duracao
	if (a->getDuracao()!=b->getDuracao())
	{
		if(a->getDuracao()<b->getDuracao())
			return true;
		else
			return false;
	}

	//condicao 5 aleatorio, precisa ter elemento grafico
	std::random_device rd; //gera uma semente utilizando o proprio hardware do sistema
	std::mt19937 gen(rd()); //inicializa o motor com a semente criada

	std::uniform_int_distribution<int> dis(0, 1); //estabelece as possibilidades como 0 ou 1

	int escolha=dis(gen); //gera o numero

	if(escolha) //==1
		return true;
	else //==0
		return false;
}