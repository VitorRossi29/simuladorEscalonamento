#include "GerenciadorGUI.hpp"

#include "MockData.hpp"
#include "imgui.h"
#include "raylib.h"

#include "../core/MotorSimulacao.hpp"
//#include "../reader/Leitor.hpp"

#include <algorithm>
#include <fstream>

#ifndef ImGuiCol_TabActive
#define ImGuiCol_TabActive ImGuiCol_HeaderActive
#endif

#ifndef ImGuiCol_TabUnfocused
#define ImGuiCol_TabUnfocused ImGuiCol_Header
#endif

#ifndef ImGuiCol_TabUnfocusedActive
#define ImGuiCol_TabUnfocusedActive ImGuiCol_HeaderHovered
#endif

// Definicao/Instanciacao da variavel estatica global de logs
std::vector<GerenciadorGUI::ItemLog> GerenciadorGUI::s_logs;

GerenciadorGUI::GerenciadorGUI(MotorSimulacao* motor) :
	m_motor(motor),
	fonteDados(ModoFonteDados::MOCK),

	m_tickAtual(0),
	m_totalTicks(50),
	m_emExecucaoAuto(false),
	m_tempoAcumulado(0.0f),
	m_delayPasso(0.5f), //Tempo em segundos por tick
	m_quantidadeCPUs(2),
	m_quantum(1),
	m_modoExecucao(0),
	m_modoExecucaoAnterior(0),
	m_algoritmoSelecionado(0),
	m_algoritmo("RM"),
	caminhoArquivo(""),
	caminhoArquivoAnterior(""),

	m_quantidadeCPUsAnterior(m_quantidadeCPUs),
	m_quantumAnterior(m_quantum),
	m_algoritmoAnterior(m_algoritmoSelecionado),
	m_fonteDadosAnterior(fonteDados),
	m_algoritmoAnteriorString(m_algoritmo),

	querUtilizacao(false),
	utilizacaoCPU(0.0f),

	m_exibirConfig(true),
	m_exibirControles(true),
	m_exibirTabela(true),
	m_exibirGantt(true),
	m_exibirLogs(true),

	m_solicitarExportacao(false),

	m_painelConfig(&m_quantidadeCPUs, &m_quantum, m_algoritmo, &fonteDados, caminhoArquivo),
	m_painelControles(&m_tickAtual, &m_totalTicks, &m_emExecucaoAuto, &m_delayPasso, &m_modoExecucao, &m_algoritmoSelecionado, &m_solicitarExportacao),
	m_painelTabela(&m_tarefas, &m_historico, &m_tickAtual),
	m_painelGantt(&m_tarefas, &m_historico, &m_tickAtual, &m_quantidadeCPUs, &m_modoExecucao)

{
	//Carregar dados dos Mocks e etc
}

void GerenciadorGUI::customizarEstilo()
{
	// Dark Ruda style by Raikiri from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 4.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 14.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(0.9490196f, 0.95686275f, 0.9764706f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.35686275f, 0.41960785f, 0.46666667f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10980392f, 0.14901961f, 0.16862746f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.14901961f, 0.1764706f, 0.21960784f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.078431375f, 0.078431375f, 0.078431375f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.078431375f, 0.09803922f, 0.11764706f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.24705882f, 0.28627452f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.11764706f, 0.2f, 0.2784314f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.08627451f, 0.11764706f, 0.13725491f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08627451f, 0.11764706f, 0.13725491f, 0.65f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.078431375f, 0.09803922f, 0.11764706f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14901961f, 0.1764706f, 0.21960784f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.019607844f, 0.019607844f, 0.019607844f, 0.39f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2f, 0.24705882f, 0.28627452f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.1764706f, 0.21960784f, 0.24705882f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.08627451f, 0.20784314f, 0.30980393f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.2784314f, 0.5568628f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.2784314f, 0.5568628f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.36862746f, 0.60784316f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.24705882f, 0.28627452f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2784314f, 0.5568628f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.05882353f, 0.5294118f, 0.9764706f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.24705882f, 0.28627452f, 0.55f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 0.8f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.2f, 0.24705882f, 0.28627452f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.09803922f, 0.4f, 0.7490196f, 0.78f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.09803922f, 0.4f, 0.7490196f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 0.25f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.10980392f, 0.14901961f, 0.16862746f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 0.8f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.2f, 0.24705882f, 0.28627452f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.10980392f, 0.14901961f, 0.16862746f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.10980392f, 0.14901961f, 0.16862746f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.60784316f, 0.60784316f, 0.60784316f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.42745098f, 0.34901962f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392f, 0.69803923f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882353f, 0.1882353f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.30980393f, 0.30980393f, 0.34901962f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.22745098f, 0.22745098f, 0.24705882f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
}

//Esta funcao roda uma �nica vez no in�cio da aplicacao ou quando a simulacao precisa ser totalmente restaurada
void GerenciadorGUI::inicializar()
{
	m_tickAtual = 0;
	m_totalTicks = 50;
	m_quantidadeCPUs = 2;
	m_quantum = 1;
	m_delayPasso = 0.5f;
	m_emExecucaoAuto = false;
	m_tempoAcumulado = 0.0f;

	m_quantidadeCPUsAnterior = m_quantidadeCPUs;
	m_quantumAnterior = m_quantum;
	m_algoritmoAnterior = m_algoritmoSelecionado;

	m_solicitarExportacao = false;

	m_tarefas = GeradorMock::criarTarefasIniciais();
	m_historico = GeradorMock::gerarHistoricoInicial(m_tarefas, m_totalTicks, m_quantidadeCPUs, m_quantum);

	customizarEstilo();
}

void GerenciadorGUI::atualizar(float deltaTime)
{
	if (m_emExecucaoAuto == false)	//Verificacao
	{
		return;
	}
	else
	{
		m_tempoAcumulado += deltaTime;	//Acumulo de tempo
		if (m_tempoAcumulado >= m_delayPasso)	//Controle Passo
		{
			m_tempoAcumulado -= m_delayPasso;

			if (m_tickAtual < static_cast<int>(m_historico.size()) - 1)
			{
				m_tickAtual++;
			}
			//else if (fonteDados == ModoFonteDados::ENGINE_REAL && m_modoExecucao == 0)
			//{
			//	// Se chegou ao fim do historico no passo a passo, executa +1 tick no motor
			//	m_motor->executaPassoAPasso(1);
			//	sincronizarComEngine();
			//	m_tickAtual++;
			//}
			else
			{
				m_emExecucaoAuto = false;
				m_tempoAcumulado = 0.0f;
			}
		}

	}
}

