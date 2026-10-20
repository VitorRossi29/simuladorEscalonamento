#include "Tarefa.hpp"
#include <random>

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

	//gera 6 caracteres e usa para o codigo hexadecimal da cor
	const char caracteres[]="0123456789ABCDEF";

	std::string corAleatoria="";
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

void Tarefa::resetaTarefa()
{
	snapshot.tempoRestante=duracao;
	snapshot.proximoDeadline=ingresso+prazo;
	snapshot.proximaLiberacao=ingresso;
	snapshot.deadlinesPerdidas=0;
	snapshot.tempoEspera=0;

	//Mudanca feita pelo vitor que pode estar errada
	snapshot.quantidadeExecucoes = 0;

	snapshot.estado=EstadoTarefa::NOVA;
}

void Tarefa::preparaProximaExecucao(int tickAtual)
{
	// Reseta a duracao para proxima execucao
	snapshot.tempoRestante=duracao;

	// Impede acumulo de liberacoes do passado se a tarefa esteve suspensa
	while (getProximaLiberacao() <= tickAtual)
	{
		snapshot.proximaLiberacao += periodo;
		
	}
	
	// Troquei para PRAZO mas verificar se esta certo
	// snapshot.proximoDeadline += periodo;

	// Atualizacao do proximo deadline
	snapshot.proximoDeadline += prazo;
}

void Tarefa::geraNumeroAleatorio()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(-2000000, 2000000);
	numAleatorio=dis(gen);
}

void Tarefa::resetaCor()
{
	/*
	std::random_device rd; //gera uma semente utilizando o proprio hardware do sistema
	std::mt19937 gen(rd()); //inicializa o motor com a semente criada

	const char caracteres[]="0123456789ABCDEF";

	std::string corAleatoria="";
	std::uniform_int_distribution<int> disCaracter(0, 15);

	for (int i=0;i<6;i++) 
	{
		corAleatoria+=caracteres[disCaracter(gen)];
	}

	cor=corAleatoria;
	*/

	// Usar 'static' faz com que o gerador seja inicializado uma única vez na memória
	static std::random_device rd;
	static std::mt19937 gen(rd());

	// Evita tons extremamente escuros para não sumir no fundo do Gantt (dígitos de 3 a F)
	const char caracteres[] = "3456789ABCDEF";
	std::uniform_int_distribution<int> disCaracter(0, 12);

	std::string corAleatoria = "#"; // Inclui o prefixo # obrigatório
	for (int i = 0; i < 6; i++)
	{
		corAleatoria += caracteres[disCaracter(gen)];
	}

	cor = corAleatoria;
}

