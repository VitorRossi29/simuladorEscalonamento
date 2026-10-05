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
	quantidadeExecucoes=-1;
	proximoDeadline=-1;
	proximaLiberacao=-1;
	deadlinesPerdidas=-1;
	tempoEspera=-1;
	tempoRestante=-1;
	estado=EstadoTarefa::NOVA;
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
	proximaLiberacao=ingresso;
	proximoDeadline=ingresso+prazo;
	quantidadeExecucoes=0;
	deadlinesPerdidas=0;
	tempoEspera=0;
	tempoRestante=duracao;
	estado=EstadoTarefa::NOVA;
}

Tarefa::~Tarefa()
{

}

void Tarefa::resetaTarefa()
{
	tempoRestante=duracao;
	proximoDeadline+=periodo;
	proximaLiberacao+=periodo;
	deadlinesPerdidas=0;
	tempoEspera=0;
	estado=EstadoTarefa::NOVA;
}

