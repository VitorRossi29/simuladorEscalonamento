#pragma once
#include <vector>
#include "../core/Metricas.hpp"
#include "../core/Tarefa.hpp"

struct Configuracao
{
    TipoAlgoritmo algoritmo;
    int quantum;
    int quantidadeCPUs;
    std::vector<Tarefa> tarefas;
};

class Leitor
{
public:
    Configuracao lerArquivo(const std::string& caminho);
	std::string paraMaiusculo(std::string texto);
};