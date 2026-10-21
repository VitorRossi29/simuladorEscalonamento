#pragma once
#include "PainelConfiguracao.hpp"
#include "PainelControles.hpp"
#include "PainelTabelaTarefas.hpp"
#include "PainelGraficoGantt.hpp"

#include <vector>
#include <string>

//Forward Declaration para avisar que a classe existe
class MotorSimulacao;


//Essa classe eh o controlador principal da GUI. Ele eh dono das 
//instancias dos paineis e dos dados da simulacao em memoria
class GerenciadorGUI
{
public:

	// Enumeracao dos tipos de log
	enum class TipoLog {
		INFO,
		AVISO,
		ERRO
	};

	// Struct para armazenr cada registro de logo

	struct ItemLog {
		TipoLog tipo;
		std::string mensagem;
	};

private:
	MotorSimulacao* m_motor; //Guarda ponteiro do Motor Simulacao

	ModoFonteDados fonteDados; //Define se vai usar o simulador real ou versao Mock
	ModoFonteDados m_fonteDadosAnterior; // Para detectar quando o usuario muda

	std::vector<TarefaGUI> m_tarefas;
	std::vector<SnapshotTick> m_historico;
	
	int m_tickAtual;
	int m_totalTicks;
	bool m_emExecucaoAuto;
	float m_tempoAcumulado;
	float m_delayPasso; //Tempo em segundos por tick
	int m_quantidadeCPUs;
	int m_quantum;

	int m_modoExecucao;     //modo 0 = passo a passo; 1 = instantaneo
	int m_modoExecucaoAnterior;
	int m_algoritmoSelecionado;

	std::string m_algoritmo;

	int m_quantidadeCPUsAnterior, m_quantumAnterior, m_algoritmoAnterior;
	std::string m_algoritmoAnteriorString;

	std::string caminhoArquivo;
	std::string caminhoArquivoAnterior;

	bool m_exibirConfig, m_exibirControles, m_exibirTabela, m_exibirGantt, m_exibirLogs;
	bool m_solicitarExportacao; //Holy aura

	double utilizacaoCPU;
	bool querUtilizacao;

	// Vetor estatico que armazena o histohrico global de logs acessível por qualquer classe
	static std::vector<ItemLog> s_logs;


	//PRECISAM SER OS ULTIMOS
	PainelConfiguracao m_painelConfig;
	PainelControles m_painelControles;
	PainelTabelaTarefas m_painelTabela;
	PainelGraficoGantt m_painelGantt;

	// Refaz os calculos apohs mudancas nas simulacao
	void recalcularSimulacao();

	// Traduz os dados da Engine para as structs da GUI
	void sincronizarComEngine();

	void atualizarTarefasNaEngine();

	void customizarEstilo();

	void desenharJanelaLogs();

public:
	GerenciadorGUI(MotorSimulacao* motor = nullptr);

	void setMotor(MotorSimulacao* motor) { m_motor = motor; }

	//Carrega as tarefas e o historico via GeradorMock
	void inicializar();

	//Controla o avanco automatico do tempo (m_tickAtual++) com base no 
	//m_delayPasso quando m_emExecucaoAuto for verdadeiro
	void atualizar(float deltaTime);

	//Chama os metodos de renderizacao de cada painel filho (m_painelConfig, m_painelControles, m_painelTabela).
	void renderizar();

	void carregarArquivoEncontrado();

	void screenShotAvancada();

	void reescalonarAPartirDoTickAtual();	

	// Método estatico global para adicionar mensagens de erro/log de qualquer ponto do sistema
	static void registrarLog(TipoLog tipo, const std::string& mensagem);

	// Método estatico para limpar o histohrico de logs
	static void limparLogs();

	bool GerenciadorGUI::validarTarefa(TarefaGUI& t, const std::string& origem);
};