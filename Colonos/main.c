#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>


//Variáveis Globais
//ALLEGRO
ALLEGRO_FONT *titulos;
ALLEGRO_FONT *textos;
ALLEGRO_KEYBOARD_STATE state;
ALLEGRO_MOUSE_STATE mouseState;
ALLEGRO_MOUSE_STATE mouseStateAnterior;
int mouseButtons;
//OUTRAS
int exitGame = 0;
const float FPS = 240;
float x, y;
FILE* data;
int saveUIactive = 0;//0 - desativada, 1 - activo
int loadUIactive = 1;//0- desactivada, 1 - activo
int quantidadeEdificios;
int quantidadeColonos;
int numeroTarefas;
//Criar um display para o Allegro
ALLEGRO_DISPLAY *display = NULL;

ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;

//Assets - Sprites
//CENÁRIO
ALLEGRO_BITMAP *water = NULL;
ALLEGRO_BITMAP *tree7 = NULL;
ALLEGRO_BITMAP *tree6 = NULL;
ALLEGRO_BITMAP *tree5 = NULL;
ALLEGRO_BITMAP *tree4 = NULL;
ALLEGRO_BITMAP *tree3 = NULL;
ALLEGRO_BITMAP *tree2 = NULL;
ALLEGRO_BITMAP *tree1 = NULL;
ALLEGRO_BITMAP *rock3 = NULL;
ALLEGRO_BITMAP *rock2 = NULL;
ALLEGRO_BITMAP *rock1 = NULL;
ALLEGRO_BITMAP *soil = NULL;
ALLEGRO_BITMAP *grass = NULL;
ALLEGRO_BITMAP *chopped_trees = NULL;

//VEGETAIS
ALLEGRO_BITMAP *farm1_1 = NULL;
ALLEGRO_BITMAP *farm1_2 = NULL;
ALLEGRO_BITMAP *farm1_3 = NULL;
ALLEGRO_BITMAP *farm2_1 = NULL;
ALLEGRO_BITMAP *farm2_2 = NULL;
ALLEGRO_BITMAP *farm2_3 = NULL;
ALLEGRO_BITMAP *farm3_1 = NULL;
ALLEGRO_BITMAP *farm3_2 = NULL;
ALLEGRO_BITMAP *farm3_3 = NULL;
ALLEGRO_BITMAP *farm4_1 = NULL;
ALLEGRO_BITMAP *farm4_2 = NULL;
ALLEGRO_BITMAP *farm4_3 = NULL;
ALLEGRO_BITMAP *farm5_1 = NULL;
ALLEGRO_BITMAP *farm5_2 = NULL;
ALLEGRO_BITMAP *farm5_3 = NULL;
ALLEGRO_BITMAP *farm6_1 = NULL;
ALLEGRO_BITMAP *farm6_2 = NULL;
ALLEGRO_BITMAP *farm6_3 = NULL;
ALLEGRO_BITMAP *farm7_1 = NULL;
ALLEGRO_BITMAP *farm7_2 = NULL;
ALLEGRO_BITMAP *farm7_3 = NULL;

//EDIFÍCIOS
ALLEGRO_BITMAP *city_hall1, *city_hall2, *farm1, *farm2, *house1, 
*house2, *warehouse1, *warehouse2, *warehouse3, *warehouse4, *house3, *house4 = NULL;

//Assets - Animated sprites
ALLEGRO_BITMAP *men1, *men2, *men3, *men4, *woman1, *woman2, *woman3, *woman4 = NULL;

//Cores
ALLEGRO_COLOR RED, BLACK, ORANGE, GREEN, YELLOW, GREY, WHITE;

//Tamanho do mapa
#define MAPWIDTH 35
#define MAPHEIGHT 35
#define TILEWIDTH 32
#define TILEHEIGHT 32
#define DISPLAYWIDTH 1024
#define DISPLAYHEIGHT 640

//Velocidades
#define VELOCIDADE_BONECOS 0.2

//Offset do ecra
float offsetX = 0;
float offsetY = 0;

int fundoX = 20;
int fundoY = DISPLAYHEIGHT - 100;

//limitador de input do teclado
int KBLimit = 5;
int KBLimitCounter = 0;

//Contadores de recursos
int pedra = 200, madeira = 200, comida = 100;
int maxPedra = 300, maxMadeira = 300, maxComida = 300;

//Textos de erro
char textoErro[256] = "";
int tempoTextoErro = 0;

// *********************************************************************************************************** //
// ESTRUTURAS DE DADOS //

//Descreve uma quinta
typedef struct farm
{
	int type; //Sprite do estado inicial
	int i, j; //Posicao da quinta no mapa
	int timer; //Contador de há quanto tempo a quinta está no estado atual
	int phase; //Fase em que a quinta se encontra (0, 1, 2)
	int minTimer; //Tempo necessário para passar para a próxima fase
	char *name; //Nome descritivo do tipo de quinta
	struct farm *next; //Apontador para o elemento seguinte
}* Farm;

//Descreve uma floresta (madeira)
typedef struct forest
{
	int type; //Sprite do estado inicial
	int originalType; //Usado para guardar o tipo original, necessário aquando da regeneração
	int i, j; //Posição da floresta no mapa
	int quantity; //Quantidade atual de madeira que esta floresta possui
	int originalQuantity; //Quantidade inicial de madeira que esta floresta possui
	int phase; //Fase em que a floresta se encontra (arvores / troncos cortados)
	int minTimer; //Tempo necessário para a regeneração da floresta
	int timer; //Contador de tempo decorrido
	char * name; //Nome descritivo do tipo de floresta
	struct forest * next; //Apontador para o elemento seguinte
}* Forest;

//Descreve uma tarefa a executar por um colono
typedef struct tarefa
{
	//0 - Ir para casa;
	//1 - Apanhar Madeira
	//2 - Descarregar Madeira
	//3 - Apanhar Pedra
	//4 - Descarregar Pedra
	//5 - Apanhar Peixe
	//6 - Descarregar Peixe
	//7 - Apanhar Vegetais
	//8 - Descarregar Vegetais
	//9 - Descansar
	int type; //Tipo de tarefa
	float energianecessaria;
	int x, y; //Coordenadas da tarefa a executar
	int tempo, tempoExecucao;
	struct tarefa * next; //Apontador para a proxima tarefa
}* Tarefa;

//Descreve um bonequinho
typedef struct character
{
	ALLEGRO_BITMAP *spriteSheet; //Spritesheet que contém a animação desta personagem
	int movimento; //0 - Parado, 1 - Movimento
	int direcao; //0 - Baixo; 1 - Esquerda; 2 - Direita; 3 - Cima
	float x, y; //Posicao, em pixeis
	int animationFrame; //Frame da animação atual
	int animationTimer; //Contador do tempo de cada frame
	struct character *next; //Apontador para o elemento seguinte
	struct node * path; //Caminho que o boneco tem a percorrer
	struct tarefa *tarefa; //Lista de tarefas a executar
	int madeira; //Quantidade de madeira que o boneco transporta
	int pedra; //Quantidade de pedra que o boneco transporta
	int comida; //Quantidade de comida que o boneco transporta
	float energia;// energia para executar tarafes
	char action[256]; //Acção que o colono está a fazer num determinado momento
	bool tarefaIniciada; //Indica se a tarefa atual foi ou não iniciada
	float casaX, casaY; //Coordenadas da casa onde este colono mora
}* Character;

//Descreve um edificio
typedef struct building
{
	int type; //Sprite do estado inicial
	int x, y; //Posição do edifício no mapa
	int timer; //Tempo passado desde o início da construção do edifício
	int minTimer; //Tempo que o edifício demora a ser construído
	int constructionCounter; //Vai aumentando cada vez que minTimer = timer, até ao máximo de 32 (altura da sprite)
	struct building *next; //Apontador para o elemento seguinte
	struct character *colonists; //Colonos que se encontram na casa
	char name[]; //Nome descritivo do edifício
}* Building;


//AI - Cálculo de caminhos
//Descreve um nó de caminho
typedef struct node
{
	int x, y; //Posição do node
	bool caminho; //Indica se esta posição é ou não caminho
	struct node * parent; //Apontador para o node que colocou este na lista aberta
	bool inOpenList; //Indica se está na lista aberta
	bool inClosedList; //Indica se está na lista fechada
	float distanceToTarget; //Distância aproximada desde este node até ao alvo
	float distanceTravelled; //Distância já viajada da origem até este node
	struct node * vizinhos[4]; //Apontadores para os vizinhos deste node (cima, baixo, esquerda, direita)
	struct node * next; //Próximo node
	int contadorVizinhos; //Nº de vizinhos uteis deste node
}* Node;

//Descreve uma opção / tecla que pode ser premida
typedef struct opcao
{
	char tecla; //Tecla que pode ser premida
	char descricao[256]; //descrição da opção
	struct opcao * next;
	int madeira; //Custo em madeira da opção
	int pedra; //Custo em pedra da opção
	bool ativa; //Indica se a opção está ativa, ou seja, se é possível selecioná-la / executá-la
}* Opcao;

Opcao opcaoAtiva; //Opção final que foi seleccionada pelo jogador e que será executada

//Conta o número de opções na lista
int CountOpcoes(Opcao opcoes){
	Opcao listaOpcoes = opcoes;
	int contador = 0;
	while (listaOpcoes != NULL){
		contador++;
		listaOpcoes = listaOpcoes->next;
	}
	return contador;
}

//Matriz que define o mapa
//[0] - Tipo de sprite
//[1] - Possibilidade de andar
//[2] - Possibilidade de construir
//TODO: Ler mapa a partir de ficheiro
int mapDef[MAPWIDTH][MAPHEIGHT][3] = 
{
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 9, 0 , 0}, { 10, 0 , 0}, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 11, 0, 0 }, { 9, 0, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 11, 0, 0 }, { 10, 0, 0 }, { 9, 0, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 10, 0, 0 }, { 11, 0, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 9, 1, 0 }, { 11, 0, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }}
};

//Matriz que guarda os tiles do mapa
ALLEGRO_BITMAP * mapa[MAPWIDTH][MAPHEIGHT];

//Lista ligada de quintas vazia
Farm quintas = NULL;

//Lista ligada de bonequinhos vazia
Character bonequinhos = NULL;

//Lista ligada de casas vazia
Building edificios = NULL;

//Lista ligada de florestas vazia
Forest florestas = NULL;

//Lista ligada de opcoes disponiveis
Opcao opcoes = NULL;

//Matriz de searchNodes
Node searchNodes[MAPWIDTH][MAPHEIGHT];
//Lista aberta de Nodes
Node listaAberta = NULL;
//Lista fechada de Nodes
Node listaFechada = NULL;

//Recicladas
Node path;
int vizinhos[4][4];

//Cenas selecionadas
Character bonecoSelecionado = NULL;
bool bonecoHovered = false;
Building edificioSelecionado = NULL;

//***********************************************************************************************************//
//AI - PATHFINDING

//Reinicializa o estado de todos os nodes
void ResetSearchNodes(){
	////Eliminar todos os elementos da lista aberta
	while (listaAberta != NULL){
		Node aux = listaAberta->next;
		//free(listaAberta); //MEMORY LEAK!!!
		listaAberta = aux;
	}
	listaAberta = NULL;
	//Eliminar todos os elementos da lista fechada
	while (listaFechada != NULL){
		Node aux = listaFechada->next;
		//free(listaFechada); //MEMORY LEAK!!!
		listaFechada = aux;
	}
	listaFechada = NULL;
	//Fazer reset às propriedades dos nodes
	for (int x = 0; x < MAPWIDTH; x++){
		for (int y = 0; y < MAPHEIGHT; y++){
			Node node = searchNodes[x][y];
			if (node == NULL) {
				continue;
			}
			node->inOpenList = false;
			node->inClosedList = false;
			node->distanceTravelled = 0;
			node->distanceToTarget = 0;
			node->parent = NULL;
			node->contadorVizinhos = 0;
			for (int i = 0; i < 4; i++){
				node->vizinhos[0]->caminho = false;
			}
		}
	}
}

int PixelToWorld(float value, int WidthOrHeight){
	int res = 0;
	if (WidthOrHeight == 0){
		//Largura
		res = (int)((value / TILEWIDTH));
	}
	else{
		//Altura
		res = (int)((value / TILEHEIGHT));
	}
	return res;
}

float WorldToPixel(int value, int WidthOrHeight){
	int res = 0;
	if (WidthOrHeight == 0){
		//Largura
		res = (value * TILEWIDTH);
	}
	else{
		//Altura
		res = (value * TILEHEIGHT);
	}
	return res;
}

//Insere uma tarefa numa lista de tarefas
Tarefa InsertTarefa(Tarefa listaTarefas, int type, int x, int y){
	Tarefa tarefa = malloc(sizeof(struct tarefa));
	tarefa->type = type;
	tarefa->x = x;
	tarefa->y = y;
	
	switch (type)
	{
	case 0:
		//Ir para casa
		tarefa->tempo = 0;
		break;
	case 1:
		//Apanhar Madeira
		tarefa->tempo = 500;
		tarefa->energianecessaria = 6;
		break;
	case 2:
		//Descarregar madeira
		tarefa->tempo = 200;
		tarefa->energianecessaria = 2;
		break;
	case 3:
		//Apanhar pedra
		tarefa->tempo = 1000;
		tarefa->energianecessaria = 7;
		break;
	case 4:
		//Descarregar pedra
		tarefa->tempo = 500;
		tarefa->energianecessaria = 2;
		break;
	case 5:
		//Apanhar peixe
		tarefa->tempo = 1000;
		tarefa->energianecessaria = 3;
		break;
	case 6:
		//Descarregar peixe
		tarefa->tempo = 100;
		tarefa->energianecessaria = 2;
		break;
	case 7:
		//Apanhar vegetais
		tarefa->tempo = 2500;
		tarefa->energianecessaria = 5;
		break;
	case 8:
		//Descarregar vegetais
		tarefa->tempo = 250;
		tarefa->energianecessaria = 2;
		break;
	case 9:
		//Descansar
		tarefa->tempo = 12000;
		break;
	case 10:
		//Construir casa 1
		tarefa->tempo = 7000;
		tarefa->energianecessaria = 11;
		break;
	case 11:
		//Construir casa 2
		tarefa->tempo = 7000;
		tarefa->energianecessaria = 11;
		break;
	case 12:
		//Construir casa 3
		tarefa->tempo = 7000;
		tarefa->energianecessaria = 11;
		break;
	case 13:
		//Construir casa 4
		tarefa->tempo = 7000;
		tarefa->energianecessaria = 11;
		break;
	case 14:
		//Construir Farmhouse
		tarefa->tempo = 3000;
		tarefa->energianecessaria = 18;
		break;
	case 15:
		//Plantar Milho
		tarefa->tempo = 3000;
		tarefa->energianecessaria = 8;
		break;
	case 16:
		//Plantar Morango
		tarefa->tempo = 3000;
		tarefa->energianecessaria = 8;
		break;
	case 17:
		//Plantar Pimento
		tarefa->tempo = 3000;
		tarefa->energianecessaria = 8;
		break;
	case 18:
		//Plantar Batata
		tarefa->tempo = 3000;
		tarefa->energianecessaria = 8;
		break;
	case 19:
		//Plantar Nabo
		tarefa->tempo = 3000;
		tarefa->energianecessaria = 8;
		break;
	case 20:
		//Plantar Cenoura
		tarefa->tempo = 3000;
		tarefa->energianecessaria = 8;
		break;
	case 21:
		//Plantar Beterraba
		tarefa->tempo = 3000;
		tarefa->energianecessaria = 8;
		break;
	default:
		break;
	}
	tarefa->tempoExecucao = 0;
	tarefa->next = listaTarefas;
	return tarefa;
}

Node InsertNode(Node lista, Node node){
	Node aux = node;
	aux->next = lista;
	return aux;
}

Opcao InsertOption(Opcao opcoes, char tecla, char descricao[256], int madeira, int pedra){
	Opcao opcao = malloc(sizeof(struct opcao));
	opcao->tecla = tecla;
	strcpy(opcao->descricao, descricao);
	opcao->next = opcoes;
	opcao->madeira = madeira;
	opcao->pedra = pedra;
	opcao->ativa = true;
	return opcao;
}

bool OptionExists(char tecla){
	Opcao aux = opcoes;
	while (aux != NULL){
		if (aux->tecla == tecla){
			return true;
		}
		aux = aux->next;
	}
	return false;
}

