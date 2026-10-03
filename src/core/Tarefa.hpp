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
    ~Tarefa();
};


