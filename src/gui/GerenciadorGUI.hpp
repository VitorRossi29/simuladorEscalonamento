#pragma once
#include "PainelConfiguracao.hpp"
#include "PainelControles.hpp"
#include "PainelTabelaTarefas.hpp"
#include "PainelGraficoGantt.hpp"

#include <vector>
#include <string>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include "../core/Tarefa.hpp"

//Forward Declaration para avisar que a classe existe
class MotorSimulacao;

// Essa struct representa a configuracao usada na leitura de arquivos
struct Configuracao
{
	TipoAlgoritmo algoritmo;
	int quantum;
	int quantidadeCPUs;
	std::vector<Tarefa> tarefas;
};

//Essa classe eh o controlador principal da GUI. Ele eh dono das 
//instancias dos paineis e dos dados da simulacao em memoria
class GerenciadorGUI
{
public:

	// Enum class dos tipos de log
	enum class TipoLog {
		INFO,
		AVISO,
		ERRO
	};

	// Struct para armazenar cada registro de log

	struct ItemLog {
		TipoLog tipo;
		std::string mensagem;
	};

private:
	MotorSimulacao* m_motor; //Guarda ponteiro do Motor Simulacao

	ModoFonteDados fonteDados; //Define se vai usar o simulador real ou versao Mock (versao com dados falsos para teste grafico)
	ModoFonteDados m_fonteDadosAnterior; // Para detectar quando o usuario muda

	std::vector<TarefaGUI> m_tarefas;
	std::vector<SnapshotTick> m_historico;
	
	int m_tickAtual;
	int m_totalTicks;
	bool m_emExecucaoAuto;
	float m_tempoAcumulado;
	float m_delayPasso; //Tempo em segundos por tick
	int m_quantidadeCPUs;
	int m_quantum;

	int m_modoExecucao;     //modo 0 = passo a passo; 1 = instantaneo
	int m_modoExecucaoAnterior;
	int m_algoritmoSelecionado;

	std::string m_algoritmo;

	int m_quantidadeCPUsAnterior, m_quantumAnterior, m_algoritmoAnterior;
	std::string m_algoritmoAnteriorString;

	std::string caminhoArquivo;
	std::string caminhoArquivoAnterior;

	bool m_exibirConfig, m_exibirControles, m_exibirTabela, m_exibirGantt, m_exibirLogs;
	bool m_solicitarExportacao; //Holy aura

	double utilizacaoCPU;
	bool querUtilizacao;

	// Vetor estatico que armazena o histohrico global de logs acessível por qualquer classe
	static std::vector<ItemLog> s_logs;
	bool resetSolicitado;

	//PRECISAM SER OS ULTIMOS
	PainelConfiguracao m_painelConfig;
	PainelControles m_painelControles;
	PainelTabelaTarefas m_painelTabela;
	PainelGraficoGantt m_painelGantt;

	// Refaz os calculos apos mudancas nas simulacao
	void recalcularSimulacao();

	// Traduz os dados da Engine para as structs da GUI
	void sincronizarComEngine();

	void atualizarTarefasNaEngine();

	void customizarEstilo();

	// Desenhar a janela de logs
	void desenharJanelaLogs();

public:
	GerenciadorGUI(MotorSimulacao* motor = nullptr);

	// Set do motor
	void setMotor(MotorSimulacao* motor) { m_motor = motor; }

	// Prepara as informacoes do GeradorMock caso o usuario queira
	void inicializar();

	//Controla o avanco automatico do tempo (m_tickAtual++) com base no 
	//m_delayPasso quando m_emExecucaoAuto for verdadeiro
	void atualizar(float deltaTime);

	//Chama os metodos de renderizacao de cada painel
	void renderizar();

	// Carrega o arquivo colocado pelo usuario no PainelConfiguracao
	void carregarArquivoEncontrado();

	// Exporta a imagem do escalonamento para o formato SVG
	void screenShotAvancada();

	// Faz o reescalonamento a partir do tick atual da simulacao
	void reescalonarAPartirDoTickAtual();	

	// Metodo estatico global para adicionar mensagens de erro/log de qualquer ponto do sistema
	static void registrarLog(TipoLog tipo, const std::string& mensagem);

	// Metodo estatico para limpar o historico de logs
	static void limparLogs();

	// Verifica se a tarefa esta fazendo sentido smeantico ou nao
	bool validarTarefa(TarefaGUI& t, const std::string& origem);
};

// Tira os caracteres invalidos do comeco e do fim
inline std::string aparar(const std::string& string)
{
	size_t primeiro = string.find_first_not_of(" \t\n\r");
	if (primeiro == std::string::npos)
		return "";

	size_t ultimo = string.find_last_not_of(" \t\n\r");
	return string.substr(primeiro, ultimo - primeiro + 1);
}

// Funcao auxiliar para registrar o erro especifico do arquivo no Console de Logs
// e lancar a excecao runtime_error correspondente
inline void notificarEErro(const std::string& mensagem)
{
	// Registra a mensagem detalhada no painel estatico de logs
	GerenciadorGUI::registrarLog(GerenciadorGUI::TipoLog::ERRO, "[Leitor] " + mensagem);
	throw std::runtime_error(mensagem);
}

// Auxiliar para remover espacos, \r e \n das extremidades da linha
inline std::string limparLinha(std::string str)
{
	return aparar(str);
}

