#include "GerenciadorGUI.hpp"

#include "MockData.hpp"
#include "imgui.h"
#include "raylib.h"

#include "../core/MotorSimulacao.hpp"

GerenciadorGUI::GerenciadorGUI(MotorSimulacao* motor = nullptr) :
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

	m_painelConfig(&m_quantidadeCPUs, &m_quantum, m_algoritmo, &fonteDados),
	m_painelControles(&m_tickAtual, &m_totalTicks, &m_emExecucaoAuto, &m_delayPasso, &m_modoExecucao, &m_algoritmoSelecionado, &m_solicitarExportacao),
	m_painelTabela(&m_tarefas, &m_historico, &m_tickAtual),
	m_painelGantt(&m_tarefas, &m_historico, &m_tickAtual, &m_quantidadeCPUs, &m_modoExecucao)

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

void GerenciadorGUI::recalcularSimulacao()
{
	if (fonteDados == ModoFonteDados::ENGINE_REAL && m_motor != nullptr)
	{
		m_motor->executaTempoReal();
		sincronizarComEngine();
	}
	else if(fonteDados == ModoFonteDados::MOCK)
	{
		m_tarefas = GeradorMock::criarTarefasIniciais();
		m_historico = GeradorMock::gerarHistoricoInicial(m_tarefas, m_totalTicks, m_quantidadeCPUs, m_quantum);
	}
}

void GerenciadorGUI::sincronizarComEngine()
{
	m_historico.clear();
	m_tarefas.clear();

	if (m_motor == nullptr) return;

	// Conversao das tarefas da Engine para TarefaGUI
	for (size_t t = 0; t < m_motor->getTarefas().size(); t++)
	{
		Tarefa tReal = m_motor->getTarefas()[t];
		TarefaGUI tGUI;
		tGUI.id = tReal.getId();
		tGUI.duracao = tReal.getDuracao();
		tGUI.periodo = tReal.getPeriodo();
		tGUI.prazo = tReal.getPrazo();
		tGUI.ingresso = tReal.getIngresso();
		strncpy(tGUI.cor, tReal.getCor().c_str(), sizeof(tGUI.cor) - 1);
		tGUI.cor[sizeof(tGUI.cor) - 1] = '\0';
		m_tarefas.push_back(tGUI);
	}

	// Conversao do historico de ticks do motor para SnapshotTick
	const auto& historicoEngine = m_motor->getHistorico();

	for (size_t i = 0; i < historicoEngine.size(); i++)
	{
		std::vector<short> idsExecutando;
		const Historico& historicoPasso = historicoEngine[i];
		SnapshotTick snapGUI;
		snapGUI.tick = historicoPasso.tempo;

		// VERSAO TEMPORARIA PARA 1 CPU
		if (historicoPasso.tarefaExecutada.getId() != -1)
		{
			idsExecutando.push_back(historicoPasso.tarefaExecutada.getId());
		}

		// QUANDO HOUVER N CPUS: ITERAR SOBRE A LISTA DE TAREFAS DA ENGINE E INSERIR TODOS OS IDS VALIDOS EM IDSEXECUTANDO


		for (size_t s = 0; s < m_motor->getTarefas().size(); s++)
		{
			Tarefa tMotor = m_motor->getTarefas()[s];
			SnapshotTarefa t;
			t.idTarefa = tMotor.getId();

			bool executandoSim = false;
			for (size_t e = 0; e < idsExecutando.size() && !executandoSim; e++)
			{
				if (idsExecutando[e] == t.idTarefa)
				{
					t.estado = EstadoTarefa::EXECUTANDO;
					executandoSim = true;
				}
			}
			if (!executandoSim && tMotor.getProximaLiberacao() <= historicoPasso.tempo && tMotor.getTempoRestante() > 0)
			{
				t.estado = EstadoTarefa::PRONTA;
			}
			else if (!executandoSim)
			{
				t.estado = EstadoTarefa::ESPERANDO;
			}

			snapGUI.tarefas.push_back(t);
		}

		for (size_t d = 0; d < historicoPasso.idDeadlinesPerdidas.size(); d++)
		{
			EventoGantt novoEvento;
			novoEvento.idTarefa = historicoPasso.idDeadlinesPerdidas[d];
			novoEvento.tipo = TipoEvento::DEADLINE_STOURO;
			snapGUI.eventos.push_back(novoEvento);
		}

		const std::vector<Tarefa>* tarefasDoMotor = &m_motor->getTarefas();

		for (size_t taf = 0; taf < tarefasDoMotor->size(); taf++)
		{
			int ingresso = tarefasDoMotor->at(taf).getIngresso();
			int periodo = tarefasDoMotor->at(taf).getPeriodo();
			int tempoAtual = historicoPasso.tempo;
			if (tempoAtual >= ingresso && (tempoAtual - ingresso) % periodo == 0)
			{
				EventoGantt novoEventoIngresso;
				novoEventoIngresso.idTarefa = tarefasDoMotor->at(taf).getId();
				novoEventoIngresso.tipo = TipoEvento::INGRESSO;
				snapGUI.eventos.push_back(novoEventoIngresso);
			}
		}

		//Adicao do Snapshot de Tick modificado no historico GUI
		m_historico.push_back(snapGUI);
	}
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