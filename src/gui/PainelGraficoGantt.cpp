#include "PainelGraficoGantt.hpp"
#include "PainelGraficoGantt.hpp"
#include "PainelGraficoGantt.hpp"
#include "PainelGraficoGantt.hpp"

#include <algorithm>
#include <string>
#include "PainelTabelaTarefas.hpp"
#include "raylib.h"

const char* estadoParaString(EstadoTarefa estado);


ImU32 PainelGraficoGantt::hexParaImU32(const char* hex, float alpha)
{
    if (hex == nullptr || strlen(hex) < 6)
        return IM_COL32(128, 128, 128, 255); // Cor cinza padrão em caso de falha

    unsigned int r_vermelho = 0, g_verde = 0, b_azul = 0;
    sscanf(hex, "%02x%02x%02x", &r_vermelho, &g_verde, &b_azul);

    return IM_COL32(r_vermelho, g_verde, b_azul, static_cast<int>(alpha * 255.0f));
}

// Função auxiliar local para converter Hexadecimal em Color da Raylib
Color PainelGraficoGantt::hexParaRaylibColor(const char* hex, float alpha)
{
    if (hex == nullptr || strlen(hex) < 6)
        return GRAY;

    unsigned int r = 0, g = 0, b = 0;
    sscanf(hex, "%02x%02x%02x", &r, &g, &b);

    return Color{
        static_cast<unsigned char>(r),
        static_cast<unsigned char>(g),
        static_cast<unsigned char>(b),
        static_cast<unsigned char>(alpha * 255.0f)
    };
}

//Desenha a regua de tempo e coloca as linhas verticais que dividem cada tick
void PainelGraficoGantt::desenharReguaEixoX(ImDrawList* listaDesenhos, ImVec2 inicio, int totalTicks)
{
    if (m_tarefasRef == nullptr) return;
    float numTarefas = static_cast<float>(m_tarefasRef->size());

    for (int t = 0; t < totalTicks; t++)
    {
        //Calculo da posicao X do tick t
        int posicaoXt = inicio.x + m_margemEsquerda + (t * m_larguraTick);
        
        std::string txt = std::to_string(t + 1);

        //Posicao do texto
        ImVec2 posicaoTexto(posicaoXt + 5.0f, inicio.y + 10.0f);

        listaDesenhos->AddText(posicaoTexto, IM_COL32(255, 255, 255, 255), txt.c_str());

        //Ponta de cima da linha
        //ImVec2 pontaCima = (posicaoXt, inicio.y + m_margemTopo);
        int maxY = inicio.y + m_margemTopo;

        //Ponta de baixo da linha
        //ImVec2 pontaBaixo = (posicaoXt, inicio.y + m_margemTopo + (m_tarefasRef->size() * m_alturaLinha) );
        int minY = inicio.y + m_margemTopo + (m_tarefasRef->size() * m_alturaLinha);

        //listaDesenhos->AddLine(pontaCima, pontaBaixo, IM_COL32(80, 80, 80, 150), 1.0f);
        listaDesenhos->AddLineV(posicaoXt, minY, maxY, IM_COL32(80, 80, 80, 150), 1.0f);
    }
}

void PainelGraficoGantt::desenharAgulhaTempoAtual(ImDrawList* listaDesenhos, ImVec2 inicio, size_t totalTarefas)
{
    if (m_tickAtualRef == nullptr || *m_modoExecucao == 1) return;

    // (*m_tickAtualRef + 1) posiciona a linha exatamente na borda DIREITA do tick mais recente
    float xAtual = inicio.x + m_margemEsquerda + ((*m_tickAtualRef + 1) * m_larguraTick);
    float yTopo = inicio.y + m_margemTopo;
    float yBase = inicio.y + m_margemTopo + (totalTarefas * m_alturaLinha);

    // Desenha a linha amarela destacada (espessura 2.5f) sobre todas as camadas
    listaDesenhos->AddLine(ImVec2(xAtual, yTopo), ImVec2(xAtual, yBase), IM_COL32(255, 215, 0, 255), 2.5f);
}

