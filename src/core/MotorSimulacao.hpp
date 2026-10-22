#pragma once
#include <vector>
#include "Tarefa.hpp"
#include <cmath>
#include "CPU.hpp"

struct MotorSimulacao
{
private:

	long int tickAtual; //tick de tempo
	TipoAlgoritmo algoritmoSelecionado;
	std::vector<Tarefa> tarefas; //todas as tarefas
	std::vector<Historico> historicoTimeline; //historico completo da execucao
	std::vector<CPU> todasCPUs;
	int quantCPUs;
	int quantum;
	std::vector<EventoGantt> eventosDesempatePendentes; //usado para registrar desempates

	enum class EstadoSimulacao
	{
		PAUSADA,
		EXECUTANDO,
		PARADA,
		PASSO_A_PASSO
	};

public:
	MotorSimulacao()
	{
		quantCPUs=1;
		quantum=1;
		resetSimulacao();
	}
	void addTarefa(const Tarefa& tarefa);
	void resetSimulacao();

	std::vector<Tarefa*> achaProxima(int quantCPUs);
	bool comparaRM(const Tarefa* a, const Tarefa* b); //ordena as tarefas mais prioritarias por menor periodo e trata desempates
	bool comparaEDF(const Tarefa* a, const Tarefa* b); //ordena as tarefas mais prioritarias por menor deadline e trata desempates

	Historico passoSimulacao(std::vector<Tarefa*> tarefasMaiorPrioridade);
	void preparaSimulacaoRM(); //inicializa as prioridades estaticas

	double calculaUtilizacaoCPU();
	bool verificaEscalabilidade();

	void executaTempoReal();
	void executaPassoAPasso(int instrucao);
	void resetaAPartirDe(int tick);

	bool desempate(const Tarefa* a, const Tarefa* b);

	//estatico para poder ser usado como funcao de comparacao para o sort
	static bool comparaPeriodo(const Tarefa& t1, const Tarefa& t2);

	//gets e sets
	const std::vector<Historico>& getHistorico() const { return historicoTimeline; }
	const std::vector<Tarefa>& getTarefas() const { return tarefas; }
	long int getTickAtual() const { return tickAtual; }
	TipoAlgoritmo getAlgoritmo() const { return algoritmoSelecionado; }
	void setAlgoritmo(TipoAlgoritmo algo) { algoritmoSelecionado=algo; }
	void limparTarefas() { tarefas.clear(); resetSimulacao(); }
	void setQuantidadeCPUs(int cpus) { quantCPUs=cpus; }
	void setQuantum(int quanter) { quantum=quanter; }
};