void GerenciadorGUI::renderizar()
{
	int numeroTarefas = static_cast<int>(m_tarefas.size());

	float utilizacao = 0.0f;
	bool escalonavel = false;
	int totalDeadlines = 0;
	float tempoOciosoPercentual = 0.0f;
	int ticksOciosos = 0;

	//Validacao dos limites de tempo
	if (!m_historico.empty() && m_tickAtual >= 0 && m_tickAtual < static_cast<int>(m_historico.size()))
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Janelas"))
			{
				ImGui::MenuItem("Tabela de Tarefas", NULL, &m_exibirTabela);
				ImGui::MenuItem("Painel de Configuracao", NULL, &m_exibirConfig);
				ImGui::MenuItem("Painel de Controle", NULL, &m_exibirControles);
				//ImGui::MenuItem("Grafico de Gantt", NULL, &m_exibirGantt);
				ImGui::MenuItem("Console de Logs", NULL, &m_exibirLogs);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		bool configAlteradaNoPainel = false;

		// Desenho Sequencial dos Paineis
		if (m_exibirConfig)
			configAlteradaNoPainel = m_painelConfig.renderizar();

		if (m_exibirControles)
			m_painelControles.renderizar();

		if (m_exibirTabela)
		{
			if (m_painelTabela.renderizar())	// Se for true houve alteracao em uma tarefa pelo menos
			{
				// Envia os dados alterados na GUI para o motor antes de recalcular

				if (m_tickAtual == 0)
				{
					// Se estive no inicio recalcula tudo, nao tem problema
					atualizarTarefasNaEngine();
					recalcularSimulacao();
				}
				else
				{
					// Se estiver no meio da simulacao
					// Recalcula apenas daquele momento em diante
					reescalonarAPartirDoTickAtual();
				}
			}
		}

		if (m_exibirGantt)
			m_painelGantt.renderizar();

		if (m_exibirLogs)
			desenharJanelaLogs();


		// Checa se houve mudanca estrutural no arquivo
		bool mudancaNoArquivo = (configAlteradaNoPainel && (caminhoArquivo != caminhoArquivoAnterior));

		// Detecta se o algoritmo de escalonamento mudou no Painel de Configuracao
		bool mudancaAlgoritmo = (m_algoritmo != m_algoritmoAnteriorString);

		// Checa se algum atributo estrutural mudou (exige resetar a simulacao para o tick 0)
		bool mudancaDeParametrosGlobais = (m_quantidadeCPUs != m_quantidadeCPUsAnterior ||
			m_quantum != m_quantumAnterior ||
			m_modoExecucao != m_modoExecucaoAnterior ||
			fonteDados != m_fonteDadosAnterior);

		if (mudancaNoArquivo)
		{
			// Faz a troca dos caminhos: o novo vira antigo tambem
			caminhoArquivoAnterior = caminhoArquivo;
			// Carrega o arquivo, reseta o motor e zera m_tickAtual
			carregarArquivoEncontrado();
		}
		else if (mudancaDeParametrosGlobais)	// Mudanca estrutural (ex: CPUs) exige resetar para o tick 0
		{
			// Notifica o usu�rio via Log AVISO sobre qual alteracao causou o reset da simulacao
			if (m_quantidadeCPUs != m_quantidadeCPUsAnterior)
			{
				registrarLog(TipoLog::AVISO, "Quantidade de CPUs alterada de " + std::to_string(m_quantidadeCPUsAnterior) + " para " + std::to_string(m_quantidadeCPUs) + ". A simulacao foi resetada para o tick 0.");
			}
			if (m_quantum != m_quantumAnterior)
			{
				registrarLog(TipoLog::AVISO, "Valor do Quantum alterado de " + std::to_string(m_quantumAnterior) + " para " + std::to_string(m_quantum) + ". A simulacao foi resetada para o tick 0.");
			}
			if (m_modoExecucao != m_modoExecucaoAnterior)
			{
				registrarLog(TipoLog::AVISO, "Modo de execucao alterado. A simulacao foi resetada para o tick 0.");
			}
			if (fonteDados != m_fonteDadosAnterior)
			{
				std::string nomeFonte = (fonteDados == ModoFonteDados::MOCK) ? "Dados Simulados (Mock)" : "Motor Real (Engine)";
				registrarLog(TipoLog::AVISO, "Fonte de dados alterada para " + nomeFonte + ". A simulacao foi resetada para o tick 0.");
			}

			recalcularSimulacao();
			// Zera os ticks
			m_tickAtual = 0;

			//Atualiza as variaveis que afetam todas as tarefas
			m_quantidadeCPUsAnterior = m_quantidadeCPUs;
			m_quantumAnterior = m_quantum;
			m_algoritmoAnteriorString = m_algoritmo; // Atualiza a string do algoritmo
			m_modoExecucaoAnterior = m_modoExecucao;
			m_fonteDadosAnterior = fonteDados;
		}
		else if (mudancaAlgoritmo) // NOVO: Alteracao din�mica de algoritmo no meio do processo
		{
			m_algoritmoAnteriorString = m_algoritmo; // Atualiza o rastreador da string

			if (m_tickAtual == 0)
			{
				// Se a simulacao estiver no in�cio, recalcula do zero
				recalcularSimulacao();
			}
			else
			{
				// Se estiver no meio da simulacao (tick > 0), preserva o hist�rico passado 
				// e reescalona o futuro a partir do tick atual com o novo algoritmo escolhido
				GerenciadorGUI::registrarLog(TipoLog::INFO, "Algoritmo alterado para " + m_algoritmo + " no tick " + std::to_string(m_tickAtual) + ". Reescalonando o futuro.");
				reescalonarAPartirDoTickAtual();
			}
		}

		if (m_solicitarExportacao)
		{
			screenShotAvancada();
		}

		if (m_exibirConfig)
		{
			if (fonteDados == ModoFonteDados::ENGINE_REAL && m_motor != nullptr)
			{
				utilizacao = m_motor->calculaUtilizacaoCPU();
				escalonavel = m_motor->verificaEscalabilidade();

				const auto& historico = m_motor->getHistorico();
				int totalTicks = static_cast<int>(historico.size());
				int totalSlotsCPU = 0;

				if (totalTicks > 0)
				{
					for (size_t i = 0; i < historico.size(); i++)
					{
						// Acumula prazos perdidos do tick
						totalDeadlines += static_cast<int>(historico[i].idDeadlinesPerdidas.size());

						// Conta slots ociosos e total de slots de CPU no tick
						for (size_t exe = 0; exe < historico[i].tarefasExecutadas.size(); exe++)
						{
							totalSlotsCPU++;
							if (historico[i].tarefasExecutadas[exe].getId() == -1)
							{
								ticksOciosos++;
							}
						}
					}

					//  Calcula a porcentagem de ociosidade EM RELACAO AO PODER TOTAL DE TODAS AS CPUS
					if (totalSlotsCPU > 0)
					{
						tempoOciosoPercentual = (((float)(ticksOciosos)) / ((float)(totalSlotsCPU))) * 100.0f;
					}
				}
			}
			if (fonteDados == ModoFonteDados::ENGINE_REAL)
				m_painelConfig.renderizarMetricas(m_tarefas, utilizacao, escalonavel, totalDeadlines, tempoOciosoPercentual);
		}
	}
}

