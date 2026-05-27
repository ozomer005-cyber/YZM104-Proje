#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

// Oyun tahtasinin boyutlari
#define SATIR 4
#define SUTUN 4
#define KART_BOYUTU 100
#define BOSLUK 20

// Kart yapimiz
typedef struct {
    int id;          // Eslesen kartlarin ortak numarasi (0-7 arasi)
    int durum;       // 0: Kapali, 1: Acik, 2: Eslesti
} Kart;

int main() {
    char oyuncuAdSoyad[100];

    printf("--- HAFIZA KARTI OYUNUNA HOS GELDINIZ ---\n");
    printf("Lutfen adinizi ve soyadinizi girin: ");
    
    // Kullanicidan veriyi eksiksiz (bosluklariyla) almak icin
    scanf(" %[^\n]", oyuncuAdSoyad);
    printf("\nBasarilar %s! Oyun basliyor...\n", oyuncuAdSoyad);

    // --- ALLEGRO KURULUMLARI ---
    al_init();
    al_install_mouse(); // Fareyi sisteme tanitiyoruz
    al_init_primitives_addon(); // Sekil cizmek icin
    al_init_font_addon();       // Yazi yazmak icin

    ALLEGRO_DISPLAY *ekran = al_create_display(600, 600);
    al_set_window_title(ekran, "Hafiza Karti Oyunu - YZM104");

    ALLEGRO_FONT *font = al_create_builtin_font();
    ALLEGRO_EVENT_QUEUE *kuyruk = al_create_event_queue();

    al_register_event_source(kuyruk, al_get_display_event_source(ekran));
    al_register_event_source(kuyruk, al_get_mouse_event_source());

    // --- OYUN MANTIGI VE KARTLARI DAGITMA ---
    Kart tahta[SATIR][SUTUN];
    int idListesi[16];
    
    // 0'dan 7'ye kadar olan sayilardan ikiser tane olusturuyoruz (Ciftler)
    for (int i = 0; i < 16; i++) {
        idListesi[i] = i / 2; 
    }

    // Kartlari karistirma (Shuffle)
    srand(time(NULL));
    for (int i = 15; i > 0; i--) {
        int r = rand() % (i + 1);
        int gecici = idListesi[i];
        idListesi[i] = idListesi[r];
        idListesi[r] = gecici;
    }

    // Karistirilmis desteyi matrise (oyun tahtasina) yerlestirme
    int sayac = 0;
    for (int i = 0; i < SATIR; i++) {
        for (int j = 0; j < SUTUN; j++) {
            tahta[i][j].id = idListesi[sayac];
            tahta[i][j].durum = 0; // Hepsi kapali baslar
            sayac++;
        }
    }

    // --- OYUN DONGUSU DEGISKENLERI ---
    bool calisiyor = true;
    int acikKartSayisi = 0;
    int ilkSatir = -1, ilkSutun = -1;
    int ikinciSatir = -1, ikinciSutun = -1;
    int eslesenCiftler = 0;
    int hamleSayisi = 0;

    // --- ANA OYUN DONGUSU ---
    while (calisiyor) {
        // CIZIM ASAMASI
        al_clear_to_color(al_map_rgb(40, 40, 40)); // Arka plan
        
        for (int i = 0; i < SATIR; i++) {
            for (int j = 0; j < SUTUN; j++) {
                int x = 60 + j * (KART_BOYUTU + BOSLUK); // X koordinati (offsetli)
                int y = 60 + i * (KART_BOYUTU + BOSLUK); // Y koordinati
                
                if (tahta[i][j].durum == 0) {
                    // Kapali Kart (Gri Renk)
                    al_draw_filled_rectangle(x, y, x + KART_BOYUTU, y + KART_BOYUTU, al_map_rgb(100, 100, 100));
                } else {
                    // Acik veya Eslesmis Kart (Beyaz Renk ve ID numarasi)
                    al_draw_filled_rectangle(x, y, x + KART_BOYUTU, y + KART_BOYUTU, al_map_rgb(220, 220, 220));
                    al_draw_textf(font, al_map_rgb(0, 0, 0), x + 50, y + 45, ALLEGRO_ALIGN_CENTER, "%d", tahta[i][j].id);
                }
            }
        }
        al_flip_display(); // Cizimleri ekrana bas

        // KONTROL VE BEKLEME ASAMASI (Eger 2 kart acildiysa karsilastir)
        if (acikKartSayisi == 2) {
            al_rest(0.8); // Oyuncunun kartlari gormesi icin kisa bir sure bekle

            if (tahta[ilkSatir][ilkSutun].id == tahta[ikinciSatir][ikinciSutun].id) {
                // Eslesti
                tahta[ilkSatir][ilkSutun].durum = 2;
                tahta[ikinciSatir][ikinciSutun].durum = 2;
                eslesenCiftler++;
            } else {
                // Eslesmedi, geri kapat
                tahta[ilkSatir][ilkSutun].durum = 0;
                tahta[ikinciSatir][ikinciSutun].durum = 0;
            }
            acikKartSayisi = 0; // Yeni hamle icin sifirla
        }

        // KAZANMA DURUMU
        if (eslesenCiftler == 8) {
            printf("\nTEBRIKLER! Tum esleri buldun!\n");
            calisiyor = false;
        }

        // KULLANICI ETKILESIMI (Fare ve Pencere)
        ALLEGRO_EVENT event;
        al_wait_for_event(kuyruk, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            calisiyor = false; // X tusuna basilirsa cik
        } 
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && acikKartSayisi < 2) {
            // Farenin X ve Y konumunu matris indeksine donusturme formulu
            int mx = event.mouse.x;
            int my = event.mouse.y;
            
            int c = (mx - 60) / (KART_BOYUTU + BOSLUK);
            int r = (my - 60) / (KART_BOYUTU + BOSLUK);

            // Tiklama kartlarin ustunde mi diye kontrol et
            if (r >= 0 && r < SATIR && c >= 0 && c < SUTUN) {
                if (tahta[r][c].durum == 0) { // Sadece kapali karta tiklanabilir
                    tahta[r][c].durum = 1;
                    if (acikKartSayisi == 0) {
                        ilkSatir = r; ilkSutun = c;
                    } else {
                        ikinciSatir = r; ikinciSutun = c;
                        hamleSayisi++; // Iki kart da secilince 1 hamle sayilir
                    }
                    acikKartSayisi++;
                }
            }
        }
    }

    // --- OYUN SONU: SKORU DOSYAYA YAZDIRMA ---
    FILE *dosya = fopen("skor_tablosu.txt", "a");
    if (dosya != NULL) {
        fprintf(dosya, "Oyuncu: %s | Hamle Sayisi: %d\n", oyuncuAdSoyad, hamleSayisi);
        fclose(dosya);
        printf("Skorun 'skor_tablosu.txt' dosyasina basariyla kaydedildi!\n");
    }

    // Bellek temizligi
    al_destroy_display(ekran);
    al_destroy_font(font);
    al_destroy_event_queue(kuyruk);

    return 0;
}