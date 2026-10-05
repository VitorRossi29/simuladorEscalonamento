#include "PainelConfiguracao.hpp"
#include "PainelControles.hpp"
#include "PainelTabelaTarefas.hpp"

//Essa classe eh o controlador principal da GUI. Ele eh dono das 
//instancias dos paineis e dos dados da simulacao em memoria
class GerenciadorGUI
{
private:
	
	std::vector<TarefaMock> m_tarefas;
	std::vector<SnapshotTick> m_historico;
	
	int m_tickAtual;
	int m_totalTicks;
	bool m_emExecucaoAuto;
	float m_tempoAcumulado;
	float m_delayPasso; //Tempo em segundos por tick
	int m_quantidadeCPUs;
	int m_quantum;

	std::string m_algoritmo;

	PainelConfiguracao m_painelConfig;
	PainelControles m_painelControles;
	PainelTabelaTarefas m_painelTabela;

public:
	GerenciadorGUI();

	//Carrega as tarefas e o historico via GeradorMock
	void inicializar();

	//Controla o avanco automatico do tempo (m_tickAtual++) com base no 
	//m_delayPasso quando m_emExecucaoAuto for verdadeiro
	void atualizar(float deltaTime);

	//Chama os metodos de renderizacao de cada painel filho (m_painelConfig, m_painelControles, m_painelTabela).

	void renderizar();
};