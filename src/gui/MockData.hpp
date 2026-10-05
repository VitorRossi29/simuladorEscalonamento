#include <vector>
#include <string>
#include "../core/MotorSimulacao.hpp"
#include "../core/Tarefa.hpp"


// Tipos de eventos visuais marcados na linha do tempo[cite: 6, 8, 9]
enum class TipoEvento {
    NENHUM,
    INGRESSO,         // Seta de entrada da tarefa no sistema[cite: 6, 7]
    CONCLUSAO,        // Ícone de término da instância[cite: 6]
    DEADLINE_STOURO,  // Alerta de estouro do prazo[cite: 8]
    SORTEIO_DESEMPATE // Marcador de desempate por sorteio[cite: 9]
};

// Registro de um evento específico ocorrido em um tick[cite: 6]
struct EventoGantt {
    int idTarefa;
    TipoEvento tipo;
    std::string detalhe;
};

// Estado individual de cada CPU no tick corrente[cite: 4, 5]
struct EstadoCPU {
    int idCpu;                // 0, 1, ..., N-1[cite: 4]
    int idTarefaExecutando;   // ID da tarefa em execução, ou -1 se a CPU estiver Ociosa/Desligada[cite: 5]
};

// Registro completo do estado de uma tarefa no tick corrente[cite: 5]
struct SnapshotTarefa {
    int idTarefa;
    EstadoTarefa estado;
    int duracaoRestante;      // Quanto tempo de CPU ainda precisa executar
    int prazoRestante;        // Quantos ticks faltam para o deadline
};

// Snapshot global de 1 Tick do Relógio (Unidade fundamental do Histórico)[cite: 5]
struct SnapshotTick {
    int tick;                                // Instante de tempo t (0, 1, 2, ...)[cite: 5]
    std::vector<EstadoCPU> cpus;             // Estado de cada CPU[cite: 4, 5]
    std::vector<SnapshotTarefa> tarefas;     // Estado de cada TCB[cite: 5]
    std::vector<EventoGantt> eventos;        // Eventos disparados neste tick[cite: 6]
};

struct TarefaMock {
    int id;                   // Identificador único (ex: 1, 2, 3)
    char cor[8];          // Hexadecimal RGB (ex: "FF0000" para vermelho)
    int ingresso;             // Instante de tempo (tick) de criação
    int duracao;              // Tempo de execução necessário em cada período
    int periodo;              // >0: Periódica, =0: Aperiódica[cite: 7]
    int prazo;                // Deadline relativo a partir da ativação
    std::string lista_eventos;// Reservado para o Projeto B
    EstadoTarefa estado;
};

//cria tarefas falsas para testar
class GeradorMock
{
private:
    //ja vai
public:
    GeradorMock() = delete;
    //instancia tarefas iniciais
    static std::vector<TarefaMock> criarTarefasIniciais();
    //gera o vetor de snapshots ao longo do tempo
    static std::vector<SnapshotTick> gerarHistoricoInicial(const std::vector<TarefaMock>& tarefas, int maxTicks, int maxCpus);
};