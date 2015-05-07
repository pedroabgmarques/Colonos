#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

//Variáveis Globais
//ALLEGRO
ALLEGRO_FONT *font;
ALLEGRO_KEYBOARD_STATE state;
//OUTRAS
int exitGame = 0;

//Criar um display para o Allegro
ALLEGRO_DISPLAY *display = NULL;

//Assets
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

//Cores
ALLEGRO_COLOR RED, BLACK, ORANGE;

//Tamanho do mapa
#define MAPWIDTH 35
#define MAPHEIGHT 35
#define TILEWIDTH 32
#define TILEHEIGHT 32
#define DISPLAYWIDTH 800
#define DISPLAYHEIGHT 600

//Offset do ecra
float offsetX = 0;
float offsetY = 0;

// *********************************************************************************************************** //
// ESTRUTURAS DE DADOS //

// Descreve uma tile do mapa
typedef struct tile
{
	ALLEGRO_BITMAP *pict; // pointer to the bitmap of the tile
}* Tile;

//Descreve uma quinta
typedef struct farm
{
	int type; //Sprite do estado inicial
	int i, j; //Posicao da quinta no mapa
	int timer; //Contador de há quanto tempo a quinta está no estado atual
	int phase; //Fase em que a quinta se encontra (1, 2, 3)
	struct farm *next; //Apontador para o elemento seguinte
}* Farm;

//

//Matriz que define o mapa
//TODO: Ler mapa a partir de ficheiro
int mapDef[MAPWIDTH][MAPHEIGHT] = { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 9, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 11, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 11, 10, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 10, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 9, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 3, 3, 6, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 4, 2, 7, 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 2, 5, 0, 0, 0, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 3, 7, 0, 0, 0, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 6, 1, 0, 0, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 8, 2, 3, 4, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };

//Matriz que guarda os tiles do mapa
Tile mapa[MAPWIDTH][MAPHEIGHT];

//Lista ligada de quintas vazia
Farm quintas = NULL;

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
	ORANGE = al_map_rgb(255, 165, 0);

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
}

//Criar o mapa
void UpdateMap(){
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			Tile tile = (Tile)malloc(sizeof(struct tile));
			switch (mapDef[i][j])
			{
			case 0: tile->pict = water;
				break;
			case 1: tile->pict = grass;
				break;
			case 2: tile->pict = tree1;
				break;
			case 3: tile->pict = tree2;
				break;
			case 4: tile->pict = tree3;
				break;
			case 5: tile->pict = tree4;
				break;
			case 6: tile->pict = tree5;
				break;
			case 7: tile->pict = tree6;
				break;
			case 8: tile->pict = tree7;
				break;
			case 9: tile->pict = rock1;
				break;
			case 10: tile->pict = rock2;
				break;
			case 11: tile->pict = rock3;
				break;
			case 12: tile->pict = chopped_trees;
				break;
			case 14: tile->pict = farm1_1;
				break;
			case 15: tile->pict = farm1_2;
				break;
			case 16: tile->pict = farm1_3;
				break;
			case 17: tile->pict = farm2_1;
				break;
			case 18: tile->pict = farm2_2;
				break;
			case 19: tile->pict = farm2_3;
				break;
			case 20: tile->pict = farm3_1;
				break;
			case 21: tile->pict = farm3_2;
				break;
			case 22: tile->pict = farm3_3;
				break;
			case 23: tile->pict = farm4_1;
				break;
			case 24: tile->pict = farm4_2;
				break;
			case 25: tile->pict = farm4_3;
				break;
			case 26: tile->pict = farm5_1;
				break;
			case 27: tile->pict = farm5_2;
				break;
			case 28: tile->pict = farm5_3;
				break;
			case 29: tile->pict = farm6_1;
				break;
			case 30: tile->pict = farm6_2;
				break;
			case 31: tile->pict = farm6_3;
				break;
			case 32: tile->pict = farm7_1;
				break;
			case 33: tile->pict = farm7_2;
				break;
			case 34: tile->pict = farm7_3;
				break;
			default:
				tile->pict = grass;
				break;
			}
			mapa[i][j] = tile;
		}
	}
}

//Inserir quinta
Farm InsertFarm(Farm endereco, int i, int j, int type){
	//Aloca o espaço necessário na memória
	//Devolve o endereço de memória do espaço alocado
	Farm farm = (Farm)malloc(sizeof(struct farm));
	//Dar os valores ao farm
	farm->i = i;
	farm->j = j;
	farm->phase = 0;
	farm->timer = 0;
	farm->type = type;
	farm->next = endereco;
	mapDef[i][j] = type;
	return farm;
}

//Atualizar quintas
void UpdateFarms(Farm endereco){
	while (endereco != NULL){
		
		//incrementar timer
		endereco->timer++;
		//passar fase?
		if (endereco->timer > 1000){
			if (endereco->phase < 2){
				endereco->phase++;
				endereco->timer = 0;
				mapDef[endereco->i][endereco->j] = endereco->type + endereco->phase;
			}
		}
		endereco = endereco->next;
	}
}

//Desenhar o mapa
void DrawMap(){
	al_clear_to_color(al_map_rgb(0, 0, 0));
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			al_draw_bitmap(mapa[j][i]->pict, i * TILEWIDTH + offsetX * TILEWIDTH, j * TILEHEIGHT + offsetY * TILEHEIGHT, 0);
		}
	}
	al_flip_display();
}

//Destroi os objetos criados
void ShutDown(){
	if (display){
		al_destroy_display(display);
	}
	if (font){
		al_destroy_font(font);
	}
}

int main(int argc, char **argv){

	//INICIALIZAÇÃO
	InitializeAllegro();

	//Load assets
	LoadAssets();

	Farm quintas = NULL; //lista ligada vazia
	quintas = InsertFarm(quintas, 8, 1, 14);
	quintas = InsertFarm(quintas, 8, 2, 17);
	quintas = InsertFarm(quintas, 8, 2, 20);
	quintas = InsertFarm(quintas, 9, 1, 23);
	quintas = InsertFarm(quintas, 9, 2, 26);
	quintas = InsertFarm(quintas, 9, 3, 29);
	quintas = InsertFarm(quintas, 10, 1, 32);

	//GAME LOOP
	while (!exitGame){

		UpdateMap();

		UpdateFarms(quintas);

		DrawMap();

		al_get_keyboard_state(&state);
		if (al_key_down(&state, ALLEGRO_KEY_ESCAPE)){
			exitGame = 1;
			ShutDown();
		}
		if (al_key_down(&state, ALLEGRO_KEY_D)){
			printf("offsetX * TILEWIDTH: %f\n", offsetX * TILEWIDTH);
			if (-(offsetX * TILEWIDTH) < (MAPWIDTH * TILEWIDTH - DISPLAYWIDTH) + 2)
				offsetX -= 0.1;
		}
		if (al_key_down(&state, ALLEGRO_KEY_A)){
			printf("offsetX * TILEWIDTH: %f\n", offsetX * TILEWIDTH);
			if (-(offsetX * TILEWIDTH) > -2)
				offsetX += 0.1;
		}
		if (al_key_down(&state, ALLEGRO_KEY_S)){
			offsetY -= 0.05;
		}
		if (al_key_down(&state, ALLEGRO_KEY_W)){
			offsetY += 0.05;
		}
	}

}