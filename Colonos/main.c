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


//Vari�veis Globais
//ALLEGRO
ALLEGRO_FONT *font;
ALLEGRO_KEYBOARD_STATE state;
ALLEGRO_MOUSE_STATE mouseState;
ALLEGRO_MOUSE_STATE mouseStateAnterior;
int mouseButtons;
//OUTRAS
int exitGame = 0;
const float FPS = 60;
float x, y;

//Criar um display para o Allegro
ALLEGRO_DISPLAY *display = NULL;

ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;

//Assets - Sprites
//CEN�RIO
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

//EDIF�CIOS
ALLEGRO_BITMAP *city_hall1, *city_hall2, *farm1, *farm2, *house1, 
*house2, *warehouse1, *warehouse2, *warehouse3, *warehouse4, *house3, *house4 = NULL;

//Assets - Animated sprites
ALLEGRO_BITMAP *men1, *men2, *men3, *men4, *woman1, *woman2, *woman3, *woman4 = NULL;

//Cores
ALLEGRO_COLOR RED, BLACK, ORANGE, GREEN, YELLOW;

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

//limitador de input do teclado
int KBLimit = 5;
int KBLimitCounter = 0;

// *********************************************************************************************************** //
// ESTRUTURAS DE DADOS //

//Descreve uma quinta
typedef struct farm
{
	int type; //Sprite do estado inicial
	int i, j; //Posicao da quinta no mapa
	int timer; //Contador de h� quanto tempo a quinta est� no estado atual
	int phase; //Fase em que a quinta se encontra (1, 2, 3)
	int minTimer; //Tempo necess�rio para passar para a pr�xima fase
	char *name;
	struct farm *next; //Apontador para o elemento seguinte
}* Farm;

//Descreve um bonequinho
typedef struct character
{
	ALLEGRO_BITMAP *spriteSheet; //Spritesheet que cont�m a anima��o desta personagem
	int movimento; //0 - Parado, 1 - Movimento
	int direcao; //0 - Baixo; 1 - Esquerda; 2 - Direita; 3 - Cima
	float x, y; //Posicao
	int animationFrame;
	int animationTimer;
	struct character *next; //Apontador para o elemento seguinte
	struct node * path;
}* Character;

//Descreve um edificio
typedef struct building
{
	int type; //Sprite do estado inicial
	int x, y; //Posi��o do edif�cio no mapa
	int timer; //Tempo passado desde o in�cio da constru��o do edif�cio
	int minTimer; //Tempo que o edif�cio demora a ser constru�do
	int constructionCounter; //Vai aumentando cada vez que minTimer = timer, at� ao m�ximo de 32 (altura da sprite)
	struct building *next; //Apontador para o elemento seguinte
}* Building;


//AI - C�lculo de caminhos
//Descreve um n� de caminho
typedef struct node
{
	int x, y; //Posi��o do node
	bool caminho; //Indica se esta posi��o � ou n�o caminho
	struct node * parent; //Apontador para o node que colocou este na lista aberta
	bool inOpenList; //Indica se est� na lista aberta
	bool inClosedList; //Indica se est� na lista fechada
	float distanceToTarget; //Dist�ncia aproximada desde este node at� ao alvo
	float distanceTravelled; //Dist�ncia j� viajada da origem at� este node
	struct node * vizinhos[4]; //Apontadores para os vizinhos deste node (cima, baixo, esquerda, direita)
	struct node * next;
	int contadorVizinhos;
}* Node;
//

//Matriz que define o mapa
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
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 3, 1, 0 }, { 3, 1, 0 }, { 6, 1, 0 }, { 5, 1, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 4, 1, 0 }, { 2, 1, 0 }, { 7, 1, 0 }, { 4, 1, 0 }, { 2, 1, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 2, 1, 0 }, { 5, 1, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 7, 1, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 3, 1, 0 }, { 7, 1, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 8, 1, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 6, 1, 0 }, { 1, 1, 1 }, { 0, 0, 0 }, { 0, 0, 0 }, { 6, 1, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 8, 1, 0 }, { 2, 1, 0 }, { 3, 1, 0 }, { 4, 1, 0 }, { 5, 1, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
	{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 12, 1, 0 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } },
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