//Faz as linhas horizontais que dividem as tarefas e coloca os "nomes" de cada tarefa no eixo y
void PainelGraficoGantt::desenharEixoYELinhasBase(ImDrawList* listaDesenhos, ImVec2 inicio, const std::vector<TarefaGUI>& tarefasOrdenadas)
{
    int totalTicks = static_cast<int>(m_historicoRef->size());
    for (size_t i = 0; i < tarefasOrdenadas.size(); i++)
    {
        //Calculo da posicao Y da linha i
        int posicaoYi = inicio.y + m_margemTopo + (i * m_alturaLinha);

        //Nome da tarefa
        std::string identificadorTarefa = "Tarefa " + std::to_string(tarefasOrdenadas[i].id);
        ImVec2 posicaoTexto(inicio.x + 10, posicaoYi + 5);
        listaDesenhos->AddText(posicaoTexto, IM_COL32(255, 255, 255, 255), identificadorTarefa.c_str());

        //Desenhar linha horizontal
        //int posicaoInicio = (inicio.x + m_margemEsquerda, posicaoYi + m_alturaLinha);
        int minX = inicio.x + m_margemEsquerda;
        //int posicaoFim = (inicio.x + m_margemEsquerda + (totalTicks * m_larguraTick), posicaoYi + m_alturaLinha);
        int maxX = inicio.x + m_margemEsquerda + (totalTicks * m_larguraTick);
        listaDesenhos->AddLineH(minX, maxX, posicaoYi, IM_COL32(80, 80, 80, 150), 1.0f);
    }
}

