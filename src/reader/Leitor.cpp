#include "Leitor.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <vector>

// Tira os caractres invalidos do comeco e do fim
static std::string aparar(const std::string& string)
{
    // Encontra o primeiro caractere que é valido
    size_t primeiro = string.find_first_not_of(" \t\n\r");

    // Se nao tiver nenhum valido retorna vazio
    if (primeiro == std::string::npos)
        return "";

    // Encontra o ultimo caractere valido
    size_t ultimo = string.find_last_not_of(" \t\n\r");

    // Retorna a SUBSTRING que comeca no "primeiro" 
    // "ultimo - primeiro + 1" é o tamanho da substring
    return string.substr(primeiro, ultimo - primeiro + 1);
}

// Cria uma copia da string e transforma cada caractere em maiusculo
std::string Leitor::paraMaiusculo(const std::string& texto)
{
    std::string resultado = texto;

    //funcao lambda para converter os caracteres
    std::transform(resultado.begin(), resultado.end(), resultado.begin(), [](unsigned char c) { return std::toupper(c); });
    return resultado;
}

// Auxiliar para remover espacos, \r e \n das extremidades da linha
static std::string limparLinha(std::string str)
{
    return aparar(str);
}

// Auxiliar para converter string em int com tratamento de excecao
static int stringParaIntSegura(const std::string& texto, int linhaNum, const std::string& nomeCampo)
{
    std::string textoLimpo = aparar(texto);
    if (textoLimpo.empty())
    {
        throw std::runtime_error("Linha " + std::to_string(linhaNum) + ": O campo '" + nomeCampo + "' esta vazio no arquivo de configuracao.");
    }
    try
    {
        size_t pos = 0;
        int valor = std::stoi(textoLimpo, &pos);
        if (pos != textoLimpo.length())
        {
            throw std::invalid_argument("caracteres nao numericos");
        }
        return valor;
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("Linha " + std::to_string(linhaNum) + ": Valor invalido para o campo '" + nomeCampo + "': '" + texto + "'. Esperado um numero.");
    }
}

// Auxiliar para dividir a linha por ponto e virgula
static std::vector<std::string> dividirPorPontoEVirgula(const std::string& linha)
{
    std::vector<std::string> tokens;
    std::stringstream ss(linha);
    std::string token;

    while (std::getline(ss, token, ';'))
    {
        tokens.push_back(aparar(token));
    }

    return tokens;
}

// Le o arquivo
Configuracao Leitor::lerArquivo(const std::string& caminho)
{
    std::ifstream arquivo(caminho);

    if (!arquivo.is_open())
    {
        throw std::runtime_error("Nao foi possivel abrir o arquivo.");
    }

    Configuracao configuracao;

    std::string linha;
    int numeroLinha = 0;
    bool cabecalhoLido = false;

    //ignora linhas em branco no comeco
    while (std::getline(arquivo, linha))
    {
        numeroLinha++;

        // Remove marcador BOM UTF-8 do inicio do arquivo se existir
        if (numeroLinha == 1 && linha.size() >= 3 && static_cast<unsigned char>(linha[0]) == 0xEF &&
            static_cast<unsigned char>(linha[1]) == 0xBB && static_cast<unsigned char>(linha[2]) == 0xBF)
        {
            linha = linha.substr(3);
        }

        std::string linhaLimpa = aparar(linha);

        // Ignora linhas em branco
        if (linhaLimpa.empty())
            continue;

        std::vector<std::string> tokens = dividirPorPontoEVirgula(linhaLimpa);

        // Se a linha terminava com ';', o ultimo token fica vazio e removemos
        if (!tokens.empty() && tokens.back().empty())
        {
            tokens.pop_back();
        }

        if (!cabecalhoLido)
        {
            //primeira linha: configuracoes gerais
            if (tokens.size() < 3)
            {
                throw std::runtime_error("Linha " + std::to_string(numeroLinha) + ": Cabecalho invalido. Esperados 3 campos (algoritmo;quantum;quantidadeCPUs).");
            }

            //algoritmo
            //converte tudas as letras para maiusculo
            std::string campoAlg = paraMaiusculo(tokens[0]);

            if (campoAlg == "RM")
            {
                configuracao.algoritmo = TipoAlgoritmo::RATE_MONOTONIC;
            }
            else if (campoAlg == "EDF")
            {
                configuracao.algoritmo = TipoAlgoritmo::EARLIEST_DEADLINE_FIRST;
            }
            else
            {
                throw std::runtime_error("Linha " + std::to_string(numeroLinha) + ": Algoritmo desconhecido: " + tokens[0]);
            }

            //quantum
            configuracao.quantum = stringParaIntSegura(tokens[1], numeroLinha, "quantum");

            //quantidade de CPUs
            configuracao.quantidadeCPUs = stringParaIntSegura(tokens[2], numeroLinha, "quantidadeCPUs");

            if (configuracao.quantidadeCPUs < 1)
            {
                throw std::runtime_error("Linha " + std::to_string(numeroLinha) + ": A quantidade de CPUs deve ser maior ou igual a 1.");
            }

            cabecalhoLido = true;
        }
        else
        {
            //resto sao as tarefas
            if (tokens.size() < 6)
            {
                throw std::runtime_error("Linha " + std::to_string(numeroLinha) + ": Tarefa invalida. Esperados pelo menos 6 campos (id;cor;ingresso;duracao;periodo;prazo).");
            }

            //id
            short int id = static_cast<short int>(stringParaIntSegura(tokens[0], numeroLinha, "id"));

            //cor
            std::string cor = tokens[1];

            //ingresso
            int ingresso = stringParaIntSegura(tokens[2], numeroLinha, "ingresso");

            //duracao
            int duracao = stringParaIntSegura(tokens[3], numeroLinha, "duracao");

            //periodo
            int periodo = stringParaIntSegura(tokens[4], numeroLinha, "periodo");

            if (periodo < 0)
            {
                throw std::runtime_error("Linha " + std::to_string(numeroLinha) + ": Periodo invalido (" + std::to_string(periodo) + "). Valores menores que zero sao erros no arquivo.");
            }

            //prazo
            int prazo = stringParaIntSegura(tokens[5], numeroLinha, "prazo");

            //lista_eventos 
            // VAMOS USAR NA PARTE 2 DO TRABALHO
            std::string listaEventos = (tokens.size() >= 7) ? tokens[6] : "";

            Tarefa tarefa(id, duracao, periodo, prazo, ingresso, 0, cor);

            configuracao.tarefas.push_back(tarefa);
        }
    }

    if (!cabecalhoLido || configuracao.tarefas.empty())
    {
        throw std::runtime_error("O arquivo de configuracao deve conter o cabecalho e pelo menos uma tarefa valida.");
    }

    return configuracao;
}