Opcao RemoveOption(Opcao opcoes, char tecla){
	Opcao aux;
	if (opcoes != NULL){
		//Lista não está vazia
		if (opcoes->tecla == tecla){
			//encontramos o elemento a eliminar
			aux = opcoes->next;
			return aux;
		}
		else{
			//não é este o elemento e eliminar, continuar a recursão
			opcoes->next = RemoveOption(opcoes->next, tecla);
			return opcoes;
		}
	}
	else{
		//Lista vazia
		return opcoes;
	}
}

Farm RemoveFarm(Farm quintas, int x, int y){
	Farm aux;
	if (quintas != NULL){
		//Lista não está vazia
		if (quintas->i == x && quintas->j == y){
			//encontramos o elemento a eliminar
			aux = quintas->next;
			return aux;
		}
		else{
			//não é este o elemento e eliminar, continuar a recursão
			quintas->next = RemoveFarm(quintas->next, x, y);
			return quintas;
		}
	}
	else{
		//Lista vazia
		return quintas;
	}
}

Tarefa RemoveTarefa(Tarefa tarefas, int type, int x, int y){
	Tarefa aux;
	if (tarefas != NULL){
		//Lista não está vazia
		if (tarefas->type == type && tarefas->x == x && tarefas->y == y){
			//encontramos o elemento a eliminar
			aux = tarefas->next;
			return aux;
		}
		else{
			//não é este o elemento e eliminar, continuar a recursão
			tarefas->next = RemoveTarefa(tarefas->next, tarefas->type, tarefas->x, tarefas->y);
			return tarefas;
		}
	}
	else{
		//Lista vazia
		return tarefas;
	}
}

//Remover elemento da lista ligada - recursivamente
Node RemoveNode(Node enderecoInicioLista, int x, int y){
	Node aux;
	if (enderecoInicioLista != NULL){
		//Lista não está vazia
		if (enderecoInicioLista->x == x && enderecoInicioLista->y == y){
			//encontramos o elemento a eliminar
			aux = enderecoInicioLista->next;
			return aux;
		}
		else{
			//não é este o elemento e eliminar, continuar a recursão
			enderecoInicioLista->next = RemoveNode(enderecoInicioLista->next, x, y);
			return enderecoInicioLista;
		}
	}
	else{
		//Lista vazia
		return enderecoInicioLista;
	}
}

Character RemoveCharacter(Character enderecoInicioLista, Character boneco){
	Character aux;
	if (enderecoInicioLista != NULL){
		//Lista não está vazia
		if (enderecoInicioLista->x == boneco->x 
			&& enderecoInicioLista->y == boneco->y
			&& enderecoInicioLista->action == boneco->action
			&& enderecoInicioLista->animationFrame == boneco->animationFrame
			&& enderecoInicioLista->animationTimer == boneco->animationTimer
			&& enderecoInicioLista->comida == boneco->comida
			&& enderecoInicioLista->direcao == boneco->direcao
			&& enderecoInicioLista->energia == boneco->energia
			&& enderecoInicioLista->madeira == boneco->madeira
			&& enderecoInicioLista->movimento == boneco->movimento
			&& enderecoInicioLista->next == boneco->next
			&& enderecoInicioLista->path == boneco->path
			&& enderecoInicioLista->pedra == boneco->pedra
			&& enderecoInicioLista->spriteSheet == boneco->spriteSheet
			&& enderecoInicioLista->tarefa == boneco->tarefa
			&& enderecoInicioLista->tarefaIniciada == boneco->tarefaIniciada){
			//encontramos o elemento a eliminar
			aux = enderecoInicioLista->next;
			enderecoInicioLista->next = NULL;
			return aux;
		}
		else{
			//não é este o elemento e eliminar, continuar a recursão
			enderecoInicioLista->next = RemoveCharacter(enderecoInicioLista->next, boneco);
			return enderecoInicioLista;
		}
	}
	else{
		//Lista vazia
		return enderecoInicioLista;
	}
}

//Devolve a posicao X do edificio headquarters
int XHeadQuarters(){
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			if (mapDef[j][i][0] == 35){
				return i;
			}
		}
	}
	return 0;
}

//Devolve a posicao Y do edificio headquarters
int YHeadQuarters(){
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			if (mapDef[j][i][0] == 35){
				return j;
			}
		}
	}
	return 0;
}

//Cria ou atualiza a matriz de searchnodes
void UpdateSearchNodes(){
	ResetSearchNodes();
	int x, y;
	//Criar um node para cada espaço do mapa
	for (x = 0; x < MAPWIDTH; x++)
	{
		for (y = 0; y < MAPHEIGHT; y++)
		{
			Node node = (Node)malloc(sizeof(struct node));
			node->x = x;
			node->y = y;
			node->caminho = false;
			node->contadorVizinhos = 0;
			node->distanceToTarget = 0;
			node->distanceTravelled = 0;
			node->inClosedList = false;
			node->inOpenList = false;
			node->next = NULL;
			node->parent = NULL;
			for (int i = 0; i < 4; i++){
				node->vizinhos[i] = NULL;
			}

			//Apenas é caminho se tiver 1 na segunda posição desta tile
			if (mapDef[y][x][1] == 1){
				node->caminho = true;
				searchNodes[x][y] = node;
			}
		}
	}
	//Para cada um dos nodes, vamos ligá-lo aos vizinhos
	for (x = 0; x < MAPWIDTH; x++)
	{
		for (y = 0; y < MAPHEIGHT; y++)
		{
			Node node = searchNodes[x][y];
			//Se não se pode andar neste node, não interessa
			if (node == NULL || !node->caminho){
				continue;
			}
			//Criamos um array com todos os possíveis vizinhos que este node pode ter
			int vizinhos[4][4] = { 
					{x, y - 1},
					{x, y + 1},
					{x - 1, y},
					{x + 1, y}
			};
			//Iteramos por cada um dos possíveis vizinhos
			for (int i = 0; i < 4; i++)
			{
				int vizinhoX = vizinhos[i][0];
				int vizinhoY = vizinhos[i][1];

				//Confirmar que este vizinho faz parte do mapa
				if (vizinhoX < 0 || vizinhoX > MAPWIDTH - 1
					|| vizinhoY < 0 || vizinhoY > MAPHEIGHT - 1){
					continue;
				}

				Node vizinho = searchNodes[vizinhoX][vizinhoY];

				//Só nos interessam vizinhos em que se possa andar
				if (vizinho == NULL || !vizinho->caminho){
					continue;
				}
				
				node->contadorVizinhos++;
				node->vizinhos[node->contadorVizinhos-1] = vizinho;
			}
		}
	}

	//DEBUG
	//Verificar se estão a ser criados searchnodes
	/*
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			Node node = searchNodes[i][j];
			if (node != NULL){
				printf("%f; ", node->distanceToTarget);
			}
		}
	}
	*/
}

//Devolve uma estimativa da distância entre dois tiles
float Heuristic(int x1, int y1, int x2, int y2){
	return abs(x1 - x2) + abs(y1 - y2);
}

//Devolve um elemento de uma lista por indice
Node GetIndex(Node lista, int index){
	int contador = 0;
	while (lista != NULL){
		if (contador == index){
			return lista;
		}
		else{
			contador++;
			lista = lista->next;
		}
	}
	return lista;
}

//Devolve o número de elementos numa lista de nodes
int ListCountNodes(Node endereco){
	if (endereco == NULL){
		return 0;
	}
	else{
		//A função invoca-se a ela própria
		return (1 + ListCountNodes(endereco->next));
	}
}

//Devolve o número de elementos numa lista de characters
int ListCountCharacters(Character endereco){
	if (endereco == NULL){
		return 0;
	}
	else{
		//A função invoca-se a ela própria
		return (1 + ListCountCharacters(endereco->next));
	}
}

//Devolve o numero de colonos que estão dentro de casas
int ListCountHouseCharacters(Building edificios){
	int contador = 0;
	Building listaEdificios = edificios;
	while (listaEdificios != NULL){
		Character colonos = listaEdificios->colonists;
		while (colonos != NULL){
			contador++;
			colonos = colonos->next;
		}
		listaEdificios = listaEdificios->next;
	}
	return contador;
}

//Conta os bonecos que estão numa casa
int ListCountCharactersHouse(Building casa){
	int contador = 0;
	if (casa != NULL){
		Character bonecos = casa->colonists;
		while (bonecos != NULL){
			contador++;
			bonecos = bonecos->next;
		}
	}
	return contador;
}

//Devolve o node com distância mais pequena ao objetivo
Node FindBestNode(){
	Node currentTile;
	currentTile = GetIndex(listaAberta, 0);
	float smallestDistanceToTarget = 999999;
	for (int i = 0; i < ListCountNodes(listaAberta); i++){
		Node aux = GetIndex(listaAberta, i);
		if (aux->distanceToTarget < smallestDistanceToTarget){
			currentTile = GetIndex(listaAberta, i);
			smallestDistanceToTarget = currentTile->distanceToTarget;
		}
	}
	return currentTile;
}

//Usa o campo parent dos nodes para refazer o caminho mais curto do end node para o start node
//Devolve uma lista ligada de nodes a percorrer para ir da origem ao destino
Node FindFinalPath(Node startNode, Node endNode){
	listaFechada = InsertNode(listaFechada, endNode);
	Node parentTile = endNode->parent;
	//Percorrer o caminho para trás, do nó final, através do parent, até ao inicial
	while (parentTile != startNode && parentTile != NULL && parentTile->parent != NULL){
		listaFechada = InsertNode(listaFechada, parentTile);
		parentTile = parentTile->parent;
	}
	Node finalPath = NULL;
	//Reverter o caminho para ir do inicial ao final
	for (int i = ListCountNodes(listaFechada) - 1; i >= 0; i--){
		finalPath = InsertNode(finalPath, GetIndex(listaFechada, i));
	}
	return finalPath;
}

Node FindPath(int x1, int y1, int x2, int y2)
{

	// Only try to find a path if the start and end points are different.
	if (x1 == x2 && y1 == y2)
	{
		return NULL;
	}

	/////////////////////////////////////////////////////////////////////
	// Step 1 : Clear the Open and Closed Lists and reset each node’s F 
	//          and G values in case they are still set from the last 
	//          time we tried to find a path. 
	/////////////////////////////////////////////////////////////////////
	UpdateSearchNodes();

	// Store references to the start and end nodes for convenience.
	Node startNode = searchNodes[x1][y1];
	Node endNode = searchNodes[x2][y2];

	/////////////////////////////////////////////////////////////////////
	// Step 2 : Set the start node’s G value to 0 and its F value to the 
	//          estimated distance between the start node and goal node 
	//          (this is where our H function comes in) and add it to the 
	//          Open List. 
	/////////////////////////////////////////////////////////////////////
	startNode->inOpenList = true;

	startNode->distanceToTarget = Heuristic(x1, y1, x2, y2);
	startNode->distanceTravelled = 0;
	
	listaAberta = InsertNode(listaAberta, startNode);

	/////////////////////////////////////////////////////////////////////
	// Setp 3 : While there are still nodes to look at in the Open list : 
	/////////////////////////////////////////////////////////////////////
	while (ListCountNodes(listaAberta) > 0)
	{
		/////////////////////////////////////////////////////////////////
		// a) : Loop through the Open List and find the node that 
		//      has the smallest F value.
		/////////////////////////////////////////////////////////////////
		Node currentNode = FindBestNode();

		/////////////////////////////////////////////////////////////////
		// b) : If the Open List empty or no node can be found, 
		//      no path can be found so the algorithm terminates.
		/////////////////////////////////////////////////////////////////
		if (currentNode == NULL)
		{
			break;
		}

		/////////////////////////////////////////////////////////////////
		// c) : If the Active Node is the goal node, we will 
		//      find and return the final path.
		/////////////////////////////////////////////////////////////////
		if (currentNode == endNode)
		{
			// Trace our path back to the start.
			return FindFinalPath(startNode, endNode);
		}

		/////////////////////////////////////////////////////////////////
		// d) : Else, for each of the Active Node’s neighbours :
		/////////////////////////////////////////////////////////////////
		for (int i = 0; i < currentNode->contadorVizinhos; i++)
		{
			Node neighbor = currentNode->vizinhos[i];

			//////////////////////////////////////////////////
			// i) : Make sure that the neighbouring node can 
			//      be walked across. 
			//////////////////////////////////////////////////
			if (neighbor == NULL || neighbor->caminho == false)
			{
				continue;
			}

			//////////////////////////////////////////////////
			// ii) Calculate a new G value for the neighbouring node.
			//////////////////////////////////////////////////
			float distanceTraveled = currentNode->distanceTravelled + 1;

			// An estimate of the distance from this node to the end node.
			float heuristic = Heuristic(neighbor->x, neighbor->y, x2, y2);

			//////////////////////////////////////////////////
			// iii) If the neighbouring node is not in either the Open 
			//      List or the Closed List : 
			//////////////////////////////////////////////////
			if (neighbor->inOpenList == false && neighbor->inClosedList == false)
			{
				// (1) Set the neighbouring node’s G value to the G value 
				//     we just calculated.
				neighbor->distanceTravelled = distanceTraveled;
				// (2) Set the neighbouring node’s F value to the new G value + 
				//     the estimated distance between the neighbouring node and
				//     goal node.
				neighbor->distanceToTarget = distanceTraveled + heuristic;
				// (3) Set the neighbouring node’s Parent property to point at the Active 
				//     Node.
				neighbor->parent = currentNode;
				// (4) Add the neighbouring node to the Open List.
				neighbor->inOpenList = true;
				listaAberta = InsertNode(listaAberta, neighbor);
			}
			//////////////////////////////////////////////////
			// iv) Else if the neighbouring node is in either the Open 
			//     List or the Closed List :
			//////////////////////////////////////////////////
			else if (neighbor->inOpenList || neighbor->inClosedList)
			{
				// (1) If our new G value is less than the neighbouring 
				//     node’s G value, we basically do exactly the same 
				//     steps as if the nodes are not in the Open and 
				//     Closed Lists except we do not need to add this node 
				//     the Open List again.
				if (neighbor->distanceTravelled > distanceTraveled)
				{
					neighbor->distanceTravelled = distanceTraveled;
					neighbor->distanceToTarget = distanceTraveled + heuristic;

					neighbor->parent = currentNode;
				}
			}
		}

		/////////////////////////////////////////////////////////////////
		// e) Remove the Active Node from the Open List and add it to the 
		//    Closed List
		/////////////////////////////////////////////////////////////////
		listaAberta = RemoveNode(listaAberta, currentNode->x, currentNode->y);
		currentNode->inClosedList = true;
	}

	// No path could be found.
	return NULL;
}


//***********************************************************************************************************//

//Inicialização do Allegro
int InitializeAllegro(){
	//Inicializar o allegro
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	if (!al_init_image_addon()) {
		fprintf(stderr, "failed to initialize allegro image module!\n");
		return -1;
	}
	//Inicializar o display
	display = al_create_display(DISPLAYWIDTH, DISPLAYHEIGHT);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}
	//Inicializar o addon de primitivas
	if (!al_init_primitives_addon()) {
		fprintf(stderr, "failed to initialize primitives addon!\n");
		return -1;
	}
	//Inicializar o rato
	mouseButtons = al_install_mouse();
	//Inicializar som 
	if (!al_install_audio()) {
		fprintf(stderr, "failed to initialize the audio addon!\n");
		return -1;
	}
	//Inicializar o timer
	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
	//Inicializar a event queue
	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	//Titulo da janela
	al_set_window_title(display, "Colonos");
	//Inicializar o keyboard
	al_install_keyboard();
	//initialize the font addon
	al_init_font_addon();
	//initialize the ttf (True Type Font) addon
	al_init_ttf_addon();
	//Inicializar uma fonte
	titulos = al_load_ttf_font("aller.ttf", 28, 0);
	if (!titulos){
		fprintf(stderr, "Could not load 'pirulen.ttf'.\n");
		return -1;
	}
	textos = al_load_ttf_font("aller.ttf", 14, 0);
	if (!textos){
		printf("Could not load 'aller.ttf'.\n");
		return -1;
	}

	//Inicializar cores
	RED = al_map_rgb(255, 0, 0);
	BLACK = al_map_rgb(0, 0, 0);
	ORANGE = al_map_rgb(255, 255, 0);
	GREEN = al_map_rgb(0, 128, 0);
	YELLOW = al_map_rgb(255, 255, 0);
	GREY = al_map_rgba(0, 0, 0, 128);
	WHITE = al_map_rgb(255, 255, 255);
	
	al_start_timer(timer);

	return 1;
}

