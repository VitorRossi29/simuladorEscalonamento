#include "PainelConfiguracao.hpp"
#include "imgui.h"
#include "PainelTabelaTarefas.hpp"
#include "MockData.hpp"


PainelConfiguracao::PainelConfiguracao(int* quantidadeCPUs, int* quantum, std::string &algoritmo, ModoFonteDados* modoFonte) :
	m_qtdeCpus(quantidadeCPUs),
	m_quantum(quantum),
	m_algoritmo(algoritmo), 
	m_modoFonte(modoFonte)
{
}

void PainelConfiguracao::renderizar()
{
	ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiCond_FirstUseEver);

	bool estaAberta = true;
	if (!ImGui::Begin("Painel de Configuracao", &estaAberta))
	{
		ImGui::End();
		return;
	}

	static int selecaoFonte = (*m_modoFonte == ModoFonteDados::MOCK) ? 0 : 1;

	if (ImGui::RadioButton("Dados Simulados (Mock)", &selecaoFonte, 0))
	{
		*m_modoFonte = ModoFonteDados::MOCK;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Motor Real (Engine)", &selecaoFonte, 1))
	{
		*m_modoFonte = ModoFonteDados::ENGINE_REAL;
	}


	ImGui::Text("Algoritmo de Escalonamento:");

	// Identificador local do estado do Radio Button
	static int selecaoAlgoritmo = (m_algoritmo == "EDF") ? 1 : (m_algoritmo == "Outro" ? 2 : 0);

	if (ImGui::RadioButton("Rate Monotonic (RM)", &selecaoAlgoritmo, 0))
	{
		m_algoritmo = "RM";
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Earliest Deadline First (EDF)", &selecaoAlgoritmo, 1))
	{
		m_algoritmo = "EDF";
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Outro", &selecaoAlgoritmo, 2))
	{
		m_algoritmo = "Outro";
	}

	ImGui::Separator();

	// Controles diretamente vinculados às variáveis do GerenciadorGUI
	if (m_qtdeCpus != nullptr)
	{
		ImGui::SliderInt("Numero de CPUs", m_qtdeCpus, 1, 16, "%d CPUs");
	}

	if (m_quantum != nullptr)
	{
		ImGui::SliderInt("Valor do Quantum", m_quantum, 1, 20, "%d Ticks");
	}

	static bool carregarTexto = false;

	if (ImGui::Button("Carregar arquivo de informacoes"))
	{
		carregarTexto = !carregarTexto;
	}

	ImGui::End();
}

void PainelConfiguracao::renderizarMetricas(const std::vector<TarefaGUI>& tarefas, 
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
		progresso = (float)utilizacaoCPU / (float) *m_qtdeCpus;
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
