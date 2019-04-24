/*	Universidade de Brasilia
*	Departamento de Ciência da Computacao
*	Trabalho 1 - Simulador do Risc V
*
*	Lucas Araújo Pena - 13/0056162
*/

#include <stdio.h> 
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>

//Limpa o console
#define CLEAR() system("clear || cls")

//Define o tamanho da memoria
#define MEM_SIZE 4096
int32_t mem[MEM_SIZE];

int32_t dump_add, dump_size;

int is_running = 1;
int op;

//Imprime o conteudo da memoria, em formato hexadecimal
void dump_mem(uint32_t add, uint32_t wsize) {
	//Variavel para corrigir a contagem do index da memoria
	int a = add / 4;

	for (unsigned int i = 0; i < MEM_SIZE; i++) {

		//Coloca em tmp o valor da memoria
		uint32_t tmp = mem[add / 4];

		//Enquanto estiver no range de busca, printa na tela o valor
		if (i < wsize) {
			printf("mem[%d] = 0x%08x \n", (i + a), tmp);
		}

		//Vai para o proximo endereco de memoria
		add += 4;
	}
}

//Garante que a memoria esteja vazia para realizar a inicializacao.
//Utilizado em uma segunda chamada da funcao de inicializacao.
void clean_mem() {
	for (int i = 0; i < MEM_SIZE; i++) {
		mem[i] = 0;
	}
}

/*						Intrucoes						*/

//Le um inteiro alinhado. Enderecos multiplos de 4
int32_t lw(uint32_t add, int16_t kte) {
	
	int32_t tmp = mem[(add + kte) / 4];

	//Verifica se o endereco nao eh multiplo de 4
	if (add % 4 != 0) {
		printf("Endereco invalido. (Erro: lw)");
		tmp = 0;
	}

	//Retorna a palavra 
	return tmp;
}

//Le meia palavra (16 bits). Retorna inteiro com sinal
int32_t lh(uint32_t add, int16_t kte) {
	uint32_t tmp = mem[(add + kte) / 4];
	uint32_t ret = 0;

	if (add % 2) {
		printf("\n\n Dados nao alinhados. (Erro: lh)");
	}

	if (kte % 4 == 0) {
		ret = (tmp >> 16) & 0xFFFF;
	} else {
		ret = tmp & 0xFFFF;
	}

	return ((ret & 0x8000) ? (ret | 0xFFFF0000) : ret);
}

//Le meia palavra (16 bits). Retorna inteiro sem sinal
uint32_t lhu(uint32_t add, int16_t kte) {

	uint32_t tmp, ret;
	
	//Coloca em tmp o valor da memoria do endereco 
	tmp = mem[(add + kte) / 4];
	ret = 0; 

	// Checa se os Dados estao alinhados
	if (add % 4 != 0) {
		printf("\n\nDados nao alinhados. (Erro: lhu)");
	}

	if (kte == 2) { //!
		ret = (tmp >> 16) & 0xFFFF;
	}
	else {
		ret = tmp & 0xFFFF;
	}

	return ret;
}

//Le um byte (8 bits) e retorna um inteiro com sinal
int32_t lb(uint32_t add, int16_t kte) {
	uint32_t tmp;

	//Acessa o byte
	tmp = (mem[(add + kte) / 4] >> (8 * (kte % 4)));
	
	//Acessa o bit
	tmp &= 0xFF;

	return ((tmp & 0x80 == 0) ? (tmp | 0xFFFFFF00) : tmp);
}

//Le um byte (8 bits) e retorna um inteiro sem sinal
int32_t lbu(uint32_t add, int16_t kte) {
	uint32_t tmp;

	tmp = mem[(add + kte) / 4];
	tmp = tmp >> (8 * kte);
	tmp &= 0xFF;

	return tmp;

	//return (tmp >> (8 * (add % 4))) && 0xFF;
}

//Escreve um inteiro alinhado na memoria. Enderecos multiplos de 4
void sw(uint32_t add, int16_t kte, int32_t dado) {

	//Verifica se a constante possui um valor aceitavel
	if ((add + kte) % 4 != 0) {
		printf("\n\n A constante precisa ser um multiplo de 4. (Erro: sw)");
	}

	//Salva na memoria o dado
	mem[(add + kte) / 4] = dado;
}