//Criar o mapa
void UpdateMap(){
	
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			switch (mapDef[i][j][0])
			{
			case 0: mapa[i][j] = water;
				break;
			case 1: mapa[i][j] = grass;
				break;
			case 2: mapa[i][j] = tree1;
				break;
			case 3: mapa[i][j] = tree2;
				break;
			case 4: mapa[i][j] = tree3;
				break;
			case 5: mapa[i][j] = tree4;
				break;
			case 6: mapa[i][j] = tree5;
				break;
			case 7: mapa[i][j] = tree6;
				break;
			case 8: mapa[i][j] = tree7;
				break;
			case 9: mapa[i][j] = rock1;
				break;
			case 10: mapa[i][j] = rock2;
				break;
			case 11: mapa[i][j] = rock3;
				break;
			case 12: mapa[i][j] = chopped_trees;
				break;
			case 14: mapa[i][j] = farm1_1;
				break;
			case 15: mapa[i][j] = farm1_2;
				break;
			case 16: mapa[i][j] = farm1_3;
				break;
			case 17: mapa[i][j] = farm2_1;
				break;
			case 18: mapa[i][j] = farm2_2;
				break;
			case 19: mapa[i][j] = farm2_3;
				break;
			case 20: mapa[i][j] = farm3_1;
				break;
			case 21: mapa[i][j] = farm3_2;
				break;
			case 22: mapa[i][j] = farm3_3;
				break;
			case 23: mapa[i][j] = farm4_1;
				break;
			case 24: mapa[i][j] = farm4_2;
				break;
			case 25: mapa[i][j] = farm4_3;
				break;
			case 26: mapa[i][j] = farm5_1;
				break;
			case 27: mapa[i][j] = farm5_2;
				break;
			case 28: mapa[i][j] = farm5_3;
				break;
			case 29: mapa[i][j] = farm6_1;
				break;
			case 30: mapa[i][j] = farm6_2;
				break;
			case 31: mapa[i][j] = farm6_3;
				break;
			case 32: mapa[i][j] = farm7_1;
				break;
			case 33: mapa[i][j] = farm7_2;
				break;
			case 34: mapa[i][j] = farm7_3;
				break;
			case 35: mapa[i][j] = city_hall1;
				break;
			case 36: mapa[i][j] = city_hall2;
				break;
			case 37: mapa[i][j] = farm1;
				break;
			case 38: mapa[i][j] = farm2;
				break;
			case 39: mapa[i][j] = house1;
				break;
			case 40: mapa[i][j] = house2;
				break;
			case 41: mapa[i][j] = warehouse1;
				break;
			case 42: mapa[i][j] = warehouse2;
				break;
			case 43: mapa[i][j] = warehouse3;
				break;
			case 44: mapa[i][j] = warehouse4;
				break;
			case 45: mapa[i][j] = house3;
				break;
			case 46: mapa[i][j] = house4;
				break;
			case 47: mapa[i][j] = soil;
				break;
			default:
				mapa[i][j] = grass;
				break;
			}
		}
	}
}

//Inserir edifício
Building InsertBuilding(Building endereco, int j, int i, int type){
	Building edificio = (Building)malloc(sizeof(struct building));
	switch (type)
	{
	case 35: //City hall1
		edificio->minTimer = 0; //Já aparece constuído
		strcpy(edificio->name, "Headquarters");
		break;
	case 36: //City hall2
		edificio->minTimer = 0; //Já aparece constuído
		strcpy(edificio->name, "Headquarters");
		break;
	case 37: //Farm1
		edificio->minTimer = 100;
		strcpy(edificio->name, "Farmhouse");
		break;
	case 38: //Farm2
		edificio->minTimer = 100;
		strcpy(edificio->name, "Farmhouse");
		break;
	case 39: //house 1
		edificio->minTimer = 200;
		strcpy(edificio->name, "House");
		break;
	case 40: //house2
		edificio->minTimer = 200;
		strcpy(edificio->name, "House");
		break;
	case 41: //warehouse1
		edificio->minTimer = 400;
		strcpy(edificio->name, "Warehouse");
		break;
	case 42: //warehouse2
		edificio->minTimer = 400;
		strcpy(edificio->name, "Warehouse");
		break;
	case 43: //warehouse3
		edificio->minTimer = 400;
		strcpy(edificio->name, "Warehouse");
		break;
	case 44: //warehouse4
		edificio->minTimer = 400;
		strcpy(edificio->name, "Warehouse");
		break;
	case 45: //house 3
		edificio->minTimer = 200;
		strcpy(edificio->name, "House");
		break;
	case 46: //house 4
		edificio->minTimer = 200;
		strcpy(edificio->name, "House");
		break;
	default:
		edificio->minTimer = 200;
		strcpy(edificio->name, "House");
		break;
	}

	edificio->x = i;
	edificio->y = j;
	edificio->timer = 0;
	edificio->type = type;
	edificio->constructionCounter = 0;
	edificio->next = endereco;
	edificio->colonists = NULL;
	mapDef[i][j][0] = type;
	mapDef[i][j][1] = 0; // Characters can't walk over buildings 
	mapDef[i][j][2] = 0; // Can't build over buildings
	mapDef[i + 1][j][2] = 0; //Espaço da entrada
	return edificio;
}

//Inserir quinta
Farm InsertFarm(Farm endereco, int j, int i, int type){
	//Aloca o espaço necessário na memória
	//Devolve o endereço de memória do espaço alocado
	Farm farm = (Farm)malloc(sizeof(struct farm));
	//Dar os valores ao farm

	switch (type)
	{
	case 14:
		farm->minTimer = 4000;
		farm->name = _strdup("Beterraba");
		break;
	case 17:
		farm->minTimer = 3000;
		farm->name = _strdup("Cenoura");
		break;
	case 20:
		farm->minTimer = 5000;
		farm->name = _strdup("Nabo");
		break;
	case 23:
		farm->minTimer = 3000;
		farm->name = _strdup("Batata");
		break;
	case 26:
		farm->minTimer = 2500;
		farm->name = _strdup("Pimento");
		break;
	case 29:
		farm->minTimer = 1500;
		farm->name = _strdup("Morango");
		break;
	case 32:
		farm->minTimer = 4000;
		farm->name = _strdup("Milho");
		break;
	default:
		break;
	}

	farm->i = i;
	farm->j = j;
	farm->phase = 0;
	farm->timer = 0;
	farm->type = type;
	farm->next = endereco;
	mapDef[i][j][0] = type;
	mapDef[i][j][1] = 0; // Characters can't walk over farms 
	mapDef[i][j][2] = 0; // Can't build over farms
	return farm;
}

//Inserir floresta
Forest InsertForest(Forest endereco, int j, int i, int type){
	Forest forest = (Forest)malloc(sizeof(struct forest));
	switch (type)
	{
	case 2:
		forest->minTimer = 30000;
		forest->quantity = 30;
		forest->name = _strdup("Forest");
		break;
	case 3:
		forest->minTimer = 30000;
		forest->quantity = 35;
		forest->name = _strdup("Forest");
		break;
	case 4:
		forest->minTimer = 30000;
		forest->quantity = 30;
		forest->name = _strdup("Forest");
		break;
	case 5:
		forest->minTimer = 30000;
		forest->quantity = 30;
		forest->name = _strdup("Forest");
		break;
	case 6:
		forest->minTimer = 30000;
		forest->quantity = 30;
		forest->name = _strdup("Forest");
		break;
	case 7:
		forest->minTimer = 30000;
		forest->quantity = 30;
		forest->name = _strdup("Forest");
		break;
	case 8:
		forest->minTimer = 30000;
		forest->quantity = 30;
		forest->name = _strdup("Forest");
		break;
	case 12:
		forest->minTimer = 30000;
		forest->quantity = 30;
		forest->name = _strdup("Chopped trees");
		break;
	default:
		forest->minTimer = 30000;
		forest->quantity = 30;
		forest->name = _strdup("Forest");
		break;
	}

	forest->originalQuantity = forest->quantity;
	forest->type = type;
	forest->originalType = type;
	forest->i = i;
	forest->j = j;
	forest->phase = 0;
	forest->timer = 0;
	forest->next = endereco;
	mapDef[i][j][0] = type;
	mapDef[i][j][1] = 0; // Characters can't walk over forests 
	mapDef[i][j][2] = 0; // Can't build over farms
	return forest;
}

void UpdateForests(Forest endereco){
	while (endereco != NULL){

		//Verificar se ainda tem madeira
		if (endereco->quantity <= 0){
			//Já não tem madeira, mudar para chopped trees
			endereco->type = 12;
			mapDef[endereco->i][endereco->j][0] = endereco->type;
			endereco->phase = 1;
		}
		
		if (endereco->phase == 1){
			endereco->timer++;
			//passar fase?
			if (endereco->timer > endereco->minTimer){
				//Passou tempo suficiente, regenerar
				endereco->phase = 0;
				endereco->timer = 0;
				//alterar o tipo de sprite no mapa
				mapDef[endereco->i][endereco->j][0] = endereco->originalType;
				endereco->type = endereco->originalType;
				endereco->quantity = endereco->originalQuantity;
			}
		}
		
		endereco = endereco->next;
	}
}

//Atualizar quintas
void UpdateFarms(Farm endereco){
	while (endereco != NULL){
		
		//incrementar timer
		endereco->timer++;
		//passar fase?
		if (endereco->timer > endereco->minTimer){
			if (endereco->phase < 2){
				//pronta para crescer
				endereco->phase++;
				endereco->timer = 0;
				//alterar o tipo de sprite no mapa
				mapDef[endereco->i][endereco->j][0] = endereco->type + endereco->phase;
			}
		}
		endereco = endereco->next;
	}
}

//Atualizar edificions
void UpdateBuildings(Building endereco){
	Building aux = endereco;

	while (aux != NULL){
		if (aux->constructionCounter < 32 && aux->minTimer > 0){
			//incrementar timer
			aux->timer += 1;
			//passar fase?
			if (aux->timer > aux->minTimer){
				aux->timer = 0;
				aux->constructionCounter++;
			}
		}

		//Dar energia aos bonecos
		Character aux2 = aux->colonists;
		while (aux->colonists != NULL){
			if (aux->colonists->energia <= 100){
				aux->colonists->energia += 0.1;
				printf("Energia: %f\n", aux->colonists->energia);
			}
			aux->colonists = aux->colonists->next;
		}
		aux->colonists = aux2;

		Character listaBonecos = aux->colonists;
		while (listaBonecos != NULL){

			if (listaBonecos->energia > 100){

				printf("Boneco pronto para sair de casa!\n");

				strcpy(listaBonecos->action, "Idle");

				aux->colonists = RemoveCharacter(aux->colonists, listaBonecos);

				//Colocar o colono na lista de bonequinhos
				listaBonecos->energia = 100;

				//Aqui podia-se tentar manter a tarefa anterior..
				listaBonecos->tarefa = NULL;

				listaBonecos->next = bonequinhos;
				bonequinhos = listaBonecos;

				/*printf("x: %f / %d\n", bonequinhos->x, PixelToWorld(bonequinhos->x, 0));
				printf("y: %f / %d\n", bonequinhos->y, PixelToWorld(bonequinhos->y, 1));*/

				bonequinhos->path = FindPath(PixelToWorld(bonequinhos->x, 0), PixelToWorld(bonequinhos->y, 1), XHeadQuarters() + 1, YHeadQuarters() + 1);
				
			}
			if (listaBonecos != NULL){
				listaBonecos = listaBonecos->next;
			}
		}

		aux = aux->next;
	}
	

}

//Encontrar edificio numa determinada posicao
Building FindBuilding(Building endereco, int x, int y){
	Building building = NULL;
	while (endereco != NULL){
		if (endereco->x == x && endereco->y == y){
			building = endereco;
		}
		endereco = endereco->next;
	}
	return building;
}

//Encontra uma opção numa lista de opções
Opcao FindOpcao(Opcao listaOpcoes, char caracter){
	Opcao opcao = NULL;
	while (listaOpcoes != NULL){
		if (listaOpcoes->tecla = caracter){
			opcao = listaOpcoes;
		}
		listaOpcoes = listaOpcoes->next;
	}
	return opcao;
}

Farm FindFarm(Farm listaQuintas, int x, int y){
	Farm farm = NULL;
	while (listaQuintas != NULL){
		if (listaQuintas->i == x && listaQuintas->j == y && listaQuintas->phase == 2){
			farm = listaQuintas;
			break;
		}
		listaQuintas = listaQuintas->next;
	}
	return farm;
}

//Encontrar floresta numa determinada posicao
Forest FindForest(Forest endereco, int i, int j){
	Forest forest = NULL;
	while (endereco != NULL){
		if (endereco->i == j && endereco->j == i){
			forest = endereco;
		}
		endereco = endereco->next;
	}
	return forest;
}

//Verifica se existe armazem
bool WarehouseBuilt(){
	bool result = false;
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			if (mapDef[i][j][0] == 43){
				result = true;
			}
		}
	}
	return result;
}

//Verifica se existe pelo menos uma farmhouse
bool FarmhouseBuilt(){
	bool result = false;
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			if (mapDef[i][j][0] == 47){
				return true;
			}
		}
	}
	return result;
}

//Desenhar o mapa
void DrawMap(){
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			if (mapDef[j][i][0] != 37 && mapDef[j][i][0] != 38 && mapDef[j][i][0] != 39 && mapDef[j][i][0] != 40 && mapDef[j][i][0] != 41
				&& mapDef[j][i][0] != 42 && mapDef[j][i][0] != 43 && mapDef[j][i][0] != 44 && mapDef[j][i][0] != 45 && mapDef[j][i][0] != 46){
				//Não é edifício
				al_draw_bitmap(mapa[j][i], i * TILEWIDTH + offsetX, j * TILEHEIGHT + offsetY, 0);
			}
			else{
				//é edifício
				//desenhar relva por baixo de todos os edifícios
				al_draw_bitmap(grass, i * TILEWIDTH + offsetX, j * TILEHEIGHT + offsetY, 0);
				//encontrar o edificio que está nesta posicao
				Building edificio = FindBuilding(edificios, j, i);
				al_draw_bitmap_region(mapa[j][i],
					0, 32, 32, -(edificio->constructionCounter), i * TILEWIDTH + offsetX, j * TILEHEIGHT + TILEHEIGHT + offsetY, 0);
			}
			
		}
	}
}

//DEBUG
void DrawNoWalkConstructionTiles(){
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			if (mapDef[j][i][2] == 0){
				//Can't build on these tiles
				al_draw_rectangle(i * TILEWIDTH + offsetX, j * TILEHEIGHT + offsetY, i * TILEWIDTH + TILEWIDTH + offsetX, j * TILEHEIGHT + TILEHEIGHT + offsetY,
					RED, 1);
			}
			//if (mapDef[j][i][1] == 0){
			//	//Can't walk on these tiles
			//	al_draw_rectangle(i * TILEWIDTH + offsetX, j * TILEHEIGHT + offsetY, i * TILEWIDTH + TILEWIDTH + offsetX, j * TILEHEIGHT + TILEHEIGHT + offsetY,
			//		RED, 1);
			//}	
		}
	}
}

Character InsertCharacter(Character endereco, ALLEGRO_BITMAP *sprite, float x, float y, int direcao, int movimento, float casaX, float casaY){
	Character boneco = (Character)malloc(sizeof(struct character));
	boneco->x = x;
	boneco->y = y;
	boneco->spriteSheet = sprite;
	boneco->movimento = movimento;
	boneco->direcao = direcao;
	boneco->animationFrame = 0;
	boneco->animationTimer = 0;
	boneco->next = endereco;
	boneco->path = NULL;
	boneco->tarefa = NULL;
	boneco->madeira = 0;
	boneco->pedra = 0;
	boneco->comida = 0;
	boneco->energia = 20;
	boneco->tarefaIniciada = false;
	boneco->casaX = casaX;
	boneco->casaY = casaY;
	strcpy(boneco->action, "Idle");
	return boneco;
}



bool AlmostEqualRelative(float A, float B)
{
	float maxRelDiff = VELOCIDADE_BONECOS / 30;
	// Calculate the difference.
	float diff = fabs(A - B);
	A = fabs(A);
	B = fabs(B);
	// Find the largest
	float largest = (B > A) ? B : A;

	if (diff < maxRelDiff){
		return true;
	}

	if (diff <= largest * maxRelDiff)
		return true;
	return false;
}



