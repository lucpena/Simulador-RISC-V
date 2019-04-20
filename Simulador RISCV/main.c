/*	Universidade de Brasilia
*	Departamento de Ciência da Computacao
*	Trabalho 1 - Simulador do Risc V
*
*	Lucas Araújo Pena - 13/0056162
*/

#include <stdio.h> 
#include <stdint.h>
#include <string.h>

//Limpa o console
#define CLEAR() system("clear || cls")

//Define o tamanho da memoria
#define MEM_SIZE 4096
int32_t mem[MEM_SIZE];


int main() {

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
	
	//Verifica se o endereco nao eh multiplo de 4
	if (add % 4) {
		printf("Endereco invalido (lw)");
		return;
	}

	return mem[add >> 2];
}

//Le meia palavra (16 bits). Retorna inteiro com sinal
int32_t lh(uint32_t add, int16_t kte) {

}

//Le meia palavra (16 bits). Retorna inteiro sem sinal
uint32_t lhu(uint32_t add, int16_t kte) {

	//Avanca na memoria para o endereco
	int tmp = mem[add << 2];
	
	//Retorno da funcao
	uint32_t ret = 0; 

	// Checa se os Dados estao alinhados
	if (add % 2) {
		printf("\n\nDados nao alinhados. (Erro: lhu)");
	}

	if (!add%4) {
		ret = (tmp >> 16) & 0xFFFF;
	}
	else {
		ret = tmp & 0xFFFF;
	}

	return ret;
}

//Le um byte (8 bits) e retorna um inteiro com sinal
int32_t lb(uint32_t add, int16_t kte) {

}

//Le um byte (8 bits) e retorna um inteiro sem sinal
int32_t lbu(uint32_t add, int16_t kte) {
	int tmp;
	tmp = mem[add >> 2];
	return (tmp >> (8 * (add % 4))) && 0xFF;
}

//Escreve um inteiro alinhado na memoria. Enderecos multiplos de 4
void sw(uint32_t add, int16_t kte, int32_t dado) {

	//Verifica se a constante possui um valor aceitavel
	if (kte % 4) {
		printf("\n\n A constante precisa ser um multiplo de 4. (Erro: sw)");
	}

	//Salva na memoria o dado
	mem[dado >> 2] = dado;
}

//Escreve meia palavra de 16 bits na memoria. Enderecos multiplos de 2
void sh(uint32_t add, int16_t kte, int16_t dado) {

	// Vai para o proximo endereco de memoria
	int tmp = mem[add >> 2 + (kte/4)];

	//Mascara apicada para poder pegar somente a meia palavra
	dado &= 0xFFFF;

	if (add % 2) {
		printf("\n\nEndereco invalido. (Erro: sh)");
		return;
	}
	if (add % 4) {
		tmp &= 0x0000FFFF;
		tmp |= dado << 16;
	}
	else {
		tmp &= 0xFFFF0000;
		tmp |= dado;
	}

	//Salva a operacao na memoria
	mem[add >> 2 + (kte / 4)] = tmp;
}

//Escreve um byte de 8 bits na memoria.
void sb(uint32_t add, int16_t kte, int8_t dado) {

}

/*                              Funcoes auxiliares                              */

//Limpa o buffer de input 
void clean() {
	while (getchar() != '\n');
}