#include "GerenciadorGUI.hpp"
#include "MockData.hpp"

GerenciadorGUI::GerenciadorGUI() :
	m_tickAtual(0),
	m_emExecucaoAuto(false),
	m_tempoAcumulado(0.0f),
	m_delayPasso(0.5f), //Tempo em segundos por tick
	m_quantidadeCPUs(2),
	m_quantum(1),
	m_algoritmo("RM"),
	m_painelConfig(&m_quantidadeCPUs, &m_quantum, &m_algoritmo),
	m_painelControles(&m_tickAtual, &m_totalTicks, &m_emExecucaoAuto),
	m_painelTabela(&m_tarefas, nullptr)

{
}

void GerenciadorGUI::inicializar()
{

}

void GerenciadorGUI::atualizar(float deltaTime)
{

}

void GerenciadorGUI::renderizar()
{
	m_painelConfig.renderizar();
	m_painelControles.renderizar();
	m_painelTabela.renderizar();
}