void VerificarEnergia(Character endereco){
	if (endereco->energia <= 20)
	{

		endereco->path = NULL;
		//endereco->tarefa = NULL;

		endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), endereco->casaY, endereco->casaX + 1);
		endereco->tarefa = InsertTarefa(endereco->tarefa, 0, endereco->casaX, endereco->casaY);

		printf("Inserida tarefa de ir para casa\n");
		printf("x: %d\n", endereco->x);
		printf("y: %d\n", endereco->y);
		printf("\n\n");

		strcpy(endereco->action, "Walking home to rest");
	}
}

//Faz o boneco andar para um vizinho de determinadas coordenadas
bool FazerBonecoAndarVizinho(Character boneco, int xi, int yi){
	bool result = false;
	if (mapDef[yi][xi + 1][1] == 1){
		boneco->path = FindPath(PixelToWorld(boneco->x, 0), PixelToWorld(boneco->y, 1), xi + 1, yi);
	}
	if (mapDef[yi][xi - 1][1] == 1 && boneco->path == NULL){
		boneco->path = FindPath(PixelToWorld(boneco->x, 0), PixelToWorld(boneco->y, 1), xi - 1, yi);
	}
	if (mapDef[yi + 1][xi][1] == 1 && boneco->path == NULL){
		boneco->path = FindPath(PixelToWorld(boneco->x, 0), PixelToWorld(boneco->y, 1), xi, yi + 1);
	}
	if (mapDef[yi - 1][xi][1] == 1 && boneco->path == NULL){
		boneco->path = FindPath(PixelToWorld(boneco->x, 0), PixelToWorld(boneco->y, 1), xi, yi - 1);
	}
	if (boneco->path != NULL){
		result = true;
	}
	return result;
}

void setTextoErro(char texto[256]){
	strcpy(textoErro, texto);
	tempoTextoErro = 200;
}

void PlantFarmTask(Character endereco, int farm){
	if (endereco->tarefa->tempoExecucao > endereco->tarefa->tempo){
		//Acabamos de plantar uma quinta!
		endereco->energia -= endereco->tarefa->energianecessaria;

		strcpy(endereco->action, "Idle");


		endereco->tarefaIniciada = false;

		int x = endereco->tarefa->x;
		int y = endereco->tarefa->y;

		//Inserir plantação
		quintas = InsertFarm(quintas, x, y, farm);

		//Remover a tarefa atual
		endereco->tarefa = RemoveTarefa(endereco->tarefa, endereco->tarefa->type, endereco->tarefa->x, endereco->tarefa->y);

		endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters() + 1, YHeadQuarters() + 1);

		VerificarEnergia(endereco);

	}
	else{
		//printf("Tempo de execucao: %d\n", endereco->tarefa->tempoExecucao);

		if (!endereco->tarefaIniciada){
			endereco->tarefaIniciada = true;
		}

		char result[500];
		sprintf(result, "%s%d%s", "Planting (", (endereco->tarefa->tempoExecucao * 100 / endereco->tarefa->tempo), "%)");
		strcpy(endereco->action, result);

		endereco->tarefa->tempoExecucao++;

	}
}

void BuildHouseTask(Character endereco, int house){
	if (endereco->tarefa->tempoExecucao > endereco->tarefa->tempo){
		//Acabamos de construir uma casa!
		endereco->energia -= endereco->tarefa->energianecessaria;

		strcpy(endereco->action, "Idle");

		endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters() + 1, YHeadQuarters() + 1);

		endereco->tarefaIniciada = false;

		int x = endereco->tarefa->x;
		int y = endereco->tarefa->y;

		//Remover a tarefa atual
		endereco->tarefa = RemoveTarefa(endereco->tarefa, endereco->tarefa->type, endereco->tarefa->x, endereco->tarefa->y);

		//Gerar dois bonecos no fim do mapa, se for uma casa
		if (house != 37){
			Character boneco = InsertCharacter(bonequinhos, woman1, WorldToPixel(MAPWIDTH - 1, 0), WorldToPixel(4, 1), 2, 1, y, x);
			boneco->path = FindPath(PixelToWorld(boneco->x, 0), PixelToWorld(boneco->y, 1), XHeadQuarters() + 1, YHeadQuarters() + 1);
			bonequinhos = boneco;

			boneco = InsertCharacter(bonequinhos, men1, WorldToPixel(MAPWIDTH - 1, 0), WorldToPixel(10, 1), 2, 1, y, x);
			boneco->path = FindPath(PixelToWorld(boneco->x, 0), PixelToWorld(boneco->y, 1), XHeadQuarters() + 1, YHeadQuarters() + 1);
			bonequinhos = boneco;
		}

		//Se for farmhouse, colocar os terrenos de plantação
		if (house == 37){
			mapDef[y + 1][x][0] = 47;
			mapDef[y + 1][x][1] = 0;
			mapDef[y + 1][x][2] = 0;

			mapDef[y + 1][x + 1][0] = 47;
			mapDef[y + 1][x + 1][1] = 0;
			mapDef[y + 1][x + 1][2] = 0;

			mapDef[y + 2][x][0] = 47;
			mapDef[y + 2][x][1] = 0;
			mapDef[y + 2][x][2] = 0;

			mapDef[y + 2][x + 1][0] = 47;
			mapDef[y + 2][x + 1][1] = 0;
			mapDef[y + 2][x + 1][2] = 0;
		}

		VerificarEnergia(endereco);
		

	}
	else{
		//printf("Tempo de execucao: %d\n", endereco->tarefa->tempoExecucao);

		if (!endereco->tarefaIniciada){

			if (house == 37){
				//Farmhouse
				edificios = InsertBuilding(edificios, endereco->tarefa->x, endereco->tarefa->y, 37);
				edificios = InsertBuilding(edificios, endereco->tarefa->x + 1, endereco->tarefa->y, 38);
			}
			else{
				//Casas
				edificios = InsertBuilding(edificios, endereco->tarefa->x, endereco->tarefa->y, house);
			}
			
			endereco->tarefaIniciada = true;
		}

		char result[500];
		sprintf(result, "%s%d%s", "Building (", (endereco->tarefa->tempoExecucao * 100 / endereco->tarefa->tempo), "%)");
		strcpy(endereco->action, result);

		endereco->tarefa->tempoExecucao++;

	}
}

//Atualizar os bonequinhos
void UpdateCharacters(Character endereco){

	bonecoHovered = false;

	if (endereco != NULL)
	{
		if (endereco->path != NULL){
			//Esta personagem tem caminho a percorrer
			if (strcmp(endereco->action, "Idle") == 0){
				strcpy(endereco->action, "Walking");
			}
			

			//Atualizar a animação
			endereco->animationTimer++;
			if (endereco->animationTimer > 15){
				if (endereco->animationFrame < 3){
					endereco->animationFrame++;
				}
				else{
					endereco->animationFrame = 0;
				}
				endereco->animationTimer = 0;
			}

			//Calcular a direção em que deve andar
			path = endereco->path;
			//printf("\n%f %f : (%d)%f, (%d)%f", endereco->x, endereco->y, path->x, WorldToPixel(path->x, 0), path->y, WorldToPixel(path->y, 1));
			if (!AlmostEqualRelative(endereco->x, WorldToPixel(path->x, 0)) ||
				!AlmostEqualRelative(endereco->y, WorldToPixel(path->y, 1))){
				if (WorldToPixel(path->y, 1) > endereco->y){
					//Baixo
					endereco->direcao = 0;
				}
				if (WorldToPixel(path->x, 0) < endereco->x){
					//Esquerda
					endereco->direcao = 1;
				}
				if (WorldToPixel(path->x, 0) > endereco->x){
					//Direita
					endereco->direcao = 2;
				}
				if (WorldToPixel(path->y, 1) < endereco->y){
					//Cima
					endereco->direcao = 3;
				}

				switch (endereco->direcao)
				{
				case 0:
					//Baixo
					endereco->y += VELOCIDADE_BONECOS;
					break;
				case 1:
					//Esquerda
					endereco->x -= VELOCIDADE_BONECOS;
					break;
				case 2:
					//Direita
					endereco->x += VELOCIDADE_BONECOS;
					break;
				case 3:
					//Cima
					endereco->y -= VELOCIDADE_BONECOS;
					break;
				default:
					break;
				}

			}
			else{
				//Chegámos a este path, eliminá-lo da lista
				endereco->x = WorldToPixel(path->x, 0);
				endereco->y = WorldToPixel(path->y, 1);
				endereco->path = RemoveNode(endereco->path, path->x, path->y);
			}

		}
		else{
			//Chegámos ao destino, atualizar tarefas
			if (strcmp(endereco->action, "Walking") == 0){
				strcpy(endereco->action, "Idle");
			}
			if (endereco->tarefa != NULL){
				switch (endereco->tarefa->type)
				{
				case 0:
					//IR PARA CASA

					if (bonequinhos != NULL){
						bonequinhos = RemoveCharacter(bonequinhos, endereco);
					}
					
				    Building edificio = FindBuilding(edificios, endereco->casaX, endereco->casaY);
					Character colono = edificio->colonists;
					endereco->next = colono;
					edificio->colonists = endereco;

					//Retirar comida
					comida -= 10;

					//Remover o boneco selecionado
					if (bonecoSelecionado != NULL){
						if (endereco->x == bonecoSelecionado->x && endereco->y == bonecoSelecionado->y){
							bonecoSelecionado = NULL;
						}
					}

					endereco = bonequinhos;

					break;
				case 1:
					//Apanhar madeira

					//Verificar se o recurso ainda existe
					if (FindForest(florestas, endereco->tarefa->x, endereco->tarefa->y)->phase == 0)
					{
		
						if (endereco->tarefa->tempoExecucao > endereco->tarefa->tempo){
							//Acabamos de apanhar madeira!
							endereco->energia -= endereco->tarefa->energianecessaria;

							strcpy(endereco->action, "Walking to unload wood");

							//Colocar madeira no boneco e retirá-la da floresta
							endereco->madeira += 5;
							Forest forest = FindForest(florestas, endereco->tarefa->x, endereco->tarefa->y);
							forest->quantity -= 5;

							//Mandar o boneco para o headquarters
							endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters(), YHeadQuarters() + 1);

							int enderecoTarefaX, enderecoTarefaY;
							enderecoTarefaX = endereco->tarefa->x;
							enderecoTarefaY = endereco->tarefa->y;

							//Remover a tarefa atual
							endereco->tarefa = RemoveTarefa(endereco->tarefa, endereco->tarefa->type, endereco->tarefa->x, endereco->tarefa->y);

							//Inserir a tarefa de descarregar madeira, guardando o x, y em que estavamos a apanhar
							//Vamos descarregar madeira!
							endereco->tarefa = InsertTarefa(endereco->tarefa, 2, enderecoTarefaX, enderecoTarefaY);

							printf("Inserida tarefa para descarregar madeira\n");
							printf("x: %d\n", enderecoTarefaX);
							printf("y: %d\n", enderecoTarefaY);
							printf("\n\n");
							

						}
						else{
							//printf("Tempo de execucao: %d\n", endereco->tarefa->tempoExecucao);

							char result[500];
							sprintf(result, "%s%d%s", "Gathering wood (", (endereco->tarefa->tempoExecucao * 100 / endereco->tarefa->tempo), "%)");
							strcpy(endereco->action, result);
							

							endereco->tarefa->tempoExecucao++;
							
						}
					}
					else{
						//A madeira que estavamos a cortar acabou!
						endereco->tarefa = RemoveTarefa(endereco->tarefa, endereco->tarefa->type, endereco->tarefa->x, endereco->tarefa->y);
						setTextoErro("Resource is depleted!");
						strcpy(endereco->action, "Idle");
						endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters() + 1, YHeadQuarters() + 1);
					}				
					break;
					
				case 2:
					//Descarregar madeira
					if ((PixelToWorld(endereco->x, 0) == XHeadQuarters() || PixelToWorld(endereco->x, 0) == XHeadQuarters() + 1) && PixelToWorld(endereco->y, 1) == YHeadQuarters() + 1){
						if (endereco->tarefa->tempoExecucao > endereco->tarefa->tempo){
							//Acabamos de descarregar madeira!
							//Decrementamos a energia do colono!
							endereco->energia -= endereco->tarefa->energianecessaria;

							//Incrementar a quantidade de madeira
							madeira += endereco->madeira;
							//Retirar a madeira que o boneco carregava
							endereco->madeira = 0;

							int enderecoX, enderecoY, enderecoTarefaX, enderecoTarefaY;
							enderecoX = endereco->x;
							enderecoY = endereco->y;
							enderecoTarefaX = endereco->tarefa->x;
							enderecoTarefaY = endereco->tarefa->y;
							//Remover a tarefa atual
							endereco->tarefa = RemoveTarefa(endereco->tarefa, endereco->tarefa->type, endereco->tarefa->x, endereco->tarefa->y);
								
							if (endereco->energia<10)
							{
								endereco->tarefa = InsertTarefa(endereco->tarefa, 0, enderecoTarefaX, enderecoTarefaY);

							}
							else if (((!WarehouseBuilt() && madeira < 500)
								|| (WarehouseBuilt() && madeira < 2000))){

								//Verificar se o recurso ainda existe
								if (FindForest(florestas, enderecoTarefaX, enderecoTarefaY)->phase == 0)
								{

									if (FazerBonecoAndarVizinho(endereco, enderecoTarefaX, enderecoTarefaY)){
										endereco->tarefa = InsertTarefa(endereco->tarefa, 1, enderecoTarefaX, enderecoTarefaY);

										printf("Inserida tarefa para apanhar madeira\n");
										printf("x: %d\n", enderecoTarefaX);
										printf("y: %d\n", enderecoTarefaY);
										printf("\n\n");

										strcpy(endereco->action, "Walking to gather wood");
									}
									else{
										setTextoErro("Can't reach resource!");
									}

								}
								else{
									//A madeira que estavamos a cortar acabou!
									setTextoErro("Resource is depleted!");
									strcpy(endereco->action, "Idle");
									endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters() + 1, YHeadQuarters() + 1);
								}

							}
							else{
								setTextoErro("Can't store/gather any more wood!");
								strcpy(endereco->action, "Idle");
								endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters() + 1, YHeadQuarters() + 1);
							}

							VerificarEnergia(endereco);

						}
						else{
							//printf("Tempo de execucao: %d\n", endereco->tarefa->tempoExecucao);
							char result[500];
							sprintf(result, "%s%d%s", "Unloading wood (", (endereco->tarefa->tempoExecucao * 100 / endereco->tarefa->tempo), "%)");
							strcpy(endereco->action, result);
							
							endereco->tarefa->tempoExecucao++;
						}
					}
					
					break;
				case 3:
					if (endereco->tarefa->tempoExecucao > endereco->tarefa->tempo){
						//Acabamos de apanhar pedra!
						endereco->energia -= endereco->tarefa->energianecessaria;

						strcpy(endereco->action, "Walking to unload stone");
						

						endereco->pedra += 5;

						//Mandar o boneco para o headquarters
						endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters(), YHeadQuarters() + 1);

						//Remover a tarefa atual
						endereco->tarefa = RemoveTarefa(endereco->tarefa, endereco->tarefa->type, endereco->tarefa->x, endereco->tarefa->y);

						//Inserir a tarefa de descarregar madeira, guardando o x, y em que estavamos a apanhar
						//Vamos descarregar pedra!
						endereco->tarefa = InsertTarefa(endereco->tarefa, 4, PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1));

					}
					else{
						//printf("Tempo de execucao: %d\n", endereco->tarefa->tempoExecucao);

						char result[500];
						sprintf(result, "%s%d%s", "Gathering stone (", (endereco->tarefa->tempoExecucao * 100 / endereco->tarefa->tempo), "%)");
						strcpy(endereco->action, result);
						

						endereco->tarefa->tempoExecucao++;
						
					}
					break;
				case 4:
					//Descarregar pedra
					if ((PixelToWorld(endereco->x, 0) == XHeadQuarters() || PixelToWorld(endereco->x, 0) == XHeadQuarters() + 1) && PixelToWorld(endereco->y, 1) == YHeadQuarters() + 1){
						if (endereco->tarefa->tempoExecucao > endereco->tarefa->tempo){
							//Acabamos de descarregar pedra!
							endereco->energia -= endereco->tarefa->energianecessaria;
							

							//Incrementar a quantidade de pedra
							pedra += endereco->pedra;
							//Retirar a madeira que o boneco carregava
							endereco->pedra = 0;

							int enderecoX, enderecoY, enderecoTarefaX, enderecoTarefaY;
							enderecoX = endereco->x;
							enderecoY = endereco->y;
							enderecoTarefaX = endereco->tarefa->x;
							enderecoTarefaY = endereco->tarefa->y;
							//Remover a tarefa atual
							endereco->tarefa = RemoveTarefa(endereco->tarefa, endereco->tarefa->type, endereco->tarefa->x, endereco->tarefa->y);

							if (((!WarehouseBuilt() && pedra < 500)
								|| (WarehouseBuilt() && pedra < 2000))){
								//Mandar o boneco para o local onde estava a apanhar madeira
								endereco->path = FindPath(PixelToWorld(enderecoX, 0), PixelToWorld(enderecoY, 1), enderecoTarefaX, enderecoTarefaY);

								//Inserir a tarefa de apanhar madeira, guardando o x, y em que estavamos a apanhar
								strcpy(endereco->action, "Walking to gather stone");
								endereco->tarefa = InsertTarefa(endereco->tarefa, 3, enderecoTarefaX, enderecoTarefaY);
							}
							else{
								setTextoErro("Can't store/gather any more stone!");
								strcpy(endereco->action, "Idle");
								endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters() + 1, YHeadQuarters() + 1);
							}

							VerificarEnergia(endereco);

						}
						else{
							//printf("Tempo de execucao: %d\n", endereco->tarefa->tempoExecucao);
							char result[500];
							sprintf(result, "%s%d%s", "Unloading stone (", (endereco->tarefa->tempoExecucao * 100 / endereco->tarefa->tempo), "%)");
							strcpy(endereco->action, result);
							
							endereco->tarefa->tempoExecucao++;
						}
					}

					break;
				case 5:
					if (endereco->tarefa->tempoExecucao > endereco->tarefa->tempo){
						//Acabamos de apanhar comida!
						endereco->energia -= endereco->tarefa->energianecessaria;

						strcpy(endereco->action, "Walking to unload fish");
						

						endereco->comida += 5;

						//Mandar o boneco para o headquarters
						endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters(), YHeadQuarters() + 1);

						//Remover a tarefa atual
						endereco->tarefa = RemoveTarefa(endereco->tarefa, endereco->tarefa->type, endereco->tarefa->x, endereco->tarefa->y);

						//Inserir a tarefa de descarregar comida, guardando o x, y em que estavamos a apanhar
						endereco->tarefa = InsertTarefa(endereco->tarefa, 6, PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1));

					}
					else{
						//printf("Tempo de execucao: %d\n", endereco->tarefa->tempoExecucao);

						char result[500];
						sprintf(result, "%s%d%s", "Fishing (", (endereco->tarefa->tempoExecucao * 100 / endereco->tarefa->tempo), "%)");
						strcpy(endereco->action, result);
						

						endereco->tarefa->tempoExecucao++;

					}
					break;
				case 6:
					//Descarregar comida
					if ((PixelToWorld(endereco->x, 0) == XHeadQuarters() || PixelToWorld(endereco->x, 0) == XHeadQuarters() + 1) && PixelToWorld(endereco->y, 1) == YHeadQuarters() + 1){
						if (endereco->tarefa->tempoExecucao > endereco->tarefa->tempo){
							//Acabamos de descarregar comida!
							endereco->energia -= endereco->tarefa->energianecessaria;

							//Incrementar a quantidade de comida
							comida += endereco->comida;
							//Retirar a madeira que o boneco carregava
							endereco->comida = 0;

							int enderecoX, enderecoY, enderecoTarefaX, enderecoTarefaY;
							enderecoX = endereco->x;
							enderecoY = endereco->y;
							enderecoTarefaX = endereco->tarefa->x;
							enderecoTarefaY = endereco->tarefa->y;
							//Remover a tarefa atual
							endereco->tarefa = RemoveTarefa(endereco->tarefa, endereco->tarefa->type, endereco->tarefa->x, endereco->tarefa->y);

							if (((!WarehouseBuilt() && comida < 500)
								|| (WarehouseBuilt() && comida < 2000))){
								//Mandar o boneco para o local onde estava a apanhar comida
								endereco->path = FindPath(PixelToWorld(enderecoX, 0), PixelToWorld(enderecoY, 1), enderecoTarefaX, enderecoTarefaY);

								//Inserir a tarefa de apanhar peixe, guardando o x, y em que estavamos a apanhar
								strcpy(endereco->action, "Walking to fish");
								endereco->tarefa = InsertTarefa(endereco->tarefa, 5, enderecoTarefaX, enderecoTarefaY);
							}
							else{
								setTextoErro("Can't store/gather any more food!");
								strcpy(endereco->action, "Idle");
								endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters() + 1, YHeadQuarters() + 1);
							}

							VerificarEnergia(endereco);

						}
						else{
							//printf("Tempo de execucao: %d\n", endereco->tarefa->tempoExecucao);
							char result[500];
							sprintf(result, "%s%d%s", "Unloading fish (", (endereco->tarefa->tempoExecucao * 100 / endereco->tarefa->tempo), "%)");
							strcpy(endereco->action, result);
							endereco->tarefa->tempoExecucao++;
						}
					}

					break;
				case 7:
					if (endereco->tarefa->tempoExecucao > endereco->tarefa->tempo){
						//Acabamos de apanhar vegetais!
						endereco->energia -= endereco->tarefa->energianecessaria;

						strcpy(endereco->action, "Walking to unload vegetables");

						endereco->comida += 35;

						//Remover a plantaçao
						mapDef[endereco->tarefa->y][endereco->tarefa->x][0] = 47;
						quintas = RemoveFarm(quintas, endereco->tarefa->y, endereco->tarefa->x);

						//Mandar o boneco para o headquarters
						endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters(), YHeadQuarters() + 1);

						//Remover a tarefa atual
						endereco->tarefa = RemoveTarefa(endereco->tarefa, endereco->tarefa->type, endereco->tarefa->x, endereco->tarefa->y);

						//Inserir a tarefa de descarregar vegetais, guardando o x, y em que estavamos a apanhar
						//Vamos descarregar comida!
						endereco->tarefa = InsertTarefa(endereco->tarefa, 8, PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1));

					}
					else{
						//printf("Tempo de execucao: %d\n", endereco->tarefa->tempoExecucao);

						char result[500];
						sprintf(result, "%s%d%s", "Gathering vegetables (", (endereco->tarefa->tempoExecucao * 100 / endereco->tarefa->tempo), "%)");
						strcpy(endereco->action, result);


						endereco->tarefa->tempoExecucao++;

					}
					break;
				case 8:
					//Descarregar vegetais
					if ((PixelToWorld(endereco->x, 0) == XHeadQuarters() || PixelToWorld(endereco->x, 0) == XHeadQuarters() + 1) && PixelToWorld(endereco->y, 1) == YHeadQuarters() + 1){
						if (endereco->tarefa->tempoExecucao > endereco->tarefa->tempo){
							//Acabamos de descarregar vegetais!
							endereco->energia -= endereco->tarefa->energianecessaria;

							//Incrementar a quantidade de comida
							comida += endereco->comida;
							//Retirar os vegetais que o boneco carregava
							endereco->comida = 0;

							int enderecoX, enderecoY, enderecoTarefaX, enderecoTarefaY;
							enderecoX = endereco->x;
							enderecoY = endereco->y;
							enderecoTarefaX = endereco->tarefa->x;
							enderecoTarefaY = endereco->tarefa->y;
							//Remover a tarefa atual
							endereco->tarefa = RemoveTarefa(endereco->tarefa, endereco->tarefa->type, endereco->tarefa->x, endereco->tarefa->y);

							strcpy(endereco->action, "Idle");
							endereco->path = FindPath(PixelToWorld(endereco->x, 0), PixelToWorld(endereco->y, 1), XHeadQuarters() + 1, YHeadQuarters() + 1);
							
							VerificarEnergia(endereco);

						}
						else{
							//printf("Tempo de execucao: %d\n", endereco->tarefa->tempoExecucao);
							char result[500];
							sprintf(result, "%s%d%s", "Unloading vegetables (", (endereco->tarefa->tempoExecucao * 100 / endereco->tarefa->tempo), "%)");
							strcpy(endereco->action, result);
							endereco->tarefa->tempoExecucao++;
						}
					}
					break;


				case 10:
					//Build House 1
					BuildHouseTask(endereco, 39);
					break;
				case 11:
					//Build House 1
					BuildHouseTask(endereco, 40);
					break;
				case 12:
					//Build House 1
					BuildHouseTask(endereco, 45);
					break;
				case 13:
					//Build House 1
					BuildHouseTask(endereco, 46);
					break;
				case 14:
					//Build Farmhouse
					BuildHouseTask(endereco, 37);
					break;
				case 15:
					//Plantar Milho
					PlantFarmTask(endereco, 32);
					break;
				case 16:
					//Plantar Morango
					PlantFarmTask(endereco, 29);
					break;
				case 17:
					//Plantar Pimento
					PlantFarmTask(endereco, 26);
					break;
				case 18:
					//Plantar Batata
					PlantFarmTask(endereco, 23);
					break;
				case 19:
					//Plantar Nabo
					PlantFarmTask(endereco, 20);
					break;
				case 20:
					//Plantar Cenoura
					PlantFarmTask(endereco, 17);
					break;
				case 21:
					//Plantar Beterraba
					PlantFarmTask(endereco, 14);
					break;
				default:
					break;
				}
				
			}
		}

		if (endereco != NULL){

			UpdateCharacters(endereco->next);
		}
		
	}
}

