#pragma once
#include <string>

enum class EstadoTarefa
{
	NOVA, //estado inicial
    PRONTA, 
    EXECUTANDO,
    SUSPENSA, //quando a tarefa nao esta disponivel ou seja nao teve o ingresso ou a proxima liberacao ainda
    DEADLINE_PERDIDA, //estado temporario para tratar de quando passa do prazo
	TERMINADA
};

enum class TipoAlgoritmo
{
    RATE_MONOTONIC,
    EARLIEST_DEADLINE_FIRST,
	OUTRO
};

struct SnapshotTarefaMotor
{
	int tempoRestante; //duracao total menos quanto ja executou
	int proximoDeadline;
	int proximaLiberacao;
	int deadlinesPerdidas;
	int tempoEspera; //tempo pronta e nao executando
	EstadoTarefa estado;
	int quantidadeExecucoes; //usado para controlar quando alcanca 10 execucoes e termina
	int ultimaCPUExecutada; //id da ultima CPU pra ver se foi preemptada 
};

class Tarefa
{
private:
    //estaticos
    short int id;
    int duracao;
    int periodo;
    int prazo;
    int ingresso;
    short int prioridade;
	std::string cor;
	int numAleatorio=0; //usado no desempate

    //dinamicos
	SnapshotTarefaMotor snapshot;

public:
    Tarefa(); //construtora inicia tudo com -1, usado para quando nao houve tarefas para executar
	Tarefa(short int id, int duracao, int periodo, int prazo, int ingresso, short int prioridadeEstatica, std::string cor); //inicia propriamente
	Tarefa(int valorMaximo); //inicia com valores aleatorios, valorMaximo eh usado para definir o ingresso maximo
    ~Tarefa();
	void resetaTarefa();
	void preparaProximaExecucao(int tickAtual);
	void geraNumeroAleatorio();
	void resetaCor();

	//sobrecargas para modificar facilmente
	void operator--(int) { snapshot.tempoRestante--; }
	void operator++(int) { snapshot.quantidadeExecucoes++; }
	void operator++() { snapshot.deadlinesPerdidas++; }
	void operator+=(int tempo) { snapshot.tempoEspera+=tempo; }

	//gets e sets
	short int getId() const { return id; }
	void setID(short int id) { this->id=id; }
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
	SnapshotTarefaMotor getSnapshot() const { return snapshot; }
	const std::string& getCor() const { return cor; }
	void setCor(const std::string& novaCor) { cor=novaCor; }
	void setEstado(EstadoTarefa estado) { snapshot.estado=estado; }
	void setIngresso(int ing) { ingresso=ing; }
	void setDuracao(int dura) { duracao=dura; }
	void setPeriodo(int per) { periodo=per; }
	void setPrazo(int praz) { prazo=praz; }
	int getNumAleatorio() const { return numAleatorio; }
};


