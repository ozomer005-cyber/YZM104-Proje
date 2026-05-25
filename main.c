#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

// Kart yapisi iskeleti hazirlandi
typedef struct {
    int id;
    int durum; 
} Kart;

int main() {
    char oyuncuAdSoyad[100];

    printf("--- HAFIZA KARTI OYUNUNA HOS GELDINIZ ---\n");
    printf("Lutfen adinizi ve soyadinizi girin: ");
    scanf(" %[^\n]", oyuncuAdSoyad);
    printf("\nBasarilar %s! Oyun basliyor...\n", oyuncuAdSoyad);

    al_init();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();

    ALLEGRO_DISPLAY *ekran = al_create_display(600, 600);
    al_set_window_title(ekran, "Hafiza Karti Oyunu - YZM104");

    ALLEGRO_EVENT_QUEUE *kuyruk = al_create_event_queue();
    al_register_event_source(kuyruk, al_get_display_event_source(ekran));

    bool calisiyor = true;
    while (calisiyor) {
        al_clear_to_color(al_map_rgb(40, 40, 40)); 
        al_flip_display();

        ALLEGRO_EVENT event;
        al_wait_for_event(kuyruk, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            calisiyor = false;
        } 
    }

    al_destroy_display(ekran);
    al_destroy_event_queue(kuyruk);
    return 0;
}