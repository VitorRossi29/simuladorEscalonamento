#pragma once
#include <string>
#include <vector>
#include "MockData.hpp"
#include "StructsTeste.hpp"

class PainelConfiguracao
{
private:
	int* m_qtdeCpus;
	int* m_quantum;
	std::string& m_algoritmo;

	ModoFonteDados* m_modoFonte;
public:
	PainelConfiguracao(int* quantidadeCPUs, int* quantum, std::string& algoritmo, ModoFonteDados* modoFonte);

	//desenhar o painel com o seletor de algoritmo e quantidade de cpus e o botao para carregar arquivo txt
	void renderizar ();
	//calcular a utilizacao da cpu, exibir a barra de progresso (imgui progressbar), indicar se o sistema é 
	// escalonavel e exibe contadores de deadlines perdidos
	void renderizarMetricas(const std::vector<TarefaGUI>& tarefas, int totalDeadlinesPerdidos, float tempoOciosoPercentual);
};