//Desenha o bloco correspondente ao estado de cada tarefa
void PainelGraficoGantt::desenharBlocosExecucao(ImDrawList* listaDesenhos, ImVec2 inicio, const std::vector<TarefaGUI>& tarefasOrdenadas)
{
    if (m_historicoRef == nullptr || m_tickAtualRef == nullptr) return;

    size_t limite;

    // Define até qual tick o gráfico deve ser revelado na interface no modo passo a passo
    // E defube o limite como o ultimo tick no modo instantaneo
    if (!(*m_modoExecucao))
    {
        int tickLimite = *m_tickAtualRef;
        size_t totalTicksDisponiveis = m_historicoRef->size();

        // Garante que não ultrapassará o tamanho do vetor do histórico
        limite = std::min(static_cast<size_t>(tickLimite + 1), totalTicksDisponiveis);
    }
    else
    {
        limite = m_historicoRef->size();
    }
    

    for (size_t t = 0; t < limite; t++)
    {
        for (size_t i = 0; i < tarefasOrdenadas.size(); i++)
        {
            // Calculo do tamanho de cada retangulo
            ImVec2 posicaoMin(inicio.x + m_margemEsquerda + (t * m_larguraTick),
                                inicio.y + m_margemTopo + (i * m_alturaLinha) );

            ImVec2 posicaoMax(posicaoMin.x + m_larguraTick, 
                                posicaoMin.y + m_alturaLinha);

            // Encontra o estado da tarefa no Snapshot para conseguir definir que tipo de preenchimento vai ter
            const SnapshotTick& snap = (*m_historicoRef)[t];
            
            const SnapshotTarefa* snapCerto = nullptr;
            for (size_t j = 0; j < snap.tarefas.size(); j++)
            {
                if (snap.tarefas[j].idTarefa == tarefasOrdenadas[i].id)
                {
                    snapCerto = &snap.tarefas[j];
                    break;
                }
            }

            if (snapCerto == nullptr)
            {
                continue; // Ignora o desenho desta célula se a tarefa não tiver registro neste tick
            }
            //Desenhar de acordo com o estado
            if (snapCerto->estado == EstadoTarefa::EXECUTANDO)
            {
                //Convertendo cor
                ImU32 corTrenteDeux = hexParaImU32(tarefasOrdenadas[i].cor);

                //Desenho do retangulo preenchido indica que esta EXECUTANDO
                listaDesenhos->AddRectFilled(posicaoMin, posicaoMax, corTrenteDeux);

                //Desenha a borda do retangulo
                listaDesenhos->AddRect(posicaoMin, posicaoMax, IM_COL32(0, 0, 0, 255));
            }
            else if (snapCerto->estado == EstadoTarefa::PRONTA)
            {
                //Desenho apenas da borda
                listaDesenhos->AddRect(posicaoMin, posicaoMax, IM_COL32(100, 100, 100, 100));
            }
            else if (snapCerto->estado == EstadoTarefa::ESPERANDO)
            {
                //Fundo preto
                listaDesenhos->AddRectFilled(posicaoMin, posicaoMax, IM_COL32(0, 0, 0, 255));

                //Desenho hachurado

                //Mascara de corte para evitar que as linhas fiquem fora dos limites do retangulo
                listaDesenhos->PushClipRect(posicaoMin, posicaoMax, true);

                float altura = posicaoMax.x - posicaoMin.x;
                float largura = posicaoMax.y - posicaoMin.y;
                float passo = 6.0f;         //Distancia entre cada linha diagonal

                // O loop comeca em -altura (para cobrir o canto inferior esquerdo) 
                // e vai ate largura (para cobrir o canto superior direito)
                for (float offset = -altura; offset < largura; offset += passo)
                {
                    ImVec2 p1(posicaoMin.x + offset, posicaoMin.y);
                    ImVec2 p2(posicaoMin.x + offset + altura, posicaoMax.y);

                    listaDesenhos->AddLine(p1, p2, IM_COL32(180, 180, 180, 255), 1.0f);
                }

                //Remocao da mascara de corte
                listaDesenhos->PopClipRect();
                listaDesenhos->AddRect(posicaoMin, posicaoMax, IM_COL32(100, 100, 100, 255));
            }
            else
            {
                //Provavelmente para Deadline Perdida apenas;

                //Convertendo cor
                ImU32 corTrenteDeux = hexParaImU32(tarefasOrdenadas[i].cor);

                //Desenho do retangulo preenchido indica que esta EXECUTANDO
                listaDesenhos->AddRectFilled(posicaoMin, posicaoMax, corTrenteDeux);

                //Desenha a borda do retangulo
                listaDesenhos->AddRect(posicaoMin, posicaoMax, IM_COL32(0, 0, 0, 255));

                //Desenho hachurado

                //Mascara de corte para evitar que as linhas fiquem fora dos limites do retangulo
                listaDesenhos->PushClipRect(posicaoMin, posicaoMax, true);

                float altura = posicaoMax.x - posicaoMin.x;
                float largura = posicaoMax.y - posicaoMin.y;
                float passo = 6.0f;         //Distancia entre cada linha diagonal

                // O loop comeca em -altura (para cobrir o canto inferior esquerdo) 
                // e vai ate largura (para cobrir o canto superior direito)
                for (float offset = -altura; offset < largura; offset += passo)
                {
                    ImVec2 p1(posicaoMin.x + offset, posicaoMin.y);
                    ImVec2 p2(posicaoMin.x + offset + altura, posicaoMax.y);

                    listaDesenhos->AddLine(p1, p2, IM_COL32(255, 0, 0, 255), 1.0f);
                }

                //Remocao da mascara de corte
                listaDesenhos->PopClipRect();
                listaDesenhos->AddRect(posicaoMin, posicaoMax, IM_COL32(100, 100, 100, 255));
            }

            //TESTE
            if (ImGui::IsMouseHoveringRect(posicaoMin, posicaoMax))
            {
                ImGui::BeginTooltip();
                ImGui::Text("Tarefa ID: %d", tarefasOrdenadas[i].id);
                ImGui::Text("Tick: %d", static_cast<int>(t));

                int cpuAlocada = -1;
                for (const auto& cpu : snap.cpus)
                {
                    if (cpu.idTarefaExecutando == tarefasOrdenadas[i].id)
                    {
                        cpuAlocada = cpu.idCpu;
                        break;
                    }
                }

                if (cpuAlocada != -1)
                    ImGui::Text("CPU Alocada: CPU %d", cpuAlocada);
                else
                    ImGui::Text("CPU Alocada: Nenhuma");

                ImGui::Text("Estado da Tarefa: %s", estadoParaString(snapCerto->estado) );
                ImGui::Text("Duracao Restante: %d", snapCerto->duracaoRestante);
                ImGui::Text("Prazo Restante: %d", snapCerto->prazoRestante);

                ImGui::EndTooltip();
            }
        }
        
    }
}

