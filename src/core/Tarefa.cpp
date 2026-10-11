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

Tarefa::~Tarefa()
{

}

void Tarefa::resetaTarefa()
{
	snapshot.tempoRestante=duracao;
	snapshot.proximoDeadline+=periodo;
	snapshot.proximaLiberacao+=periodo;
	snapshot.deadlinesPerdidas=0;
	snapshot.tempoEspera=0;
	snapshot.estado=EstadoTarefa::NOVA;
}

