#include "PainelConfiguracao.hpp"
#include "imgui.h"
#include "PainelTabelaTarefas.hpp"
#include "MockData.hpp"


PainelConfiguracao::PainelConfiguracao(int* quantidadeCPUs, int* quantum, std::string &algoritmo, ModoFonteDados* modoFonte, std::string& caminho) :
	m_qtdeCpus(quantidadeCPUs),
	m_quantum(quantum),
	m_algoritmo(algoritmo), 
	m_modoFonte(modoFonte),
	caminhoArquivoCaixa(false),
	caminhoArquivo(caminho)
{
}

bool PainelConfiguracao::renderizar()
{
	bool alterado = false;
	ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiCond_FirstUseEver);

	bool estaAberta = true;
	if (!ImGui::Begin("Painel de Configuracao", &estaAberta))
	{
		ImGui::End();
		return false;
	}

	static int selecaoFonte = (*m_modoFonte == ModoFonteDados::MOCK) ? 0 : 1;

	if (ImGui::RadioButton("Dados Simulados (Mock)", &selecaoFonte, 0))
	{
		*m_modoFonte = ModoFonteDados::MOCK;
		alterado = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Motor Real (Engine)", &selecaoFonte, 1))
	{
		*m_modoFonte = ModoFonteDados::ENGINE_REAL;
		alterado = true;
	}


	ImGui::Text("Algoritmo de Escalonamento:");

	// Identificador local do estado do Radio Button
	static int selecaoAlgoritmo = (m_algoritmo == "EDF") ? 1 : (m_algoritmo == "Outro" ? 2 : 0);

	if (ImGui::RadioButton("Rate Monotonic (RM)", &selecaoAlgoritmo, 0))
	{
		m_algoritmo = "RM";
		alterado = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Earliest Deadline First (EDF)", &selecaoAlgoritmo, 1))
	{
		m_algoritmo = "EDF";
		alterado = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Outro", &selecaoAlgoritmo, 2))
	{
		m_algoritmo = "Outro";
		alterado = true;
	}

	ImGui::Separator();

	// Controles diretamente vinculados às variaveis do GerenciadorGUI
	if (m_qtdeCpus != nullptr)
	{
		if ( ImGui::SliderInt("Numero de CPUs", m_qtdeCpus, 1, 16, "%d CPUs") )
		{
			alterado = true;
		}
	}

	if (m_quantum != nullptr)
	{
		if (ImGui::SliderInt("Valor do Quantum", m_quantum, 1, 20, "%d Ticks"))
		{
			alterado = true;
		}
	}

	static bool carregarTexto = false;
	static char cam[200];

	if (*m_modoFonte == ModoFonteDados::ENGINE_REAL)
	{
		if (ImGui::Button("Carregar arquivo de informacoes"))
		{
			caminhoArquivoCaixa = !caminhoArquivoCaixa;
		}
		if (caminhoArquivoCaixa)
		{
			ImGui::InputText("Caminho do arquivo", cam, 200);
			if (ImGui::Button("Confirmar"))
			{
				caminhoArquivo = cam;
				alterado = true;
				caminhoArquivoCaixa = !caminhoArquivoCaixa;

				cam[0] = '\0';
			}
		}
	}

	ImGui::End();
	return alterado;
}

void PainelConfiguracao::renderizarMetricas(const std::vector<TarefaGUI>& tarefas, 
	double utilizacaoCPU,
	bool escalonavel,
	int totalDeadlinesPerdidos, 
	float tempoOciosoPercentual)
{
	float progresso;
	bool estaAberta = true;

	if (!ImGui::Begin("Painel de Configuracao", &estaAberta))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Nivel de uso das CPUs");
	if (m_qtdeCpus != 0)
		progresso = (float)utilizacaoCPU / (float) *m_qtdeCpus;
	else
		progresso = 0.0f;

	ImGui::ProgressBar(progresso, ImVec2(300, 30));

	if (escalonavel)
		ImGui::TextColored(ImColor(0, 255, 0, 255), "Sistema escalonavel");
	else
		ImGui::TextColored(ImColor(255, 0, 0, 255), "Sistema nao-escalonavel"); 

	ImGui::Text("Total de Prazos Perdidos: %d", totalDeadlinesPerdidos);
	ImGui::Text("Tempo Ocioso Percentual: %f", tempoOciosoPercentual);

	ImGui::End();
}
