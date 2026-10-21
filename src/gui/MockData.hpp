
#pragma once
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// Declaração antecipada (Forward Declaration) para resolver dependências circulares
enum class EstadoTarefa;

// Tipos de eventos visuais marcados na linha do tempo
enum class TipoEvento {
    NENHUM,
    INGRESSO,         // Seta de entrada da tarefa no sistema
    CONCLUSAO,        // Ícone de término da instância
    DEADLINE_STOURO,  // Alerta de estouro do prazo
    SORTEIO_DESEMPATE // Marcador de desempate por sorteio
};

// Registro de um evento específico ocorrido em um tick
typedef struct {
    int idTarefa;
    TipoEvento tipo;
    std::string detalhe;
} EventoGantt;

// Estado individual de cada CPU no tick corrente
struct EstadoCPU {
    int idCpu;                // 0, 1, ..., N-1
    int idTarefaExecutando;   // ID da tarefa em execução, ou -1 se a CPU estiver Ociosa/Desligada
};

// Registro completo do estado de uma tarefa no tick corrente
struct SnapshotTarefaGUI {
    int idTarefa;
    EstadoTarefa estado;
    int duracaoRestante;      // Quanto tempo de CPU ainda precisa executar
    int prazoRestante;        // Quantos ticks faltam para o deadline
};

// Snapshot global de 1 Tick do Relógio (Unidade fundamental do Histórico)
struct SnapshotTick {
    int tick;                                // Instante de tempo t (0, 1, 2, ...)
    std::vector<EstadoCPU> cpus;             // Estado de cada CPU
    std::vector<SnapshotTarefaGUI> tarefas;  // Estado de cada TCB
    std::vector<EventoGantt> eventos;        // Eventos disparados neste tick
};

typedef struct {
    unsigned int id;           // Identificador único (ex: 1, 2, 3)
    char cor[8];               // Hexadecimal RGB (ex: "FF0000" para vermelho)
    int ingresso;              // Instante de tempo (tick) de criação
    int duracao;               // Tempo de execução necessário em cada período
    int periodo;               // >0: Periódica, =0: Aperiódica
    int prazo;                 // Deadline relativo a partir da ativação
    std::string lista_eventos; // Reservado para o Projeto B
    EstadoTarefa estado;
    short int prioridade;
} TarefaGUI;

// Define se usará o modelo de escalonamento REAL ou modelo falso MOCK para testes
enum class ModoFonteDados {
    MOCK,
    ENGINE_REAL
};

// Inclusão da definição de Tarefa.hpp para acesso aos valores de EstadoTarefa
#include "../core/Tarefa.hpp"

// Classe utilitária para gerar dados simulados (Mock)
class GeradorMock
{
public:
    GeradorMock() = delete;

    // Instancia tarefas iniciais
    static inline std::vector<TarefaGUI> criarTarefasIniciais()
    {
        std::vector<TarefaGUI> tarefasCadastradas(3);

        tarefasCadastradas[0] = { 1, "FF4500", 0, 2, 5, 5, "" , EstadoTarefa::PRONTA };
        tarefasCadastradas[1] = { 2, "1E90FF", 0, 3, 8, 8, "" , EstadoTarefa::PRONTA };
        tarefasCadastradas[2] = { 3, "3CB371", 2, 2, 10, 9, "", EstadoTarefa::PRONTA };

        return tarefasCadastradas;
    }

    // Gera o vetor de snapshots ao longo do tempo
    static inline std::vector<SnapshotTick> gerarHistoricoInicial(std::vector<TarefaGUI>& tarefasCadastradas, int maxTicks, int maxCPUs, int quantum)
    {
        srand(static_cast<unsigned int>(time(0)));

        int q = (quantum > 0) ? quantum : 1;
        const int numTarefas = static_cast<int>(tarefasCadastradas.size());

        std::vector<SnapshotTick> historico(maxTicks);

        for (int t = 0; t < maxTicks; t++)
        {
            historico[t].tick = t;

            // Definição do estado das CPUs
            for (int c = 0; c < maxCPUs; c++)
            {
                int sorteio = ((t / q) + c) % numTarefas;

                int idTarefaExec;
                if (sorteio >= 0)
                    idTarefaExec = tarefasCadastradas[sorteio].id;
                else
                    idTarefaExec = -1;

                EstadoCPU cpuState;
                cpuState.idCpu = c;
                cpuState.idTarefaExecutando = idTarefaExec;

                historico[t].cpus.push_back(cpuState);
            }

            // Definição do estado de cada tarefa
            for (int k = 0; k < numTarefas; k++)
            {
                int idAtual = tarefasCadastradas[k].id;

                bool emExecucao = false;
                for (int j = 0; j < maxCPUs; j++)
                {
                    if (historico[t].cpus[j].idTarefaExecutando == idAtual)
                    {
                        emExecucao = true;
                        break;
                    }
                }

                SnapshotTarefaGUI snapTask;
                snapTask.idTarefa = idAtual;
                if (emExecucao)
                    snapTask.estado = EstadoTarefa::EXECUTANDO;
                else
                    snapTask.estado = EstadoTarefa::PRONTA;
                snapTask.duracaoRestante = tarefasCadastradas[k].duracao;
                snapTask.prazoRestante = tarefasCadastradas[k].prazo;

                historico[t].tarefas.push_back(snapTask);
            }

            // Registro dos eventos de ingresso
            for (int k = 0; k < numTarefas; k++)
            {
                if (tarefasCadastradas[k].ingresso == t)
                {
                    EventoGantt ev;
                    ev.idTarefa = tarefasCadastradas[k].id;
                    ev.tipo = TipoEvento::INGRESSO;
                    ev.detalhe = "Tarefa " + std::to_string(ev.idTarefa) + " ingressou no sistema";

                    historico[t].eventos.push_back(ev);
                }
            }
        }

        return historico;
    }
};