//Desenhar os bonequinhos
void DrawCharacters(Character endereco){
	if (endereco != NULL){
		al_draw_bitmap_region(endereco->spriteSheet,
			endereco->animationFrame * 16, endereco->direcao * 24, 16, 24, endereco->x + offsetX, endereco->y + offsetY, 0);
		if (endereco->next != NULL){
			DrawCharacters(endereco->next);
		}
	}
}

//Destroi os objetos criados
void ShutDown(){

	bonecoSelecionado = NULL;
	edificioSelecionado = NULL;

	if (display){
		al_destroy_display(display);
	}
	if (titulos){
		al_destroy_font(titulos);
	}
	if (textos){
		al_destroy_font(textos);
	}
	if (timer){
		al_destroy_timer(timer);
	}
	if (event_queue){
		al_destroy_event_queue(event_queue);
	}
}

void DrawHoveredTile(){
	x = (round(mouseState.x / TILEWIDTH)  * TILEWIDTH);
	y = (round(mouseState.y / TILEHEIGHT)  * TILEHEIGHT);

	if (opcaoAtiva != NULL && opcaoAtiva->tecla == 'f'){
		//Farmhouse
		al_draw_rectangle(x, y, x + TILEWIDTH * 3, y + TILEHEIGHT,
			GREEN, 2);
		al_draw_rectangle(x - TILEWIDTH, y + TILEHEIGHT, x + TILEWIDTH * 3, y + TILEHEIGHT * 3,
			GREEN, 2);
	}
	else{
		//Normal
		al_draw_rectangle(x, y, x + TILEWIDTH, y + TILEHEIGHT,
			GREEN, 2);
	}

}

//Verifica se existe colisão entre duas bounding boxes
int bounding_box_collision(float b1_x, float b1_y, int b1_w, int b1_h, float b2_x, float b2_y, int b2_w, int b2_h)
{
	if ((b1_x > b2_x + b2_w - 1) || // is b1 on the right side of b2?
		(b1_y > b2_y + b2_h - 1) || // is b1 under b2?
		(b2_x > b1_x + b1_w - 1) || // is b2 on the right side of b1?
		(b2_y > b1_y + b1_h - 1))   // is b2 under b1?
	{
		// no collision
		return 0;
	}

	// collision
	return 1;
}

//Desenha o caminho que uma personagem tem para percorrer
void DrawCharacterPath(Character boneco){
	if (boneco != NULL){
		Node auxPath = boneco->path;
		while (auxPath != NULL){
			al_draw_rectangle(auxPath->x * TILEWIDTH + offsetX, auxPath->y * TILEHEIGHT + offsetY,
				auxPath->x * TILEWIDTH + TILEWIDTH + offsetX, auxPath->y * TILEHEIGHT + TILEHEIGHT + offsetY,
				GREEN, 2);
			auxPath = auxPath->next;
		}
	}
}

void DrawCharacterBoundingBox(Character endereco){
	//Posição do rato
	x = mouseState.x;
	y = mouseState.y;

	while (endereco != NULL){
		if (bounding_box_collision(x, y, 10, 20, endereco->x + offsetX, endereco->y + offsetY, 16, 24)){
			//O rato está por cima de um bonequinho!
			bonecoHovered = true;
			al_draw_rectangle(endereco->x + offsetX, endereco->y + offsetY, endereco->x + 16 + offsetX, endereco->y + 24 + offsetY,
				GREEN, 2);
			break;
		}
		endereco = endereco->next;
	}
	
	//Desenhar o caminho que o boneco está a percorrer, se existir
	DrawCharacterPath(endereco);
}

void DrawBoundingBoxBuilding(Building edificios){

	float edificioX, edificioY;

	//Atenção que parece estar ao contrário mas tem que ser assim, por causa da rotação do mapa
	edificioX = (round(WorldToPixel(edificios->y, 1) / TILEWIDTH)  * TILEWIDTH);
	edificioY = (round(WorldToPixel(edificios->x, 0) / TILEHEIGHT)  * TILEHEIGHT);

	//Casas duplas
	if (edificios->type == 35 || edificios->type == 37){
		//Hedquarters e Farmhouse, esquerdo
		al_draw_rectangle(edificioX + offsetX, edificioY + offsetY, edificioX + offsetX + TILEWIDTH * 2, edificioY + offsetY + TILEHEIGHT,
			GREEN, 2);
	}
	else if (edificios->type == 36 || edificios->type == 38){
		//Headquartes e Farmhouse, direito
		al_draw_rectangle(edificioX + offsetX - TILEWIDTH, edificioY + offsetY, edificioX + offsetX + TILEWIDTH, edificioY + offsetY + TILEHEIGHT,
			GREEN, 2);
	}


	//Casas quadruplas
	else if (edificios->type == 41){
		//Warehouse, esquerdo cima
		al_draw_rectangle(edificioX + offsetX, edificioY + offsetY, edificioX + offsetX + TILEWIDTH * 2, edificioY + offsetY + TILEHEIGHT * 2,
			GREEN, 2);
	}
	else if (edificios->type == 42){
		//Warehouse, direito cima
		al_draw_rectangle(edificioX + offsetX - TILEWIDTH, edificioY + offsetY, edificioX + offsetX + TILEWIDTH, edificioY + offsetY + TILEHEIGHT * 2,
			GREEN, 2);
	}
	else if (edificios->type == 43){
		//Warehouse, esquerdo baixo
		al_draw_rectangle(edificioX + offsetX, edificioY + offsetY - TILEHEIGHT, edificioX + offsetX + TILEWIDTH * 2, edificioY + offsetY + TILEHEIGHT,
			GREEN, 2);
	}
	else if (edificios->type == 44){
		//Warehouse, direito baixo
		al_draw_rectangle(edificioX + offsetX - TILEWIDTH, edificioY + offsetY - TILEHEIGHT, edificioX + offsetX + TILEWIDTH, edificioY + offsetY + TILEHEIGHT,
			GREEN, 2);
	}

	else{
		//Casas simples
		al_draw_rectangle(edificioX + offsetX, edificioY + offsetY, edificioX + offsetX + TILEWIDTH, edificioY + offsetY + TILEHEIGHT,
			GREEN, 2);
	}
}

void DrawBuildingHover(Building edificios){
	//Posição do rato
	x = mouseState.x;
	y = mouseState.y;

	float edificioX, edificioY;
	
	while (edificios != NULL){

		//Atenção que parece estar ao contrário mas tem que ser assim, por causa da rotação do mapa
		edificioX = (round(WorldToPixel(edificios->y, 1) / TILEWIDTH)  * TILEWIDTH);
		edificioY = (round(WorldToPixel(edificios->x, 0) / TILEHEIGHT)  * TILEHEIGHT);

		if (bounding_box_collision(x, y, 10, 20, edificioX + offsetX, edificioY + offsetY, TILEWIDTH, TILEHEIGHT)){

			//O rato está por cima de um edificio!
			DrawBoundingBoxBuilding(edificios);

		}
		edificios = edificios->next;
	}
}

