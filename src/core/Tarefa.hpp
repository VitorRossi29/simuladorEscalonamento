#pragma once
#include <string>

enum class EstadoTarefa
{
	NOVA,
    PRONTA,
    EXECUTANDO,
    ESPERANDO,
    DEADLINE_PERDIDA,
	TERMINADA
};

enum class TipoAlgoritmo
{
    RATE_MONOTONIC,
    EARLIEST_DEADLINE_FIRST
};

class Tarefa
{
private:
    //estáticos
    short int id;
    int duracao;
    int periodo;
    int prazo;
    int ingresso;
    short int prioridade;
	std::string cor;

    //dinâmicos
	int tempoRestante;
	int proximoDeadline;
	int proximaLiberacao;
    int deadlinesPerdidas;
	int tempoEspera;
	EstadoTarefa estado;
	int quantidadeExecucoes;

public:
    Tarefa();
	Tarefa(short int id, int duracao, int periodo, int prazo, int ingresso, short int prioridadeEstatica, std::string cor);
    ~Tarefa();
	void resetaTarefa();
	void operator--(int) { tempoRestante--; }
	void operator++(int) { quantidadeExecucoes++; }
	void operator++() { deadlinesPerdidas++; }
	void operator+=(int tempo) { tempoEspera += tempo; }

	short int getId() const { return id; }
	int getDuracao() const { return duracao; }
	int getPeriodo() const { return periodo; }
	int getPrazo() const { return prazo; }
    int getIngresso() const { return ingresso; }
	short int getPrioridade() const { return prioridade; }
	void setPrioridade(short int prioridade) { this->prioridade=prioridade; }
	int getTempoRestante() const { return tempoRestante; }
	void setTempoRestante(int tempo) { tempoRestante=tempo; }
	int getProximoDeadline() const { return proximoDeadline; }
	int getProximaLiberacao() const { return proximaLiberacao; }
	int getDeadlinesPerdidas() const { return deadlinesPerdidas; }
	int getTempoEspera() const { return tempoEspera; }
	int getQuantidadeExecucoes() const { return quantidadeExecucoes; }
	EstadoTarefa getEstado() const { return estado; }
	void setEstado(EstadoTarefa estado) { this->estado=estado; }
};


