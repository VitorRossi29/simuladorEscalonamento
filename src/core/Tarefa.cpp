#include "Tarefa.hpp"

short int Tarefa::proximoId=1;

Tarefa::Tarefa()
{
	id=proximoId++;
	nome="null";
	tempoDeComputacao=-1;
	periodo=-1;
	deadlineRelativo=-1;
	inicio=-1;
	prioridadeEstatica=-1;
	cor="null";
	resetaTarefa();
}

Tarefa::Tarefa(std::string nome, int tempoDeComputacao, int periodo, int deadlineRelativo, int inicio, short int prioridadeEstatica, std::string cor)
{
	id=proximoId++;
	this->nome=nome;
	this->tempoDeComputacao=tempoDeComputacao;
	this->periodo=periodo;
	this->deadlineRelativo=deadlineRelativo;
	this->inicio=inicio;
	this->prioridadeEstatica=prioridadeEstatica;
	this->cor=cor;
	resetaTarefa();
}

Tarefa::~Tarefa()
{

}

void Tarefa::resetaTarefa()
{
	tempoRestante=tempoDeComputacao;
	proximoDeadline=inicio+deadlineRelativo;
	proximaLiberacao=inicio+periodo;
	deadlinesPerdidas=0;
	tempoEspera=0;
	estado=EstadoTarefa::PRONTA;
}
