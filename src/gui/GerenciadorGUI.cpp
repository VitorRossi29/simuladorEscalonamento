#include "GerenciadorGUI.hpp"

#include "MockData.hpp"
#include "imgui.h"
#include "raylib.h"

GerenciadorGUI::GerenciadorGUI() :
	m_tickAtual(0),
	m_totalTicks(50),
	m_emExecucaoAuto(false),
	m_tempoAcumulado(0.0f),
	m_delayPasso(0.5f), //Tempo em segundos por tick
	m_quantidadeCPUs(2),
	m_quantum(1),
	m_modoExecucao(0),
	m_algoritmoSelecionado(0),
	m_algoritmo("RM"),

	m_quantidadeCPUsAnterior(m_quantidadeCPUs),
	m_quantumAnterior(m_quantum),
	m_algoritmoAnterior(m_algoritmoSelecionado),

	m_exibirConfig(true),
	m_exibirControles(true),
	m_exibirTabela(true),
	m_exibirGantt(true),

	m_solicitarExportacao(false),

	m_painelConfig(&m_quantidadeCPUs, &m_quantum, m_algoritmo),
	m_painelControles(&m_tickAtual, &m_totalTicks, &m_emExecucaoAuto, &m_delayPasso, &m_modoExecucao, &m_algoritmoSelecionado, &m_solicitarExportacao),
	m_painelTabela(&m_tarefas, &m_historico, &m_tickAtual),
	m_painelGantt(&m_tarefas, &m_historico, &m_tickAtual, &m_quantidadeCPUs)

{
	//Carregar dados dos Mocks e etc
}

//Esta função roda uma única vez no início da aplicação ou quando a simulação precisa ser totalmente restaurada
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
	
	//Validacao dos limites de tempo
	if (!m_historico.empty() && m_tickAtual >= 0 && m_tickAtual < static_cast<int>(m_historico.size()) )
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Janelas"))
			{
				ImGui::MenuItem("Tabela de Tarefas", NULL, &m_exibirTabela);
				ImGui::MenuItem("Painel de Configuracao", NULL, &m_exibirConfig);
				ImGui::MenuItem("Painel de Controle", NULL, &m_exibirControles);
				ImGui::MenuItem("Grafico de Gantt", NULL, &m_exibirGantt);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		//Desenho Sequencial dos Paineis
		if (m_exibirConfig)
			m_painelConfig.renderizar();

		if(m_exibirControles)
			m_painelControles.renderizar();

		if(m_exibirTabela)
			m_painelTabela.renderizar();

		if (m_exibirGantt)
			m_painelGantt.renderizar();

		// Checa se as tarefas mudaram OU se alguma configuração foi alterada na interface
		bool tarefasMudaram = (numeroTarefas != static_cast<int>(m_tarefas.size()));
		bool configMudou = (m_quantidadeCPUs != m_quantidadeCPUsAnterior ||
			m_quantum != m_quantumAnterior ||
			m_algoritmoSelecionado != m_algoritmoAnterior);

		//Deteccao e Reacao da Mudancas nos Dados
		if (tarefasMudaram || configMudou)
		{
			recalcularHistorico();

			// Atualiza os rastreadores de estado
			m_quantidadeCPUsAnterior = m_quantidadeCPUs;
			m_quantumAnterior = m_quantum;
			m_algoritmoAnterior = m_algoritmoSelecionado;
		}

		if (m_solicitarExportacao)
		{
			screenShotAvancada();
		}
	}
}

void GerenciadorGUI::recalcularHistorico()
{
	m_historico = GeradorMock::gerarHistoricoInicial(m_tarefas, 
		m_totalTicks, m_quantidadeCPUs, m_quantum);
}

// Caso o grafico de Gantt seja maior que a tela do monitor, essa screenshot o contera por completo
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

	// Lógica para nome de arquivo único
	std::string prefixo = "../../../resultado_simulacao_gantt";
	std::string extensao = ".png";
	std::string caminhoFinal = prefixo + extensao;

	int contador = 1;
	// Enquanto o arquivo já existir no diretório, incrementa o sufixo numérico
	while (FileExists(caminhoFinal.c_str()))
	{
		caminhoFinal = prefixo + "_" + std::to_string(contador) + extensao;
		contador++;
	}

	ExportImage(imagem, caminhoFinal.c_str());

	UnloadImage(imagem);
	UnloadRenderTexture(alvo);

	m_solicitarExportacao = false;
}