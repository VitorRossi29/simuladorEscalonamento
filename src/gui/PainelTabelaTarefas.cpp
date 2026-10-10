#include "PainelTabelaTarefas.hpp"
#include "imgui.h"
#include <cstring>

// Função utilitária para converter a enum class em texto formatado para o ImGui
const char* estadoParaString(EstadoTarefa estado)
{
    switch (estado)
    {
    case EstadoTarefa::EXECUTANDO: return "EXECUTANDO";
    case EstadoTarefa::PRONTA:     return "PRONTA";
    case EstadoTarefa::ESPERANDO:   return "ESPERANDO";
    case EstadoTarefa::DEADLINE_PERDIDA:  return "DEADLINE PERDIDA";
    default:                       return "DESCONHECIDO";
    }
}


PainelTabelaTarefas::PainelTabelaTarefas(std::vector<TarefaMock>* tarefas,
    const std::vector<SnapshotTick>* historico,
    const int* tickAtual) :
    m_tarefasRef(tarefas),
    m_historicoRef(historico),
    m_tickAtualRef(tickAtual),
    m_novoIngresso(0),
    m_novaDuracao(1),
    m_novoPeriodo(1),
    m_novoPrazo(1)
{
    m_novaCorBuffer[0] = '\0';
}

void PainelTabelaTarefas::renderizar()
{
    ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_FirstUseEver);

	bool estaAberta = true;
	if (!ImGui::Begin("Painel de Tarefas", &estaAberta))
	{
		ImGui::End();
		return;
	}
	
    // Busca o snapshot do tick atual de forma segura
    const SnapshotTick* snapshotAtual = nullptr;
    if (m_historicoRef != nullptr && m_tickAtualRef != nullptr &&
        !m_historicoRef->empty())
    {
        int tick = *m_tickAtualRef;

        // Testa se o tick obtido pela referencia esta dentro dos limites
        if (tick >= 0 && tick < static_cast<int>(m_historicoRef->size()))
        {
            // Adquire o snapshot no historico no momento certo
            snapshotAtual = &( (*m_historicoRef) [tick] );
        }
    }

    //Se estiver nulo nao vai funcionar
	if (m_tarefasRef != NULL)
	{
		desenharTabela(snapshotAtual);
		desenharFormularioInsercao();
	}

	ImGui::End();
}

void PainelTabelaTarefas::desenharTabela(const SnapshotTick* snapshotAtual)
{
    int idParaRemover = -1;

    // 1. Abre a tabela com 8 colunas e bordas estilizadas
    if (ImGui::BeginTable("TabelaTarefasSimulacao", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
    {
        // 2. Configura os nomes de cada coluna
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Cor");
        ImGui::TableSetupColumn("Ingresso");
        ImGui::TableSetupColumn("Duracao (C)");
        ImGui::TableSetupColumn("Periodo (T)");
        ImGui::TableSetupColumn("Prazo (D)");
        ImGui::TableSetupColumn("Estado");
        ImGui::TableSetupColumn("Acoes");

        // 3. Renderiza a linha com os títulos das colunas
        ImGui::TableHeadersRow();

        // 4. Percorre o vetor de dados preenchendo as linhas da tabela
        for (size_t i = 0; i < m_tarefasRef->size(); i++)
        {
            TarefaMock& tarefa = (*m_tarefasRef)[i];

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
            //Busca o estado da tarefa no snapshot do tick atual
            ImGui::TableNextColumn();

            std::string estadoStr = "ESPERANDO";
            if (snapshotAtual != nullptr)
            {
                for (size_t j = 0; j < snapshotAtual->tarefas.size(); j++)
                {
                    if (snapshotAtual->tarefas[j].idTarefa == tarefa.id)
                    {
                        estadoStr = estadoParaString(snapshotAtual->tarefas[j].estado);
                        break;
                    }
                }
            }
            
            ImGui::Text("%s", estadoStr.c_str() );

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

}

void PainelTabelaTarefas::desenharFormularioInsercao()
{
    ImGui::Separator();

    ImGui::Text("Nova Tarefa");

    ImGui::SetNextItemWidth(70.0f);
    ImGui::InputText("Cor Hex", m_novaCorBuffer, sizeof(m_novaCorBuffer));
    ImGui::SameLine();

    ImGui::SetNextItemWidth(80.0f);
    ImGui::InputInt("Ingresso", &m_novoIngresso);
    ImGui::SameLine();

    ImGui::SetNextItemWidth(80.0f);
    ImGui::InputInt("Duracao", &m_novaDuracao);
    ImGui::SameLine();

    ImGui::SetNextItemWidth(80.0f);
    ImGui::InputInt("Periodo", &m_novoPeriodo);
    ImGui::SameLine();

    ImGui::SetNextItemWidth(80.0f);
    ImGui::InputInt("Prazo", &m_novoPrazo);

    if (ImGui::Button("+ Adicionar Tarefa"))
    {
        TarefaMock tarefaNova;
        tarefaNova.id = proximoId();

        // Copia a string do buffer para o campo char[8] da struct garantindo a terminação nula
        strncpy(tarefaNova.cor, m_novaCorBuffer, sizeof(tarefaNova.cor) - 1);
        tarefaNova.cor[sizeof(tarefaNova.cor) - 1] = '\0';

        tarefaNova.ingresso = m_novoIngresso;
        tarefaNova.duracao = m_novaDuracao;
        tarefaNova.periodo = m_novoPeriodo;
        tarefaNova.prazo = m_novoPrazo;
        tarefaNova.lista_eventos = ""; //String que sera usada no projeto B
        tarefaNova.estado = EstadoTarefa::ESPERANDO; // Estado inicial padrao

        m_tarefasRef->push_back(tarefaNova);
    }
}

int PainelTabelaTarefas::proximoId()
{
    if (m_tarefasRef == nullptr || m_tarefasRef->empty())
    {
        return 1;
    }
    
    return (m_tarefasRef->back().id) + 1;
}

void PainelTabelaTarefas::resetarFormulario()
{
}