void GerenciadorGUI::carregarArquivoEncontrado()
{
	// Bloco try/catch para capturar falhas de abertura ou sintaxe do arquivo sem travar o sistema
	try
	{
		// Instanciacao local do leitor
		Leitor leitor;
		Configuracao config = leitor.lerArquivo(caminhoArquivo);

		if (caminhoArquivoAnterior != "")
			registrarLog(TipoLog::AVISO, "Novo arquivo carregado ('" + caminhoArquivo + "'). A simulacao foi resetada para o tick 0.");

		// GARANTIA: Forca a fonte de dados para o Motor Real (ENGINE_REAL) para que a GUI exiba os dados lidos
		fonteDados = ModoFonteDados::ENGINE_REAL;
		m_fonteDadosAnterior = ModoFonteDados::ENGINE_REAL;

		// Atualizacao do estado na GUI
		m_quantidadeCPUs = config.quantidadeCPUs;
		m_quantidadeCPUsAnterior = m_quantidadeCPUs;

		m_quantum = config.quantum;
		m_quantumAnterior = m_quantum;

		TipoAlgoritmo tipodesse = config.algoritmo;

		if (tipodesse == TipoAlgoritmo::RATE_MONOTONIC)
		{
			m_algoritmo = "RM";
			m_algoritmoSelecionado = 0;
		}
		else if (tipodesse == TipoAlgoritmo::EARLIEST_DEADLINE_FIRST)
		{
			m_algoritmo = "EDF";
			m_algoritmoSelecionado = 1;
		}
		else
		{
			m_algoritmo = "Outro";
			m_algoritmoSelecionado = 2;
		}
		m_algoritmoAnterior = m_algoritmoSelecionado;

		// Limpa os vetores do GUI e do motor
		m_tarefas.clear();
		m_motor->limparTarefas();

		const std::vector<std::string> paletaCores = {
			"#FF5733", "#33FF57", "#3357FF", "#F39C12",
			"#8E44AD", "#1ABC9C", "#D35400", "#2C3E50"
		};

		// Converte as tarefas lidas do arquivo e POPULA O VETOR m_tarefas (GUI)
		for (size_t i = 0; i < config.tarefas.size(); i++)
		{
			Tarefa& t = config.tarefas[i];

			TarefaGUI tGUI;
			tGUI.id = t.getId();
			tGUI.duracao = t.getDuracao();
			tGUI.periodo = t.getPeriodo();
			tGUI.prazo = t.getPrazo();
			tGUI.ingresso = t.getIngresso();
			tGUI.prioridade = t.getPrioridade();

			std::string corValida = t.getCor();
			if (corValida.empty() || corValida == "\0" || corValida == "null")
			{
				corValida = paletaCores[i % paletaCores.size()];
			}
			strncpy(tGUI.cor, corValida.c_str(), sizeof(tGUI.cor) - 1);
			tGUI.cor[sizeof(tGUI.cor) - 1] = '\0';

			// As tarefas devem ser inseridas em m_tarefas (vetor da GUI),
			// e NaO diretamente no m_motor, pois a chamada atualizarTarefasNaEngine() limpa o m_motor!
			m_tarefas.push_back(tGUI);
		}

		// Prepara os par�metros no motor
		m_tickAtual = 0;
		m_motor->setAlgoritmo(config.algoritmo);
		m_motor->setQuantidadeCPUs(m_quantidadeCPUs);
		m_motor->setQuantum(m_quantum);

		// Chama atualizarTarefasNaEngine para "corrigir" m_tarefas com o validarTarefa() 
		// e repassar os dados de m_tarefas para o motor antes de simular
		atualizarTarefasNaEngine();

		// Reinicio da simulacao
		m_motor->resetSimulacao();
		m_motor->executaTempoReal();

		sincronizarComEngine();

		// Registra sucesso no console de logs
		registrarLog(TipoLog::INFO, "Arquivo '" + caminhoArquivo + "' carregado com sucesso (" + std::to_string(m_tarefas.size()) + " tarefas).");
	}
	catch (const std::exception& e)
	{
		// Registra erro no console de logs caso o arquivo nao exista ou possua sintaxe invalida
		registrarLog(TipoLog::ERRO, "Falha ao carregar arquivo '" + caminhoArquivo + "': " + std::string(e.what()));
	}
}

void GerenciadorGUI::recalcularSimulacao()
{
	if (fonteDados == ModoFonteDados::ENGINE_REAL && m_motor != nullptr)
	{
		// Configura o algoritmo
		if (m_algoritmo == "RM")
			m_motor->setAlgoritmo(TipoAlgoritmo::RATE_MONOTONIC);
		else if (m_algoritmo == "EDF")
			m_motor->setAlgoritmo(TipoAlgoritmo::EARLIEST_DEADLINE_FIRST);

		m_motor->setQuantidadeCPUs(m_quantidadeCPUs);
		m_motor->setQuantum(m_quantum);

		m_motor->resetSimulacao();

		//if (m_modoExecucao == 1) // Modo Instant�neo Completo
		//{
		//	m_motor->executaTempoReal();
		//}
		//else // Modo Passo a Passo
		//{
		//	// Processa apenas o primeiro passo para inicializar o tick 0
		//	m_motor->executaPassoAPasso(1);
		//}

		// Executa a simulacao completa para gerar todo o histohrico do hiperper�odo/t�rmino
		m_motor->executaTempoReal();

		sincronizarComEngine();
	}
	else if (fonteDados == ModoFonteDados::MOCK)
	{
		m_tarefas = GeradorMock::criarTarefasIniciais();
		m_historico = GeradorMock::gerarHistoricoInicial(m_tarefas, 50, m_quantidadeCPUs, m_quantum);
	}

	m_totalTicks = static_cast<int>(m_historico.size());
}

