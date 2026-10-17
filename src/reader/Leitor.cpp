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

    //ignora linhas em branco no comeco
    while (std::getline(arquivo, linha))
    {
        if(linha.empty())
            continue;
        else
            break;
    }

    //primeira linha: configuracoes gerais
    if (!linha.empty() && linha.back()=='\r') 
        linha.pop_back(); //remove o retorno de carro do windows
    if (!linha.empty() && linha.back()==';') 
        linha.pop_back(); //remove o ponto e virgula do final 
        
    std::stringstream ss(linha); //le a linha
    std::string campo;
    std::getline(ss, campo, ';');
    campo.erase(std::remove_if(campo.begin(), campo.end(), ::isspace), campo.end()); //remove todos os espacos

    //converte tudas as letras para maiusculo
    campo=paraMaiusculo(campo);

    //algoritmo
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
    campo.erase(std::remove_if(campo.begin(), campo.end(), ::isspace), campo.end()); //remove todos os espacos
    configuracao.quantum=std::stoi(campo);

    //quantidade de CPUs
    std::getline(ss, campo, ';');
    campo.erase(std::remove_if(campo.begin(), campo.end(), ::isspace), campo.end()); //remove todos os espacos
    configuracao.quantidadeCPUs=std::stoi(campo);

	//resto sao as tarefas
    while (std::getline(arquivo, linha))
    {
        if(linha.empty())
            continue; //ignora linhas em branco

        if (!linha.empty() && linha.back()=='\r') 
            linha.pop_back(); //remove o retorno de carro do windows
        if (!linha.empty() && linha.back()==';') 
            linha.pop_back(); //remove o ponto e virgula do final 

        ss.str(linha); //le a linha
        ss.clear(); //limpa flags de erro pra reutilizar ss

        //id
        std::getline(ss, campo, ';');
        campo.erase(std::remove_if(campo.begin(), campo.end(), ::isspace), campo.end()); //remove todos os espacos
        short int id=std::stoi(campo);

        //cor
        std::getline(ss, campo, ';');
        campo.erase(std::remove_if(campo.begin(), campo.end(), ::isspace), campo.end()); //remove todos os espacos
        std::string cor=campo;

        //ingresso
        std::getline(ss, campo, ';');
        campo.erase(std::remove_if(campo.begin(), campo.end(), ::isspace), campo.end()); //remove todos os espacos
        int ingresso=std::stoi(campo);

        //duracao
        std::getline(ss, campo, ';');
        campo.erase(std::remove_if(campo.begin(), campo.end(), ::isspace), campo.end()); //remove todos os espacos
        int duracao=std::stoi(campo);

        //periodo
        std::getline(ss, campo, ';');
        campo.erase(std::remove_if(campo.begin(), campo.end(), ::isspace), campo.end()); //remove todos os espacos
        int periodo=std::stoi(campo);

        //prazo
        std::getline(ss, campo, ';');
        campo.erase(std::remove_if(campo.begin(), campo.end(), ::isspace), campo.end()); //remove todos os espacos
        int prazo=std::stoi(campo);

        Tarefa tarefa(id, duracao, periodo, prazo, ingresso, 0, cor);

        configuracao.tarefas.push_back(tarefa);
    }

    return configuracao;
}



