#include "PainelTabelaTarefas.hpp"
#include "imgui.h"
#include <cstring>


// Funcao utilitaria para converter a enum class em texto formatado para o ImGui
const char* estadoParaString(EstadoTarefa estado)
{
    switch (estado)
    {
    case EstadoTarefa::EXECUTANDO: return "EXECUTANDO";
    case EstadoTarefa::PRONTA:     return "PRONTA";
    case EstadoTarefa::SUSPENSA:   return "SUSPENSA";
    case EstadoTarefa::TERMINADA:  return "TERMINADA";
    default:                       return "DESCONHECIDO";
    }
}

// Retorna verdadeiro se a transicao entre os estados for semanticamente valida
bool validarTransicaoEstado(EstadoTarefa atual, EstadoTarefa novo, std::string& mensagemErro)
{
    if (novo == EstadoTarefa::DEADLINE_PERDIDA)
    {
        mensagemErro = "DEADLINE_PERDIDA eh um evento automatico do sistema e nao pode ser definido manualmente.";
        return false;
    }

    if (atual == EstadoTarefa::NOVA)
    {
        if (novo == EstadoTarefa::PRONTA) return true;
        mensagemErro = "Uma tarefa NOVA so pode transicionar para o estado PRONTA (admissao no sistema).";
        return false;
    }

    if (atual == EstadoTarefa::PRONTA)
    {
        if (novo == EstadoTarefa::EXECUTANDO || novo == EstadoTarefa::SUSPENSA) return true;
        mensagemErro = "Uma tarefa PRONTA so pode ir para EXECUTANDO (alocada na CPU) ou SUSPENSA.";
        return false;
    }

    if (atual == EstadoTarefa::EXECUTANDO)
    {
        if (novo == EstadoTarefa::PRONTA || novo == EstadoTarefa::SUSPENSA || novo == EstadoTarefa::TERMINADA) return true;
        mensagemErro = "Uma tarefa EXECUTANDO so pode ir para PRONTA (preempcao), SUSPENSA ou TERMINADA.";
        return false;
    }

    if (atual == EstadoTarefa::SUSPENSA) // ALTERAcaO: Validacao para o estado SUSPENSA
    {
        if (novo == EstadoTarefa::PRONTA) return true;
        mensagemErro = "Uma tarefa SUSPENSA so pode voltar para o estado PRONTA.";
        return false;
    }

    if (atual == EstadoTarefa::TERMINADA)
    {
        mensagemErro = "Uma tarefa TERMINADA nao pode mudar de estado diretamente. Use o botao Reset para reiniciar.";
        return false;
    }

    return true;
}