void GerenciadorGUI::sincronizarComEngine()
{
	m_historico.clear();
	m_tarefas.clear();

	if (m_motor == nullptr) return;

	short int maiorID = 0;

	// Conversao das tarefas da Engine para TarefaGUI
	for (size_t t = 0; t < m_motor->getTarefas().size(); t++)
	{
		Tarefa tReal = m_motor->getTarefas()[t];
		TarefaGUI tGUI;

		int idAtual = tReal.getId();

		//confere se tem algum id igual
		for (const auto& jaAdicionada : m_tarefas)
		{
			if (jaAdicionada.id == idAtual)
			{
				//BOTAR MENSAGEM DE AVISO AQ
				idAtual = maiorID + 1;
				break;
			}
		}

		tGUI.id = idAtual;
		if (tGUI.id > maiorID)
		{
			maiorID = tGUI.id;
		}

		std::string corAtual = tReal.getCor();

		//impede duas tarefas de terem a mesma cor
		bool corRepetida = true;
		while (corRepetida)
		{
			corRepetida = false;
			for (const auto& jaAdicionada : m_tarefas)
			{
				if (std::string(jaAdicionada.cor) == corAtual)
				{
					corRepetida = true;
					break;
				}
			}

			if (corRepetida)
			{
				//BOTAR MENSAGEM DE AVISO AQ
				tReal.resetaCor();
				corAtual = tReal.getCor();
			}
		}

		tGUI.duracao = tReal.getDuracao();
		tGUI.periodo = tReal.getPeriodo();
		tGUI.prazo = tReal.getPrazo();
		tGUI.ingresso = tReal.getIngresso();

		strncpy(tGUI.cor, tReal.getCor().c_str(), sizeof(tGUI.cor) - 1);
		tGUI.cor[sizeof(tGUI.cor) - 1] = '\0';

		//Falta pegar os eventos mas isso � pra parte 2

		tGUI.prioridade = tReal.getPrioridade();

		m_tarefas.push_back(tGUI);
	}

	//ordena as tarefas do menor para o maior id com funcao lambda
	std::sort(m_tarefas.begin(), m_tarefas.end(), [](const TarefaGUI& a, const TarefaGUI& b) {
		return a.id > b.id;
		});

	// Conversao do historico de ticks do motor para SnapshotTick
	const auto& historicoEngine = m_motor->getHistorico();

	for (size_t i = 0; i < historicoEngine.size(); i++)
	{
		const Historico& historicoPasso = historicoEngine[i];
		SnapshotTick snapGUI;
		snapGUI.tick = historicoPasso.tempo;

		// Verifica cada cpu que estava sendo usada no instante em analise
		for (size_t cpuj = 0; cpuj < historicoPasso.tarefasExecutadas.size(); cpuj++)
		{
			EstadoCPU estadocpu;
			estadocpu.idCpu = static_cast<int>(cpuj + 1); // Atribui um id pro cpu, que comeca no cpu 1, cpu 2, etc
			estadocpu.idTarefaExecutando = historicoPasso.tarefasExecutadas[cpuj].getId();	// Pega o id da tarefa que estava sendo executada
			snapGUI.cpus.push_back(estadocpu);	// Coloca esse cpu na lista desse insante, com o id dele e da tarefa
		}

		// Aqui tem a parte que adquirimos as informacoes da TCB das tarefas
		for (size_t s = 0; s < historicoPasso.tarefas.size(); s++)
		{
			Tarefa tMotor = historicoPasso.tarefas[s];

			SnapshotTarefaGUI t;
			t.idTarefa = tMotor.getId();

			// Duracao restante eh igual na GUI e no Motor
			t.duracaoRestante = tMotor.getTempoRestante();

			// Prazo restante eh a proxima deadline/prazo tirando o tempo atual
			// Se for negativo, por seguranca, escolhe o zero
			t.prazoRestante = std::max(0, tMotor.getProximoDeadline() - historicoPasso.tempo);

			// Procura se a tarefa esta na lista de tarefas que estao executando
			// Se sim confirma no booleano
			bool executandoSim = false;
			for (size_t e = 0; e < snapGUI.cpus.size(); e++)
			{
				// Verifica se para a tarefa t em analise, ela esta sendo executada por algum cpu
				EstadoCPU& cpuEmAnalise = snapGUI.cpus[e];
				if (cpuEmAnalise.idTarefaExecutando == t.idTarefa)
				{
					// Caso o id aramazenado no cpu seja o mesmo que o ID da tarefa,
					// Ela esta sendo executada
					executandoSim = true;
					break;
				}
			}

			if (executandoSim)
			{
				t.estado = EstadoTarefa::EXECUTANDO;
			}
			else if (tMotor.getEstado() == EstadoTarefa::TERMINADA || tMotor.getQuantidadeExecucoes() >= 10)
			{
				t.estado = EstadoTarefa::TERMINADA;
			}
			else if (tMotor.getProximaLiberacao() <= historicoPasso.tempo && tMotor.getTempoRestante() > 0 && tMotor.getEstado() != EstadoTarefa::SUSPENSA)
			{
				// ALTERAcaO/CORREcaO: A tarefa soh entra na Fila de Prontas (PRONTA) quando seu per�odo chegou
				// (proximaLiberacao <= tempo) e ela possui duracao restante a ser executada na inst�ncia.
				t.estado = EstadoTarefa::PRONTA;
			}
			else
			{
				// ALTERAcaO/CORREcaO: Se a tarefa encerrou sua execucao da inst�ncia e aguarda a chegada do prohximo per�odo
				// (ou se foi suspensa manualmente pelo usuario), seu estado operacional � SUSPENSA.
				t.estado = EstadoTarefa::SUSPENSA;
			}

			snapGUI.tarefas.push_back(t);
			m_totalTicks = static_cast<int>(m_historico.size());
		}

		snapGUI.eventos = historicoPasso.eventos;

		//Adicao do Snapshot de Tick modificado no historico GUI
		m_historico.push_back(snapGUI);
	}
}

/*// Caso o grafico de Gantt seja maior que a tela do monitor, essa screenshot o contera por completo
void GerenciadorGUI::screenShotAvancada()
{
	ImVec2 dimensoesTotais = m_painelGantt.dimensoesTotais();

	RenderTexture2D alvo = LoadRenderTexture(dimensoesTotais.x, dimensoesTotais.y);
	BeginTextureMode(alvo);
	ClearBackground(RAYWHITE);
	m_painelGantt.desenharExportacaoRaylib(dimensoesTotais.x, dimensoesTotais.y);
	EndTextureMode();

	Image imagem = LoadImageFromTexture(alvo.texture);
	ImageFlipVertical(&imagem);

	// Lohgica para nome de arquivo �nico
	std::string prefixo = "../../../resultado_simulacao_gantt";
	std::string extensao = ".png";
	std::string caminhoFinal = prefixo + extensao;

	int contador = 1;
	// Enquanto o arquivo ja existir no diretohrio, incrementa o sufixo num�rico
	while (FileExists(caminhoFinal.c_str()))
	{
		caminhoFinal = prefixo + "_" + std::to_string(contador) + extensao;
		contador++;
	}

	ExportImage(imagem, caminhoFinal.c_str());

	UnloadImage(imagem);
	UnloadRenderTexture(alvo);

	m_solicitarExportacao = false;
}*/