//Matriz de searchNodes
Node searchNodes[MAPWIDTH][MAPHEIGHT];
//Lista aberta de Nodes
Node listaAberta = NULL;
//Lista fechada de Nodes
Node listaFechada = NULL;

//Recicladas
Node path;

//Cenas selecionadas
Character bonecoSelecionado = NULL;
bool bonecoHovered = false;

//***********************************************************************************************************//

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
	//Fazer reset �s propriedades dos nodes
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
		}
	}
}

Node InsertNode(Node lista, Node node){
	Node aux = node;
	aux->next = lista;
	return aux;
}

//Remover elemento da lista ligada - recursivamente
Node RemoveNode(Node enderecoInicioLista, int x, int y){
	Node aux;
	if (enderecoInicioLista != NULL){
		//Lista n�o est� vazia
		if (enderecoInicioLista->x == x && enderecoInicioLista->y == y){
			//encontramos o elemento a eliminar
			aux = enderecoInicioLista->next;
			return aux;
		}
		else{
			//n�o � este o elemento e eliminar, continuar a recurs�o
			enderecoInicioLista->next = RemoveNode(enderecoInicioLista->next, x, y);
			return enderecoInicioLista;
		}
	}
	else{
		//Lista vazia
		return enderecoInicioLista;
	}
}

//Cria ou atualiza a matriz de searchnodes
void UpdateSearchNodes(){
	ResetSearchNodes();
	int x, y;
	//Criar um node para cada espa�o do mapa
	Node node;
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

			//Apenas � caminho se tiver 1 na segunda posi��o desta tile
			if (mapDef[y][x][1] == 1 && mapDef[y][x][2] == 1){
				node->caminho = true;
				searchNodes[x][y] = node;
			}
		}
	}
	//Para cada um dos nodes, vamos lig�-lo aos vizinhos
	for (x = 0; x < MAPWIDTH; x++)
	{
		for (y = 0; y < MAPHEIGHT; y++)
		{
			Node node = searchNodes[x][y];
			//Se n�o se pode andar neste node, n�o interessa
			if (node == NULL || !node->caminho){
				continue;
			}
			//Criamos um array com todos os poss�veis vizinhos que este node pode ter
			int vizinhos[4][4] = { 
					{x, y - 1},
					{x, y + 1},
					{x - 1, y},
					{x + 1, y}
			};
			//Iteramos por cada um dos poss�veis vizinhos
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

				//S� nos interessam vizinhos em que se possa andar
				if (vizinho == NULL || !vizinho->caminho){
					continue;
				}
				
				node->contadorVizinhos++;
				node->vizinhos[node->contadorVizinhos-1] = vizinho;
			}
		}
	}

	//DEBUG
	//Verificar se est�o a ser criados searchnodes
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