//Escreve meia palavra de 16 bits na memoria. Enderecos multiplos de 2
void sh(uint32_t add, int16_t kte, int16_t dado) {

	// Endereco de memoria
	uint32_t tmp = mem[(add + kte) / 4];

	//Offset da meia palavra em relacao ao endereco da palavra
	int hadd = (add + kte) % 4;

	//Mascara apicada para poder pegar somente a meia palavra
	dado &= 0xFFFF;

	if ((add + kte) % 2 != 0) {
		printf("\n\nEndereco invalido. (Erro: sh)");
		return;
	}

	if ((add + kte) % 4 == 0) {
		tmp &= 0xFFFF0000;
		tmp |= dado;
	}
	else {
		tmp &= 0x0000FFFF;
		tmp |= dado << 16;
	}

	//Salva a operacao na memoria
	mem[(add + kte) / 4] = tmp;
}

//Escreve um byte de 8 bits na memoria.
void sb(uint32_t add, int16_t kte, int8_t dado) {
	//Carrega para tmp o valor que esta na memoria
	uint32_t tmp = mem[(add + kte) / 4];

	//Pega somente o dado fornecido
	int8_t exbyte = dado & 0xFF;

	//Array contendo as mascaras necessarias para a operacao
	int mask[] = { 0xFF, 0xFF00, 0xFF0000, 0xFF000000 };

	//Coloca o byte no local correto em relacao ao bit original
	uint32_t a = exbyte << (8 * kte);
	uint32_t b = a & mask[kte];

	//Adiciona o byte ao bit
	tmp += b;

	//Salva na memoria o dado
	mem[(add + kte) / 4] = tmp;
}

/*                              Funcoes auxiliares                              */

//Limpa o buffer de input 
void clean_buffer() {
	while (getchar() != '\n');
}

//Funcao para pausar o programa e esperar o enter para continuar
void enter() {
	printf("\n\nPressione Enter para continuar...");

	//Se utilizado sozinho só para o programa até qualuqer input ser colocado
	clean_buffer();
}

//Printa na tela o menu principal para navegacao no programa
void menuInicial() {
	printf("Acesso a memoria RISC V \n\n");
	printf("Escolha a opcao desejada:\n");
	printf("1- Inicializar a memoria \n");
	printf("2- Imprimir o conteudo da memoria\n");
	printf("3- Ler os dados da memoria\n");
	printf("4- Sair\n\n");
}

//Printa tela da opcao 'dump_mem()'
void dump_memMenu() {
	printf("Dump de memoria. \n\n");
	printf("Entre com o endereco inicial (1024 max): ");

	scanf("%" SCNd32, &dump_add);
	clean_buffer();

	//Testa se o endereco esta dentro do permitido
	while (dump_add < 0 || dump_add > 1024 ) {
		CLEAR();
		printf("Entre com um endereco inicial valido (1024 max): ");

		scanf("%" SCNd32, &dump_add);
		clean_buffer();
	}

	printf("Entre com o a quantidade de palavras que deseja retornar (1024 max): ");

	scanf("%" SCNd32, &dump_size);
	clean_buffer();

	//Testa se o tamanho esta dentro do permitido
	while (dump_size < 0 || dump_size > (1024 - dump_add) ) {
		CLEAR();
		printf("Entre com a quantidade de palavras validas (1024 max): ");

		scanf("%" SCNd32, &dump_size);
		clean_buffer();
	}

	printf("\n");
}

//Inicializa a memoria com valores previamente estabelecidos
void initMem() {

	clean_mem();

	// A
	sb(0, 0, 0x04);
	sb(0, 1, 0x03);
	sb(0, 2, 0x02);
	sb(0, 3, 0x01);

	// B
	sb(4, 0, 0xFF);
	sb(4, 1, 0xFE);
	sb(4, 2, 0xFD);
	sb(4, 3, 0xFC);

	// C
	sh(8, 0, 0xFFF0);
	sh(8, 2, 0x8C);

	// D
	sw(12, 0, 0xFF);

	// E
	sw(16, 0, 0xFFFF);

	// F
	sw(20, 0, 0xFFFFFFFF);

	// G
	sw(24, 0, 0x80000000);

	//Testes criados alem dos fornecidos
	// 1
	sb(28, 0, 0xBA);
	sb(28, 1, 0xBA);
	sb(28, 2, 0xEF);
	sb(28, 3, 0xDD);

	// 2
	sb(32, 0, 0x11);
	sb(32, 1, 0x22);
	sb(32, 2, 0xFF);
	sb(32, 3, 0xEE);

	// 3
	sh(36, 0, 0xABBA);
	sh(36, 2, 0xEF);

	// 4
	sw(40, 0, 0xFAFBFCFD);

	// 5
	sw(44, 0, 0x88CA);
}