/*//versao ruim
void GerenciadorGUI::screenShotAvancada()
{
	if (m_historico.empty() || m_tarefas.empty())
		return;

	int totalTicks = static_cast<int>(m_historico.size());
	int numTarefas = static_cast<int>(m_tarefas.size());

	// Define quantos ticks cabem em cada linha/bloco horizontal
	int ticksPorBloco = 115;
	int qtdBlocos = (totalTicks + ticksPorBloco - 1) / ticksPorBloco; // Arredonda para cima

	float larguraTick = 70.0f;
	float alturaLinha = 40.0f;
	float margemEsq = 70.0f;
	float margemTopo = 70.0f;

	int larguraBloco = static_cast<int>(margemEsq + (ticksPorBloco * larguraTick) + 50.0f);
	int alturaBloco = static_cast<int>(margemTopo + (numTarefas * alturaLinha) + 40.0f);

	int larguraFinal = std::min(larguraBloco, 8192);
	int alturaFinal = std::min(alturaBloco * qtdBlocos, 8192); // Limite seguro de VRAM

	RenderTexture2D alvo = LoadRenderTexture(larguraFinal, alturaFinal);
	BeginTextureMode(alvo);
	ClearBackground(GetColor(0x1E1E1EFF)); // Fundo escuro padr�o

	// Ordena as tarefas do mesmo modo que o painel faz (ID decrescente)
	std::vector<TarefaGUI> t_Ord = m_tarefas;
	std::sort(t_Ord.begin(), t_Ord.end(), [](const TarefaGUI& a, const TarefaGUI& b) {
		return a.id > b.id;
		});

	size_t limiteTicks = (m_modoExecucao == 0) ? std::min(static_cast<size_t>(m_tickAtual + 1), m_historico.size()) : m_historico.size();

	// Renderiza cada bloco empilhado verticalmente
	for (int b = 0; b < qtdBlocos; b++)
	{
		int tickInicio = b * ticksPorBloco;
		int tickFim = std::min(tickInicio + ticksPorBloco, totalTicks);

		float offsetY = b * alturaBloco;
		float offsetX = 0.0f;

		// 1. R�gua de ticks e linhas verticais
		for (int t = tickInicio; t < tickFim; t++)
		{
			int tRelativo = t - tickInicio;
			float x = offsetX + margemEsq + (tRelativo * larguraTick);

			DrawText(TextFormat("%d", t), static_cast<int>(x + 5.0f), static_cast<int>(offsetY + 10.0f), 16, DARKGRAY);

			float yTop = offsetY + margemTopo;
			float yBottom = offsetY + margemTopo + (t_Ord.size() * alturaLinha);
			DrawLineEx(Vector2{ x, yTop }, Vector2{ x, yBottom }, 1.0f, LIGHTGRAY);
		}

		// 2. R�tulos das tarefas no eixo Y e linhas horizontais
		for (size_t i = 0; i < t_Ord.size(); i++)
		{
			float y = offsetY + margemTopo + (i * alturaLinha);
			std::string label = "Tarefa " + std::to_string(t_Ord[i].id);
			DrawText(label.c_str(), static_cast<int>(offsetX + 10.0f), static_cast<int>(y + 5.0f), 16, DARKGRAY);

			float xMin = offsetX + margemEsq;
			float xMax = offsetX + margemEsq + ((tickFim - tickInicio) * larguraTick);
			DrawLineEx(Vector2{ xMin, y + alturaLinha }, Vector2{ xMax, y + alturaLinha }, 1.0f, LIGHTGRAY);
		}

		// 3. Blocos de execu��o correspondentes ao intervalo
		for (int t = tickInicio; t < tickFim; t++)
		{
			if (static_cast<size_t>(t) >= limiteTicks) continue;
			const SnapshotTick& snap = m_historico[t];
			int tRelativo = t - tickInicio;

			for (size_t i = 0; i < t_Ord.size(); i++)
			{
				float x = offsetX + margemEsq + (tRelativo * larguraTick);
				float y = offsetY + margemTopo + (i * alturaLinha);

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
					std::string corHex = t_Ord[i].cor;
					unsigned int r = 100, g = 150, b = 250;

					if (!corHex.empty())
					{
						const char* hexStr = corHex.c_str();
						if (hexStr[0] == '#') hexStr++;

						unsigned int hexVal = 0;
						if (sscanf(hexStr, "%x", &hexVal) == 1)
						{
							r = (hexVal >> 16) & 0xFF;
							g = (hexVal >> 8) & 0xFF;
							b = hexVal & 0xFF;
						}
					}

					Color cor = { static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b), 255 };
					DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(larguraTick), static_cast<int>(alturaLinha), cor);
					DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(larguraTick), static_cast<int>(alturaLinha), BLACK);
				}
				else if (snapCerto->estado == EstadoTarefa::PRONTA)
				{
					DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(larguraTick), static_cast<int>(alturaLinha), GRAY);
				}
				else if (snapCerto->estado == EstadoTarefa::SUSPENSA)
				{
					DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(larguraTick), static_cast<int>(alturaLinha), BLACK);
					DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(larguraTick), static_cast<int>(alturaLinha), GRAY);
				}
				else if (snapCerto->estado == EstadoTarefa::TERMINADA)
				{
					Color cinzaEscuro = Color{ 45, 45, 45, 255 };
					DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(larguraTick), static_cast<int>(alturaLinha), cinzaEscuro);
					DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(larguraTick), static_cast<int>(alturaLinha), DARKGRAY);
				}
			}
		}
	}

	EndTextureMode();

	Image imagem = LoadImageFromTexture(alvo.texture);
	ImageFlipVertical(&imagem);

	std::string prefixo = "../../../resultado_simulacao_gantt";
	std::string extensao = ".png";
	std::string caminhoFinal = prefixo + extensao;

	int contador = 1;
	while (FileExists(caminhoFinal.c_str()))
	{
		caminhoFinal = prefixo + "_" + std::to_string(contador) + extensao;
		contador++;
	}

	ExportImage(imagem, caminhoFinal.c_str());

	UnloadImage(imagem);
	UnloadRenderTexture(alvo);

	m_solicitarExportacao = false;
}*/