//Devolve uma estimativa da dist�ncia entre dois tiles
float Heuristic(x1, y1, x2, y2){
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

//Devolve o n�mero de elementos numa lista
int ListCount(Node endereco){
	if (endereco == NULL){
		return 0;
	}
	else{
		//A fun��o invoca-se a ela pr�pria
		return (1 + ListCount(endereco->next));
	}
}

//Devolve o node com dist�ncia mais pequena ao objetivo
Node FindBestNode(){
	Node currentTile;
	currentTile = GetIndex(listaAberta, 0);
	float smallestDistanceToTarget = 999999;
	for (int i = 0; i < ListCount(listaAberta); i++){
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
	//Percorrer o caminho para tr�s, do n� final, atrav�s do parent, at� ao inicial
	while (parentTile != startNode && parentTile != NULL && parentTile->parent != NULL){
		listaFechada = InsertNode(listaFechada, parentTile);
		parentTile = parentTile->parent;
	}
	Node finalPath = NULL;
	//Reverter o caminho para ir do inicial ao final
	for (int i = ListCount(listaFechada) - 1; i >= 0; i--){
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
	// Step 1 : Clear the Open and Closed Lists and reset each node�s F 
	//          and G values in case they are still set from the last 
	//          time we tried to find a path. 
	/////////////////////////////////////////////////////////////////////
	//UpdateSearchNodes();
	UpdateSearchNodes();

	// Store references to the start and end nodes for convenience.
	Node startNode = searchNodes[x1][y1];
	Node endNode = searchNodes[x2][y2];

	/////////////////////////////////////////////////////////////////////
	// Step 2 : Set the start node�s G value to 0 and its F value to the 
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
	while (ListCount(listaAberta) > 0)
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
		// d) : Else, for each of the Active Node�s neighbours :
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
				// (1) Set the neighbouring node�s G value to the G value 
				//     we just calculated.
				neighbor->distanceTravelled = distanceTraveled;
				// (2) Set the neighbouring node�s F value to the new G value + 
				//     the estimated distance between the neighbouring node and
				//     goal node.
				neighbor->distanceToTarget = distanceTraveled + heuristic;
				// (3) Set the neighbouring node�s Parent property to point at the Active 
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
				//     node�s G value, we basically do exactly the same 
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

//Inicializa��o do Allegro
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
	font = al_load_ttf_font("pirulen.ttf", 28, 0);
	if (!font){
		fprintf(stderr, "Could not load 'pirulen.ttf'.\n");
		return -1;
	}

	//Inicializar cores
	RED = al_map_rgb(255, 0, 0);
	BLACK = al_map_rgb(0, 0, 0);
	ORANGE = al_map_rgb(255, 255, 0);
	GREEN = al_map_rgb(0, 128, 0);
	YELLOW = al_map_rgb(255, 255, 0);
	
	al_start_timer(timer);

	return 1;
}

//Load assets from disk
void LoadAssets(){
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
			default:
				mapa[i][j] = grass;
				break;
			}
		}
	}
}

//Inserir edif�cio
Building InsertBuilding(Building endereco, int j, int i, int type){
	Building edificio = (Building)malloc(sizeof(struct building));
	switch (type)
	{
	case 35: //City hall1
		edificio->minTimer = 0; //J� aparece constu�do
		break;
	case 36: //City hall2
		edificio->minTimer = 0; //J� aparece constu�do
		break;
	case 37: //Farm1
		edificio->minTimer = 100;
		break;
	case 38: //Farm2
		edificio->minTimer = 100;
		break;
	case 39: //house 1
		edificio->minTimer = 200;
		break;
	case 40: //house2
		edificio->minTimer = 200;
		break;
	case 41: //warehouse1
		edificio->minTimer = 400;
		break;
	case 42: //warehouse2
		edificio->minTimer = 400;
		break;
	case 43: //warehouse3
		edificio->minTimer = 400;
		break;
	case 44: //warehouse4
		edificio->minTimer = 400;
		break;
	case 45: //house 3
		edificio->minTimer = 200;
		break;
	case 46: //house 4
		edificio->minTimer = 200;
		break;
	default:
		edificio->minTimer = 200;
		break;
	}

	edificio->x = i;
	edificio->y = j;
	edificio->timer = 0;
	edificio->type = type;
	edificio->constructionCounter = 0;
	edificio->next = endereco;
	mapDef[i][j][0] = type;
	mapDef[i][j][1] = 0; // Characters can't walk over buildings 
	mapDef[i][j][2] = 0; // Can't build over farms
	return edificio;
}

