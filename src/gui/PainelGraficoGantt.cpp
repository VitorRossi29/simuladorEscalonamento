#include "PainelGraficoGantt.hpp"
#include <algorithm>
#include <string>
#include "PainelTabelaTarefas.hpp"
#include "raylib.h"

const char* estadoParaString(EstadoTarefa estado);


ImU32 PainelGraficoGantt::hexParaImU32(const char* hex, float alpha)
{
    if (hex == nullptr || strlen(hex) < 6)
        return IM_COL32(128, 128, 128, 255); // Cor cinza padrao em caso de falha

    unsigned int r_vermelho = 0, g_verde = 0, b_azul = 0;
    sscanf(hex, "%02x%02x%02x", &r_vermelho, &g_verde, &b_azul);

    return IM_COL32(r_vermelho, g_verde, b_azul, static_cast<int>(alpha * 255.0f));
}

// Funcao auxiliar local para converter Hexadecimal em Color da Raylib
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
        
        std::string txt = std::to_string(t);

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

    // ALTERAcaO/CORREcaO: Desenha os rohtulos das linhas dedicadas das CPUs no eixo Y (acima da lista de tarefas)
    if (m_qtdeCpusRef != nullptr)
    {
        int totalCpus = *m_qtdeCpusRef;
        for (int c = 0; c < totalCpus; c++)
        {
            int idCpu = c + 1;
            // Posicao Y da linha da CPU correspondente
            float yCentro = inicio.y + m_margemTopo - 18.0f - (c * 22.0f);
            std::string labelCpu = "CPU " + std::to_string(idCpu);

            ImVec2 posTexto(inicio.x + 10, yCentro - 6.0f);
            listaDesenhos->AddText(posTexto, IM_COL32(180, 220, 180, 255), labelCpu.c_str());
        }
    }

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

    // Define até qual tick o grafico deve ser revelado na interface no modo passo a passo
    // E defube o limite como o ultimo tick no modo instantaneo
    if (!(*m_modoExecucao))
    {
        int tickLimite = *m_tickAtualRef;
        size_t totalTicksDisponiveis = m_historicoRef->size();

        // Garante que nao ultrapassara o tamanho do vetor do histohrico
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
            
            const SnapshotTarefaGUI* snapCerto = nullptr;
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
                continue; // Ignora o desenho desta célula se a tarefa nao tiver registro neste tick
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
                //Desenho apenas da borda (ausencia de cor)
                listaDesenhos->AddRect(posicaoMin, posicaoMax, IM_COL32(100, 100, 100, 100));
            }
            else if (snapCerto->estado == EstadoTarefa::SUSPENSA)
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
            else if (snapCerto->estado == EstadoTarefa::TERMINADA)
            {
                // Preenchimento cinza-escuro sohlido e liso (sem hachuras)
                listaDesenhos->AddRectFilled(posicaoMin, posicaoMax, IM_COL32(45, 45, 45, 255));
                // Borda cinza bem fina/discreta
                listaDesenhos->AddRect(posicaoMin, posicaoMax, IM_COL32(70, 70, 70, 150));
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

    // Define até qual tick o grafico deve ser revelado na interface no modo passo a passo
    // E define o limite como o último tick no modo instantâneo
    if (!(*m_modoExecucao))
    {
        int tickLimite = *m_tickAtualRef;
        size_t totalTicksDisponiveis = m_historicoRef->size();

        // Garante que nao ultrapassara o tamanho do vetor do histohrico
        limite = std::min(static_cast<size_t>(tickLimite + 1), totalTicksDisponiveis);
    }
    else
    {
        limite = m_historicoRef->size();
    }

    for (size_t t = 0; t < limite; t++)
    {
        const SnapshotTick& snap = (*m_historicoRef)[t];

        // Deteccao e Desenho dos Sinais de Mudanca de Estado da CPU (Ligou / Desligou)
        for (size_t c = 0; c < snap.cpus.size(); c++)
        {
            int idCpu = snap.cpus[c].idCpu;
            bool executandoAgora = (snap.cpus[c].idTarefaExecutando != -1);
            bool executandoAntes = false;

            if (t > 0)
            {
                const SnapshotTick& snapAnterior = (*m_historicoRef)[t - 1];
                for (const auto& cpuAnt : snapAnterior.cpus)
                {
                    if (cpuAnt.idCpu == idCpu)
                    {
                        executandoAntes = (cpuAnt.idTarefaExecutando != -1);
                        break;
                    }
                }
            }

            bool ligou = (!executandoAntes && executandoAgora);
            bool desligou = (executandoAntes && !executandoAgora);

            if (ligou || desligou)
            {
                float xMinCell = inicio.x + m_margemEsquerda + (t * m_larguraTick);
                float raio = 9.0f;

                // ALTERAcaO/CORREcaO (Deslocamento nas Bordas):
                // LIGOU: Posicionado no lado esquerdo do tick (início da execucao)
                // DESLIGOU: Posicionado no lado direito do tick (término da execucao/início do desligamento)
                float xCentro = ligou ? (xMinCell + 14.0f) : (xMinCell + m_larguraTick - 14.0f);

                int idxCpu = idCpu - 1;
                float yCentro = inicio.y + m_margemTopo - 18.0f - (idxCpu * 22.0f);

                ImVec2 centro(xCentro, yCentro);
                ImU32 corCirculo = ligou ? IM_COL32(0, 180, 0, 255) : IM_COL32(220, 0, 0, 255);

                // Desenha o círculo do sinal
                listaDesenhos->AddCircleFilled(centro, raio, corCirculo);
                listaDesenhos->AddCircle(centro, raio, IM_COL32(0, 0, 0, 255), 0, 1.5f); // Borda preta

                // Desenha o número do ID da CPU centralizado
                std::string idTxt = std::to_string(idCpu);
                ImVec2 tamTexto = ImGui::CalcTextSize(idTxt.c_str());
                ImVec2 posTexto(centro.x - (tamTexto.x / 2.0f), centro.y - (tamTexto.y / 2.0f));
                listaDesenhos->AddText(posTexto, IM_COL32(255, 255, 255, 255), idTxt.c_str());
            }
        }

        // Laco interno para percorrer os eventos das tarefas
        for (size_t e = 0; e < snap.eventos.size(); e++)
        {
            const EventoGantt& eventoDaVez = snap.eventos[e];

            // Busca a linha i do eixo Y da tarefa
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

            // Calculo da posicao X e Y da célula da tarefa no tick t
            float xMin = inicio.x + m_margemEsquerda + (t * m_larguraTick);
            float yMin = inicio.y + m_margemTopo + (linhaIndex * m_alturaLinha);

            ImVec2 pMin(xMin, yMin);
            ImVec2 pMax(xMin + m_larguraTick, yMin + m_alturaLinha);

            if (eventoDaVez.tipo == TipoEvento::INGRESSO)
            {
                // Posiciona a seta verde na BORDA ESQUERDA do tick (início do intervalo de tempo t)
                int tamanhoTriangulo = 8;
                float xEsq = xMin;                   // Alinhado exatamente na borda vertical esquerda do tick
                float yTopo = yMin + 12.0f;          // Ponta do triângulo apontando para baixo dentro da célula
                float yBase = yMin;                  // Base alinhada na linha superior da célula

                // Definicao das pontas do triângulo
                ImVec2 pTopo(xEsq, yTopo);
                ImVec2 pBaseR(xEsq + tamanhoTriangulo, yBase);
                ImVec2 pBaseL(xEsq - tamanhoTriangulo, yBase);

                // Desenha o triângulo verde apontando para baixo na borda esquerda
                listaDesenhos->AddTriangleFilled(pTopo, pBaseL, pBaseR, IM_COL32(0, 255, 0, 255));
            }
            if (eventoDaVez.tipo == TipoEvento::DEADLINE_STOURO)
            {
                // Desenha um X vermelho
                listaDesenhos->AddLine(pMin, pMax, IM_COL32(255, 0, 0, 255), 2.0f);
                listaDesenhos->AddLine(ImVec2(pMin.x + m_larguraTick, pMin.y), ImVec2(pMax.x - m_larguraTick, pMax.y), IM_COL32(255, 0, 0, 255), 2.0f);
            }
            if (eventoDaVez.tipo == TipoEvento::SORTEIO_DESEMPATE)
            {
                // Desenha um círculo amarelo
                ImVec2 centro(pMin.x + (m_larguraTick / 2.0f), pMin.y + (m_alturaLinha / 2.0f));
                listaDesenhos->AddCircleFilled(centro, 4.0f, IM_COL32(255, 215, 0, 255));
            }
            if (eventoDaVez.tipo == TipoEvento::CONCLUSAO)
            {
                // Posiciona a seta azul na BORDA DIREITA do tick (término do intervalo de tempo t)
                int tamanhoTriangulo = 8;
                float xDir = xMin + m_larguraTick;   // Alinhado exatamente na borda vertical direita do tick
                float yTopo = yMin - 12.0f;          // Ponta do triângulo apontando para cima acima da célula
                float yBase = yMin;                  // Base alinhada na linha superior da célula

                // Definicao das pontas do triângulo
                ImVec2 pTopo(xDir, yTopo);
                ImVec2 pBaseR(xDir + tamanhoTriangulo, yBase);
                ImVec2 pBaseL(xDir - tamanhoTriangulo, yBase);

                // Desenha o triângulo azul apontando para cima na borda direita
                listaDesenhos->AddTriangleFilled(pTopo, pBaseL, pBaseR, IM_COL32(0, 0, 255, 255));
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
    m_larguraTick(70.0f),
    m_alturaLinha(40.0f),
    m_margemEsquerda(70.0f),
    m_margemTopo(70.0f),
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

// Renderizacao Off-Screen usando apenas Raylib nativo (Atende o Requisito 2.4 do Edital)
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

    // Régua X e Linhas Verticais
    for (int t = 0; t < totalTicks; t++)
    {
        float x = origemX + m_margemEsquerda + (t * m_larguraTick);
        DrawText(TextFormat("%d", t), static_cast<int>(x + 5.0f), static_cast<int>(origemY + 10.0f), 16, DARKGRAY);

        float yTop = origemY + m_margemTopo;
        float yBottom = origemY + m_margemTopo + (t_Ord.size() * m_alturaLinha);
        DrawLineEx(Vector2{ x, yTop }, Vector2{ x, yBottom }, 1.0f, LIGHTGRAY);
    }

    // Eixo Y e Linhas Horizontais (CPUs e Tarefas)
    if (m_qtdeCpusRef != nullptr)
    {
        int totalCpus = *m_qtdeCpusRef;
        for (int c = 0; c < totalCpus; c++)
        {
            int idCpu = c + 1;
            float yCentro = origemY + m_margemTopo - 18.0f - (c * 22.0f);
            std::string labelCpu = "CPU " + std::to_string(idCpu);
            DrawText(labelCpu.c_str(), static_cast<int>(origemX + 10.0f), static_cast<int>(yCentro - 6.0f), 14, DARKGRAY);
        }
    }

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

    if ((*m_modoExecucao) == 0)
    {
        int tickLimite = *m_tickAtualRef;
        size_t totalTicksDisponiveis = m_historicoRef->size();
        limite = std::min(static_cast<size_t>(tickLimite + 1), totalTicksDisponiveis);
    }
    else
    {
        limite = m_historicoRef->size();
    }

    // Blocos de Execucao e Hachura
    for (int t = 0; t < limite; t++)
    {
        const SnapshotTick& snap = (*m_historicoRef)[t];

        for (size_t i = 0; i < t_Ord.size(); i++)
        {
            float x = origemX + m_margemEsquerda + (t * m_larguraTick);
            float y = origemY + m_margemTopo + (i * m_alturaLinha);

            const SnapshotTarefaGUI* snapCerto = nullptr;
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
            else if (snapCerto->estado == EstadoTarefa::SUSPENSA)
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
            else if (snapCerto->estado == EstadoTarefa::TERMINADA)
            {
                Color cinzaEscuro = Color{ 45, 45, 45, 255 };
                DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(m_larguraTick), static_cast<int>(m_alturaLinha), cinzaEscuro);
                DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(m_larguraTick), static_cast<int>(m_alturaLinha), DARKGRAY);
            }
        }
    }

    // Desenho dos Eventos e Sinais da CPU na Exportacao Raylib
    for (int t = 0; t < limite; t++)
    {
        const SnapshotTick& snap = (*m_historicoRef)[t];

        for (size_t c = 0; c < snap.cpus.size(); c++)
        {
            int idCpu = snap.cpus[c].idCpu;
            bool executandoAgora = (snap.cpus[c].idTarefaExecutando != -1);
            bool executandoAntes = false;

            if (t > 0)
            {
                const SnapshotTick& snapAnterior = (*m_historicoRef)[t - 1];
                for (const auto& cpuAnt : snapAnterior.cpus)
                {
                    if (cpuAnt.idCpu == idCpu)
                    {
                        executandoAntes = (cpuAnt.idTarefaExecutando != -1);
                        break;
                    }
                }
            }

            bool ligou = (!executandoAntes && executandoAgora);
            bool desligou = (executandoAntes && !executandoAgora);

            if (ligou || desligou)
            {
                float xCell = origemX + m_margemEsquerda + (t * m_larguraTick);
                float raio = 9.0f;

                // ALTERAcaO/CORREcaO (Deslocamento nas Bordas - Raylib):
                float xCentro = ligou ? (xCell + 14.0f) : (xCell + m_larguraTick - 14.0f);

                int idxCpu = idCpu - 1;
                float yCentro = origemY + m_margemTopo - 18.0f - (idxCpu * 22.0f);

                Vector2 centro{ xCentro, yCentro };
                Color corCirculo = ligou ? Color{ 0, 180, 0, 255 } : Color{ 220, 0, 0, 255 };

                DrawCircleV(centro, raio, corCirculo);
                DrawCircleLines(static_cast<int>(xCentro), static_cast<int>(yCentro), raio, BLACK);

                std::string idTxt = std::to_string(idCpu);
                int larguraTexto = MeasureText(idTxt.c_str(), 12);
                DrawText(idTxt.c_str(), static_cast<int>(xCentro - (larguraTexto / 2.0f)), static_cast<int>(yCentro - 5.0f), 12, WHITE);
            }
        }

        // Eventos das tarefas
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
                float xEsq = x;
                Vector2 p1{ xEsq - 8.0f, y };
                Vector2 p2{ xEsq + 8.0f, y };
                Vector2 p3{ xEsq, y + 10.0f };
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
            else if (ev.tipo == TipoEvento::CONCLUSAO)
            {
                float xDir = x + m_larguraTick;
                Vector2 p1{ xDir - 8.0f, y };
                Vector2 p2{ xDir + 8.0f, y };
                Vector2 p3{ xDir, y - 10.0f };

                DrawTriangle(p1, p2, p3, BLUE);
            }
        }
    }
}
    