//versao svg
void GerenciadorGUI::screenShotAvancada()
{
	if (m_historico.empty() || m_tarefas.empty())
		return;

	int totalTicks = static_cast<int>(m_historico.size());
	int numTarefas = static_cast<int>(m_tarefas.size());
	int totalCpus = (m_quantidadeCPUs != 0) ? m_quantidadeCPUs : 1;

	// Dimens�es do gr�fico vetorial (ajustadas para dar espa�o �s CPUs no topo)
	float larguraTick = 70.0f;
	float alturaLinha = 40.0f;
	float margemEsq = 90.0f;
	float margemTopo = 50.0f + (totalCpus * 25.0f); // Espa�o din�mico para as linhas das CPUs

	float larguraTotal = margemEsq + (totalTicks * larguraTick) + 50.0f;
	float alturaTotal = margemTopo + (numTarefas * alturaLinha) + 50.0f;

	std::string prefixo = "../../../resultado_simulacao_gantt";
	std::string extensao = ".svg";
	std::string caminhoFinal = prefixo + extensao;

	int contador = 1;
	while (FileExists(caminhoFinal.c_str()))
	{
		caminhoFinal = prefixo + "_" + std::to_string(contador) + extensao;
		contador++;
	}

	std::ofstream svg(caminhoFinal);
	if (!svg.is_open())
	{
		m_solicitarExportacao = false;
		return;
	}

	svg << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
	svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << larguraTotal
		<< "\" height=\"" << alturaTotal << "\" viewBox=\"0 0 " << larguraTotal << " " << alturaTotal << "\">\n";

	svg << "<rect width=\"100%\" height=\"100%\" fill=\"#1E1E1E\"/>\n";

	std::vector<TarefaGUI> t_Ord = m_tarefas;
	std::sort(t_Ord.begin(), t_Ord.end(), [](const TarefaGUI& a, const TarefaGUI& b) {
		return a.id > b.id;
		});

	size_t limiteTicks = (m_modoExecucao == 0) ? std::min(static_cast<size_t>(m_tickAtual + 1), m_historico.size()) : m_historico.size();

	// 1. Desenhar as Linhas e R�tulos das CPUs no Topo
	for (int c = 0; c < totalCpus; c++)
	{
		int idCpu = c + 1;
		float yCentro = margemTopo - 30.0f - (c * 22.0f);

		// R�tulo da CPU
		svg << "<text x=\"15\" y=\"" << (yCentro + 5.0f)
			<< "\" fill=\"#B4EEB4\" font-family=\"sans-serif\" font-size=\"13\" font-weight=\"bold\">CPU " << idCpu << "</text>\n";
	}

	// 2. R�gua de Ticks e Linhas Verticais de Grade
	for (int t = 0; t < totalTicks; t++)
	{
		float x = margemEsq + (t * larguraTick);
		float yTop = margemTopo;
		float yBottom = margemTopo + (t_Ord.size() * alturaLinha);

		// Texto do Tick
		svg << "<text x=\"" << (x + 5.0f) << "\" y=\"" << (margemTopo - 15.0f)
			<< "\" fill=\"#A0A0A0\" font-family=\"sans-serif\" font-size=\"14\">" << t << "</text>\n";

		// Linha vertical de grade
		svg << "<line x1=\"" << x << "\" y1=\"" << yTop << "\" x2=\"" << x << "\" y2=\"" << yBottom
			<< "\" stroke=\"#404040\" stroke-width=\"1\"/>\n";
	}

	// 3. R�tulos das Tarefas no Eixo Y e Linhas Horizontais
	for (size_t i = 0; i < t_Ord.size(); i++)
	{
		float y = margemTopo + (i * alturaLinha);
		float xMin = margemEsq;
		float xMax = margemEsq + (totalTicks * larguraTick);

		svg << "<text x=\"10\" y=\"" << (y + 25.0f)
			<< "\" fill=\"#FFFFFF\" font-family=\"sans-serif\" font-size=\"14\" font-weight=\"bold\">Tarefa "
			<< t_Ord[i].id << "</text>\n";

		svg << "<line x1=\"" << xMin << "\" y1=\"" << (y + alturaLinha) << "\" x2=\"" << xMax << "\" y2=\"" << (y + alturaLinha)
			<< "\" stroke=\"#404040\" stroke-width=\"1\"/>\n";
	}

	// 4. Blocos de Execu��o do Gantt e Sinais de Estado das CPUs
	for (int t = 0; t < static_cast<int>(limiteTicks); t++)
	{
		const SnapshotTick& snap = m_historico[t];

		// Indicadores de estado da CPU neste tick espec�fico
		for (size_t c = 0; c < snap.cpus.size(); c++)
		{
			int idCpu = snap.cpus[c].idCpu;
			bool executandoAgora = (snap.cpus[c].idTarefaExecutando != -1);
			bool executandoAntes = false;

			if (t > 0)
			{
				const SnapshotTick& snapAnterior = m_historico[t - 1];
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
				float xCell = margemEsq + (t * larguraTick);
				float xCentro = ligou ? (xCell + 14.0f) : (xCell + larguraTick - 14.0f);
				int idxCpu = idCpu - 1;
				float yCentro = margemTopo - 30.0f - (idxCpu * 22.0f);
				std::string corCirculo = ligou ? "#00B400" : "#DC0000";

				// Desenha o c�rculo indicador (Verde = Ligou, Vermelho = Desligou)
				svg << "<circle cx=\"" << xCentro << "\" cy=\"" << yCentro << "\" r=\"9\" fill=\"" << corCirculo << "\" stroke=\"#000000\" stroke-width=\"1.5\"/>\n";
				// Texto do ID da CPU dentro do c�rculo
				svg << "<text x=\"" << (xCentro - 4.0f) << "\" y=\"" << (yCentro + 4.0f)
					<< "\" fill=\"#FFFFFF\" font-family=\"sans-serif\" font-size=\"11\" font-weight=\"bold\">" << idCpu << "</text>\n";
			}
		}

		// Blocos das tarefas
		for (size_t i = 0; i < t_Ord.size(); i++)
		{
			float x = margemEsq + (t * larguraTick);
			float y = margemTopo + (i * alturaLinha);

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
				std::string corHex = t_Ord[i].cor;
				if (corHex.empty() || corHex[0] != '#') corHex = "#6495ED";

				svg << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << larguraTick << "\" height=\"" << alturaLinha
					<< "\" fill=\"" << corHex << "\" stroke=\"#000000\" stroke-width=\"1\"/>\n";
			}
			else if (snapCerto->estado == EstadoTarefa::PRONTA)
			{
				svg << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << larguraTick << "\" height=\"" << alturaLinha
					<< "\" fill=\"none\" stroke=\"#808080\" stroke-width=\"1\"/>\n";
			}
			else if (snapCerto->estado == EstadoTarefa::SUSPENSA)
			{
				svg << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << larguraTick << "\" height=\"" << alturaLinha
					<< "\" fill=\"#000000\" stroke=\"#808080\" stroke-width=\"1\"/>\n";
			}
			else if (snapCerto->estado == EstadoTarefa::TERMINADA)
			{
				svg << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << larguraTick << "\" height=\"" << alturaLinha
					<< "\" fill=\"#2D2D2D\" stroke=\"#464646\" stroke-width=\"1\"/>\n";
			}
		}
	}

	svg << "</svg>\n";
	svg.close();

	m_solicitarExportacao = false;
}

