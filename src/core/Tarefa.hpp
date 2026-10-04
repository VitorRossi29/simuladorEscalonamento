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

struct Tarefa
{
private:
    //estáticos
    static short int proximoId;
    short int id;

    std::string nome;
    int tempoDeComputacao;
    int periodo;
    int deadlineRelativo;
    int inicio;
    short int prioridadeEstatica;
	std::string cor;

    //dinâmicos
	int tempoRestante;
	int proximoDeadline;
	int proximaLiberacao;
    int deadlinesPerdidas;
	int tempoEspera;
	EstadoTarefa estado;

public:
    Tarefa();
	Tarefa(std::string nome, int tempoDeComputacao, int periodo, int deadlineRelativo, int inicio, short int prioridadeEstatica, std::string cor);
    ~Tarefa();
	void resetaTarefa();

	short int getId() const { return id; }
	int getTempoDeComputacao() const { return tempoDeComputacao; }
	int getPeriodo() const { return periodo; }
	int getDeadlineRelativo() const { return deadlineRelativo; }
    int getInicio() const { return inicio; }
	short int getPrioridadeEstatica() const { return prioridadeEstatica; }
	int getTempoRestante() const { return tempoRestante; }
	int getProximoDeadline() const { return proximoDeadline; }
	int getProximaLiberacao() const { return proximaLiberacao; }
	int getDeadlinesPerdidas() const { return deadlinesPerdidas; }
	int getTempoEspera() const { return tempoEspera; }
	EstadoTarefa getEstado() const { return estado; }
};


