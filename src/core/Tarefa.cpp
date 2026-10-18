#include "Tarefa.hpp"
# include <random>

static int idPronto=0;

Tarefa::Tarefa()
{
	id=-1;
	duracao=-1;
	periodo=-1;
	prazo=-1;
	ingresso=-1;
	prioridade=-1;
	cor="null";

	snapshot.quantidadeExecucoes=-1;
	snapshot.proximoDeadline=-1;
	snapshot.proximaLiberacao=-1;
	snapshot.deadlinesPerdidas=-1;
	snapshot.tempoEspera=-1;
	snapshot.tempoRestante=-1;
	snapshot.estado=EstadoTarefa::NOVA;
}

Tarefa::Tarefa(short int id, int duracao, int periodo, int prazo, int ingresso, short int prioridade, std::string cor)
{
	this->id=id;
	this->duracao=duracao;
	this->periodo=periodo;
	this->prazo=prazo;
	this->ingresso=ingresso;
	this->prioridade=prioridade;
	this->cor=cor;

	snapshot.proximaLiberacao=ingresso;
	snapshot.proximoDeadline=ingresso+prazo;
	snapshot.quantidadeExecucoes=0;
	snapshot.deadlinesPerdidas=0;
	snapshot.tempoEspera=0;
	snapshot.tempoRestante=duracao;
	snapshot.estado=EstadoTarefa::NOVA;
}

//gera uma tarefa com valores prontos
Tarefa::Tarefa(int ingressoMaximo)
{
	std::random_device rd; //gera uma semente utilizando o proprio hardware do sistema
	std::mt19937 gen(rd()); //inicializa o motor com a semente criada

	std::uniform_int_distribution<int> disIngresso(0, ingressoMaximo); //estabelece o intervalo possivel de ingresso
	ingresso=disIngresso(gen);

	//garante que prazo sempre eh menor que periodo e duracao sempre menor que o prazo
	std::uniform_int_distribution<int> disTempo(6, 10); 
	periodo=disTempo(gen);
	prazo=periodo-1;
	duracao=prazo-2;

	std::vector<std::string> coresDisponiveis={"Azul", "Vermelho", "Verde", "Amarelo", "Roxo", "Laranja"}; //cores possiveis
	std::uniform_int_distribution<int> disCor(0, coresDisponiveis.size()-1);
	cor=coresDisponiveis[disCor(gen)];

	id=idPronto++; //garante ids diferente para todas inicialmente
	prioridade=-1; //caso o algoritmo utilize o proprio calcula

	snapshot.quantidadeExecucoes=0;
	snapshot.proximoDeadline=ingresso+prazo;
	snapshot.proximaLiberacao=ingresso;
	snapshot.deadlinesPerdidas=0;
	snapshot.tempoEspera=0;
	snapshot.tempoRestante=duracao;
	snapshot.estado=EstadoTarefa::NOVA;
}

Tarefa::~Tarefa()
{

}

void Tarefa::resetaTarefa()
{
	snapshot.tempoRestante=duracao;
	snapshot.proximoDeadline=ingresso+prazo;
	snapshot.proximaLiberacao=ingresso;
	snapshot.deadlinesPerdidas=0;
	snapshot.tempoEspera=0;
	snapshot.estado=EstadoTarefa::NOVA;
}

void Tarefa::preparaProximaExecucao()
{
	snapshot.tempoRestante=duracao;
	snapshot.proximoDeadline+=periodo;
	snapshot.proximaLiberacao+=periodo;
	snapshot.estado=EstadoTarefa::NOVA;
}

void Tarefa::geraNumeroAleatorio()
{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(-2000000, 2000000);
		numAleatorio=dis(gen);
}