void PainelGraficoGantt::renderizar()
{
    ImVec2 posicao = ImGui::GetMainViewport()->Pos;
    ImVec2 tamanho = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos(posicao);
    ImGui::SetNextWindowSize(tamanho);

    ImVec2 origemQuadro;
    ImDrawList* listaDesenhos;

    if (ImGui::Begin("Grafico de Gantt Multi-CPU", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        //Coisas importantes
        origemQuadro = ImGui::GetCursorScreenPos();
        listaDesenhos = ImGui::GetWindowDrawList();
        ClearBackground(DARKGRAY);

        if (m_tarefasRef == nullptr || m_historicoRef == nullptr)
        {
            ImGui::End();
            return;
        }

        // ALTERAcaO/CORREcaO: Reajusta dinamicamente a margem superior com base no número de CPUs ativas
        int qtdeCpus = (m_qtdeCpusRef != nullptr) ? *m_qtdeCpusRef : 1;
        m_margemTopo = std::max(70.0f, 30.0f + (qtdeCpus * 22.0f));

        // Ordenacao decrescente dos IDs para posicionamento vertical no Eixo Y
        std::vector<TarefaGUI> t_Ord = *m_tarefasRef;

        std::sort(t_Ord.begin(), t_Ord.end(), [](const TarefaGUI& a, const TarefaGUI& b) {
            return a.id > b.id;
            });

        int totalTicks = static_cast<int>(m_historicoRef->size());
        float larguraTotal = m_margemEsquerda + (totalTicks * m_larguraTick) + 50.0f;
        float alturaTotal = m_margemTopo + (t_Ord.size() * m_alturaLinha) + 50.0f;

        // Adiciona area com barra de rolagem (Scrollbar) automatica do ImGui
        ImGui::Dummy(ImVec2(larguraTotal, alturaTotal));

        // Chamada encadeada dos renderizadores do Gantt
        desenharReguaEixoX(listaDesenhos, origemQuadro, totalTicks);
        desenharEixoYELinhasBase(listaDesenhos, origemQuadro, t_Ord);
        desenharBlocosExecucao(listaDesenhos, origemQuadro, t_Ord);
        desenharEventosESinais(listaDesenhos, origemQuadro, t_Ord);
        desenharAgulhaTempoAtual(listaDesenhos, origemQuadro, t_Ord.size());

        ImGui::Text("Legenda:");
        ImGui::Text("Tarefas: Cada linha do grafico, cada retangulo eh a tarefa em um tick.");
        ImGui::Text("Ticks: Cada coluna do grafico.");
        ImGui::Text("ESTADOS DAS TAREFAS:");
        ImGui::Text("Executando: Representado pelo retangulo preenchido por uma cor.");
        ImGui::Text("Pronta: Representado pelo retangulo com contorno mas nao preenchido. A tarefa quando esta pronta para executar mas nao esta executando");
        ImGui::Text("Suspensa: Representado pelo retangulo preto e com hachuras na diagonal. Quando a tarefa ainda nao esta pronta para executar.");
        ImGui::Text("EVENTOS DE ESCALONAMENTO:");
        ImGui::Text("Ingresso: Quando uma tarefa entra na fila de prontos, aparece uma seta verde para baixo");
        ImGui::Text("Conclusao: Quando uma tarefa conclui um ciclo de execucao, aparece uma seta azul para cima.");
        ImGui::Text("Estouro de Deadline: Quando uma tarefa perde seu prazo, aparece um X vermelho.");
        ImGui::Text("Sorteio de Desempate: Quando ocorre um sorteio para definir qual tarefa sera executada, aparece um circulo amarelo!");
        ImGui::Text("SINAIS DE ESTADO DA CPU:");
        ImGui::Text("Circulo Verde com ID na linha da CPU: A CPU indicada pelo numero entrou no estado EXECUTANDO (LIGOU) neste tick.");
        ImGui::Text("Circulo Vermelho com ID na linha da CPU: A CPU indicada pelo numero ficou sem tarefa e passou para o estado DESLIGADA neste tick.");
    }

    ImGui::End();
}