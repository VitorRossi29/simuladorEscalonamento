#include "PainelTabelaTarefas.hpp"
#include "imgui.h"


PainelTabelaTarefas::PainelTabelaTarefas(std::vector<TarefaMock>* tarefas, const SnapshotTick* snapshotAtual) :
    m_tarefasRef(tarefas),
    m_snapshotAtualRef(snapshotAtual),
    m_novoIngresso(-1),
    m_novaDuracao(-1),
    m_novoPeriodo(-1),
    m_novoPrazo(-1),
    m_novaCorBuffer()
{
}

void PainelTabelaTarefas::renderizar()
{
	bool estaAberta = true;
	if (!ImGui::Begin("Painel de Tarefas", &estaAberta))
	{
		ImGui::End();
		return;
	}

	//Se estiverem nulos nao vai funcionar
	if (m_tarefasRef != NULL && m_snapshotAtualRef != NULL)
	{
		desenharTabela();
		desenharFormularioInsercao();
	}

	ImGui::End();
}

void PainelTabelaTarefas::desenharTabela()
{
    int idParaRemover = -1;

    // 1. Abre a tabela com 8 colunas e bordas estilizadas
    if (ImGui::BeginTable("TabelaTarefasSimulacao", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
    {
        // 2. Configura os nomes de cada coluna
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Cor");
        ImGui::TableSetupColumn("Ingresso");
        ImGui::TableSetupColumn("Duração (C)");
        ImGui::TableSetupColumn("Período (T)");
        ImGui::TableSetupColumn("Prazo (D)");
        ImGui::TableSetupColumn("Estado");
        ImGui::TableSetupColumn("Ações");

        // 3. Renderiza a linha com os títulos das colunas
        ImGui::TableHeadersRow();

        // 4. Percorre o vetor de dados preenchendo as linhas da tabela
        for (size_t i = 0; i < m_tarefasRef->size(); i++)
        {
            TarefaMock tarefa = (*m_tarefasRef)[i];

            // Evita conflito de IDs entre elementos das linhas
            ImGui::PushID(tarefa.id);

            // Coluna 0: ID (Texto simples)
            ImGui::TableNextColumn();
            ImGui::Text("%d", tarefa.id);

            // Coluna 1: Cor Hex (Texto simples)
            ImGui::TableNextColumn();
            ImGui::Text("#%s", tarefa.cor);

            // Coluna 2: Ingresso (Input numérico editável)
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1); // Faz o input ocupar toda a largura da célula
            ImGui::InputInt("##ingresso", &tarefa.ingresso, 0); // "##" esconde o rótulo do input

            // Coluna 3: Duração (C)
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);
            ImGui::InputInt("##duracao", &tarefa.duracao, 0);

            // Coluna 4: Período (T)
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);
            ImGui::InputInt("##periodo", &tarefa.periodo, 0);

            // Coluna 5: Prazo (D)
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);
            ImGui::InputInt("##prazo", &tarefa.prazo, 0);

            // Coluna 6: Estado Atual no Tick
            //TALVEZ INCORRETO
            ImGui::TableNextColumn();
            ImGui::Text("%s", tarefa.estado);

            // Coluna 7: Botão de Ação
            ImGui::TableNextColumn();
            if (ImGui::Button("Remover")) 
            {
                idParaRemover = tarefa.id;
            }

            // Libera o ID da linha atual
            ImGui::PopID();
        }

        // 5. Encerra a tabela
        ImGui::EndTable();
    }

    if (idParaRemover != -1)
    {
        //procura o id para remover
        for (size_t i = 0; i < m_tarefasRef->size(); i++)
        {
            if ((*m_tarefasRef)[i].id == idParaRemover)
            {
                m_tarefasRef->erase(m_tarefasRef->begin() + i);
                break;
            }
        }
    }


	ImGui::End();
}

void PainelTabelaTarefas::desenharFormularioInsercao()
{
    ImGui::Separator();

    ImGui::Text("Nova Tarefa");

    ImGui::InputText("Cor Hex", m_novaCorBuffer, sizeof(m_novaCorBuffer));
    ImGui::SameLine();
    ImGui::InputInt("Ingresso", &m_novoIngresso);
    ImGui::SameLine();
    ImGui::InputInt("Duracao", &m_novaDuracao);
    ImGui::SameLine();
    ImGui::InputInt("Periodo", &m_novoPeriodo);
    ImGui::SameLine();
    ImGui::InputInt("Prazo", &m_novoPrazo);

    if (ImGui::Button("+ Adicionar Tarefa"))
    {
        
        TarefaMock tarefaNova = { proximoId(), *m_novaCorBuffer, m_novoIngresso, 
            m_novaDuracao, m_novoPeriodo, m_novoPrazo};
        m_tarefasRef->push_back(tarefaNova);
    }
}

int PainelTabelaTarefas::proximoId()
{
    return (m_tarefasRef->back().id) + 1;
}

void PainelTabelaTarefas::resetarFormulario()
{
}
