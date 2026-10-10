#include "GerenciadorGUI.hpp"

#include "MockData.hpp"
#include "imgui.h"

GerenciadorGUI::GerenciadorGUI() :
	m_tickAtual(0),
	m_totalTicks(50),
	m_emExecucaoAuto(false),
	m_tempoAcumulado(0.0f),
	m_delayPasso(0.5f), //Tempo em segundos por tick
	m_quantidadeCPUs(2),
	m_quantum(1),
	m_modoExecucao(0),
	m_algoritmoSelecionado(0),
	m_algoritmo("RM"),
	m_painelConfig(&m_quantidadeCPUs, &m_quantum, m_algoritmo),
	m_painelControles(&m_tickAtual, &m_totalTicks, &m_emExecucaoAuto, &m_delayPasso, &m_modoExecucao, &m_algoritmoSelecionado),
	m_painelTabela(&m_tarefas, &m_historico, &m_tickAtual),

	m_quantidadeCPUsAnterior(m_quantidadeCPUs),
	m_quantumAnterior(m_quantum),
	m_algoritmoAnterior(m_algoritmoSelecionado)

{
	//Carregar dados dos Mocks e etc
}

//Esta função roda uma única vez no início da aplicação ou quando a simulação precisa ser totalmente restaurada
void GerenciadorGUI::inicializar()
{
	m_tickAtual = 0;
	m_totalTicks = 50;
	m_quantidadeCPUs = 2;
	m_quantum = 1;
	m_delayPasso = 0.5f;
	m_emExecucaoAuto = false;
	m_tempoAcumulado = 0.0f;

	m_quantidadeCPUsAnterior = m_quantidadeCPUs;
	m_quantumAnterior = m_quantum;
	m_algoritmoAnterior = m_algoritmoSelecionado;

	m_tarefas = GeradorMock::criarTarefasIniciais();
	m_historico = GeradorMock::gerarHistoricoInicial(m_tarefas, m_totalTicks, m_quantidadeCPUs, m_quantum);
}

void GerenciadorGUI::atualizar(float deltaTime)
{
	if (m_emExecucaoAuto == false)	//Verificacao
	{
		return;
	}
	else
	{
		m_tempoAcumulado += deltaTime;	//Acumulo de tempo
		if (m_tempoAcumulado >= m_delayPasso)	//Controle Passo
		{
			m_tempoAcumulado -= m_delayPasso;

			if (m_tickAtual < static_cast<int>(m_historico.size()) - 1)
			{
				m_tickAtual++;
			}
			else
			{
				m_emExecucaoAuto = false;
				m_tempoAcumulado = 0.0f;
			}
		}	

	}
}

void GerenciadorGUI::renderizar()
{
	int numeroTarefas = static_cast<int>(m_tarefas.size());

	ImGui::Text("Teste testador tester");
	
	//Validacao dos limites de tempo
	if (!m_historico.empty() && m_tickAtual >= 0 && m_tickAtual < static_cast<int>(m_historico.size()) )
	{
		//Desenho Sequencial dos Paineis
		
		m_painelConfig.renderizar();
		m_painelControles.renderizar();
		m_painelTabela.renderizar();

		// Checa se as tarefas mudaram OU se alguma configuração foi alterada na interface
		bool tarefasMudaram = (numeroTarefas != static_cast<int>(m_tarefas.size()));
		bool configMudou = (m_quantidadeCPUs != m_quantidadeCPUsAnterior ||
			m_quantum != m_quantumAnterior ||
			m_algoritmoSelecionado != m_algoritmoAnterior);

		//Deteccao e Reacao da Mudancas nos Dados
		if (tarefasMudaram || configMudou)
		{
			recalcularHistorico();

			// Atualiza os rastreadores de estado
			m_quantidadeCPUsAnterior = m_quantidadeCPUs;
			m_quantumAnterior = m_quantum;
			m_algoritmoAnterior = m_algoritmoSelecionado;
		}

	}
}

void GerenciadorGUI::recalcularHistorico()
{
	m_historico = GeradorMock::gerarHistoricoInicial(m_tarefas, 
		m_totalTicks, m_quantidadeCPUs, m_quantum);
}