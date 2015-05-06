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
ALLEGRO_BITMAP *depleted_rock = NULL;
ALLEGRO_BITMAP *chopped_trees = NULL;

//Cores
ALLEGRO_COLOR RED, BLACK, ORANGE;

//Tamanho do mapa
#define MAPWIDTH 20
#define MAPHEIGHT 15
#define TILEWIDTH 32
#define TILEHEIGHT 32

// *********************************************************************************************************** //
// ESTRUTURAS DE DADOS //

// Descreve uma tile do mapa
typedef struct tile
{
	BITMAP *pict; // pointer to the bitmap of the tile
}* Tile;

//Matriz que define o mapa
int mapDef[MAPWIDTH][MAPHEIGHT] = { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 9, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
									{ 1, 11, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
									{ 1, 11, 10, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
									{ 1, 1, 1, 10, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
									{ 1, 1, 9, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 1, 3, 3, 6, 5, 1, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 1, 4, 2, 7, 4, 2, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 2, 5, 0, 0, 0, 7, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 3, 7, 0, 0, 0, 8, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 1, 6, 1, 0, 0, 6, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 1, 8, 2, 3, 4, 5, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
									{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }};

//Matriz que guarda os tiles do mapa
Tile mapa[MAPWIDTH][MAPHEIGHT];

// *********************************************************************************************************** //

//Inicialização do Allegro
void InitializeAllegro(){
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
	display = al_create_display(640, 480);
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
	depleted_rock = al_load_bitmap("assets/depleted_rock.png");
	chopped_trees = al_load_bitmap("assets/chopped_trees.png");
}

//Criar o mapa
void CreateMap(){
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
			case 13: tile->pict = depleted_rock;
				break;
			default:
				tile->pict = grass;
				break;
			}
			mapa[i][j] = tile;
		}
	}
}

//Desenhar o mapa
void DrawMap(){
	al_clear_to_color(al_map_rgb(0, 0, 0));
	for (int i = 0; i < MAPWIDTH; i++){
		for (int j = 0; j < MAPHEIGHT; j++){
			al_draw_bitmap(mapa[i][j]->pict, i * TILEWIDTH, j * TILEHEIGHT, 0);
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

	//Criar mapa
	CreateMap();

	//GAME LOOP
	while (!exitGame){

		printf("Isto corre!\n");

		DrawMap();

		al_get_keyboard_state(&state);
		if (al_key_down(&state, ALLEGRO_KEY_ESCAPE)){
			exitGame = 1;
			ShutDown();
		}
	}

}