void PainelGraficoGantt::desenharEventosESinais(ImDrawList* listaDesenhos, ImVec2 inicio, const std::vector<TarefaGUI>& tarefasOrdenadas)
{
    if (m_historicoRef == nullptr || m_tickAtualRef == nullptr) return;

    size_t limite;

    // Define até qual tick o gráfico deve ser revelado na interface no modo passo a passo
    // E defube o limite como o ultimo tick no modo instantaneo
    if (!(*m_modoExecucao))
    {
        int tickLimite = *m_tickAtualRef;
        size_t totalTicksDisponiveis = m_historicoRef->size();

        // Garante que não ultrapassará o tamanho do vetor do histórico
        limite = std::min(static_cast<size_t>(tickLimite + 1), totalTicksDisponiveis);
    }
    else
    {
        limite = m_historicoRef->size();
    }

    for (size_t t = 0; t < limite; t++)
    {
        const SnapshotTick& snap = (*m_historicoRef)[t];
        
        // Laco interno para percorrer os eventos

        for (size_t e = 0; e < snap.eventos.size(); e++)
        {
            const EventoGantt& eventoDaVez = snap.eventos[e];

            // Buaca a linha i do eixo Y da tarefa
            int linhaIndex = -1;
            for (size_t i = 0; i < tarefasOrdenadas.size(); i++)
            {
                if (tarefasOrdenadas[i].id == eventoDaVez.idTarefa)
                {
                    linhaIndex = static_cast<int>(i);
                    break;
                }
            }

            if (linhaIndex == -1) continue; // Tarefa nao encontrada no vetor

            // Calcul oda posicao X e Y da celula da tarefa no tick t
            float xMin = inicio.x + m_margemEsquerda + (t * m_larguraTick);
            float yMin = inicio.y + m_margemTopo + (linhaIndex * m_alturaLinha);

            ImVec2 pMin(xMin, yMin);
            ImVec2 pMax(xMin + m_larguraTick, yMin + m_alturaLinha);

            if (eventoDaVez.tipo == TipoEvento::INGRESSO)
            {
                // Calcular o centro e as dimensões do pequeno triângulo
                float xMid = xMin + (m_larguraTick / 2.0f);
                float yApex = yMin + 8.0f; // Ápice no topo da célula
                float yBase = yMin; // Base 8 pixels abaixo

                // Criar um pequeno triângulo centralizado
                ImVec2 pApex(xMid, yApex);
                ImVec2 pBaseR(xMid + 8, yBase);
                ImVec2 pBaseL(xMid - 8, yBase);

                // O ImGui não exige ordem específica, mas esta é a ordem correta para a lógica.
                listaDesenhos->AddTriangleFilled(pApex, pBaseL, pBaseR, IM_COL32(0, 255, 0, 255));
            }
            if (eventoDaVez.tipo == TipoEvento::DEADLINE_STOURO)
            {
                //Desenha um x vermelho
                listaDesenhos->AddLine (pMin, pMax, IM_COL32(255, 0, 0, 255), 2.0f);
                listaDesenhos->AddLine (ImVec2(pMin.x + m_larguraTick, pMin.y), ImVec2(pMax.x - m_larguraTick, pMax.y), IM_COL32(255, 0, 0, 255), 2.0f);
            }
            if (eventoDaVez.tipo == TipoEvento::SORTEIO_DESEMPATE)
            {
                // Desenha um circulo amarelo
                ImVec2 centro(pMin.x + (m_larguraTick / 2.0f), pMin.y + (m_alturaLinha / 2.0f));
                listaDesenhos->AddCircleFilled(centro, 4.0f, IM_COL32(255, 215, 0, 255));
            }
        }

    }

}

PainelGraficoGantt::PainelGraficoGantt(const std::vector<TarefaGUI>* tarefas, const std::vector<SnapshotTick>* historico, 
    const int* tickAtual, const int* qtdeCpus, int* modoExecucao) :
    m_tarefasRef(tarefas),
    m_historicoRef(historico),
    m_tickAtualRef(tickAtual),
    m_qtdeCpusRef(qtdeCpus),
    m_larguraTick(100.0f),
    m_alturaLinha(70.0f),
    m_margemEsquerda(100.0f),
    m_margemTopo(100.0f),
    m_modoExecucao(modoExecucao)
{

}

ImVec2 PainelGraficoGantt::dimensoesTotais()
{
    if (m_historicoRef == nullptr || m_tarefasRef == nullptr)
        return ImVec2(800, 600);
    /*
    const std::vector<TarefaGUI>* m_tarefasRef;
    const std::vector<SnapshotTick>* m_historicoRef;
    */
    int totalTicks = static_cast<int>(m_historicoRef->size());
    int numTarefas = static_cast<int>(m_tarefasRef->size());

    float larguraTotal = m_margemEsquerda + ((1 + totalTicks) * m_larguraTick);
    float alturaTotal = m_margemTopo + ((1 + numTarefas) * m_alturaLinha);

    return ImVec2(larguraTotal, alturaTotal);
}