//Inserir quinta
Farm InsertFarm(Farm endereco, int i, int j, int type){
	//Aloca o espa�o necess�rio na mem�ria
	//Devolve o endere�o de mem�ria do espa�o alocado
	Farm farm = (Farm)malloc(sizeof(struct farm));
	//Dar os valores ao farm

	switch (type)
	{
	case 14:
		farm->minTimer = 40000;
		farm->name = _strdup("Beterraba");
		break;
	case 17:
		farm->minTimer = 20000;
		farm->name = _strdup("Cenoura");
		break;
	case 20:
		farm->minTimer = 50000;
		farm->name = _strdup("Nabo");
		break;
	case 23:
		farm->minTimer = 30000;
		farm->name = _strdup("Batata");
		break;
	case 26:
		farm->minTimer = 25000;
		farm->name = _strdup("Pimento");
		break;
	case 29:
		farm->minTimer = 15000;
		farm->name = _strdup("Morango");
		break;
	case 32:
		farm->minTimer = 40000;
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
	mapDef[i][j][1] = 1; // Characters can walk over farms 
	mapDef[i][j][2] = 0; // Can't build over farms
	return farm;
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
	while (endereco != NULL){
		if (endereco->constructionCounter < 32 && endereco->minTimer > 0){
			//incrementar timer
			endereco->timer += 1;
			//passar fase?
			if (endereco->timer > endereco->minTimer){
				endereco->timer = 0;
				endereco->constructionCounter++;
			}
		}
		endereco = endereco->next;
	}
}

//Encontrar edificio numa determinada posicao
Building FindBuilding(Building endereco, int i, int j){
	while (endereco != NULL){
		if (endereco->x == i && endereco->y == j){
			return endereco;
		}
		endereco = endereco->next;
	}
}

//Desenhar o mapa
void DrawMap(){
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			if (mapDef[j][i][0] != 37 && mapDef[j][i][0] != 38 && mapDef[j][i][0] != 39 && mapDef[j][i][0] != 40 && mapDef[j][i][0] != 41
				&& mapDef[j][i][0] != 42 && mapDef[j][i][0] != 43 && mapDef[j][i][0] != 44 && mapDef[j][i][0] != 45 && mapDef[j][i][0] != 46){
				//N�o � edif�cio
				al_draw_bitmap(mapa[j][i], i * TILEWIDTH + offsetX, j * TILEHEIGHT + offsetY, 0);
			}
			else{
				//� edif�cio
				//desenhar relva por baixo de todos os edif�cios
				al_draw_bitmap(grass, i * TILEWIDTH + offsetX, j * TILEHEIGHT + offsetY, 0);
				//encontrar o edificio que est� nesta posicao
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
			if (mapDef[j][i][1] == 0){
				//Can't walk on these tiles
				al_draw_rectangle(i * TILEWIDTH + offsetX, j * TILEHEIGHT + offsetY, i * TILEWIDTH + TILEWIDTH + offsetX, j * TILEHEIGHT + TILEHEIGHT + offsetY,
					RED, 1);
			}	
		}
	}
}