void BuildFarmClick(Character bonecoSelecionado, int xi, int yi, int tarefa){
	//Plantar cenas

	//Verificar se podemos plantar neste espaço
	if (mapDef[yi][xi][0] == 47){
		//Terra vazia, podemos plantar
		//Encontrar um vizinho em que se possa andar
		bonecoSelecionado->path = NULL;
		if (FazerBonecoAndarVizinho(bonecoSelecionado, xi, yi)){
			opcaoAtiva = NULL;
			bonecoSelecionado->tarefa = NULL;
			bonecoSelecionado->tarefa = InsertTarefa(bonecoSelecionado->tarefa, tarefa, xi, yi);

			printf("Inserida tarefa para plantar\n");
			printf("x: %d\n", xi);
			printf("y: %d\n", yi);
			printf("\n\n");

			strcpy(bonecoSelecionado->action, "Walking to plant a farm");
			bonecoSelecionado = NULL;
		}
		else{
			setTextoErro("Can't reach farm!");
		}
	}
	else{
		setTextoErro("Can't plant in that space!");
	}
}

void BuildHouseSpaceClick(Character bonecoSelecionado, int xi, int yi, int tarefa){
	//Construir uma casa

	//Verificar se se pode construir neste espaço
	if (mapDef[yi][xi][2] == 1 && FindBuilding(edificios, yi, xi) == NULL){
		bonecoSelecionado->path = NULL;
		//Encontrar um vizinho em que se possa andar
		if (FazerBonecoAndarVizinho(bonecoSelecionado, xi, yi)){
			//Limpar tarefas que tenha a criar uma nova TODO: free???
			bonecoSelecionado->tarefa = NULL;
			bonecoSelecionado->tarefa = InsertTarefa(bonecoSelecionado->tarefa, tarefa, xi, yi);

			mapDef[yi][xi][2] = 0;
			mapDef[yi + 1][xi][2] = 0;

			printf("Inserida tarefa para construir casa\n");
			printf("x: %d\n", xi);
			printf("y: %d\n", yi);
			printf("\n\n");

			strcpy(bonecoSelecionado->action, "Walking to build House");
			bonecoSelecionado = NULL;
			opcaoAtiva = NULL;
		}
		else{
			setTextoErro("Can't reach space!");
		}
	}
	else{
		setTextoErro("Can't build in that space!");
	}
	
}

void ProcessMouseClicks(Character bonequinhos){
	x = mouseState.x;
	y = mouseState.y;
	float edificioX, edificioY;
	Building aux = edificios;
	bool continuar = true;

	//Verificar cliques para selecionar bonecos
	while (bonequinhos != NULL){
		if (bounding_box_collision(x, y, 10, 20, bonequinhos->x + offsetX, bonequinhos->y + offsetY, 16, 24)){
			//O rato está por cima de um bonequinho!
			bonecoSelecionado = bonequinhos;

			//Adicionar opção de construir
			opcoes = NULL;
			opcaoAtiva = NULL;
			if (FarmhouseBuilt()){
				opcoes = InsertOption(opcoes, 'p', "Plant", 0, 0);
			}
			opcoes = InsertOption(opcoes, 'b', "Build", 0, 0);

			edificioSelecionado = NULL;
			continuar = false;
			break;
		}
		bonequinhos = bonequinhos->next;
	}

	//Verificar cliques para mandar bonecos andar
	if (continuar && (bonecoSelecionado != NULL && 
		(bonecoSelecionado->tarefa == NULL || bonecoSelecionado->tarefa != NULL 
		&& bonecoSelecionado->tarefa->type < 10 && bonecoSelecionado->tarefa->type != 2 && bonecoSelecionado->tarefa->type != 4 && bonecoSelecionado->tarefa->type != 6 && bonecoSelecionado->tarefa->type != 8))
		&& bonecoSelecionado != NULL && !bounding_box_collision(x, y, 10, 20, bonecoSelecionado->x + offsetX, bonecoSelecionado->y + offsetY, 16, 24)){
		
		//Verificar se está a ser mandado para casa
		while (aux != NULL){
			//Atenção que parece estar ao contrário mas tem que ser assim, por causa da rotação do mapa
			edificioX = (round(WorldToPixel(aux->y, 1) / TILEWIDTH)  * TILEWIDTH);
			edificioY = (round(WorldToPixel(aux->x, 0) / TILEHEIGHT)  * TILEHEIGHT);

			if (bounding_box_collision(x, y, 10, 20, edificioX + offsetX, edificioY + offsetY, TILEWIDTH, TILEHEIGHT)){
				//Clique em cima de um edifício
				if (strcmp(aux->name, "House") == 0){
					//Clique em cima de uma casa, mandar boneco para casa
					bonecoSelecionado->path = FindPath(PixelToWorld(bonecoSelecionado->x, 0), PixelToWorld(bonecoSelecionado->y, 1), PixelToWorld(x - offsetX, 0), PixelToWorld(y - offsetY, 1) + 1);
					//Tarefa - Ir até casa
					Tarefa tarefa = (Tarefa)malloc(sizeof(struct tarefa));
					tarefa->type = 0;
					tarefa->x = PixelToWorld(x - offsetX, 0);
					tarefa->y = PixelToWorld(y - offsetY, 1) + 1;
					bonecoSelecionado->tarefa = tarefa;
					continuar = false;
					bonecoSelecionado = NULL;
					opcaoAtiva = NULL;

					break;
				}

				if (strcmp(aux->name, "Headquarters") == 0){

					//Se tras coisas, descarregar
					if (bonecoSelecionado->madeira > 0 || bonecoSelecionado->pedra > 0 || bonecoSelecionado->comida > 0){
						//Clique em cima dos headquarters
						strcpy(bonecoSelecionado->action, "Walking to unload wood");
						bonecoSelecionado->path = FindPath(PixelToWorld(bonecoSelecionado->x, 0), PixelToWorld(bonecoSelecionado->y, 1), PixelToWorld(x - offsetX, 0), PixelToWorld(y - offsetY, 1) + 1);

					}

					continuar = false;
					bonecoSelecionado = NULL;
					
					break;
				}
			}

			aux = aux->next;
		}

		
	

		if (continuar){
			//Andar para uma localização no mapa

			if (opcaoAtiva != NULL){

				int xi = PixelToWorld((x / TILEWIDTH) * TILEWIDTH - offsetX, 0);
				int yi = PixelToWorld((y / TILEHEIGHT) * TILEHEIGHT - offsetY, 1);

				switch (opcaoAtiva->tecla)
				{
				case '1':
					madeira -= opcaoAtiva->madeira;
					pedra -= opcaoAtiva->pedra;
					BuildHouseSpaceClick(bonecoSelecionado, xi, yi, 10);
					
					break;
				case '2':
					madeira -= opcaoAtiva->madeira;
					pedra -= opcaoAtiva->pedra;
					BuildHouseSpaceClick(bonecoSelecionado, xi, yi, 11);
					
					break;
				case '3':
					madeira -= opcaoAtiva->madeira;
					pedra -= opcaoAtiva->pedra;
					BuildHouseSpaceClick(bonecoSelecionado, xi, yi, 12);
					
					break;
				case '4':
					madeira -= opcaoAtiva->madeira;
					pedra -= opcaoAtiva->pedra;
					BuildHouseSpaceClick(bonecoSelecionado, xi, yi, 13);
					
					break;

				case 'l':
					BuildFarmClick(bonecoSelecionado, xi, yi, 15);
					break;
				case 'o':
					BuildFarmClick(bonecoSelecionado, xi, yi, 16);
					break;
				case 'i':
					BuildFarmClick(bonecoSelecionado, xi, yi, 17);
					break;
				case 'a':
					BuildFarmClick(bonecoSelecionado, xi, yi, 18);
					break;
				case 'n':
					BuildFarmClick(bonecoSelecionado, xi, yi, 19);
					break;
				case 'c':
					BuildFarmClick(bonecoSelecionado, xi, yi, 20);
					break;
				case 't':
					BuildFarmClick(bonecoSelecionado, xi, yi, 21);
					break;

				case 'f':
					//Farmhouse
					if (
						//Edificio da farmhouse
						mapDef[yi][xi][2] == 1 && FindBuilding(edificios, yi, xi) == NULL
						&& mapDef[yi][xi + 1][2] == 1 && FindBuilding(edificios, yi, xi + 1) == NULL

						//Terreno superior esquerdo + margem esquerda
						&& mapDef[yi + 1][xi][2] == 1 && FindBuilding(edificios, yi + 1, xi) == NULL
						&& mapDef[yi + 1][xi - 1][2] == 1 && FindBuilding(edificios, yi + 1, xi) == NULL

						//Terreno superior direito + margem direita
						&& mapDef[yi + 1][xi + 1][2] == 1 && FindBuilding(edificios, yi + 1, xi + 1) == NULL
						&& mapDef[yi + 1][xi + 2][2] == 1 && FindBuilding(edificios, yi + 1, xi + 1) == NULL

						//Terreno inferior esquerdo + margem esquerda
						&& mapDef[yi + 2][xi][2] == 1 && FindBuilding(edificios, yi + 2, xi) == NULL
						&& mapDef[yi + 2][xi - 1][2] == 1 && FindBuilding(edificios, yi + 2, xi) == NULL

						//Terreno inferior direito + margem direita
						&& mapDef[yi + 2][xi + 1][2] == 1 && FindBuilding(edificios, yi + 2, xi + 1) == NULL
						&& mapDef[yi + 2][xi + 2][2] == 1 && FindBuilding(edificios, yi + 2, xi + 1) == NULL

						//Espaço para o trabalhador
						&& mapDef[yi][xi + 2][2] == 1 && FindBuilding(edificios, yi, xi + 2) == NULL
						){
						//Encontrar um vizinho em que se possa andar
						if (FazerBonecoAndarVizinho(bonecoSelecionado, xi + 1, yi)){
							//Limpar tarefas que tenha a criar uma nova TODO: free???
							bonecoSelecionado->tarefa = NULL;
							bonecoSelecionado->tarefa = InsertTarefa(bonecoSelecionado->tarefa, 14, xi, yi);

							mapDef[yi][xi][2] = 0;
							mapDef[yi][xi + 1][2] = 0;
							mapDef[yi + 1][xi][2] = 0;
							mapDef[yi + 1][xi - 1][2] = 0;
							mapDef[yi + 1][xi + 1][2] = 0;
							mapDef[yi + 1][xi + 2][2] = 0;
							mapDef[yi + 2][xi][2] = 0;
							mapDef[yi + 2][xi - 1][2] = 0;
							mapDef[yi + 2][xi + 1][2] = 0;
							mapDef[yi + 2][xi + 2][2] = 0;

							printf("Inserida tarefa para construir Farmhouse\n");
							printf("x: %d\n", xi);
							printf("y: %d\n", yi);
							printf("\n\n");

							strcpy(bonecoSelecionado->action, "Walking to build Farmhouse");
							bonecoSelecionado = NULL;
							opcaoAtiva = NULL;

							madeira -= 160;
							pedra -= 200;
						}
						else{
							setTextoErro("Can't reach space!");
						}
					}
					else{
						//Não pode ser construido aqui
						setTextoErro("Can't build in that space!");
					}
					break;
				default:
					break;
				}
				continuar = false;
			}
			else{
				//Não há nenhuma opção seleccionada, vamos simplemente andar para uma posição no mapa
				bonecoSelecionado->path = FindPath(PixelToWorld(bonecoSelecionado->x, 0), PixelToWorld(bonecoSelecionado->y, 1), PixelToWorld(x - offsetX, 0), PixelToWorld(y - offsetY, 1));
				strcpy(bonecoSelecionado->action, "Walking");
				bonecoSelecionado->tarefa = NULL;
				opcaoAtiva = NULL;
				if (bonecoSelecionado->path != NULL){
					bonecoSelecionado = NULL;
				}
			}
		}
		
	}



	//Verificar clique em cima de edificio
	while (continuar && aux != NULL){
		//Atenção que parece estar ao contrário mas tem que ser assim, por causa da rotação do mapa
		edificioX = (round(WorldToPixel(aux->y, 1) / TILEWIDTH)  * TILEWIDTH);
		edificioY = (round(WorldToPixel(aux->x, 0) / TILEHEIGHT)  * TILEHEIGHT);

		if (bounding_box_collision(x, y, 10, 20, edificioX + offsetX, edificioY + offsetY, TILEWIDTH, TILEHEIGHT)){
			//Clique em cima de um edifício!
			edificioSelecionado = aux;
			continuar = false;
			break;
		}
		else{
			edificioSelecionado = NULL;
		}

		aux = aux->next;
	}

	if (continuar){
		//Verificar se estamos a clicar numa quinta com vegetais maduros
		int xi = PixelToWorld((x / TILEWIDTH) * TILEWIDTH - offsetX, 0);
		int yi = PixelToWorld((y / TILEHEIGHT) * TILEHEIGHT - offsetY, 1);

		Farm farm = FindFarm(quintas, yi, xi);
		if (farm != NULL){
			//Encontrar um vizinho em que se possa andar
			if (FazerBonecoAndarVizinho(bonecoSelecionado, xi, yi)){
				//Limpar tarefas que tenha a criar uma nova TODO: free???
				bonecoSelecionado->tarefa = NULL;
				bonecoSelecionado->tarefa = InsertTarefa(bonecoSelecionado->tarefa, 7, xi, yi);

				printf("Inserida tarefa para apanhar vegetais\n");
				printf("x: %d\n", xi);
				printf("y: %d\n", yi);
				printf("\n\n");

				strcpy(bonecoSelecionado->action, "Walking to pick vegetables");
				continuar = false;
				bonecoSelecionado = NULL;
			}
			else{
				setTextoErro("Can't reach farm!");
			}
		}
	}

	//Verificar clique em cima de pedra, madeira ou comida
	if (continuar){
		int xi = PixelToWorld((x / TILEWIDTH) * TILEWIDTH - offsetX, 0);
		int yi = PixelToWorld((y / TILEHEIGHT) * TILEHEIGHT - offsetY, 1);
		
		if (bonecoSelecionado){
			if (bonecoSelecionado->madeira > 0
				|| bonecoSelecionado->pedra > 0
				|| bonecoSelecionado->comida > 0){
				//Este boneco já está a carregar com coisas, não pode apanhar mais antes de descarregar
				setTextoErro("Must unload cargo first!");
			}
			else{
				//Está livre para trabalhar
				if (mapDef[yi][xi][0] > 1 && mapDef[yi][xi][0] < 9){
					//Clique em madeira

					if ((!WarehouseBuilt() && madeira < 500)
						|| (WarehouseBuilt() && madeira < 2000)){
						
						//Encontrar um vizinho em que se possa andar
						if (FazerBonecoAndarVizinho(bonecoSelecionado, xi, yi)){
							//Limpar tarefas que tenha a criar uma nova TODO: free???
							bonecoSelecionado->tarefa = NULL;
							bonecoSelecionado->tarefa = InsertTarefa(bonecoSelecionado->tarefa, 1, xi, yi);

							printf("Inserida tarefa para apanhar madeira\n");
							printf("x: %d\n", xi);
							printf("y: %d\n", yi);
							printf("\n\n");

							strcpy(bonecoSelecionado->action, "Walking to gather wood");
							continuar = false;
							bonecoSelecionado = NULL;
						}
						else{
							setTextoErro("Can't reach resource!");
						}
						
					}
					else{
						setTextoErro("Can't store/gather any more wood!");
						strcpy(bonecoSelecionado->action, "Idle");

					}

				}
				if (mapDef[yi][xi][0] >= 9 && mapDef[yi][xi][0] < 12){
					//Clique em pedra
					if ((!WarehouseBuilt() && pedra < 500)
						|| (WarehouseBuilt() && pedra < 2000)){

						//Encontrar um vizinho em que se possa andar
						if (FazerBonecoAndarVizinho(bonecoSelecionado, xi, yi)){
							//Limpar tarefas que tenha a criar uma nova TODO: free???
							bonecoSelecionado->tarefa = NULL;
							bonecoSelecionado->tarefa = InsertTarefa(bonecoSelecionado->tarefa, 3, xi, yi);
							strcpy(bonecoSelecionado->action, "Walking to gather stone");
							continuar = false;
							bonecoSelecionado = NULL;
						}
						else{
							setTextoErro("Can't reach resource!");
						}

					}
					else{
						setTextoErro("Can't store/gather any more stone!");
						strcpy(bonecoSelecionado->action, "Idle");
					}

				}
				if (mapDef[yi][xi][0] ==0 /*&& mapDef[yi][xi][0] < 20*/){
					//Clique em agua
					if ((!WarehouseBuilt() && comida < 500)
						|| (WarehouseBuilt() && comida < 2000)){

						//Encontrar um vizinho em que se possa andar
						if (FazerBonecoAndarVizinho(bonecoSelecionado, xi, yi)){
							//Limpar tarefas que tenha a criar uma nova TODO: free???
							bonecoSelecionado->tarefa = NULL;
							bonecoSelecionado->tarefa = InsertTarefa(bonecoSelecionado->tarefa, 5, xi, yi);
							strcpy(bonecoSelecionado->action, "Walking to gather food");
							continuar = false;
							bonecoSelecionado = NULL;
						}
						else{
							setTextoErro("Can't reach resource!");
						}

					}
					else{
						setTextoErro("Can't store/gather any more food!");
						strcpy(bonecoSelecionado->action, "Idle");
					}

				}
					//bonecoSelecionado->tarefa = InsertTarefa(bonecoSelecionado->tarefa, 3, xi, yi, NULL);
				}
			}
		}
	}