// Renderização Off-Screen usando apenas Raylib nativo (Atende o Requisito 2.4 do Edital)
void PainelGraficoGantt::desenharExportacaoRaylib(int larguraCanvas, int alturaCanvas)
{
    if (m_tarefasRef == nullptr || m_historicoRef == nullptr || m_historicoRef->empty())
        return;

    std::vector<TarefaGUI> t_Ord = *m_tarefasRef;
    std::sort(t_Ord.begin(), t_Ord.end(), [](const TarefaGUI& a, const TarefaGUI& b) {
        return a.id > b.id;
        });

    int totalTicks = static_cast<int>(m_historicoRef->size());
    float origemX = 0.0f;
    float origemY = 0.0f;

    // 1. Régua X e Linhas Verticais
    for (int t = 0; t < totalTicks; t++)
    {
        float x = origemX + m_margemEsquerda + (t * m_larguraTick);
        DrawText(TextFormat("%d", t), static_cast<int>(x + 5.0f), static_cast<int>(origemY + 10.0f), 16, DARKGRAY);

        float yTop = origemY + m_margemTopo;
        float yBottom = origemY + m_margemTopo + (t_Ord.size() * m_alturaLinha);
        DrawLineEx(Vector2{ x, yTop }, Vector2{ x, yBottom }, 1.0f, LIGHTGRAY);
    }

    // 2. Eixo Y e Linhas Horizontais
    for (size_t i = 0; i < t_Ord.size(); i++)
    {
        float y = origemY + m_margemTopo + (i * m_alturaLinha);
        std::string label = "Tarefa " + std::to_string(t_Ord[i].id);
        DrawText(label.c_str(), static_cast<int>(origemX + 10.0f), static_cast<int>(y + 5.0f), 16, DARKGRAY);

        float xMin = origemX + m_margemEsquerda;
        float xMax = origemX + m_margemEsquerda + (totalTicks * m_larguraTick);
        DrawLineEx(Vector2{ xMin, y + m_alturaLinha }, Vector2{ xMax, y + m_alturaLinha }, 1.0f, LIGHTGRAY);
    }

    size_t limite;

    // Define até qual tick o gráfico deve ser revelado na interface no modo Passo a Passo
    // Revela o grafico inteiro no modo instantaneo
    if ((*m_modoExecucao) == 0)
    {
        int tickLimite = *m_tickAtualRef;
        size_t totalTicksDisponiveis = m_historicoRef->size();

        // Garante que não ultrapassará o tamanho do vetor do histórico
        limite = std::min(static_cast<size_t>(tickLimite + 1), totalTicksDisponiveis);
    }
    else
    {
        limite = m_historicoRef->size();
    }
    

    // 3. Blocos de Execução e Hachura
    for (int t = 0; t < limite; t++)
    {
        const SnapshotTick& snap = (*m_historicoRef)[t];

        for (size_t i = 0; i < t_Ord.size(); i++)
        {
            float x = origemX + m_margemEsquerda + (t * m_larguraTick);
            float y = origemY + m_margemTopo + (i * m_alturaLinha);

            const SnapshotTarefa* snapCerto = nullptr;
            for (size_t j = 0; j < snap.tarefas.size(); j++)
            {
                if (snap.tarefas[j].idTarefa == t_Ord[i].id)
                {
                    snapCerto = &snap.tarefas[j];
                    break;
                }
            }

            if (!snapCerto) continue;

            if (snapCerto->estado == EstadoTarefa::EXECUTANDO)
            {
                Color cor = hexParaRaylibColor(t_Ord[i].cor, 1.0f);
                DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(m_larguraTick), static_cast<int>(m_alturaLinha), cor);
                DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(m_larguraTick), static_cast<int>(m_alturaLinha), BLACK);
            }
            else if (snapCerto->estado == EstadoTarefa::PRONTA)
            {
                DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(m_larguraTick), static_cast<int>(m_alturaLinha), GRAY);
            }
            else if (snapCerto->estado == EstadoTarefa::ESPERANDO)
            {
                DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(m_larguraTick), static_cast<int>(m_alturaLinha), BLACK);

                BeginScissorMode(static_cast<int>(x), static_cast<int>(y), static_cast<int>(m_larguraTick), static_cast<int>(m_alturaLinha));
                float passo = 6.0f;
                for (float offset = -m_alturaLinha; offset < m_larguraTick; offset += passo)
                {
                    Vector2 p1{ x + offset, y };
                    Vector2 p2{ x + offset + m_alturaLinha, y + m_alturaLinha };
                    DrawLineEx(p1, p2, 1.0f, LIGHTGRAY);
                }
                EndScissorMode();

                DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(m_larguraTick), static_cast<int>(m_alturaLinha), GRAY);
            }
        }
    }

    // 4. Desenho dos Eventos
    for (int t = 0; t < limite; t++)
    {
        const SnapshotTick& snap = (*m_historicoRef)[t];

        for (size_t e = 0; e < snap.eventos.size(); e++)
        {
            const EventoGantt& ev = snap.eventos[e];

            int linhaIndex = -1;
            for (size_t i = 0; i < t_Ord.size(); i++)
            {
                if (t_Ord[i].id == ev.idTarefa)
                {
                    linhaIndex = static_cast<int>(i);
                    break;
                }
            }

            if (linhaIndex == -1) continue;

            float x = origemX + m_margemEsquerda + (t * m_larguraTick);
            float y = origemY + m_margemTopo + (linhaIndex * m_alturaLinha);

            if (ev.tipo == TipoEvento::INGRESSO)
            {
                Vector2 p1{ x, y };
                Vector2 p2{ x + m_larguraTick, y };
                Vector2 p3{ x + (m_larguraTick / 2.0f), y + 8.0f };
                // A Raylib exige vértices em ordem anti-horária para preenchimento
                DrawTriangle(p1, p3, p2, GREEN);
            }
            else if (ev.tipo == TipoEvento::DEADLINE_STOURO)
            {
                Vector2 p1{ x + 4.0f, y + 4.0f };
                Vector2 p2{ x + m_larguraTick - 4.0f, y + m_alturaLinha - 4.0f };
                Vector2 p3{ x + m_larguraTick - 4.0f, y + 4.0f };
                Vector2 p4{ x + 4.0f, y + m_alturaLinha - 4.0f };

                DrawLineEx(p1, p2, 2.0f, RED);
                DrawLineEx(p3, p4, 2.0f, RED);
            }
            else if (ev.tipo == TipoEvento::SORTEIO_DESEMPATE)
            {
                Vector2 centro{ x + (m_larguraTick / 2.0f), y + (m_alturaLinha / 2.0f) };
                DrawCircleV(centro, 4.0f, GOLD);
            }
        }
    }
}
    