// Se houve mudanca nas tarefas pela GUI ele vai atualizar as tarefas na ENGINE
void GerenciadorGUI::atualizarTarefasNaEngine()
{
	if (m_motor == nullptr)
	{
		return;
	}

	// Sanitiza todas as tarefas vindas da UI ou do arquivo de uma soh vez
	for (size_t i = 0; i < m_tarefas.size(); i++)
	{
		validarTarefa(m_tarefas[i], "Simulacao");
	}

	if (m_tickAtual == 0) // SE ESTIVER NO IN�CIO: Limpa e recria a lista do motor
	{
		m_motor->limparTarefas();

		for (size_t i = 0; i < m_tarefas.size(); i++)
		{
			const TarefaGUI& tGUI = m_tarefas[i];
			std::string corStringue = tGUI.cor;
			Tarefa tReal(tGUI.id,
				tGUI.duracao,
				tGUI.periodo,
				tGUI.prazo,
				tGUI.ingresso,
				tGUI.prioridade,
				corStringue);

			tReal.setEstado(tGUI.estado);
			m_motor->addTarefa(tReal);
		}
	}
	else // SE ESTIVER NO MEIO DA SIMULAcaO (TICK > 0): Repassa TODOS os par�metros editados
	{
		auto& tarefasMotor = const_cast<std::vector<Tarefa>&>(m_motor->getTarefas());

		// Atualiza tarefas existentes ou insere novas
		for (size_t i = 0; i < m_tarefas.size(); i++)
		{
			const TarefaGUI& tGUI = m_tarefas[i];
			bool encontrada = false;

			for (size_t j = 0; j < tarefasMotor.size(); j++)
			{
				if (tarefasMotor[j].getId() == tGUI.id)
				{
					encontrada = true;
					// Atualiza todos os par�metros alterados na tabela
					tarefasMotor[j].setIngresso(tGUI.ingresso);
					tarefasMotor[j].setDuracao(tGUI.duracao);
					tarefasMotor[j].setPeriodo(tGUI.periodo);
					tarefasMotor[j].setPrazo(tGUI.prazo);
					tarefasMotor[j].setEstado(tGUI.estado);
					tarefasMotor[j].setCor(tGUI.cor);
					break;
				}
			}

			// Se for uma tarefa totalmente nova adicionada durante a simulacao:
			if (!encontrada)
			{
				Tarefa tNova(tGUI.id,
					tGUI.duracao,
					tGUI.periodo,
					tGUI.prazo,
					tGUI.ingresso,
					tGUI.prioridade,
					tGUI.cor);

				tNova.setEstado(tGUI.estado);
				m_motor->addTarefa(tNova);
			}
		}

		// Remove do motor qualquer tarefa que tenha sido deletada na tabela
		for (auto it = tarefasMotor.begin(); it != tarefasMotor.end(); )
		{
			bool aindaExiste = false;
			for (size_t i = 0; i < m_tarefas.size(); i++)
			{
				if (m_tarefas[i].id == it->getId())
				{
					aindaExiste = true;
					break;
				}
			}

			if (!aindaExiste)
			{
				it = tarefasMotor.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void GerenciadorGUI::reescalonarAPartirDoTickAtual()
{
	if (m_motor == nullptr || fonteDados != ModoFonteDados::ENGINE_REAL)
		return;

	// Trunca o histohrico da Engine do tick atual em diante
	m_motor->resetaAPartirDe(m_tickAtual);

	atualizarTarefasNaEngine();

	// Garante que o algoritmo correto esteja definido
	if (m_algoritmo == "RM")
		m_motor->setAlgoritmo(TipoAlgoritmo::RATE_MONOTONIC);
	else if (m_algoritmo == "EDF")
		m_motor->setAlgoritmo(TipoAlgoritmo::EARLIEST_DEADLINE_FIRST);

	// Simula o futuro a partir da agulha de tempo atual at� o final
	m_motor->executaTempoReal();

	// Sincroniza o novo histohrico com a GUI preservando a posicao do tick
	sincronizarComEngine();

	// IMPORTANTE : REDEFINIR O M_TOTALTICKS
	m_totalTicks = static_cast<int>(m_historico.size());
}

// M�todo estatico para registrar erros e avisos de qualquer lugar do projeto
void GerenciadorGUI::registrarLog(TipoLog tipo, const std::string& mensagem)
{
	s_logs.push_back({ tipo, mensagem });
}

// M�todo estatico para limpar os registros de log
void GerenciadorGUI::limparLogs()
{
	s_logs.clear();
}

// Desenha a janela ImGui �nica com o histohrico de erros e violac�es do usuario
void GerenciadorGUI::desenharJanelaLogs()
{
	if (!m_exibirLogs) return;

	ImGui::SetNextWindowSize(ImVec2(500, 200), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Console de Logs e Violacoes", &m_exibirLogs))
	{
		if (ImGui::Button("Limpar Logs"))
		{
			limparLogs();
		}

		ImGui::SameLine();
		ImGui::TextDisabled("(Total: %zu mensagens)", s_logs.size());

		ImGui::Separator();

		// area rolavel contendo o histohrico de mensagens
		ImGui::BeginChild("ScrollRegionLogs", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		for (size_t i = 0; i < s_logs.size(); i++)
		{
			const ItemLog& log = s_logs[i];

			// Define a cor de acordo com o tipo de mensagem
			if (log.tipo == TipoLog::ERRO)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "[ERRO] %s", log.mensagem.c_str());
			}
			else if (log.tipo == TipoLog::AVISO)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "[AVISO] %s", log.mensagem.c_str());
			}
			else
			{
				ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "[INFO] %s", log.mensagem.c_str());
			}
		}

		// Rola automaticamente para a mensagem mais recente quando novas mensagens chegarem
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		{
			ImGui::SetScrollHereY(1.0f);
		}

		ImGui::EndChild();
	}
	ImGui::End();
}

// Funcao utilitaria para corrigir atributos invalidos e registrar logs de uma soh vez
bool GerenciadorGUI::validarTarefa(TarefaGUI& t, const std::string& origem)
{
	bool teveCorrecao = false;

	int idBase = 1;
	if (m_tarefas.empty())
	{
		idBase = 1;
	}
	else
	{
		idBase = m_tarefas.back().id + 1;
	}

	if (t.id < 0)	// ID NEGATIVO EH PROIBIDO
	{
		registrarLog(TipoLog::ERRO, origem + " - Tarefa " + std::to_string(t.id) + ": ID negativo. Ajustado para " + std::to_string(idBase));
		t.id = idBase;
		teveCorrecao = true;
	}

	int correcao = 1;
	for (size_t j = 0; j < correcao; j++)
	{
		for (size_t i = 0; i < m_tarefas.size(); i++)
		{
			// Fazemos o primeiro teste para impedir de comparar com si propria
			if (&t != &m_tarefas.at(i) && t.id == m_tarefas.at(i).id)		// ID REPETIDO EH PROIBIDO
			{
				registrarLog(TipoLog::ERRO, origem + " - Tarefa " + std::to_string(t.id) + ": ID repetido (" + std::to_string(t.ingresso) + "). Ajustado para 0" + std::to_string(m_tarefas.at(i).id + 1));
				t.id = m_tarefas.at(i).id + 1;
				teveCorrecao = true;
				correcao++;
			}
		}
	}

	if (t.duracao <= 0) // DURACAO NAO POSITIVA EH PROIBIDA
	{
		registrarLog(TipoLog::ERRO, origem + " - Tarefa " + std::to_string(t.id) + ": Duracao invalida (" + std::to_string(t.duracao) + "). Ajustada para 1.");
		t.duracao = 1;
		teveCorrecao = true;
	}

	if (t.periodo == 0) // PERIODO IGUAL A ZERO APENAS NA PARTE 2 DO TRABALHO
	{
		registrarLog(TipoLog::AVISO, origem + " - Tarefa " + std::to_string(t.id) + ": Tarefa aperiodica (sera implementada apenas no projeto B) (" + std::to_string(t.periodo) + "). Ajustado para 1.");
		t.periodo = 1;
		teveCorrecao = true;
	}

	if (t.periodo < 0) // Periodo menor que zero eh erro
	{
		registrarLog(TipoLog::ERRO, origem + " - Tarefa " + std::to_string(t.id) + ": Periodo negativo (" + std::to_string(t.periodo) + "). Ajustado para 1.");
		t.periodo = 1;
		teveCorrecao = true;
	}

	if (t.prazo <= 0)	// Prazo nao positivo nao eh permitido
	{
		registrarLog(TipoLog::ERRO, origem + " - Tarefa " + std::to_string(t.id) + ": Prazo invalido (" + std::to_string(t.prazo) + "). Ajustado para 1.");
		t.prazo = 1;
		teveCorrecao = true;
	}

	if (t.ingresso < 0)	// Ingresso menor que zero nao eh permitido (negativo)
	{
		registrarLog(TipoLog::ERRO, origem + " - Tarefa " + std::to_string(t.id) + ": Ingresso negativo (" + std::to_string(t.ingresso) + "). Ajustado para 0.");
		t.ingresso = 0;
		teveCorrecao = true;
	}

	// Duracao maior que o Periodo 
	if (t.duracao > t.periodo)
	{
		registrarLog(TipoLog::ERRO, origem + " - Tarefa " + std::to_string(t.id) + ": Duracao (" + std::to_string(t.duracao) + ") maior que o Periodo (" + std::to_string(t.periodo) + "). Duracao ajustada para " + std::to_string(t.periodo) + ".");
		t.duracao = t.periodo;
		teveCorrecao = true;
	}

	// Prazo menor que duracao nao eh permitido 
	if (t.prazo < t.duracao)
	{
		registrarLog(TipoLog::ERRO, origem + " - Tarefa " + std::to_string(t.id) + ": Prazo (" + std::to_string(t.prazo) + ") menor que duracao (" + std::to_string(t.duracao) + "). Ajustados para serem iguais.");
		t.prazo = t.duracao;
		teveCorrecao = true;
	}

	// Prazo maior que o Per�odo (viola modelo de prazos restritos)
	if (t.prazo > t.periodo)
	{
		registrarLog(TipoLog::AVISO, origem + " - Tarefa " + std::to_string(t.id) + ": Prazo (" + std::to_string(t.prazo) + ") maior que o Periodo (" + std::to_string(t.periodo) + "). Prazo ajustado para " + std::to_string(t.periodo) + ".");
		t.prazo = t.periodo;
		teveCorrecao = true;
	}


	// TRATAMENTO DA COR HEXADECIMAL

	size_t tamCor = std::strlen(t.cor);

	// Se o arquivo/usu�rio informou a cor no formato de 6 caracteres sem '#' (ex: "FF5733"),
	// normalizamos adicionando o '#' automaticamente para o renderizador sem registrar log de erro.
	if (tamCor == 6)
	{
		bool todosHex = true;
		for (size_t k = 0; k < 6; k++)
		{
			if (!std::isxdigit(static_cast<unsigned char>(t.cor[k])))
			{
				todosHex = false;
				break;
			}
		}

		if (todosHex)
		{
			char bufferCor[8];
			snprintf(bufferCor, sizeof(bufferCor), "#%s", t.cor);
			strncpy(t.cor, bufferCor, sizeof(t.cor) - 1);
			t.cor[sizeof(t.cor) - 1] = '\0';
			tamCor = std::strlen(t.cor); // Atualiza para 7 caracteres
		}
	}

	// Validacao de ver se a cor final no formato #RRGGBB eh valida
	bool corInvalida = false;

	// Deve ter exatamente 7 caracteres e comecar com '#'
	if (tamCor != 7 || t.cor[0] != '#')
	{
		corInvalida = true;
	}
	else
	{
		// Verifica se os 6 caracteres seguintes sao digitos hexadecimais validos
		for (size_t k = 1; k < 7; k++)
		{
			if (!std::isxdigit(static_cast<unsigned char>(t.cor[k])))
			{
				corInvalida = true;
				break;
			}
		}
	}

	if (corInvalida)
	{
		registrarLog(TipoLog::ERRO, origem + " - Tarefa " + std::to_string(t.id) + ": Formato de cor invalido (" + std::string(t.cor) + "). Redefinida para a cor padrao #FF5733.");

		const char* corPadrao = "#FF5733";
		strncpy(t.cor, corPadrao, sizeof(t.cor) - 1);
		t.cor[sizeof(t.cor) - 1] = '\0';

		teveCorrecao = true;
	}

	// Validacao pra impedir que tenha cor repetida
	bool corRepetida = false;
	for (size_t i = 0; i < m_tarefas.size(); i++)
	{
		// Compara a cor ignorando a pr�pria tarefa
		if (&t != &m_tarefas.at(i) && std::string(t.cor) == std::string(m_tarefas.at(i).cor))
		{
			corRepetida = true;
			break;
		}
	}

	if (corRepetida)
	{
		std::string corAntiga = t.cor;

		// Paleta de cores distintas para substituicao
		const std::vector<std::string> paletaCoresReserva = {
			"#FF5733", "#33FF57", "#3357FF", "#F39C12",
			"#8E44AD", "#1ABC9C", "#D35400", "#2C3E50",
			"#E74C3C", "#3498DB", "#2ECC71", "#F1C40F"
		};

		// Procura uma cor da paleta que ainda nao esteja em uso por nenhuma outra tarefa
		std::string novaCorEncontrada = "";
		for (const auto& corOpcao : paletaCoresReserva)
		{
			bool emUso = false;
			for (size_t i = 0; i < m_tarefas.size(); i++)
			{
				if (&t != &m_tarefas.at(i) && corOpcao == std::string(m_tarefas.at(i).cor))
				{
					emUso = true;
					break;
				}
			}

			if (!emUso)
			{
				novaCorEncontrada = corOpcao;
				break;
			}
		}

		// PLANO B caso todas as cores da paleta j� estejam em uso
		if (novaCorEncontrada.empty())
		{
			unsigned int hash = static_cast<unsigned int>(t.id * 1664525 + 1013904223);
			char bufferHex[8];
			snprintf(bufferHex, sizeof(bufferHex), "#%02X%02X%02X",
				(hash & 0xFF), ((hash >> 8) & 0xFF), ((hash >> 16) & 0xFF));
			novaCorEncontrada = bufferHex;
		}

		registrarLog(TipoLog::ERRO, origem + " - Tarefa " + std::to_string(t.id) + ": Cor repetida (" + corAntiga + "). Redefinida para a cor unica " + novaCorEncontrada + ".");

		strncpy(t.cor, novaCorEncontrada.c_str(), sizeof(t.cor) - 1);
		t.cor[sizeof(t.cor) - 1] = '\0';

		teveCorrecao = true;
	}

	return teveCorrecao;
}