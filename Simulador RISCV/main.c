/*	Universidade de Brasilia
*	Departamento de Ciência da Computacao
*	Trabalho 1 - Simulador do Risc V
*
*	Lucas Araújo Pena - 13/0056162
*/

#include <stdio.h> 
#include <stdint.h>

//Definindo o tamanho da memoria
#define MEM_SIZE 4096

int main() {
	
	//Memoria a ser simulada
	int32_t mem[MEM_SIZE];

	printf("sizeof(mem) = %d\n\n", sizeof(mem));

	system("PAUSE");

	return 0;
}

//Imprime o conteudo da memoria, em formato hexadecimal.
void dump_mem(uint32_t add, uint32_t size) {
	// Ex: mem[0] = 01020304
}

//Le um inteiro alinhado. Enderecos multiplos de 4
int32_t lw(uint32_t add, int16_t kte) {

}

//Le meia palavra (16 bits). Retorna inteiro com sinal
int32_t lh(uint32_t add, int16_t kte) {

}

//Le meia palavra (16 bits). Retorna inteiro sem sinal
uint32_t lhu(uint32_t add, int16_t kte) {

}

//Le um byte (8 bits) e retorna um inteiro com sinal
int32_t lb(uint32_t add, int16_t kte) {

}

//Le um byte (8 bits) e retorna um inteiro sem sinal
int32_t lbu(uint32_t add, int16_t kte) {

}

//Escreve um inteiro alinhado na memoria. Enderecos multiplos de 4
void sw(uint32_t add, int16_t kte, int32_t dado) {

}

//Escreve meia palavra de 16 bits na memoria. Enderecos multiplos de 2
void sh(uint32_t add, int16_t kte, int16_t dado) {

}

//Escreve um byte de 8 bits na memoria.
void sb(uint32_t add, int16_t kte, int8_t dado) {

}