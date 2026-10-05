#include "PainelControles.hpp"
#include "imgui.h"


PainelControles::PainelControles(int* tickAtual, int* totalTicks, 
	bool* emExecucaoAuto, float* delayPasso, 
	int* modoExecucao, int* algoritmoSelecionado) :
	m_tickAtualRef(tickAtual),
	m_totalTicksRef(totalTicks),
	m_emExecucaoAutoRef(emExecucaoAuto),
	m_delayPasso(delayPasso),
	m_modoExecucao(modoExecucao),
	m_algoritmoSelecionado(algoritmoSelecionado)
{
}

void PainelControles::renderizar()
{
	bool estaAberta = true;
	if (!ImGui::Begin("Controles da Simulacao", &estaAberta))
	{
		ImGui::End();
		return;
	}

	ImGui::TextColored(ImColor(0, 127, 255, 255), "Tick Atual: %d/ %d", m_tickAtual, m_totalTicks);

	if(m_emExecucaoAuto)
		ImGui::TextColored(ImColor(0, 255, 0, 255), "Escalonador em Execucao");
	else
		ImGui::TextColored(ImColor(255, 0, 0, 255), "Escalonador Pausado");

	ImGui::Separator();

	ImGui::RadioButton("Rate Monotonic", &m_algoritmoSelecionado, 0);
	ImGui::SameLine(); // Mantém os botões na mesma linha (opcional)
	ImGui::RadioButton("Earliest Deadline First", &m_algoritmoSelecionado, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Outro", &m_algoritmoSelecionado, 2);

	ImGui::Separator();

	if (ImGui::Button("Executar"))
	{
		m_emExecucaoAuto = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Pausar"))
	{
		m_emExecucaoAuto = false;
	}
	ImGui::SameLine();

	ImGui::BeginDisabled(m_emExecucaoAuto);

	if (ImGui::Button ("Avancar (+1)"))
	{
		if(m_tickAtual < m_totalTicks - 1)
			m_tickAtual += 1;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip("Pause a execucao para usar o botao");

	ImGui::SameLine();
	if (ImGui::Button("Retroceder (-1)"))
	{
		if (m_tickAtual > 0)
			m_tickAtual -= 1;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip("Pause a execucao para usar o botao");

	ImGui::EndDisabled();

	ImGui::SameLine();
	if (ImGui::Button("Avancar (+1)"))
	{
		m_tickAtual = 0;
		m_emExecucaoAuto = false;
	}

	ImGui::SliderFloat3("Velocidade de simulacao", &m_delayPasso, 0.5f, 3.f, "%.2f segundos/tick");

	ImGui::End();
}