//Desenha o boneco selecionado e a tile que está hovered
void DrawBonecoSelecionado(){
	if (bonecoSelecionado != NULL){

		//Fundo
		al_draw_filled_rounded_rectangle(fundoX, fundoY, DISPLAYWIDTH - 20, DISPLAYHEIGHT - 20,
			10, 10, GREY);

		//Nome do objeto
		al_draw_text(titulos,
			WHITE, fundoX + 10, fundoY + 10, 0,
			"Colonist");

		al_draw_text(textos,
			WHITE, fundoX + 10, fundoY + 45, 0,
			bonecoSelecionado->action);
		char result[500];
		sprintf(result, "%s%d%s", "Energy (", (int)bonecoSelecionado->energia, "%)");
		al_draw_text(textos, WHITE, fundoX + 700, fundoY + 45, 0, result);

		//Desenhar erros / avisos
		if (tempoTextoErro > 0){
			al_draw_text(textos,
				RED, DISPLAYWIDTH - 280, fundoY + 30, 0,
				textoErro);
			tempoTextoErro--;
		}

		al_draw_rectangle(bonecoSelecionado->x + offsetX, bonecoSelecionado->y + offsetY, bonecoSelecionado->x + 16 + offsetX, bonecoSelecionado->y + 24 + offsetY,
			RED, 2);


		int tipoTarefa = 0;
		if (bonecoSelecionado->tarefa != NULL){
			tipoTarefa = bonecoSelecionado->tarefa->type;
		}
		//Verificar se não está a construir / descarregar
		if (tipoTarefa == 0 || (tipoTarefa != 0 && tipoTarefa < 10)){
			//Desenhar opções
			Opcao listaOpcoes = opcoes;
			int offset = 100;
			int offsetEsquerda = 50 * CountOpcoes(opcoes);
			while (listaOpcoes != NULL){

				char str[100];
				sprintf(str, "%c", listaOpcoes->tecla);

				ALLEGRO_COLOR cor;
				if (listaOpcoes->ativa){
					cor = WHITE;
				}
				else{
					cor = RED;
				}

				al_draw_text(titulos,
					cor, fundoX + 400 - offsetEsquerda + offset, fundoY + 5, 0,
					str);

				al_draw_text(textos,
					cor, fundoX + 400 - offsetEsquerda + offset, fundoY + 32, 0,
					listaOpcoes->descricao);

				if (listaOpcoes->madeira > 0 || listaOpcoes->pedra > 0){
					char str[100];
					sprintf(str, "%s%d", "Wood: ", listaOpcoes->madeira);
					al_draw_text(textos,
						cor, fundoX + 400 - offsetEsquerda + offset, fundoY + 45, 0,
						str);
					sprintf(str, "%s%d", "Stone: ", listaOpcoes->pedra);
					al_draw_text(textos,
						cor, fundoX + 400 - offsetEsquerda + offset, fundoY + 58, 0,
						str);
				}

				offset += 100;
				listaOpcoes = listaOpcoes->next;
			}
		}
		
		

		if (!bonecoHovered){
			DrawHoveredTile();
		}

		if (opcaoAtiva != NULL)
			DrawNoWalkConstructionTiles();
	}
}

//Desenha a UI de edificio selecionado
void DrawEdificioSelecionado(){
	if (edificioSelecionado != NULL){

		//Fundo
		al_draw_filled_rounded_rectangle(fundoX, fundoY, DISPLAYWIDTH - 20, DISPLAYHEIGHT - 20,
			10, 10, GREY);

		//Nome do edifício
		al_draw_text(titulos,
			WHITE, fundoX + 10, fundoY + 10, 0,
			edificioSelecionado->name);

		if (strcmp(edificioSelecionado->name, "House") == 0){

			int nColonistas = ListCountCharacters(edificioSelecionado->colonists);

			char str[100];
			sprintf(str, "%s%d", "Colonists: ", nColonistas);
			if (nColonistas>0)
			{
				char result[500];
				sprintf(result, "%s%d%s", "Energy (", (int)edificioSelecionado->colonists->energia, "%)");
				al_draw_text(textos, WHITE, fundoX + 700, fundoY + 45, 0, result);

			}
			al_draw_text(textos,
				WHITE, fundoX + 10, fundoY + 48, 0,
				str);

			if (nColonistas > 0){

				//Opção de esvaziar a casa

				int hSpace = 0;

				al_draw_text(titulos,
					WHITE, fundoX + 430, fundoY + 25, 0,
					"E");

				hSpace = 20;

				al_draw_text(textos,
					WHITE, fundoX + 435 + hSpace, fundoY + 34, 0,
					"Empty house");

				if (OptionExists('e') == false){
					opcoes = InsertOption(opcoes, 'e', "Empty House", 0, 0);
				}

			}

		}

		DrawBoundingBoxBuilding(edificioSelecionado);
	}
}

void DrawFixedUI(){
	if (!exitGame){
		al_draw_filled_rounded_rectangle(DISPLAYWIDTH - 460, 20, DISPLAYWIDTH - 20, 60,
			10, 10, GREY);

		char str[100];
		sprintf(str, "%s%d%s%d%s%d%s%d", "Pedra: ", pedra, "          Madeira: ", madeira, "          Comida: ", comida, "          Colonos: ", ListCountCharacters(bonequinhos) + ListCountHouseCharacters(edificios));

		//Nome do edifício
		al_draw_text(textos,
			WHITE, DISPLAYWIDTH - 430, 32, 0,
			str);
	}
}
//contar numero de edificios
int contarEdificios()
{
	int numeroEdificios=0;
	
	Building aux=edificios;
	while (aux != NULL)
	{
		numeroEdificios++;
		aux = aux->next;
	}
	return (numeroEdificios-3);
}
//contar numero de colonos
int contarColonos()
{
	int numColonos = 0;

	Character aux = bonequinhos;
	while (aux != NULL)
	{
		numColonos++;
		aux = aux->next;
	}
	return (numColonos-2);
}
//contar numero de tarefas de um colono
int contarTarefas(Tarefa endereco)
{
	int contador = 0;
	while (endereco != NULL)
	{
		contador++;
		endereco = endereco->next;
	}
	return contador;
}
//remover tarefas antes de guardar
void removerTarefas(Character endereco)
{
	
		endereco->tarefa = NULL;
		strcpy(endereco->action, "Idle");

		

}
//Guardar estado actual do mapa
void saveMap(int map[MAPWIDTH][MAPHEIGHT][3])
{
	int i, j, z=0;
	int qtEdificios=0;
		data = fopen("data.txt", "w");
		for (i = 0; i < MAPWIDTH; i++)
		{
			for (j = 0; j < MAPHEIGHT; j++)
			{
				for (z = 0; z < 3; z++)
				{
					fprintf(data, "%d\n", map[i][j][z]);
				}
				//fprintf(data,"\n");
			}
		}
		fprintf(data, "%d\n", madeira);
		fprintf(data, "%d\n", comida);
		fprintf(data, "%d\n", pedra);
		//guardar edificios -- falta os colonos que estao dentro
		//guardar quantidade de edificios
		quantidadeEdificios = contarEdificios();
		fprintf(data, "%d\n",quantidadeEdificios);
		Building aux = edificios;
		while (aux != NULL)
		{
			fprintf(data, "%d\n", aux->constructionCounter);
			fprintf(data, "%d\n", aux->minTimer);
			fprintf(data, "%s\n", aux->name);
			fprintf(data, "%d\n", aux->timer);
			fprintf(data, "%d\n", aux->type);
			fprintf(data, "%d\n", aux->x);
			fprintf(data, "%d\n", aux->y);
			
			aux = aux->next;
		}
		//Guardar colonos, falta path e tarefas
		quantidadeColonos = contarColonos();
		fprintf(data, "%d\n", quantidadeColonos);
		Character colonosAux = bonequinhos;
		
		
		while (colonosAux != NULL)
		{
			removerTarefas(colonosAux);
			removerTarefas(bonequinhos);
			
			fprintf(data, "%s\n", colonosAux->action);
//bug			fprintf(data, "%d\n", colonosAux->animationFrame);
//bug			fprintf(data, "%s\n", colonosAux->animationTimer);
			fprintf(data, "%d\n", colonosAux->comida);
			fprintf(data, "%d\n", colonosAux->direcao);
			fprintf(data, "%d\n", colonosAux->madeira);
			fprintf(data, "%d\n", colonosAux->movimento);
			fprintf(data, "%d\n", colonosAux->pedra);
			//fprintf(data, "%s\n", colonosAux->spriteSheet);
			
			fprintf(data, "%d\n", PixelToWorld(colonosAux->x,0));
			fprintf(data, "%d\n", PixelToWorld(colonosAux->y,1));
			//guardar as tarefas de cada colono
			
			
			
			colonosAux = colonosAux->next;
			
		}
		fclose(data);
	
}


//Load do estado do mapa guardado
void loadMap(/*int map[MAPWIDTH][MAPHEIGHT][3]*/)
{
	int z = 0;
	data = fopen("data.txt", "r");
	if (data != NULL)
	{
		for (int i = 0; i < MAPWIDTH; i++)
		{
			for (int j = 0; j < MAPHEIGHT; j++)
			{
				for (int z = 0; z < 3; z++)
				{
					fscanf(data, "%d\n", &mapDef[i][j][z]);
				}
				
			}
		}
		fscanf(data, "%d\n", &madeira);
		fscanf(data, "%d\n", &comida);
		fscanf(data, "%d\n", &pedra);
		//ler os edificios iniciais
		fscanf(data, "%d\n", &quantidadeEdificios);
		Building aux=edificios;
		for (int i=0; i < 3; i++)
		{
			fscanf(data, "%d\n", &edificios->constructionCounter);
			fscanf(data, "%d\n", &edificios->minTimer);
			fscanf(data, "%s\n", &edificios->name);
			fscanf(data, "%d\n", &edificios->timer);
			fscanf(data, "%d\n", &edificios->type);
			fscanf(data, "%d\n", &edificios->x);
			fscanf(data, "%d\n", &edificios->y);
			edificios = edificios->next;
		}
		edificios = aux;
		//ler edificios construidos
		while (quantidadeEdificios > 0)
		{
			edificios=InsertBuilding(edificios, 0, 0, 0);
			fscanf(data, "%d\n", &edificios->constructionCounter);
			fscanf(data, "%d\n", &edificios->minTimer);
			fscanf(data, "%s\n", &edificios->name);
			fscanf(data, "%d\n", &edificios->timer);
			fscanf(data, "%d\n", &edificios->type);
			fscanf(data, "%d\n", &edificios->x);
			fscanf(data, "%d\n", &edificios->y);
			quantidadeEdificios--;
			//edificios = edificios->next;
		}
		//ler colonos
		fscanf(data, "%d", &quantidadeColonos);
		Character CharAux = bonequinhos;
		int posicaoX=0,posicaoY=0;
		Tarefa tarefaAux = bonequinhos->tarefa;
		int tipoTarefa = 0;
		int procurarBuildingX = 0,procurarBuildingY;
		int man = 0;
		Building buildingPesquisado = NULL;
		for (int j = 0; j < 2; j++)
		{
			fscanf(data, "%s\n", &bonequinhos->action);
			//bug			fprintf(data, "%d\n", colonosAux->animationFrame);
			//bug			fprintf(data, "%s\n", colonosAux->animationTimer);
			fscanf(data, "%d\n", &bonequinhos->comida);
			fscanf(data, "%d\n", &bonequinhos->direcao);
			fscanf(data, "%d\n", &bonequinhos->madeira);
			fscanf(data, "%d\n", &bonequinhos->movimento);
			fscanf(data, "%d\n", &bonequinhos->pedra);
			//fprintf(data, "%s\n", colonosAux->spriteSheet);
			fscanf(data, "%d\n", &posicaoX);
			fscanf(data, "%d\n", &posicaoY);
			bonequinhos->x = WorldToPixel(posicaoX, 0);
			bonequinhos->y = WorldToPixel(posicaoY, 1);
			
			bonequinhos = bonequinhos->next;
		}
		bonequinhos = CharAux;
		while (quantidadeColonos > 0)
		{
			if (man == 1)
			{
				bonequinhos = InsertCharacter(bonequinhos, woman1, 0, 0, 0, 0, 0, 0);
				man = 0;
			}
			else
			{
				bonequinhos = InsertCharacter(bonequinhos, men1, 0, 0, 0, 0, 0, 0);
				man = 1;
			}
			fscanf(data, "%s\n", &bonequinhos->action);
			//bug			fprintf(data, "%d\n", colonosAux->animationFrame);
			//bug			fprintf(data, "%s\n", colonosAux->animationTimer);
			fscanf(data, "%d\n", &bonequinhos->comida);
			fscanf(data, "%d\n", &bonequinhos->direcao);
			fscanf(data, "%d\n", &bonequinhos->madeira);
			fscanf(data, "%d\n", &bonequinhos->movimento);
			fscanf(data, "%d\n", &bonequinhos->pedra);
			//fprintf(data, "%s\n", colonosAux->spriteSheet);
			fscanf(data, "%d\n", &posicaoX);
			fscanf(data, "%d\n", &posicaoY);
			bonequinhos->x = WorldToPixel(posicaoX, 0);
			bonequinhos->y = WorldToPixel(posicaoY, 1);
			quantidadeColonos--;
		}
		fclose(data);
	}
	
}

