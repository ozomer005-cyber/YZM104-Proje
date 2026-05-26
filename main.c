#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#define SATIR 4
#define SUTUN 4
#define KART_BOYUTU 100
#define BOSLUK 20

typedef struct {
    int id;
    int durum; 
} Kart;

int main() {
    char oyuncuAdSoyad[100];
    printf("Adinizi girin: ");
    scanf(" %[^\n]", oyuncuAdSoyad);

    al_init();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();

    ALLEGRO_DISPLAY *ekran = al_create_display(600, 600);
    al_set_window_title(ekran, "Hafiza Karti Oyunu - YZM104");
    ALLEGRO_FONT *font = al_create_builtin_font();
    ALLEGRO_EVENT_QUEUE *kuyruk = al_create_event_queue();
    al_register_event_source(kuyruk, al_get_display_event_source(ekran));

    Kart tahta[SATIR][SUTUN];
    int idListesi[16];
    for (int i = 0; i < 16; i++) idListesi[i] = i / 2; 

    srand(time(NULL));
    for (int i = 15; i > 0; i--) {
        int r = rand() % (i + 1);
        int gecici = idListesi[i];
        idListesi[i] = idListesi[r];
        idListesi[r] = gecici;
    }

    int sayac = 0;
    for (int i = 0; i < SATIR; i++) {
        for (int j = 0; j < SUTUN; j++) {
            tahta[i][j].id = idListesi[sayac];
            tahta[i][j].durum = 0;
            sayac++;
        }
    }

    bool calisiyor = true;
    while (calisiyor) {
        al_clear_to_color(al_map_rgb(40, 40, 40)); 
        
        for (int i = 0; i < SATIR; i++) {
            for (int j = 0; j < SUTUN; j++) {
                int x = 60 + j * (KART_BOYUTU + BOSLUK); 
                int y = 60 + i * (KART_BOYUTU + BOSLUK); 
                al_draw_filled_rectangle(x, y, x + KART_BOYUTU, y + KART_BOYUTU, al_map_rgb(100, 100, 100));
            }
        }
        al_flip_display();

        ALLEGRO_EVENT event;
        al_wait_for_event(kuyruk, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            calisiyor = false;
        } 
    }

    al_destroy_display(ekran);
    al_destroy_font(font);
    al_destroy_event_queue(kuyruk);
    return 0;
}