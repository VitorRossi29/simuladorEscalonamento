#pragma once
#include <string>

enum class EstadoTarefa
{
    PRONTA,
    EXECUTANDO,
    ESPERANDO,
    DEADLINE_PERDIDA
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
	void operator--(int);

	short int getId() const { return id; }
	int getTempoDeComputacao() const { return duracao; }
	int getPeriodo() const { return periodo; }
	int getPrazo() const { return prazo; }
    int getIngresso() const { return ingresso; }
	short int getPrioridade() const { return prioridade; }
	void setPrioridade(short int prioridade) { prioridade=prioridade; }
	int getTempoRestante() const { return tempoRestante; }
	int getProximoDeadline() const { return proximoDeadline; }
	int getProximaLiberacao() const { return proximaLiberacao; }
	int getDeadlinesPerdidas() const { return deadlinesPerdidas; }
	int getTempoEspera() const { return tempoEspera; }
	EstadoTarefa getEstado() const { return estado; }
};


