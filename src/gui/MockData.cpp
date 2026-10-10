#include "MockData.hpp"
#include <time.h>
#include <stdlib.h>

//Historico de mentira de tarefas para testar

std::vector<TarefaMock> GeradorMock::criarTarefasIniciais()
{
    //conjunto das tarefas cadastradas

    std::vector<TarefaMock> tarefasCadastradas(3);

    tarefasCadastradas[0] = { 1, "FF4500", 0, 2, 5, 5, "" , EstadoTarefa::PRONTA };
    tarefasCadastradas[1] = { 2, "1E90FF", 0, 3, 8, 8, "" , EstadoTarefa::PRONTA };
    tarefasCadastradas[2] = { 3, "3CB371", 2, 2, 10, 9, "", EstadoTarefa::PRONTA };
    
    return tarefasCadastradas;
}

std::vector<SnapshotTick> GeradorMock::gerarHistoricoInicial(std::vector<TarefaMock>& tarefasCadastradas, int maxTicks, int maxCPUs, int quantum)
{
    srand(time(0));

    int q = (quantum > 0) ? quantum : 1;

    const int numTarefas = static_cast<int>(tarefasCadastradas.size());

    //historico de cada tick

    std::vector<SnapshotTick> historico(maxTicks);

    //ocorrencia de  todos os ticks
    for (int t = 0; t < maxTicks; t++)
    {
        //coloca as infos no tick atual
        historico[t].tick = t;

        //escolhas para a cpu
        for (int c = 0; c < maxCPUs; c++)
        {
            // A divisão inteira (t / q) faz com que o resultado só mude a cada 'q' ticks,
            // simulando a permanência da tarefa na CPU durante o quantum.
            int sorteio = ((t / q) + c) % numTarefas;

            //id da tarefa sendo executada
            int idTarefaExec;
            if (sorteio >= 0)
                idTarefaExec = tarefasCadastradas[sorteio].id;
            else
                idTarefaExec = -1;

            //define o estado da cpu que esta sendo analisada
            EstadoCPU cpuState;
            cpuState.idCpu = c;
            cpuState.idTarefaExecutando = idTarefaExec;

            //no tempo do tick t adicionou essa cpu com esse estado na lista de cpus
            historico[t].cpus.push_back(cpuState);
        }

        //definicao de cada tarefa (o antes era das cpus)
        for (int k = 0; k < numTarefas; k++)
        {
            //seleciona a tarefa atual
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

            //Cria foto momentanea instantanea da tarefa
            SnapshotTarefa snapTask;
            snapTask.idTarefa = idAtual;
            if (emExecucao)
                snapTask.estado = EstadoTarefa::EXECUTANDO;
            else
                snapTask.estado = EstadoTarefa::PRONTA;
            snapTask.duracaoRestante = tarefasCadastradas[k].duracao;
            snapTask.prazoRestante = tarefasCadastradas[k].prazo;

            //adiciona a tarefa no historico
            historico[t].tarefas.push_back(snapTask);
        }

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