void UpdateInput(){


	al_get_keyboard_state(&state);
	//Escape
	if (al_key_down(&state, ALLEGRO_KEY_ESCAPE)){
		
		saveUIactive = 1;
			
	}
	if (al_key_down(&state, ALLEGRO_KEY_M))
	{
		for (int i = 0; i < MAPWIDTH; i++)
		{
			for (int j = 0; j < MAPHEIGHT; j++)
			{
				for (int z = 0; z < 3; z++)
				{
					printf("%d\t", mapDef[i][j][z]);
				}
				printf("\n");
			}
		}
	}

	//se pressionar mos "y" na UI de save game, guarda o map e sai
	if (saveUIactive == 1 && al_key_down(&state, ALLEGRO_KEY_Y))
	{
		
		saveUIactive = 0;
		saveMap(mapDef);
		exitGame = 1;
		ShutDown();
	}
	//se pressionarmos "y" na UI de load game, faz load do mapa guardado
	if (loadUIactive == 1 && al_key_down(&state, ALLEGRO_KEY_Y))
	{
		loadUIactive = 0;
		loadMap();
	}
	if (loadUIactive == 1 && al_key_down(&state, ALLEGRO_KEY_N))
	{
		loadUIactive = 0;
	}
	
	if (KBLimitCounter > KBLimit){
		//Right
		if (al_key_down(&state, ALLEGRO_KEY_D)){
			if (-(offsetX) < (MAPWIDTH * TILEWIDTH - DISPLAYWIDTH))
				offsetX -= TILEWIDTH;
		}
		//Left
		if (al_key_down(&state, ALLEGRO_KEY_A)){
			if (-(offsetX) > 0)
				offsetX += TILEWIDTH;
		}
		//Down
		if (al_key_down(&state, ALLEGRO_KEY_S)){
			if (-(offsetY) < (MAPHEIGHT * TILEHEIGHT - DISPLAYHEIGHT) - MAPHEIGHT)
				offsetY -= TILEHEIGHT;
		}
		//Up
		if (al_key_down(&state, ALLEGRO_KEY_W)){
			if (-(offsetY) > 0)
				offsetY += TILEHEIGHT;
		}

		//CHEATS
		if (al_key_down(&state, ALLEGRO_KEY_F1))
		{
			madeira += 50;
		}
		if (al_key_down(&state, ALLEGRO_KEY_F2))
		{
			pedra += 50;
		}

		//Percorrer a lista de opções e executar opções premidas
		Opcao aux = opcoes;
		while (aux != NULL){
			if (aux->madeira <= madeira && aux->pedra <= pedra){
				aux->ativa = true;
				switch (aux->tecla)
				{
				case 'e':
					if (al_key_down(&state, ALLEGRO_KEY_E)){
						//Esvaziar um edificio
						if (edificioSelecionado != NULL){
							//retirar o bonequinho da lista da casa e colocá-lo na lista de bonequinhos

							Character aux = edificioSelecionado->colonists;
							while (aux != NULL){

								//guardar o apontador para o proximo colono nesta casa
								Character next = aux->next;
								//Colocar o colono na lista de bonequinhos
								Character aux2 = aux;
								aux2->next = bonequinhos;
								bonequinhos = aux2;
								//Remover o colono da lista de colonos nesta casa
								edificioSelecionado->colonists = RemoveCharacter(edificioSelecionado->colonists, aux);

								aux->tarefa = NULL;

								//Passar para o proximo colono da casa
								aux = next;
							}
						}
						edificioSelecionado = NULL;
					}
					//Remover esta opção da lista de opções
					opcoes = RemoveOption(opcoes, 'e');
					break;

				case 'b':
					//Opção "Build"
					if (al_key_down(&state, ALLEGRO_KEY_B)){
						opcoes = NULL;
						//Todo: adicionar opções do tipo de edificio.
						opcoes = InsertOption(opcoes, 'w', "Warehouse", 200, 300);
						opcoes = InsertOption(opcoes, 'f', "Farmhouse", 160, 200);
						opcoes = InsertOption(opcoes, 'h', "House", 0, 0);
					}

					break;

				case 'h':
					//Opção "Build" -> "House"
					if (al_key_down(&state, ALLEGRO_KEY_H)){
						opcoes = NULL;
						//Todo: adicionar opções do tipo de casa.
						opcoes = InsertOption(opcoes, '4', "House 4", 130, 130);
						opcoes = InsertOption(opcoes, '3', "House 3", 120, 100);
						opcoes = InsertOption(opcoes, '2', "House 2", 40, 100);
						opcoes = InsertOption(opcoes, '1', "House 1", 100, 100);
					}

					break;

				case 'f':
					//Opção "Build" -> "Farmhouse"
					if (al_key_down(&state, ALLEGRO_KEY_F)){
						opcoes = NULL;
						opcaoAtiva = InsertOption(opcaoAtiva, 'f', "build Farmhouse", 160, 200);
						opcoes = NULL;
					}

					break;

				case 'p':
					//Opção "Plant"
					if (al_key_down(&state, ALLEGRO_KEY_P)){
						opcoes = NULL;
						//Tipos de plantações
						opcoes = InsertOption(opcoes, 'l', "Milho", 0, 0);
						opcoes = InsertOption(opcoes, 'o', "Morango", 0, 0);
						opcoes = InsertOption(opcoes, 'i', "Pimento", 0, 0);
						opcoes = InsertOption(opcoes, 'a', "Batata", 0, 0);
						opcoes = InsertOption(opcoes, 'n', "Nabo", 0, 0);
						opcoes = InsertOption(opcoes, 'c', "Cenoura", 0, 0);
						opcoes = InsertOption(opcoes, 't', "Beterraba", 0, 0);
					}
					break;

				case 'l':
					//Milho
					if (al_key_down(&state, ALLEGRO_KEY_L)){
						opcaoAtiva = InsertOption(opcaoAtiva, 'l', "plantar Milho", 0, 0);
						opcoes = NULL;
					}
					break;

				case 'o':
					//Morango
					if (al_key_down(&state, ALLEGRO_KEY_O)){
						opcaoAtiva = InsertOption(opcaoAtiva, 'o', "plantar Morango", 0, 0);
						opcoes = NULL;
					}
					break;

				case 'i':
					//Pimento
					if (al_key_down(&state, ALLEGRO_KEY_I)){
						opcaoAtiva = InsertOption(opcaoAtiva, 'i', "plantar Pimento", 0, 0);
						opcoes = NULL;
					}
					break;

				case 'a':
					//Batata
					if (al_key_down(&state, ALLEGRO_KEY_A)){
						opcaoAtiva = InsertOption(opcaoAtiva, 'a', "plantar Batata", 0, 0);
						opcoes = NULL;
					}
					break;

				case 'n':
					//Nabo
					if (al_key_down(&state, ALLEGRO_KEY_N)){
						opcaoAtiva = InsertOption(opcaoAtiva, 'n', "plantar Nabo", 0, 0);
						opcoes = NULL;
					}
					break;

				case 'c':
					//Cenoura
					if (al_key_down(&state, ALLEGRO_KEY_C)){
						opcaoAtiva = InsertOption(opcaoAtiva, 'c', "plantar Cenoura", 0, 0);
						opcoes = NULL;
					}
					break;

				case 't':
					//Baterraba
					if (al_key_down(&state, ALLEGRO_KEY_T)){
						opcaoAtiva = InsertOption(opcaoAtiva, 't', "plantar Beterraba", 0, 0);
						opcoes = NULL;
					}
					break;

				case '1':
					//Casa 1
					if (al_key_down(&state, ALLEGRO_KEY_1)){
						opcaoAtiva = InsertOption(opcaoAtiva, '1', "build House 1", 100, 100);
						opcoes = NULL;
					}
					break;
				case '2':
					//Casa 2
					if (al_key_down(&state, ALLEGRO_KEY_2)){
						opcaoAtiva = InsertOption(opcaoAtiva, '2', "build House 2", 40, 100);
						opcoes = NULL;
					}
					break;
				case '3':
					//Casa 3
					if (al_key_down(&state, ALLEGRO_KEY_3)){
						opcaoAtiva = InsertOption(opcaoAtiva, '3', "build House 3", 120, 100);
						opcoes = NULL;
					}
					break;
				case '4':
					//Casa 4
					if (al_key_down(&state, ALLEGRO_KEY_4)){
						opcaoAtiva = InsertOption(opcaoAtiva, '4', "build House 4", 130, 130);
						opcoes = NULL;
					}
					break;


				default:
					break;
				}
			}
			else{
				//Não temos recursos para executar esta opção
				if (aux->madeira > madeira || aux->pedra > pedra)
					aux->ativa = false;
			}
			
			aux = aux->next;
		}

		KBLimitCounter = 0;
	}
	KBLimitCounter++;


	//RATO
	if (!exitGame) {
		al_get_mouse_state(&mouseState);
		//DrawHoveredTile();
		DrawCharacterBoundingBox(bonequinhos);
		DrawBuildingHover(edificios);

		//Detetar cliques
		if (mouseState.buttons & 1 && (!mouseStateAnterior.buttons & 1)) {
			/* Primary (e.g. left) mouse button is held. */
			ProcessMouseClicks(bonequinhos);
		}
		if (mouseState.buttons & 2) {
			/* Secondary (e.g. right) mouse button is held. */
			bonecoSelecionado = NULL;
			edificioSelecionado = NULL;
		}

		mouseStateAnterior = mouseState;
	}

	
		
}

void LoadInitialState(){
	//Edifícios iniciais
	edificios = InsertBuilding(edificios, 15, 5, 35);
	edificios = InsertBuilding(edificios, 16, 5, 36);

	//Primeira casa
	Building primeiraCasa = InsertBuilding(edificios, 11, 3, 39);
	primeiraCasa->minTimer = 0;
	primeiraCasa->constructionCounter = 32;
	edificios = primeiraCasa;

	//Casas a ser construídas
	/*edificios = InsertBuilding(edificios, 16, 7, 40);
	edificios = InsertBuilding(edificios, 5, 12, 45);
	edificios = InsertBuilding(edificios, 23, 5, 46);*/

	//Warehouse
	/*edificios = InsertBuilding(edificios, 16, 9, 41);
	edificios = InsertBuilding(edificios, 17, 9, 42);
	edificios = InsertBuilding(edificios, 16, 10, 43);
	edificios = InsertBuilding(edificios, 17, 10, 44);*/

	//Farmhouse
	/*edificios = InsertBuilding(edificios, 5, 8, 37);
	edificios = InsertBuilding(edificios, 6, 8, 38);*/

	//Quintas
	/*quintas = InsertFarm(quintas, 5, 9, 17);
	quintas = InsertFarm(quintas, 6, 9, 18);*/

	//Florestas
	florestas = InsertForest(florestas, 7, 6, 2);
	florestas = InsertForest(florestas, 8, 6, 3);
	florestas = InsertForest(florestas, 5, 6, 4);
	florestas = InsertForest(florestas, 9, 8, 5);
	florestas = InsertForest(florestas, 10, 9, 6);
	florestas = InsertForest(florestas, 10, 10, 7);
	florestas = InsertForest(florestas, 5, 5, 8);

	//Bonequinhos iniciais
	bonequinhos = InsertCharacter(bonequinhos, woman1, WorldToPixel(13, 0), WorldToPixel(4, 1), 2, 1, edificios->x, edificios->y);
	bonequinhos = InsertCharacter(bonequinhos, men1, WorldToPixel(14, 0), WorldToPixel(5, 1), 2, 1, edificios->x, edificios->y);
	//bonequinhos = InsertCharacter(bonequinhos, woman2, WorldToPixel(12, 0), WorldToPixel(5, 1), 2, 1);
	//bonequinhos = InsertCharacter(bonequinhos, men2, WorldToPixel(15, 0), WorldToPixel(7, 1), 2, 1);


}

//Load assets from disk
void Load(){
	water = al_load_bitmap("assets/water.png");
	tree7 = al_load_bitmap("assets/tree7.png");
	tree6 = al_load_bitmap("assets/tree6.png");
	tree5 = al_load_bitmap("assets/tree5.png");
	tree4 = al_load_bitmap("assets/tree4.png");
	tree3 = al_load_bitmap("assets/tree3.png");
	tree2 = al_load_bitmap("assets/tree2.png");
	tree1 = al_load_bitmap("assets/tree1.png");
	rock3 = al_load_bitmap("assets/rock3.png");
	rock2 = al_load_bitmap("assets/rock2.png");
	rock1 = al_load_bitmap("assets/rock1.png");
	soil = al_load_bitmap("assets/depleted_rock.png");
	grass = al_load_bitmap("assets/grass.png");
	chopped_trees = al_load_bitmap("assets/chopped_trees.png");
	farm1_1 = al_load_bitmap("assets/farm1_1.png");
	farm1_2 = al_load_bitmap("assets/farm1_2.png");
	farm1_3 = al_load_bitmap("assets/farm1_3.png");
	farm2_1 = al_load_bitmap("assets/farm2_1.png");
	farm2_2 = al_load_bitmap("assets/farm2_2.png");
	farm2_3 = al_load_bitmap("assets/farm2_3.png");
	farm3_1 = al_load_bitmap("assets/farm3_1.png");
	farm3_2 = al_load_bitmap("assets/farm3_2.png");
	farm3_3 = al_load_bitmap("assets/farm3_3.png");
	farm4_1 = al_load_bitmap("assets/farm4_1.png");
	farm4_2 = al_load_bitmap("assets/farm4_2.png");
	farm4_3 = al_load_bitmap("assets/farm4_3.png");
	farm5_1 = al_load_bitmap("assets/farm5_1.png");
	farm5_2 = al_load_bitmap("assets/farm5_2.png");
	farm5_3 = al_load_bitmap("assets/farm5_3.png");
	farm6_1 = al_load_bitmap("assets/farm6_1.png");
	farm6_2 = al_load_bitmap("assets/farm6_2.png");
	farm6_3 = al_load_bitmap("assets/farm6_3.png");
	farm7_1 = al_load_bitmap("assets/farm7_1.png");
	farm7_2 = al_load_bitmap("assets/farm7_2.png");
	farm7_3 = al_load_bitmap("assets/farm7_3.png");

	men1 = al_load_bitmap("assets/characters/men1.png");
	men2 = al_load_bitmap("assets/characters/men2.png");
	men3 = al_load_bitmap("assets/characters/men3.png");
	men4 = al_load_bitmap("assets/characters/men4.png");
	woman1 = al_load_bitmap("assets/characters/woman1.png");
	woman2 = al_load_bitmap("assets/characters/woman2.png");
	woman3 = al_load_bitmap("assets/characters/woman3.png");
	woman4 = al_load_bitmap("assets/characters/woman4.png");

	city_hall1 = al_load_bitmap("assets/buildings/city_hall1.png");
	city_hall2 = al_load_bitmap("assets/buildings/city_hall2.png");
	farm1 = al_load_bitmap("assets/buildings/farm1.png");
	farm2 = al_load_bitmap("assets/buildings/farm2.png");
	house1 = al_load_bitmap("assets/buildings/house1.png");
	house2 = al_load_bitmap("assets/buildings/house2.png");
	warehouse1 = al_load_bitmap("assets/buildings/warehouse1.png");
	warehouse2 = al_load_bitmap("assets/buildings/warehouse2.png");
	warehouse3 = al_load_bitmap("assets/buildings/warehouse3.png");
	warehouse4 = al_load_bitmap("assets/buildings/warehouse4.png");
	house3 = al_load_bitmap("assets/buildings/house3.png");
	house4 = al_load_bitmap("assets/buildings/house4.png");

	//Load do estado inicial do jogo
	LoadInitialState();
}

void Update(){
	
	UpdateFarms(quintas);

	UpdateBuildings(edificios);

	UpdateCharacters(bonequinhos);

	UpdateForests(florestas);

	UpdateMap();
}
//desenha ui do ecra de savegame
void DrawSaveUI()
{
	al_draw_filled_rounded_rectangle(DISPLAYWIDTH - 800, 150, DISPLAYWIDTH - 200, 300, 10, 10, GREY);
	////al_draw_filled_rounded_rectangle(0, 200, 10, 200, 25, 25, GREY);//460,20,20,60,10,10,red
	char str[1000];
	sprintf(str, "Do you want to save your progress? [y]Yes, [n]No");
	al_draw_text(textos, WHITE, DISPLAYWIDTH - 660, 210, 0, str);
	//Sleep(5000);
}
//desenha UI do loadGame
void DrawLoadUI()
{
	al_draw_filled_rounded_rectangle(DISPLAYWIDTH - 800, 150, DISPLAYWIDTH - 200, 300, 10, 10, GREY);
	////al_draw_filled_rounded_rectangle(0, 200, 10, 200, 25, 25, GREY);//460,20,20,60,10,10,red
	char str[1000];
	sprintf(str, "Do you want to load the previous game? [y]Yes, [n]No");
	al_draw_text(textos, WHITE, DISPLAYWIDTH - 660, 210, 0, str);
}
//verificar se existe jogo guardado anteriormente
void checkLoadedGame()
{
	//desenha UI de load quando o jogo começa
	//data = fopen("data.txt", "r");
	if (loadUIactive==1)
	{
		if (fopen("data.txt", "r") != NULL)
		{
			//fclose(data);
			DrawLoadUI();
		}
	}
}
void Draw(){
	al_clear_to_color(al_map_rgb(0, 0, 0));

	DrawMap();

	DrawCharacters(bonequinhos);

	UpdateInput(); //Desenha também hovers do rato

	DrawEdificioSelecionado();

	DrawBonecoSelecionado();

	DrawFixedUI();
	//desenha UI de save game se pressionarmos ESC
	if (saveUIactive == 1)
	{
		DrawSaveUI();
	}
	checkLoadedGame();
	al_flip_display();
}

//Entry point
int main(){

	//INICIALIZAÇÃO
	InitializeAllegro();

	//Load assets e estado inicial
	Load();
	
	//loadMap(mapDef, data);
	//Gerar grafo de Pathfinding
	UpdateSearchNodes();
	
	//GAME LOOP
	while (!exitGame){

		//Allegro timer
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		if (ev.type == ALLEGRO_EVENT_TIMER) {

			Update();

			Draw();
		}
	}
	
	return 1;
}
