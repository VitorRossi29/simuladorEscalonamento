#include "Tarefa.hpp"

Tarefa::Tarefa()
{
	id=-1;
	duracao=-1;
	periodo=-1;
	prazo=-1;
	ingresso=-1;
	prioridade=-1;
	cor="null";
	quantidadeExecucoes=0;
	resetaTarefa();
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
	quantidadeExecucoes=0;
	resetaTarefa();
}

Tarefa::~Tarefa()
{

}

void Tarefa::resetaTarefa()
{
	tempoRestante=duracao;
	proximoDeadline=ingresso+prazo;
	proximaLiberacao=ingresso+periodo;
	deadlinesPerdidas=0;
	tempoEspera=0;
	estado=EstadoTarefa::PRONTA;
}

//Sobrecarga de operador para decrementar fácil o tempo
void Tarefa::operator--(int)
{
	tempoRestante--;
}
