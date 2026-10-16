#include "PainelControles.hpp"
#include "imgui.h"
#include "raylib.h"


PainelControles::PainelControles(int* tickAtual, int* totalTicks, 
	bool* emExecucaoAuto, float* delayPasso, 
	int* modoExecucao, int* algoritmoSelecionado, bool* exportacao) :
	m_tickAtualRef(tickAtual),
	m_totalTicksRef(totalTicks),
	m_emExecucaoAutoRef(emExecucaoAuto),
	m_delayPasso(delayPasso),
	m_modoExecucao(modoExecucao),
	m_algoritmoSelecionado(algoritmoSelecionado),
	solicitarExportacaoRef (exportacao)
{
}

void PainelControles::renderizar()
{
	ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiCond_FirstUseEver);

	bool estaAberta = true;
	if (!ImGui::Begin("Controles da Simulacao", &estaAberta))
	{
		ImGui::End();
		return;
	}

	if (m_tickAtualRef == nullptr || m_totalTicksRef == nullptr || m_emExecucaoAutoRef == nullptr)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Erro: Ponteiros de controle nao vinculados!");
		ImGui::End();
		return;
	}

	//modo 0 = passo a passo; 1 = instantaneo

	ImGui::RadioButton("Modo Passo a Passo", m_modoExecucao, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Modo Instantaneo Completo", m_modoExecucao, 1);

	if(*m_modoExecucao)
		ImGui::TextColored(ImColor(0, 127, 255, 255), "Tick Atual: %d/ %d", *m_totalTicksRef, *m_totalTicksRef);
	else
		ImGui::TextColored(ImColor(0, 127, 255, 255), "Tick Atual: %d/ %d", (*m_tickAtualRef) + 1, *m_totalTicksRef);

	if (m_emExecucaoAutoRef != nullptr && *m_emExecucaoAutoRef)
		ImGui::TextColored(ImColor(0, 255, 0, 255), "Escalonador em Execucao");
	else if(m_emExecucaoAutoRef != nullptr && ((*m_tickAtualRef) + 1 == *m_totalTicksRef) || *m_modoExecucao == 1)
		ImGui::TextColored(ImColor(0, 0, 255, 255), "Escalonamento Completo");
	else
		ImGui::TextColored(ImColor(255, 0, 0, 255), "Escalonador Pausado");

	ImGui::Separator();

	ImGui::BeginDisabled(m_emExecucaoAutoRef != nullptr && *m_modoExecucao == 1);	//se estiver no modo completo

	if (ImGui::Button("Executar"))
	{
		*m_emExecucaoAutoRef = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Pausar"))
	{
		*m_emExecucaoAutoRef = false;
	}
	ImGui::SameLine();

	ImGui::BeginDisabled(m_emExecucaoAutoRef != nullptr && *m_emExecucaoAutoRef);

	if (ImGui::Button ("Avancar (+1)"))
	{
		if (*m_tickAtualRef < *m_totalTicksRef - 1)
			*m_tickAtualRef += 1;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && *m_emExecucaoAutoRef)
		ImGui::SetTooltip("Pause a execucao para usar o botao");

	ImGui::SameLine();
	if (ImGui::Button("Retroceder (-1)"))
	{
		if (*m_tickAtualRef > 0)
			*m_tickAtualRef -= 1;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && *m_emExecucaoAutoRef)
		ImGui::SetTooltip("Pause a execucao para usar o botao");

	ImGui::EndDisabled();

	ImGui::SameLine();
	if (ImGui::Button("Reset"))
	{	
		*m_tickAtualRef = 0;
		*m_emExecucaoAutoRef = false;
	}	

	ImGui::SliderFloat("Velocidade de simulacao", m_delayPasso, 0.1f, 3.0f, "%.2f segundos/tick");

	ImGui::EndDisabled();
	
	if (ImGui::Button("Tirar Foto Completa"))
	{
		if (solicitarExportacaoRef != nullptr)
		{
			*solicitarExportacaoRef = true; // Notifica o GerenciadorGUI
		}
	}

	ImGui::End();
}

//Tira foto simples da tela
void PainelControles::screenShotSimples()
{
	TakeScreenshot("../../../escalonamento_gantt.png");
}
