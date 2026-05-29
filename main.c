#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> 
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h> // RESIMLER ICIN YENI KUTUPHANE

// Oyun tahtasinin boyutlari ve gorsel ayarlar
#define SATIR 4
#define SUTUN 4
#define KART_BOYUTU 100
#define BOSLUK 20

typedef struct {
    int id;          
    int durum;       
} Kart;

typedef struct {
    char ad[100];
    int sure;
    int hamle;
} SkorBilgisi;

int main() {
    char oyuncuAdSoyad[100];

    printf("--- HAFIZA KARTI OYUNUNA HOS GELDINIZ ---\n");
    printf("Lutfen adinizi ve soyadinizi girin: ");
    scanf(" %[^\n]", oyuncuAdSoyad);
    printf("\nBasarilar %s! Oyun basliyor...\n", oyuncuAdSoyad);

    // --- ALLEGRO BASLATMA VE KURULUM KISMI ---
    al_init();
    al_install_mouse(); 
    al_init_primitives_addon(); 
    al_init_font_addon(); 
    al_init_image_addon(); // RESIM EKLENTISINI BASLAT

    ALLEGRO_DISPLAY *ekran = al_create_display(600, 600);
    al_set_window_title(ekran, "Hafiza Karti Oyunu - YZM104");

    ALLEGRO_FONT *font = al_create_builtin_font();
    ALLEGRO_EVENT_QUEUE *kuyruk = al_create_event_queue();
    ALLEGRO_TIMER *zamanlayici = al_create_timer(1.0); 

    // --- RESIMLERI BELLEGE YUKLEME ---
    ALLEGRO_BITMAP *arkaPlan = al_load_bitmap("arka.png");
    if (!arkaPlan) {
        printf("HATA: 'arka.png' dosyasi bulunamadi! Lutfen klasoru kontrol et.\n");
        return -1;
    }

    ALLEGRO_BITMAP *kartResimleri[8];
    for (int i = 0; i < 8; i++) {
        char dosyaAdi[20];
        sprintf(dosyaAdi, "%d.png", i); // 0.png, 1.png diye otomatik isimlendirir
        kartResimleri[i] = al_load_bitmap(dosyaAdi);
        if (!kartResimleri[i]) {
            printf("HATA: '%s' dosyasi bulunamadi!\n", dosyaAdi);
            return -1;
        }
    }

    al_register_event_source(kuyruk, al_get_display_event_source(ekran));
    al_register_event_source(kuyruk, al_get_mouse_event_source());
    al_register_event_source(kuyruk, al_get_timer_event_source(zamanlayici));

    // --- OYUN MANTIGI VE KARTLARI DAGITMA ---
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
    int acikKartSayisi = 0;
    int ilkSatir = -1, ilkSutun = -1;
    int ikinciSatir = -1, ikinciSutun = -1;
    int eslesenCiftler = 0;
    int hamleSayisi = 0;
    int gecenSure = 0; 

    al_start_timer(zamanlayici); 

    // --- ANA OYUN DONGUSU ---
    while (calisiyor) {
        al_clear_to_color(al_map_rgb(40, 40, 40)); 
        
        al_draw_textf(font, al_map_rgb(255, 255, 255), 300, 20, ALLEGRO_ALIGN_CENTER, 
                      "Gecen Sure: %d sn   |   Hamle: %d", gecenSure, hamleSayisi);

        for (int i = 0; i < SATIR; i++) {
            for (int j = 0; j < SUTUN; j++) {
                int x = 60 + j * (KART_BOYUTU + BOSLUK); 
                int y = 60 + i * (KART_BOYUTU + BOSLUK); 
                
                if (tahta[i][j].durum == 0) {
                    // KART KAPALIYSA: arka.png resmini cizdir
                    al_draw_scaled_bitmap(arkaPlan, 0, 0, al_get_bitmap_width(arkaPlan), al_get_bitmap_height(arkaPlan), 
                                          x, y, KART_BOYUTU, KART_BOYUTU, 0);
                } else {
                    // KART ACIK VEYA ESLESMISSE: O karta ait numarali resmi (orn: 3.png) cizdir
                    ALLEGRO_BITMAP *guncelResim = kartResimleri[tahta[i][j].id];
                    al_draw_scaled_bitmap(guncelResim, 0, 0, al_get_bitmap_width(guncelResim), al_get_bitmap_height(guncelResim), 
                                          x, y, KART_BOYUTU, KART_BOYUTU, 0);
                    
                  
                }
            }
        }
        al_flip_display(); 

        if (acikKartSayisi == 2) {
            al_rest(0.8); 

            if (tahta[ilkSatir][ilkSutun].id == tahta[ikinciSatir][ikinciSutun].id) {
                tahta[ilkSatir][ilkSutun].durum = 2;
                tahta[ikinciSatir][ikinciSutun].durum = 2;
                eslesenCiftler++;
            } else {
                tahta[ilkSatir][ilkSutun].durum = 0;
                tahta[ikinciSatir][ikinciSutun].durum = 0;
            }
            acikKartSayisi = 0; 
        }

        if (eslesenCiftler == 8) {
            printf("\nTEBRIKLER! Tum esleri buldun!\n");
            calisiyor = false; 
        }

        ALLEGRO_EVENT event;
        al_wait_for_event(kuyruk, &event); 

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            calisiyor = false; 
        } 
        else if (event.type == ALLEGRO_EVENT_TIMER) {
            gecenSure++; 
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && acikKartSayisi < 2) {
            int mx = event.mouse.x;
            int my = event.mouse.y;
            
            int c = (mx - 60) / (KART_BOYUTU + BOSLUK);
            int r = (my - 60) / (KART_BOYUTU + BOSLUK);

            if (r >= 0 && r < SATIR && c >= 0 && c < SUTUN) {
                if (tahta[r][c].durum == 0) { 
                    tahta[r][c].durum = 1; 
                    
                    if (acikKartSayisi == 0) {
                        ilkSatir = r; ilkSutun = c; 
                    } else {
                        ikinciSatir = r; ikinciSutun = c; 
                        hamleSayisi++; 
                    }
                    acikKartSayisi++;
                }
            }
        }
    }

    // --- SKOR TABLOSU KAYDETME VE SIRALAMA (Ayni biraktik) ---
    SkorBilgisi skorlar[100]; 
    int skorSayisi = 0;

    FILE *dosya = fopen("skor_tablosu.txt", "r");
    if (dosya != NULL) {
        char satirOkuma[256];
        while (fgets(satirOkuma, sizeof(satirOkuma), dosya) && skorSayisi < 100) {
            if (sscanf(satirOkuma, "Oyuncu: %[^|]| Sure: %d sn | Hamle Sayisi: %d", 
                       skorlar[skorSayisi].ad, &skorlar[skorSayisi].sure, &skorlar[skorSayisi].hamle) == 3) {
                int uzunluk = strlen(skorlar[skorSayisi].ad);
                if (uzunluk > 0 && skorlar[skorSayisi].ad[uzunluk - 1] == ' ') skorlar[skorSayisi].ad[uzunluk - 1] = '\0';
                skorSayisi++;
            }
        }
        fclose(dosya);
    }

    strcpy(skorlar[skorSayisi].ad, oyuncuAdSoyad);
    skorlar[skorSayisi].sure = gecenSure;
    skorlar[skorSayisi].hamle = hamleSayisi;
    skorSayisi++;

    for (int i = 0; i < skorSayisi - 1; i++) {
        for (int j = 0; j < skorSayisi - i - 1; j++) {
            if (skorlar[j].sure > skorlar[j+1].sure || 
               (skorlar[j].sure == skorlar[j+1].sure && skorlar[j].hamle > skorlar[j+1].hamle)) {
                SkorBilgisi gecici = skorlar[j];
                skorlar[j] = skorlar[j+1];
                skorlar[j+1] = gecici;
            }
        }
    }

    dosya = fopen("skor_tablosu.txt", "w");
    if (dosya != NULL) {
        for (int i = 0; i < skorSayisi; i++) {
            fprintf(dosya, "Oyuncu: %s | Sure: %d sn | Hamle Sayisi: %d\n", 
                    skorlar[i].ad, skorlar[i].sure, skorlar[i].hamle);
        }
        fclose(dosya);
        printf("\nLiderlik tablosu guncellendi! Skorlar siralandi.\n");
    }

    // --- BELLEK TEMIZLIGI (RESIMLERI DE SILYORUZ) ---
    al_destroy_bitmap(arkaPlan);
    for (int i = 0; i < 8; i++) al_destroy_bitmap(kartResimleri[i]);
    
    al_destroy_timer(zamanlayici);
    al_destroy_display(ekran);
    al_destroy_font(font);
    al_destroy_event_queue(kuyruk);

    return 0;
}