// Auxiliar para converter string em int com tratamento de excecao
inline int stringParaIntSegura(const std::string& texto, int linhaNum, const std::string& nomeCampo)
{
	std::string textoLimpo = aparar(texto);
	if (textoLimpo.empty())
	{
		notificarEErro("Linha " + std::to_string(linhaNum) + ": O campo '" + nomeCampo + "' esta vazio no arquivo de configuracao.");
	}
	try
	{
		size_t pos = 0;
		int valor = std::stoi(textoLimpo, &pos);
		if (pos != textoLimpo.length())
		{
			notificarEErro("Linha " + std::to_string(linhaNum) + ": Valor invalido para o campo '" + nomeCampo + "': '" + texto + "'. Contem caracteres nao numericos.");
		}
		return valor;
	}
	catch (const std::exception&)
	{
		notificarEErro("Linha " + std::to_string(linhaNum) + ": Valor invalido para o campo '" + nomeCampo + "': '" + texto + "'. Esperado um numero inteiro.");
	}
	return 0;
}

// Auxiliar para dividir a linha por ponto e virgula
inline std::vector<std::string> dividirPorPontoEVirgula(const std::string& linha)
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

class Leitor
{
public:
	// Usa funcao lambda pra um determinado trecho ficar todo em maiusculo
	std::string paraMaiusculo(const std::string& texto)
	{
		std::string resultado = texto;
		std::transform(resultado.begin(), resultado.end(), resultado.begin(), [](unsigned char c) { return std::toupper(c); });
		return resultado;
	}

	// Leitura do arquivo
	Configuracao lerArquivo(const std::string& caminho)
	{
		std::ifstream arquivo(caminho);

		// Avisa se tiver tido erro pra abrir o arquivo (normalmente caminho escrito errado) 
		if (!arquivo.is_open())
		{
			notificarEErro("Nao foi possivel abrir o arquivo no caminho '" + caminho + "'. Verifique se o caminho esta correto.");
		}

		// A struct configuracao contem as informacoes estruturais do escalonamento, numero de cpus, algoritmo, quantum, etc
		Configuracao configuracao;

		std::string linha;
		int numeroLinha = 0;
		bool cabecalhoLido = false;

		while (std::getline(arquivo, linha))
		{
			numeroLinha++;

			// Retira alguns caracteres especiais que podem ter no inicio do arquivo e atrapalhar a leitura
			if (numeroLinha == 1 && linha.size() >= 3 && static_cast<unsigned char>(linha[0]) == 0xEF &&
				static_cast<unsigned char>(linha[1]) == 0xBB && static_cast<unsigned char>(linha[2]) == 0xBF)
			{
				linha = linha.substr(3);
			}


			std::string linhaLimpa = aparar(linha);

			if (linhaLimpa.empty())
				continue;

			// A gente divide cada linha em varios tokens pra analisar eles separadamente
			std::vector<std::string> tokens = dividirPorPontoEVirgula(linhaLimpa);

			if (!tokens.empty() && tokens.back().empty())
			{
				tokens.pop_back();
			}

			if (!cabecalhoLido)
			{
				if (tokens.size() < 3)
				{
					notificarEErro("Linha " + std::to_string(numeroLinha) + ": Cabecalho invalido. Esperados 3 campos (algoritmo;quantum;quantidadeCPUs). Encontrados: " + std::to_string(tokens.size()));
				}

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
					notificarEErro("Linha " + std::to_string(numeroLinha) + ": Algoritmo desconhecido: '" + tokens[0] + "'. Esperado 'RM' ou 'EDF'.");
				}

				configuracao.quantum = stringParaIntSegura(tokens[1], numeroLinha, "quantum");
				configuracao.quantidadeCPUs = stringParaIntSegura(tokens[2], numeroLinha, "quantidadeCPUs");

				if (configuracao.quantidadeCPUs < 1)
				{
					notificarEErro("Linha " + std::to_string(numeroLinha) + ": A quantidade de CPUs deve ser maior ou igual a 1. Valor informado: " + std::to_string(configuracao.quantidadeCPUs));
				}

				cabecalhoLido = true;
			}
			else
			{
				if (tokens.size() < 6)
				{
					notificarEErro("Linha " + std::to_string(numeroLinha) + ": Tarefa invalida. Esperados pelo menos 6 campos (id;cor;ingresso;duracao;periodo;prazo). Encontrados: " + std::to_string(tokens.size()));
				}

				// Pega as informacoes de verdade aqui e transforma em int antes obviamente

				short int id = static_cast<short int>(stringParaIntSegura(tokens[0], numeroLinha, "id"));
				std::string cor = tokens[1];
				int ingresso = stringParaIntSegura(tokens[2], numeroLinha, "ingresso");
				int duracao = stringParaIntSegura(tokens[3], numeroLinha, "duracao");
				int periodo = stringParaIntSegura(tokens[4], numeroLinha, "periodo");

				if (periodo < 0)
				{
					notificarEErro("Linha " + std::to_string(numeroLinha) + ": Periodo invalido (" + std::to_string(periodo) + "). Valores menores que zero sao erros no arquivo.");
				}

				int prazo = stringParaIntSegura(tokens[5], numeroLinha, "prazo");
				std::string listaEventos = (tokens.size() >= 7) ? tokens[6] : "";

				Tarefa tarefa(id, duracao, periodo, prazo, ingresso, 0, cor);
				configuracao.tarefas.push_back(tarefa);
			}
		}

		if (!cabecalhoLido || configuracao.tarefas.empty())
		{
			notificarEErro("O arquivo de configuracao deve conter o cabecalho e pelo menos uma tarefa valida.");
		}

		return configuracao;
	}
};