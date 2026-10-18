#include "GerenciadorGUI.hpp"

#include "MockData.hpp"
#include "imgui.h"
#include "raylib.h"

#include "../core/MotorSimulacao.hpp"
#include "../reader/Leitor.hpp"

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

	m_exibirConfig(true),
	m_exibirControles(true),
	m_exibirTabela(true),
	m_exibirGantt(true),

	m_solicitarExportacao(false),

	m_painelConfig(&m_quantidadeCPUs, &m_quantum, m_algoritmo, &fonteDados, caminhoArquivo),
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
			else if (fonteDados == ModoFonteDados::ENGINE_REAL && m_modoExecucao == 0)
			{
				// Se chegou ao fim do historico no passo a passo, executa +1 tick no motor
				m_motor->executaPassoAPasso(1);
				sincronizarComEngine();
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

	float utilizacao = 0.0f;
	bool escalonavel = false;
	int totalDeadlines = 0;
	float tempoOciosoPercentual = 0.0f;
	int ticksOciosos = 0;

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

		bool configAlteradaNoPainel = false;

		// Desenho Sequencial dos Paineis
		if (m_exibirConfig)
			configAlteradaNoPainel = m_painelConfig.renderizar();	

		if(m_exibirControles)
			m_painelControles.renderizar();

		if (m_exibirTabela)
		{
			if (m_painelTabela.renderizar())
			{
				// Envia os dados alterados na GUI para o motor antes de recalcular

				atualizarTarefasNaEngine();
				if (m_tickAtual == 0)
				{
					recalcularSimulacao();
				}
				else
				{
					reescalonarAPartirDoTickAtual();
				}
			}
		}
			
		if (m_exibirGantt)
			m_painelGantt.renderizar();


		// Checa se as tarefas mudaram OU se alguma configuração foi alterada na interface
		// Deteccao e Reacao unificada de mudanças
		bool tarefasMudaram = (numeroTarefas != static_cast<int>(m_tarefas.size()));
		bool valoresMudaram = (m_quantidadeCPUs != m_quantidadeCPUsAnterior ||
			m_quantum != m_quantumAnterior ||
			m_algoritmoSelecionado != m_algoritmoAnterior ||
			m_modoExecucao != m_modoExecucaoAnterior ||
			fonteDados != m_fonteDadosAnterior);

		if (configAlteradaNoPainel || tarefasMudaram || valoresMudaram)
		{
			if (configAlteradaNoPainel)
			{
				if (caminhoArquivo != caminhoArquivoAnterior)
				{
					caminhoArquivoAnterior = caminhoArquivo;
					carregarArquivoEncontrado();
				}
			}

			recalcularSimulacao();

			//Reset da agulha de tempo
			m_tickAtual = 0;
			//recalcularHistorico();

			// Atualiza os rastreadores de estado
			m_quantidadeCPUsAnterior = m_quantidadeCPUs;
			m_quantumAnterior = m_quantum;
			m_algoritmoAnterior = m_algoritmoSelecionado;
			m_modoExecucaoAnterior = m_modoExecucao;
			m_fonteDadosAnterior = fonteDados;
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
						tempoOciosoPercentual = ( ((float)(ticksOciosos)) / ((float)(totalSlotsCPU)) ) * 100.0f;
					}
				}
			}
			if(fonteDados == ModoFonteDados::ENGINE_REAL)
				m_painelConfig.renderizarMetricas(m_tarefas, utilizacao, escalonavel, totalDeadlines, tempoOciosoPercentual);
		}
		
	}
}

void GerenciadorGUI::carregarArquivoEncontrado()
{
	// Instanciacao local do leitor
	Leitor leitor;
	Configuracao config = leitor.lerArquivo(caminhoArquivo);

	// Atualizacao do estado na GUI
	m_quantidadeCPUs = config.quantidadeCPUs;
	m_quantum = config.quantum;
	TipoAlgoritmo tipodesse = config.algoritmo;
	if (tipodesse == TipoAlgoritmo::RATE_MONOTONIC)
		m_algoritmo = "RM";
	else if (tipodesse == TipoAlgoritmo::EARLIEST_DEADLINE_FIRST)
		m_algoritmo = "EDF";
	else
		m_algoritmo = "Outro";

	// Atualizacao do Motor
	m_motor->limparTarefas();

	const std::vector<std::string> paletaCores = {
		"#FF5733", "#33FF57", "#3357FF", "#F39C12",
		"#8E44AD", "#1ABC9C", "#D35400", "#2C3E50"
	};

	for (size_t i = 0; i < config.tarefas.size(); i++)
	{
		Tarefa& t = config.tarefas[i];

		if (t.getCor().empty() || t.getCor() == "\0" || t.getCor() == "null")
		{
			std::string corNova = paletaCores[i % paletaCores.size()];
			t.setCor(corNova);
		}

		m_motor->addTarefa(t);
	}
	m_motor->setAlgoritmo(config.algoritmo);

	// Reinicio da simulacao
	m_tickAtual = 0;
	m_motor->resetSimulacao();
	m_motor->executaTempoReal();

	sincronizarComEngine();

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

		m_motor->resetSimulacao();

		if (m_modoExecucao == 1) // Modo Instantâneo Completo
		{
			m_motor->executaTempoReal();
		}
		else // Modo Passo a Passo
		{
			// Processa apenas o primeiro passo para inicializar o tick 0
			m_motor->executaPassoAPasso(1);
		}

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

		//Falta pegar os eventos mas isso é pra parte 2

		tGUI.prioridade = tReal.getPrioridade();
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
		/*
		if (historicoPasso.tarefaExecutada.getId() != -1)
		{
			idsExecutando.push_back(historicoPasso.tarefaExecutada.getId());
		}
		*/

		// QUANDO HOUVER N CPUS: ITERAR SOBRE A LISTA DE TAREFAS DA ENGINE E INSERIR TODOS OS IDS VALIDOS EM IDSEXECUTANDO

		for (size_t exe = 0; exe < historicoPasso.tarefasExecutadas.size(); exe++)
		{
			if (historicoPasso.tarefasExecutadas[exe].getId() != -1)
			{
				idsExecutando.push_back(historicoPasso.tarefasExecutadas[exe].getId());
			}
		}


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

// Se houve mudança nas tarefas pela GUI ele vai atualizar as tarefas na ENGINE
void GerenciadorGUI::atualizarTarefasNaEngine()
{
	if (m_motor == nullptr)
	{
		return;
	}

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
			tGUI.prioridade,				// Prioridade estatica (igual a zero)
			corStringue);

		m_motor->addTarefa(tReal);
	}
}

void GerenciadorGUI::reescalonarAPartirDoTickAtual()
{
	if (m_motor == nullptr || fonteDados != ModoFonteDados::ENGINE_REAL) 
		return;

	// Trunca o histórico da Engine do tick atual em diante
	m_motor->resetaAPartirDe(m_tickAtual);

	// Garante que o algoritmo correto esteja definido
	if (m_algoritmo == "RM")
		m_motor->setAlgoritmo(TipoAlgoritmo::RATE_MONOTONIC);
	else if (m_algoritmo == "EDF")
		m_motor->setAlgoritmo(TipoAlgoritmo::EARLIEST_DEADLINE_FIRST);

	// Simula o futuro a partir da agulha de tempo atual até o final
	m_motor->executaTempoReal();

	// Sincroniza o novo histórico com a GUI preservando a posição do tick
	sincronizarComEngine();

	// IMPORTANTE : REDEFINIR O M_TOTALTICKS
	m_totalTicks = static_cast<int>(m_historico.size());
}