PainelTabelaTarefas::PainelTabelaTarefas(std::vector<TarefaGUI>* tarefas,
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

bool PainelTabelaTarefas::renderizar()
{
    ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiCond_FirstUseEver);

	bool estaAberta = true;
	if (!ImGui::Begin("Painel de Tarefas", &estaAberta))
	{
		ImGui::End();
		return false;
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

    bool edicaoTarefa = false;
    bool novaTarefa = false;

    //Se estiver nulo nao vai funcionar
	if (m_tarefasRef != NULL)
	{
		edicaoTarefa = desenharTabela(snapshotAtual);
        novaTarefa = desenharFormularioInsercao();
	}

	ImGui::End();

    if (edicaoTarefa || novaTarefa)
        return true;
    else
        return false;
}

bool PainelTabelaTarefas::desenharTabela(const SnapshotTick* snapshotAtual)
{
    int idParaRemover = -1;
    bool editouAlgumCampo = false;
    static bool exibirModalErro = false;
    static std::string mensagemErroTransicao = "";

    // Abre a tabela com 8 colunas
    if (ImGui::BeginTable("TabelaTarefasSimulacao", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
    {
        // Configura os nomes de cada coluna
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Cor");
        ImGui::TableSetupColumn("Ingresso");
        ImGui::TableSetupColumn("Duracao (C)");
        ImGui::TableSetupColumn("Periodo (T)");
        ImGui::TableSetupColumn("Prazo (D)");
        ImGui::TableSetupColumn("Estado");
        ImGui::TableSetupColumn("Acoes");

        // Renderiza a linha com os títulos das colunas
        ImGui::TableHeadersRow();

        // Percorre o vetor de dados preenchendo as linhas da tabela
        for (size_t i = 0; i < m_tarefasRef->size(); i++)
        {
            TarefaGUI& tarefa = (*m_tarefasRef)[i];

            // Evita conflito de IDs entre elementos das linhas
            ImGui::PushID(tarefa.id);

            // Coluna 0: ID (Texto simples)
            ImGui::TableNextColumn();
            ImGui::Text("%d", tarefa.id);

            // Coluna 1: Cor Hex (Texto simples)
            ImGui::TableNextColumn();
            ImGui::Text("#%s", tarefa.cor);

            // Coluna 2: Ingresso (Input numérico editavel)
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1); // Faz o input ocupar toda a largura da celula
            if (ImGui::InputInt("##ingresso", &tarefa.ingresso, 0)) // "##" esconde o rotulo do input
                editouAlgumCampo = true;

            // Coluna 3: Duracao (C)
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputInt("##duracao", &tarefa.duracao, 0))
                editouAlgumCampo = true;

            // Coluna 4: Período (T)
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputInt("##periodo", &tarefa.periodo, 0))
                editouAlgumCampo = true;

            // Coluna 5: Prazo (D)
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputInt("##prazo", &tarefa.prazo, 0))
                editouAlgumCampo = true;

            // Coluna 6: Estado Atual no Tick
            //Busca o estado da tarefa no snapshot do tick atual
            ImGui::TableNextColumn();


            EstadoTarefa estadoAtual = EstadoTarefa::PRONTA;
            if (snapshotAtual != nullptr)
            {
                for (size_t j = 0; j < snapshotAtual->tarefas.size(); j++)
                {
                    if (snapshotAtual->tarefas[j].idTarefa == tarefa.id)
                    {
                        estadoAtual = snapshotAtual->tarefas[j].estado;
                        break;
                    }
                }
            }

            const char* opcoesEstados[] = { "NOVA", "PRONTA", "EXECUTANDO", "SUSPENSA", "TERMINADA" };
            EstadoTarefa mapaEstadosEnum[] = { EstadoTarefa::NOVA, EstadoTarefa::PRONTA, EstadoTarefa::EXECUTANDO, EstadoTarefa::SUSPENSA, EstadoTarefa::TERMINADA };

            int itemSelecionadoIndex = 1; // Padrao PRONTA
            for (int e = 0; e < 5; e++)
            {
                if (mapaEstadosEnum[e] == estadoAtual)
                {
                    itemSelecionadoIndex = e;
                    break;
                }
            }

            ImGui::SetNextItemWidth(-1);
            if (ImGui::Combo("##EstadoCombo", &itemSelecionadoIndex, opcoesEstados, 5))
            {
                EstadoTarefa novoEstadoProposto = mapaEstadosEnum[itemSelecionadoIndex];

                if (novoEstadoProposto != estadoAtual)
                {
                    std::string erro;
                    // Valida se a transicao de estado é permitida
                    if (validarTransicaoEstado(estadoAtual, novoEstadoProposto, erro))
                    {
                        tarefa.estado = novoEstadoProposto;
                        editouAlgumCampo = true; // Aplica e aciona o reescalonamento
                    }
                    else
                    {
                        // Bloqueia a alteracao e exibe o aviso ao usuario (Requisito 3.4)
                        mensagemErroTransicao = erro;
                        exibirModalErro = true;
                        ImGui::OpenPopup("Transicao de Estado Invalida");
                    }
                }
            }

            // Coluna 7: Botao de Remover
            ImGui::TableNextColumn();
            if (ImGui::Button("Remover")) 
            {
                idParaRemover = tarefa.id;
            }

            // Libera o ID da linha atual
            ImGui::PopID();
        }

        // Encerra a tabela
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
                editouAlgumCampo = true;
                break;
            }
        }
    }

    if (ImGui::BeginPopupModal("Transicao de Estado Invalida", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Erro de Transicao de Estado no RTOS:");
        ImGui::Separator();
        ImGui::TextWrapped("%s", mensagemErroTransicao.c_str());
        ImGui::Spacing();

        if (ImGui::Button("Compreendido", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            exibirModalErro = false;
        }
        ImGui::EndPopup();
    }

    // Se nao houve alteracao retorna falso
    return editouAlgumCampo;
}

bool PainelTabelaTarefas::desenharFormularioInsercao()
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
        TarefaGUI tarefaNova;
        tarefaNova.id = proximoId();

        // Copia a string do buffer para o campo char[8] da struct garantindo a terminacao nula
        strncpy(tarefaNova.cor, m_novaCorBuffer, sizeof(tarefaNova.cor) - 1);
        tarefaNova.cor[sizeof(tarefaNova.cor) - 1] = '\0';

        tarefaNova.ingresso = m_novoIngresso;
        tarefaNova.duracao = m_novaDuracao;
        tarefaNova.periodo = m_novoPeriodo;
        tarefaNova.prazo = m_novoPrazo;
        tarefaNova.lista_eventos = ""; //String que sera usada no projeto B
        tarefaNova.estado = EstadoTarefa::PRONTA; // Estado inicial padrao

        m_tarefasRef->push_back(tarefaNova);

        return true;
    }

    if (ImGui::Button("Criar Tarefa Aleatoria"))
    {
        Tarefa tarefa(10);
        bool igual=false;
        for (TarefaGUI tarefasAtuais:*m_tarefasRef)
        {
            if(std::string(tarefasAtuais.cor)==tarefa.getCor())
                igual=true;
        }
        
        //garante que todas as tarefas aleatorias possuem cores diferentes das existentes
        while(igual)
        {
            igual=false;
            tarefa=Tarefa(10);
            for (TarefaGUI tarefasAtuais:*m_tarefasRef)
            {
                if(std::string(tarefasAtuais.cor)==tarefa.getCor())
                    igual=true;
            }
        }

        //preenche a struct da GUI a partir da tarefa gerada
        TarefaGUI tarefaNova;
        tarefaNova.id=proximoId(); //usa a sequencia de IDs da GUI
        tarefaNova.ingresso=tarefa.getIngresso();
        tarefaNova.duracao=tarefa.getDuracao();
        tarefaNova.periodo=tarefa.getPeriodo();
        tarefaNova.prazo=tarefa.getPrazo();
        tarefaNova.prioridade=tarefa.getPrioridade();
        tarefaNova.lista_eventos="";
        tarefaNova.estado=EstadoTarefa::PRONTA;

        //copia a cor hex direto da tarefa para o char array da GUI
        strncpy(tarefaNova.cor, tarefa.getCor().c_str(), sizeof(tarefaNova.cor)-1);
        tarefaNova.cor[sizeof(tarefaNova.cor)-1]='\0';

        m_tarefasRef->push_back(tarefaNova);

        return true; // Retorna true para atualizar e recalcular a simulacao
    }
    return false;
}

int PainelTabelaTarefas::proximoId()
{
    if (m_tarefasRef == nullptr || m_tarefasRef->empty())
    {
        return 1;
    }

    int maiorId=0;
    for (const auto& tarefa:*m_tarefasRef)
    {
        if (tarefa.id>maiorId)
        {
            maiorId=tarefa.id;
        }
    }

    return maiorId+1;
    //return (m_tarefasRef->back().id) + 1;
}

void PainelTabelaTarefas::resetarFormulario()
{
}