Character InsertCharacter(Character endereco, ALLEGRO_BITMAP *sprite, float x, float y, int direcao, int movimento){
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
	return boneco;
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

bool AlmostEqualRelative(float A, float B)
{
	float maxRelDiff = VELOCIDADE_BONECOS / 100;
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

//Atualizar os bonequinhos
void UpdateCharacters(Character endereco){

	bonecoHovered = false;

	if (endereco != NULL)
	{
		if (endereco->path != NULL){
			//Esta personagem tem caminho a percorrer

			//Atualizar a anima��o
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

			//Calcular a dire��o em que deve andar
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
				//Cheg�mos a este path, elimin�-lo da lista
				endereco->x = WorldToPixel(path->x, 0);
				endereco->y = WorldToPixel(path->y, 1);
				endereco->path = RemoveNode(endereco->path, path->x, path->y);
			}

			
		}
		if (endereco->next != NULL){
			UpdateCharacters(endereco->next);
		}
	}
}

//Desenhar os bonequinhos
void DrawCharacters(Character endereco){
	al_draw_bitmap_region(endereco->spriteSheet,
		endereco->animationFrame * 16, endereco->direcao * 24, 16, 24, endereco->x + offsetX, endereco->y + offsetY, 0);
	if (endereco->next != NULL){
		DrawCharacters(endereco->next);
	}
}

//Destroi os objetos criados
void ShutDown(){

	bonecoSelecionado = false;

	if (display){
		al_destroy_display(display);
	}
	if (font){
		al_destroy_font(font);
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

	al_draw_rectangle(x, y, x + TILEWIDTH, y + TILEHEIGHT,
		GREEN, 2);
}

//Verifica se existe colis�o entre duas bounding boxes
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
	//Posi��o do rato
	x = mouseState.x;
	y = mouseState.y;

	while (endereco != NULL){
		if (bounding_box_collision(x, y, 10, 20, endereco->x + offsetX, endereco->y + offsetY, 16, 24)){
			//O rato est� por cima de um bonequinho!
			bonecoHovered = true;
			al_draw_rectangle(endereco->x + offsetX, endereco->y + offsetY, endereco->x + 16 + offsetX, endereco->y + 24 + offsetY,
				GREEN, 2);
			break;
		}
		endereco = endereco->next;
	}
	
	//Desenhar o caminho que o boneco est� a percorrer, se existir
	DrawCharacterPath(endereco);
}

void DrawBuildingBoudingBox(Building edificios){
	//Posi��o do rato
	x = mouseState.x;
	y = mouseState.y;

	float edificioX, edificioY;
	
	while (edificios != NULL){

		//Aten��o que parece estar ao contr�rio mas tem que ser assim, por causa da rota��o do mapa
		edificioX = (round(WorldToPixel(edificios->y, 1) / TILEWIDTH)  * TILEWIDTH);
		edificioY = (round(WorldToPixel(edificios->x, 0) / TILEHEIGHT)  * TILEHEIGHT);

		if (bounding_box_collision(x, y, 10, 20, edificioX, edificioY, TILEWIDTH, TILEHEIGHT)){
			//O rato est� por cima de um edificio!
			al_draw_rectangle(edificioX, edificioY, edificioX + TILEWIDTH, edificioY + TILEHEIGHT,
				GREEN, 2);
			break;
		}
		edificios = edificios->next;
	}
}

void ProcessMouseClicks(Character bonequinhos){
	x = mouseState.x;
	y = mouseState.y;

	//Verificar cliques para selecionar bonecos
	while (bonequinhos != NULL){
		if (bounding_box_collision(x, y, 10, 20, bonequinhos->x + offsetX, bonequinhos->y + offsetY, 16, 24)){
			//O rato est� por cima de um bonequinho!
			bonecoSelecionado = bonequinhos;
			break;
		}
		bonequinhos = bonequinhos->next;
	}

	//Verificar cliques para mandar bonecos andar
	if (bonecoSelecionado != NULL && !bounding_box_collision(x, y, 10, 20, bonecoSelecionado->x + offsetX, bonecoSelecionado->y + offsetY, 16, 24)){
		//Mandar este boneco andar para o destino clicado
		printf("\n%d; %d; : %d; %d\n", PixelToWorld(bonecoSelecionado->x, 0), PixelToWorld(bonecoSelecionado->y, 1), PixelToWorld(x - offsetX, 0), PixelToWorld(y - offsetY, 1));
		bonecoSelecionado->path = FindPath(PixelToWorld(bonecoSelecionado->x, 0), PixelToWorld(bonecoSelecionado->y, 1), PixelToWorld(x - offsetX, 0), PixelToWorld(y - offsetY, 1));
		bonecoSelecionado = NULL;
	}
}

void UpdateInput(){


	al_get_keyboard_state(&state);
	//Escape
	if (al_key_down(&state, ALLEGRO_KEY_ESCAPE)){
		exitGame = 1;
		ShutDown();
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
		KBLimitCounter = 0;
	}
	KBLimitCounter++;


	//RATO
	al_get_mouse_state(&mouseState);
	if (!exitGame) {
		//DrawHoveredTile();
		DrawCharacterBoundingBox(bonequinhos);
		DrawBuildingBoudingBox(edificios);
	}

	//Detetar cliques
	if (mouseState.buttons & 1 && (!mouseStateAnterior.buttons & 1)) {
		/* Primary (e.g. left) mouse button is held. */
		printf("Mouse position: (%d, %d)\n", mouseState.x, mouseState.y);
		ProcessMouseClicks(bonequinhos);
	}

	mouseStateAnterior = mouseState;
		
}

int main(int argc, char **argv){

	//INICIALIZA��O
	InitializeAllegro();

	//Load assets
	LoadAssets();

	quintas = InsertFarm(quintas, 8, 1, 14);
	quintas = InsertFarm(quintas, 8, 2, 17);
	quintas = InsertFarm(quintas, 8, 2, 20);
	quintas = InsertFarm(quintas, 9, 1, 23);
	quintas = InsertFarm(quintas, 9, 2, 26);
	quintas = InsertFarm(quintas, 9, 3, 29);
	quintas = InsertFarm(quintas, 10, 1, 32);

	edificios = InsertBuilding(edificios, 15, 5, 35);
	edificios = InsertBuilding(edificios, 16, 5, 36);

	edificios = InsertBuilding(edificios, 11, 3, 41);
	edificios = InsertBuilding(edificios, 12, 3, 42);
	edificios = InsertBuilding(edificios, 11, 4, 43);
	edificios = InsertBuilding(edificios, 12, 4, 44);

	edificios = InsertBuilding(edificios, 1, 7, 37);
	edificios = InsertBuilding(edificios, 2, 7, 38);

	edificios = InsertBuilding(edificios, 2, 12, 39);

	edificios = InsertBuilding(edificios, 7, 6, 40);

	edificios = InsertBuilding(edificios, 12, 10, 45);

	edificios = InsertBuilding(edificios, 8, 3, 46);

	//Inicilizar Pathfinding
	UpdateSearchNodes();

	/*bonequinhos = InsertCharacter(bonequinhos, men1, 300, 100, 1, 0);
	bonequinhos = InsertCharacter(bonequinhos, woman1, 1000, 200, 1, 1);
	bonequinhos = InsertCharacter(bonequinhos, men2, 320, 640, 3, 1);
	bonequinhos = InsertCharacter(bonequinhos, woman2, 10, 300, 2, 1);
	bonequinhos = InsertCharacter(bonequinhos, men3, 150, 620, 3, 1);
	bonequinhos = InsertCharacter(bonequinhos, woman3, 200, 330, 2, 1);
	bonequinhos = InsertCharacter(bonequinhos, men4, 170, 650, 3, 1);*/
	bonequinhos = InsertCharacter(bonequinhos, men1, WorldToPixel(20, 0), WorldToPixel(16, 1), 2, 1);
	bonequinhos = InsertCharacter(bonequinhos, men2, WorldToPixel(0, 0), WorldToPixel(2, 1), 2, 1);
	bonequinhos = InsertCharacter(bonequinhos, men3, WorldToPixel(10, 0), WorldToPixel(7, 1), 2, 1);
	bonequinhos = InsertCharacter(bonequinhos, men4, WorldToPixel(8, 0), WorldToPixel(2, 1), 2, 1);
	bonequinhos = InsertCharacter(bonequinhos, woman1, WorldToPixel(13, 0), WorldToPixel(4, 1), 2, 1);
	bonequinhos = InsertCharacter(bonequinhos, woman2, WorldToPixel(20, 0), WorldToPixel(18, 1), 2, 1);
	bonequinhos = InsertCharacter(bonequinhos, woman3, WorldToPixel(2, 0), WorldToPixel(13, 1), 2, 1);
	bonequinhos = InsertCharacter(bonequinhos, woman4, WorldToPixel(18, 0), WorldToPixel(14, 1), 2, 1);
	
	//boneco->path = FindPath(PixelToWorld(boneco->x, 0), PixelToWorld(boneco->y, 1), 0, 3);


	//GAME LOOP
	while (!exitGame){

		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			UpdateMap();

			UpdateFarms(quintas);

			UpdateCharacters(bonequinhos);

			UpdateBuildings(edificios);

			al_clear_to_color(al_map_rgb(0, 0, 0));

			DrawMap();

			DrawCharacters(bonequinhos);

			UpdateInput(); //Desenha tamb�m hovers do rato

			if (bonecoSelecionado != NULL){
				al_draw_rectangle(bonecoSelecionado->x + offsetX,bonecoSelecionado->y + offsetY,bonecoSelecionado->x + 16 + offsetX,bonecoSelecionado->y + 24 + offsetY,
					RED, 2);

				if (!bonecoHovered){
					DrawHoveredTile();
				}
			}

			if (bonecoSelecionado){
				DrawNoWalkConstructionTiles();
			}

			al_flip_display();
		}
	}
}