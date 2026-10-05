


class PainelControles
{
private:
    int* m_tickAtualRef;        //Posicao atual do relogio
    int* m_totalTicksRef;       //Limite maximo de ticks do historico carregado
    bool* m_emExecucaoAutoRef;  //Booleano se se animacao esta em tempo real


    float* m_delayPasso;     //Velocidade do play tempo em espera em segundos por tick
    int* m_modoExecucao;     //modo 0 = passo a passo 1 = continuo
    int* m_algoritmoSelecionado;      //0: RM 1: EDF OU 2: OUTRO
public:

    PainelControles(int* tickAtual, int* totalTicks, bool* emExecucaoAuto, 
        float* delayPasso, int* modoExecucao, int* algoritmoSelecionado);
    //Desenha a janela Imgui::Begin(Controles) contendo os botoes
    //play pause passo a passo reset, seletor de modo e o slider de velocidade
    void RenderizarPainelControles();
};