//Le a memoria
void read_mem() {
	//Variaveis
	int32_t a;

	// A
	a = lb(0, 0);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(0, 1);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(0, 2);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(0, 3);
	printf("Hex: 0x%02x  Dec: %d \n\n", a, a);

	// B
	a = lb(4, 0);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(4, 1);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(4, 2);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(4, 3);
	printf("Hex: 0x%02x  Dec: %d \n\n", a, a);

	// C
	a = lbu(4, 0);
	printf("Dec: %d \n", a);
	a = lbu(4, 1);
	printf("Dec: %d \n", a);
	a = lbu(4, 2);
	printf("Dec: %d \n", a);
	a = lbu(4, 3);
	printf("Dec: %d \n\n", a);

	// D
	a = lh(8, 0);
	printf("Hex: 0x%02x  Dec: %d \n\n", a, a);
	a = lh(8, 2);
	printf("Hex: 0x%02x  Dec: %d \n\n", a, a);
	
	// E
	a = lhu(8, 0);
	printf("Dec: %d \n\n", a);
	a = lhu(8, 2);
	printf("Dec: %d \n\n", a);

	// F
	a = lw(12, 0);
	printf("Hex: 0x%02x  Dec: %d \n\n", a, a);
	a = lw(16, 0);
	printf("Hex: 0x%02x  Dec: %d \n\n", a, a);
	a = lw(20, 0); 
	printf("Hex: 0x%02x  Dec: %d \n\n", a, a);

	//Testes realizados alem dos fornecidos
	// 1
	a = lb(28, 0);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(28, 1);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(28, 2);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(28, 3);
	printf("Hex: 0x%02x  Dec: %d \n\n", a, a);

	// 2
	a = lb(32, 0);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(32, 1);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(32, 2);
	printf("Hex: 0x%02x  Dec: %d \n", a, a);
	a = lb(32, 3);
	printf("Hex: 0x%02x  Dec: %d \n\n", a, a);

	// 3
	a = lbu(36, 0);
	printf("Dec: %d \n", a);
	a = lbu(36, 1);
	printf("Dec: %d \n", a);
	a = lbu(36, 2);
	printf("Dec: %d \n", a);
	a = lbu(36, 3);
	printf("Dec: %d \n\n", a);

	// 4
	a = lw(40, 0);
	printf("Hex: 0x%02x  Dec: %d \n\n", a, a);

	// 5

}

//Loop principal do prorama
void run() {
	menuInicial();

	while (is_running) {
		scanf("%d", &op);
		clean_buffer();

		// Seleciona a opcao escolhida
		switch (op) {
		case 1:
			//Inicializa a memoria
			initMem();

			//Limpa a tela
			CLEAR();

			menuInicial();
			printf("\nMemoria inicializada com sucesso. \n\n");

			break;
		case 2:
			//Limpa a tela
			CLEAR();

			//Mostra na tela o menu de dump_mem
			dump_memMenu();

			//Chama a funcao principal de dump_mem
			dump_mem(dump_add, dump_size);

			//Pausa para mostrar na tela o dump de memoria
			enter();

			//Limpa a tela
			CLEAR();

			menuInicial();

			break;

		case 3:
			//Limpa a tela para mostrar os dados
			CLEAR();

			//Puxa da memoria os dados
			read_mem();

			//Pausa a aplicacao e espera pelo enter do usuario
			enter();

			//Limpa a tela novamente
			CLEAR();

			menuInicial();

			break;

		case 4:
			//Sai do programa
			is_running = 0;

		default:
			CLEAR();
			menuInicial();
			printf("\nEntre uma opcao valida\n\n");
		}

	}
}

/**********************************************/

// Funcao principal do programa
int main() {

	//Inicializa a memoria com zeros
	clean_mem();

	//Chama a funcao principal do programa
	run();

	return 0;
}