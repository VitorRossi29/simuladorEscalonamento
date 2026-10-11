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

struct SnapshotTarefas
{
	int tempoRestante;
	int proximoDeadline;
	int proximaLiberacao;
	int deadlinesPerdidas;
	int tempoEspera;
	EstadoTarefa estado;
	int quantidadeExecucoes;
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
	SnapshotTarefas snapshot;

public:
    Tarefa();
	Tarefa(short int id, int duracao, int periodo, int prazo, int ingresso, short int prioridadeEstatica, std::string cor);
    ~Tarefa();
	void resetaTarefa();
	void operator--(int) { snapshot.tempoRestante--; }
	void operator++(int) { snapshot.quantidadeExecucoes++; }
	void operator++() { snapshot.deadlinesPerdidas++; }
	void operator+=(int tempo) { snapshot.tempoEspera+=tempo; }

	short int getId() const { return id; }
	int getDuracao() const { return duracao; }
	int getPeriodo() const { return periodo; }
	int getPrazo() const { return prazo; }
    int getIngresso() const { return ingresso; }
	short int getPrioridade() const { return prioridade; }
	void setPrioridade(short int prioridade) { this->prioridade=prioridade; }
	int getTempoRestante() const { return snapshot.tempoRestante; }
	void setTempoRestante(int tempo) { snapshot.tempoRestante=tempo; }
	int getProximoDeadline() const { return snapshot.proximoDeadline; }
	int getProximaLiberacao() const { return snapshot.proximaLiberacao; }
	int getDeadlinesPerdidas() const { return snapshot.deadlinesPerdidas; }
	int getTempoEspera() const { return snapshot.tempoEspera; }
	int getQuantidadeExecucoes() const { return snapshot.quantidadeExecucoes; }
	EstadoTarefa getEstado() const { return snapshot.estado; }
	SnapshotTarefas getSnapshot() const { return snapshot; }
	void setEstado(EstadoTarefa estado) { snapshot.estado=estado; }
};


