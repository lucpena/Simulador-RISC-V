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

int is_running = 1;
int op;

//Imprime o conteudo da memoria, em formato hexadecimal.
void dump_mem(uint32_t add, uint32_t size) {
	// Ex: mem[0] = 01020304
}


/*						Intrucoes						*/

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
	uint32_t tmp = mem[add >> 2];
	uint32_t ret = 0;

	if (add % 2) {
		printf("\n\n Dados nao alinhados. (Erro: lh)");
	}

	if (add % 4) {
		ret = (tmp >> 16) & 0xFFFF;
	} else {
		ret = tmp & 0xFFFF;
	}

	return ((ret & 0x8000) ? (ret | 0xFFFF0000) : ret);
}

//Le meia palavra (16 bits). Retorna inteiro sem sinal
uint32_t lhu(uint32_t add, int16_t kte) {

	//Avanca na memoria para o endereco
	uint32_t tmp = mem[add << 2];
	
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
	uint32_t tmp;

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
	uint32_t tmp = mem[add >> 2 + (kte/4)];

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
	uint32_t tmp = mem[add >> 2];
	uint32_t off = add % 4;
	int8_t exbyte = dado & 0xFF;
	int masks[] = { 0xFF000000, 0xFF0000, 0xFF00, 0xFF };

	tmp = (tmp & masks[off]) + (exbyte << (8 * off));
	mem[add >> 2] = tmp;
}

/*                              Funcoes auxiliares                              */

//Limpa o buffer de input 
void clean() {
	while (getchar() != '\n');
}

void menuInicial() {
	printf("Acesso a memoria RISC V \n\n");
	printf("Escolha a opcao desejada:\n");
	printf("1- Opcao\n");
	printf("2- Opcao\n");
	printf("3- Sair\n\n");
}

void opcao1() {}
void opcao2() {}

//Loop principal do prorama
void run() {
	while (is_running) {
		menuInicial();

		scanf("%d", &op);
		clean();

		switch (op) {
		case 1:
			opcao1();
			break;
		case 2:
			opcao2();
			break;
		case 3:
			is_running = 0;

		default:
			CLEAR();
			menuInicial();
			printf("Entre uma opcao valida.");
		}

	}
}

/**********************************************/

// Funcao principal do programa
int main() {

	run();

	return 0;
}