void PainelGraficoGantt::renderizar()
{
    ImVec2 posicao = ImGui::GetMainViewport()->Pos;
    ImVec2 tamanho = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos (posicao);
    ImGui::SetNextWindowSize (tamanho);

    ImVec2 origemQuadro;
    ImDrawList* listaDesenhos;

    /*ImGuiWindowFlags_NoTitleBar = remove a barra de título superior,
        ImGuiWindowFlags_NoResize / ImGuiWindowFlags_NoMove = impede o usuário de arrastar o fundo,
        ImGuiWindowFlags_NoBringToFrontOnFocus = garante que o fundo fique sempre atrás dos painéis de controle*/

    if (ImGui::Begin("Grafico de Gantt Multi-CPU", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        //Coisas importantes
        origemQuadro = ImGui::GetCursorScreenPos();
        listaDesenhos = ImGui::GetWindowDrawList();

        if (m_tarefasRef == nullptr || m_historicoRef == nullptr)
        {
            ImGui::End();
            return;
        }
        
        // Ordenacao decrescente dos IDs para posicionamento vertical no Eixo Y
        std::vector<TarefaGUI> t_Ord = *m_tarefasRef;

        std::sort(t_Ord.begin(), t_Ord.end(), [](const TarefaGUI& a, const TarefaGUI& b) {
            return a.id > b.id;
            });

        int totalTicks = static_cast<int>(m_historicoRef->size());
        float larguraTotal = m_margemEsquerda + (totalTicks * m_larguraTick) + 50.0f;
        float alturaTotal = m_margemTopo + (t_Ord.size() * m_alturaLinha) + 50.0f;

        // Adiciona área com barra de rolagem (Scrollbar) automática do ImGui
        ImGui::Dummy(ImVec2(larguraTotal, alturaTotal));

        // Chamada encadeada dos renderizadores do Gantt
        desenharReguaEixoX(listaDesenhos, origemQuadro, totalTicks);
        desenharEixoYELinhasBase(listaDesenhos, origemQuadro, t_Ord);
        desenharBlocosExecucao(listaDesenhos, origemQuadro, t_Ord);
        desenharEventosESinais(listaDesenhos, origemQuadro, t_Ord);

        desenharAgulhaTempoAtual(listaDesenhos, origemQuadro, t_Ord.size());
    }
    
    ImGui::End();
}