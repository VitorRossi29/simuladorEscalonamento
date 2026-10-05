#include "PainelConfiguracao.hpp"
#include "imgui.h"
#include "PainelTabelaTarefas.hpp"


PainelConfiguracao::PainelConfiguracao(int* quantidadeCPUs, int* quantum, std::string algoritmo) :
	m_qtdeCpus(quantidadeCPUs),
	m_quantum(quantum),
	m_algoritmo(algoritmo)
{
}

void PainelConfiguracao::renderizar()
{
	bool estaAberta = true;
	if (!ImGui::Begin("Painel de Configuracao", &estaAberta))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Seletor de Algoritmo de Escalonamento");

	if (ImGui::Button("Rate Monotonic")) 
    {
		m_algoritmo = "RM"; 
    }
	else if (ImGui::Button("Earliest Deadline First"))
	{
		m_algoritmo = "EDF";
	}
	else if(ImGui::Button("Outro"))
	{
		m_algoritmo = "Outro";
	}

	static int valorCPUS = 1, valorQUANTUM = 2;


	ImGui::SliderInt("Numero de CPUs", &valorCPUS, 1, 50, "%d CPUs");
	ImGui::SliderInt("Valor do Quantum", &valorQUANTUM, 1, 50, "%d Ticks");

	m_qtdeCpus = valorCPUS;
	m_quantum = valorQUANTUM;

	static bool carregarTexto = false;

	if (ImGui::Button("Carregar arquivo de informacoes"))
	{
		carregarTexto = !carregarTexto;
	}

	ImGui::End();
}

void PainelConfiguracao::renderizarMetricas(const std::vector<TarefaMock>& tarefas, 
	int totalDeadlinesPerdidos, 
	float tempoOciosoPercentual)
{
	float utilizacaoCPU = 0;

	for (int i = 0; i < (int)(tarefas.size()); i++)
	{
		utilizacaoCPU += tarefas[i].duracao / (float)(tarefas[i].periodo);
	}

	float progresso;

	if (m_qtdeCpus != 0)
		progresso = (float)utilizacaoCPU / (float)m_qtdeCpus;
	else
		progresso = 0.0f;

	ImGui::ProgressBar(progresso, ImVec2(300, 200));

	bool escalonavel = false;
	if (escalonavel)
		ImGui::Text("Sistema escalonavel");
	else
		ImGui::Text("Sistema nao-escalonavel");

	ImGui::Text("Total de Prazos Pedidos: %d", totalDeadlinesPerdidos);
	ImGui::Text("Tempo Ocioso Percentual: %f", tempoOciosoPercentual);
}
