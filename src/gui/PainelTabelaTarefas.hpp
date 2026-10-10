#pragma once
#include <vector>
#include "MockData.hpp"

//Classe responsavel por desenhar a tabela de tarefas cadastradas
// e o formulario de insercao ao vivo

class PainelTabelaTarefas
{
private:
	std::vector<TarefaMock>* m_tarefasRef;
	//Ponteiro para vetor real de tarefas
	
	const std::vector<SnapshotTick>* m_historicoRef;
	const int* m_tickAtualRef;
	//Ponteiro para o snapshot de tempo atual no player
	
	//Informacoes para inserir novas tarefas (formulario)
	int m_novoIngresso;
	int m_novaDuracao;
	int m_novoPeriodo;
	int m_novoPrazo;
	char m_novaCorBuffer[8];

public:

	//Associa os ponteiros do painel as estruturas ativas do GerenciadorGUI
	PainelTabelaTarefas(std::vector<TarefaMock>* tarefas = nullptr, 
		const std::vector<SnapshotTick>* historico = nullptr,
		const int* tickAtual = 0);

	//Executa o desenho da janela sem receber parametros na chamada do laco
	void renderizar();

private:
	//renderiza a tabela, snapshot encontrado eh passado para a funcao
	void desenharTabela(const SnapshotTick* snapshotAtual);

	//renderiza os campos de entrada e o botão para adicionar tarefas
	void desenharFormularioInsercao();

	//restaura os atributos do formulario para valores padrao apos uma insercao com sucesso
	void resetarFormulario();

	int proximoId();
};