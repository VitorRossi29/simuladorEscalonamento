#include "Tarefa.hpp"
#include <random>

static int idPronto=0; //usado ao gerar tarefas aleatorias

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

	//gera 6 caracteres e usa para o codigo hexadecimal da cor
	const char caracteres[]="0123456789ABCDEF";

	std::string corAleatoria="#";
	std::uniform_int_distribution<int> disCaracter(0, 15);

	for (int i=0;i<6;i++) 
	{
		corAleatoria+=caracteres[disCaracter(gen)];
	}

	cor=corAleatoria;

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

//reseta os atributos dinamicos
void Tarefa::resetaTarefa()
{
	snapshot.tempoRestante=duracao;
	snapshot.proximoDeadline=ingresso+prazo;
	snapshot.proximaLiberacao=ingresso;
	snapshot.deadlinesPerdidas=0;
	snapshot.tempoEspera=0;
	snapshot.quantidadeExecucoes=0;

	snapshot.estado=EstadoTarefa::NOVA;
}

//usado quando termina uma execucao
void Tarefa::preparaProximaExecucao(int tickAtual)
{
	//reseta a duracao para proxima execucao
	snapshot.tempoRestante=duracao;

	//impede acumulo de liberacoes do passado se a tarefa esteve suspensa
	while (getProximaLiberacao()<=tickAtual)
	{
		snapshot.proximaLiberacao+=periodo;
	}

	//atualiza o proximo deadline
	snapshot.proximoDeadline=snapshot.proximaLiberacao+prazo;
}

//inicializa o numero aleatorio usado pro desempate de cada tarefa
void Tarefa::geraNumeroAleatorio()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(-2000000, 2000000);
	numAleatorio=dis(gen);
}

//utiliza mesma logica da construtora aleatoria porem evita cores mais escuras sem o 0,1 e 2
void Tarefa::resetaCor()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	const char caracteres[]="3456789ABCDEF";
	std::uniform_int_distribution<int> disCaracter(0, 12);

	std::string corAleatoria="#";
	for (int i=0;i<6;i++)
	{
		corAleatoria+=caracteres[disCaracter(gen)];
	}

	cor=corAleatoria;
}

