/*	Universidade de Brasilia
*	Departamento de Ciencia da Computacao
*	Trabalho 1 - Simulador do Risc V
*
*	IDE: Visual Studio 2015
*	Compilador: Padrao do VS2019 para C
*	Sistema operacional: Windows 7
*
*	Lucas Araujo Pena - 13/0056162
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

int32_t regs[32];

/*********************************************************************************************************************/

// lembrem que so sao considerados os 7 primeiros bits dessas constantes
enum OPCODES { 
	LUI = 0x37, AUIPC = 0x17, // atribui 20 bits mais significativos
	ILType = 0x03, // Load type
	BType = 0x63, // branch condicional
	JAL = 0x6F, JALR = 0x67, // jumps - JAL formato UJ, JALR formato I
	StoreType = 0x23, // store
	ILAType = 0x13, // logico-aritmeticas com imediato
	RegType = 0x33, // operacoes LA com registradores
	ECALL = 0x73 // chamada do sistema - formato I
};

// campo auxiliar de 3 bits
enum FUNCT3 {
	BEQ3 = 0, BNE3 = 01, BLT3 = 04, BGE3 = 05, BLTU3 = 0x06, BGEU3 = 07, // branches
	LB3 = 0, LH3 = 01, LW3 = 02, LBU3 = 04, LHU3 = 05, // loads
	SB3 = 0, SH3 = 01, SW3 = 02, // stores
	ADDSUB3 = 0, SLL3 = 01, SLT3 = 02, SLTU3 = 03, // LA com
	XOR3 = 04, SR3 = 05, OR3 = 06, AND3 = 07, // registradores
	ADDI3 = 0, ORI3 = 06, SLTI3 = 02, XORI3 = 04, ANDI3 = 07, // LA com
	SLTIU3 = 03, SLLI3 = 01, SRI3 = 05 // imediatos
};

// campo auxiliar de 7 bits para as instrucoes SRLI/SRAI, ADD/SUB, SRL/SRA
enum FUNCT7 {
	ADD7 = 0, SUB7 = 0x20, SRA7 = 0x20, SRL7 = 0, SRLI7 = 0x00, SRAI7 = 0x20
};

/*********************************************************************************************************************/

int32_t dump_add, dump_size;

uint32_t ri, pc;

int is_running = 1;
int op;

/*										Funcoes Principais										*/

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


void fetch() {
	ri = mem[pc >> 2];
	pc += 4;
}

void decode() {

}

void execute() {

};

//Chama oa funcoes de execucao do programa
void step() {
	fetch();
	decode();
	execute();
};

/*											Intrucoes											*/

//Le um inteiro alinhado. Enderecos multiplos de 4
int32_t lw(uint32_t add, int16_t kte) {
	
	int32_t tmp = mem[add >> 2];

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
	uint32_t tmp = mem[add >> 2];
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
	tmp = mem[add >> 2];
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
	tmp = (mem[add >> 2] >> (8 * (kte % 4)));
	
	//Acessa o bit
	tmp &= 0xFF;

	return (((tmp & 0x80) == 0) ? (tmp | 0xFFFFFF00) : tmp);
}

//Le um byte (8 bits) e retorna um inteiro sem sinal
int32_t lbu(uint32_t add, int16_t kte) {
	uint32_t tmp;

	tmp = mem[add >> 2];
	tmp = tmp >> (8 * kte);
	tmp &= 0xFF;

	return tmp;
}

//Escreve um inteiro alinhado na memoria. Enderecos multiplos de 4
void sw(uint32_t add, int16_t kte, int32_t dado) {

	//Verifica se a constante possui um valor aceitavel
	if ((add + kte) % 4 != 0) {
		printf("\n\n A constante precisa ser um multiplo de 4. (Erro: sw)");
	}

	//Salva na memoria o dado
	mem[add >> 2] = dado;
}

//Escreve meia palavra de 16 bits na memoria. Enderecos multiplos de 2
void sh(uint32_t add, int16_t kte, int16_t dado) {

	// Endereco de memoria
	uint32_t tmp = mem[add >> 4];

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
	mem[add >> 2] = tmp;
}

//Escreve um byte de 8 bits na memoria.
void sb(uint32_t add, int16_t kte, int8_t dado) {
	//Carrega para tmp o valor que esta na memoria
	uint32_t tmp = mem[add >> 2];

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
	mem[add >> 2] = tmp;
}

/*                              Funcoes auxiliares                              */

//Limpa o buffer de input 
void clean_buffer() {
	while (getchar() != '\n');
}

//Funcao para pausar o programa e esperar o enter para continuar
void enter() {
	printf("\n\nPressione Enter para continuar...");

	//Se utilizado sozinho s� para o programa at� qualuqer input ser colocado
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

void initMem(char *textbin, char *databin) {

	FILE* text, * data;
	char byte;
	int num = 0, i, word, auxb;

	for (i = 0; i < 32; i++) {
		regs[i] = 0;
	}

	regs[28] = 0x1800;

	//Stack Pointer
	regs[29] = 0x3FFC;

	//Abre o arquivo textbin
	text = fopen(textbin, "rb+");
	if (!text) {
		printf("Arquivo %s nao econtrado. (Text Bin)", textbin);
		enter();
		return;
	}

	//Abre o arquivo de databin
	data = fopen(databin, "rb+");
	if (!data) {
		printf("Arquivo %s nao encontrado. (Data Bin)", databin);
		enter();
		return;
	}

	//Percorre o arquivo Text
	while (!feof(text) && num < (0x2000 >> 2)) {
		word = 0;
		for (i = 0; i < 4; i++)	{
			if (1 == fread(&byte, sizeof(char), 1, text)) {
				auxb = (int)byte & 0xFF;
				word += auxb << (8 * i);
			}
		}

		mem[num] = word;
		num++;
	}
	fclose(text);

	num = (0x2000 >> 2);

	//Percorre o arquivo data
	while (!feof(data) && num < (0x8000 >> 2)) {
		word = 0;
		for (i = 0; i < 4; i++)	{
			if (1 == fread(&byte, sizeof(char), 1, data)) {
				auxb = (int)byte & 0xFF;
				word += auxb << (8 * i);
			}
		}
		mem[num] = word;
		num++;
	}
	fclose(data);
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
			//initMem();

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
			//read_mem();

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

	//Inicializa o PC em zero
	pc = 0;

	//Chama a funcao principal do programa
	run();

	return 0;
}