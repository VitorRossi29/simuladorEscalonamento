#include "Leitor.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

//cria uma copia da string e transforma cada caractere em maiusculo
std::string Leitor::paraMaiusculo(const std::string& texto)
{
    std::string resultado=texto;

	//funcao lambda para converter os caracteres
    std::transform(resultado.begin(), resultado.end(), resultado.begin(), [](unsigned char c) {return std::toupper(c);}); 
    return resultado;
}

Configuracao Leitor::lerArquivo(const std::string& caminho)
{
    std::ifstream arquivo(caminho);

    if (!arquivo.is_open())
    {
        throw std::runtime_error("Nao foi possivel abrir o arquivo.");
    }

    Configuracao configuracao;

    std::string linha;

    //primeira linha: configuracoes gerais
    if (std::getline(arquivo, linha))
    {
        std::stringstream ss(linha);
        std::string campo;

        //converte tudas as letras para maiusculo
        campo=paraMaiusculo(campo);

        //algoritmo
        std::getline(ss, campo, ';');
        if (campo=="RM")
        {
            configuracao.algoritmo=TipoAlgoritmo::RATE_MONOTONIC;
        }
        else if (campo=="EDF")
        {
            configuracao.algoritmo=TipoAlgoritmo::EARLIEST_DEADLINE_FIRST;
        }
        else
        {
            throw std::runtime_error("Algoritmo desconhecido: "+campo);
		}

        //quantum
        std::getline(ss, campo, ';');
        configuracao.quantum=std::stoi(campo);

        //quantidade de CPUs
        std::getline(ss, campo, ';');
        configuracao.quantidadeCPUs=std::stoi(campo);
    }

	//resto sao as tarefas
    while (std::getline(arquivo, linha))
    {
        std::stringstream ss(linha);
        std::string campo;

        //id
        std::getline(ss, campo, ';');
        short int id=std::stoi(campo);

        //cor
        std::getline(ss, campo, ';');
        std::string cor=campo;

        //ingresso
        std::getline(ss, campo, ';');
        int ingresso=std::stoi(campo);

        //duracao
        std::getline(ss, campo, ';');
        int duracao=std::stoi(campo);

        //periodo
        std::getline(ss, campo, ';');
        int periodo=std::stoi(campo);

        //prazo
        std::getline(ss, campo, ';');
        int prazo=std::stoi(campo);

        Tarefa tarefa(id, duracao, periodo, prazo, ingresso, 0, cor);

        configuracao.tarefas.push_back(tarefa);
    }

    return configuracao;
}



