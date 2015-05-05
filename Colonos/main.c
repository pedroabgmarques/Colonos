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

//Variáveis Globais
//ALLEGRO
ALLEGRO_FONT *font;
ALLEGRO_KEYBOARD_STATE state;
//OUTRAS
int exitGame = 0;


//Criar um display para o Allegro
ALLEGRO_DISPLAY *display = NULL;

//Cores
ALLEGRO_COLOR RED, BLACK, ORANGE;

//Destroi os objetos criados
void shutDown(){
	if (display){
		al_destroy_display(display);
	}
	if (font){
		al_destroy_font(font);
	}
}

int main(int argc, char **argv){

	//Inicializar o allegro
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
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

	while (!exitGame){

		printf("Isto corre!\n");

		al_clear_to_color(al_map_rgb(63, 20, 42));
		al_draw_rectangle(5 * 10, 5 * 10, 5 * 10 + 10, 5 * 10 + 10, BLACK, 2);
		al_flip_display();

		al_get_keyboard_state(&state);
		if (al_key_down(&state, ALLEGRO_KEY_ESCAPE)){
			exitGame = 1;
			shutDown();
		}
	}

}