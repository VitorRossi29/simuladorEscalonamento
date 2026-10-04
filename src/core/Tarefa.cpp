#include "Tarefa.hpp"

short int Tarefa::proximoId=1;

Tarefa::Tarefa()
{
	id=proximoId++;
}

Tarefa::~Tarefa()
{

}
