#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> 

// Metin kopyalama (strcpy) ve metin uzunlugu (strlen) islemleri
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> 
// Ekrana geometrik sekiller cizmek icin gerekli eklenti
#include <allegro5/allegro_font.h>       
// Ekrana yazi (Sure, Hamle sayisi) yazdirmak icin gerekli eklenti
#include <allegro5/allegro_image.h>      
// Kartlarin gorsellerini (.png) yuklemek ve ekrana cizmek icin eklenti

// --- OYUN AYARLARI (MAKROLAR) ---
#define SATIR 4          
// Oyun tahtasinin dikeydeki kart sayisi

#define SUTUN 4          
// Oyun tahtasinin yataydaki kart sayisi

#define KART_BOYUTU 100  
// Her bir kartin ekrandaki genislik ve yukseklik piksel degeri

#define BOSLUK 20        
// Kartlarin birbirine yapismamasi icin aralarina konulacak mesafe


// --- STRUCT (YAPI) TANIMLAMALARI ---

// Her bir kartin arka plandaki mantiksal durumunu tutan yapi
typedef struct {
    int id;          
    // Kartin eslestirme numarasi (0 ile 7 arasinda degisir)
    
    int durum;       
    // Kartin anlik hali -> 0: Kapali, 1: Acik, 2: Eslesti
} Kart;

// Skor tablosunu dosyadan okurken ve siralarken kullanacagimiz yapi
typedef struct {
    char ad[100];    
    // Oyuncunun ad soyad bilgisi
    
    int sure;        
    // Oyunu kac saniyede bitirdigi
    
    int hamle;       
    // Oyunu kac hamlede bitirdigi
} SkorBilgisi;


int main() {
    char oyuncuAdSoyad[100];

    // Grafik ekrani acilmadan once terminal uzerinden kullanici verisini topluyoruz
    printf("--- HAFIZA KARTI OYUNUNA HOS GELDINIZ ---\n");
    printf("Lutfen adinizi ve soyadinizi girin: ");
    
    // %[^\n] ifadesi, kullanici Enter'a basana kadar bosluklarla okumayi saglar
    scanf(" %[^\n]", oyuncuAdSoyad);
    printf("\nBasarilar %s! Oyun basliyor...\n", oyuncuAdSoyad);

    // --- ALLEGRO KUTUPHANELERINI VE EKLENTILERINI BASLATMA ---
    al_init();                  
    al_install_mouse();         
    al_init_primitives_addon(); 
    al_init_font_addon();       
    al_init_image_addon();      

    // 600x600 piksel boyutlarinda oyun penceresini olusturuyoruz
    ALLEGRO_DISPLAY *ekran = al_create_display(600, 600);
    al_set_window_title(ekran, "Hafiza Karti Oyunu - YZM104"); 

    ALLEGRO_FONT *font = al_create_builtin_font();       
    ALLEGRO_EVENT_QUEUE *kuyruk = al_create_event_queue(); 
    ALLEGRO_TIMER *zamanlayici = al_create_timer(1.0);  

    // --- RESIMLERI BILGISAYARIN BELLEGINER (RAM) YUKLEME ---
    
    // Kartlarin arkasindaki standart kapali gorsel
    ALLEGRO_BITMAP *arkaPlan = al_load_bitmap("arka.png");
    
    if (!arkaPlan) {
        printf("HATA: 'arka.png' dosyasi bulunamadi! Proje klasorunu kontrol edin.\n");
        return -1;
    }

    // 8 farkli kart gorselini dongu yardimiyla dinamik olarak yukluyoruz
    ALLEGRO_BITMAP *kartResimleri[8];
    
    for (int i = 0; i < 8; i++) {
        char dosyaAdi[20];
        sprintf(dosyaAdi, "%d.png", i); 
        // Dongu sayisina gore dosya adini metne cevirir (orn: "0.png")
        
        kartResimleri[i] = al_load_bitmap(dosyaAdi);
        
        if (!kartResimleri[i]) {
            printf("HATA: '%s' gorsel dosyasi bulunamadi!\n", dosyaAdi);
            return -1;
        }
    }

    // Olay kuyruguna neleri takip etmesi gerektigini bildiriyoruz
    al_register_event_source(kuyruk, al_get_display_event_source(ekran)); 
    al_register_event_source(kuyruk, al_get_mouse_event_source());        
    al_register_event_source(kuyruk, al_get_timer_event_source(zamanlayici)); 

    // --- MATRIS MANTIGI VE KARTLARI RASGELE DAGITMA (SHUFFLE) ---
    Kart tahta[SATIR][SUTUN]; 
    int idListesi[16];        
    
    // 0'dan 7'ye kadar olan her numarayi diziye ikiser kez yaziyoruz
    for (int i = 0; i < 16; i++) {
        idListesi[i] = i / 2; 
    }

    // Fisher-Yates Shuffle algoritmasi ile diziyi karistiriyoruz
    srand(time(NULL)); 
    
    for (int i = 15; i > 0; i--) {
        int r = rand() % (i + 1); 
        
        int gecici = idListesi[i];
        idListesi[i] = idListesi[r];
        idListesi[r] = gecici; 
    }

    // Karistirilmis numaralari 4x4'luk matrise yerlestiriyoruz
    int sayac = 0;
    
    for (int i = 0; i < SATIR; i++) {
        for (int j = 0; j < SUTUN; j++) {
            tahta[i][j].id = idListesi[sayac];
            tahta[i][j].durum = 0; 
            // Oyundaki tum kartlar baslangicta kapali (0) konumdadir
            sayac++;
        }
    }

    // --- ANA OYUN DEGISKENLERI ---
    bool calisiyor = true;       
    bool oyunBitti = false; // OYUN BITIS DURUMUNU TUTAN YENI BAYRAK
    
    int acikKartSayisi = 0;      
    
    int ilkSatir = -1;
    int ilkSutun = -1;       
    
    int ikinciSatir = -1;
    int ikinciSutun = -1; 
    
    int eslesenCiftler = 0;      
    int hamleSayisi = 0;         
    int gecenSure = 0;           

    al_start_timer(zamanlayici); 

    // --- ANA OYUN DONGUSU (GAME LOOP) ---
    while (calisiyor) {
        
        // --- 1. ADIM: EKRANI CIZDIRME ASAMASI ---
        al_clear_to_color(al_map_rgb(40, 40, 40)); 
        
        // Ekranın en ust orta kismina canli kronometre ve hamle sayisini yazdiriyoruz
        al_draw_textf(font, al_map_rgb(255, 255, 255), 300, 20, ALLEGRO_ALIGN_CENTER, 
                      "Gecen Sure: %d sn   |   Hamle: %d", gecenSure, hamleSayisi);

        // Ic ice iki for dongusuyle 4x4 oyun tahtasini ekrana ciziyoruz
        for (int i = 0; i < SATIR; i++) {
            for (int j = 0; j < SUTUN; j++) {
                
                int x = 60 + j * (KART_BOYUTU + BOSLUK); 
                int y = 60 + i * (KART_BOYUTU + BOSLUK); 
                
                if (tahta[i][j].durum == 0) {
                    // KART KAPALIYSA: Ekrana arka.png kapak resmini ciz
                    al_draw_scaled_bitmap(arkaPlan, 0, 0, al_get_bitmap_width(arkaPlan), al_get_bitmap_height(arkaPlan), 
                                          x, y, KART_BOYUTU, KART_BOYUTU, 0);
                } 
                else {
                    // KART ACIK VEYA ESLESMISSE: O karta ait olan resmi ekrana bas
                    ALLEGRO_BITMAP *guncelResim = kartResimleri[tahta[i][j].id];
                    al_draw_scaled_bitmap(guncelResim, 0, 0, al_get_bitmap_width(guncelResim), al_get_bitmap_height(guncelResim), 
                                          x, y, KART_BOYUTU, KART_BOYUTU, 0);
                }
            }
        }

        // EGER OYUN BITTIYSE EKRANIN UZERINE YARI SAYDAM SIYAH BIR PERDE CEK VE TEBRIKLER YAZISI YAZ
        if (oyunBitti) {
            al_draw_filled_rectangle(0, 0, 600, 600, al_map_rgba(0, 0, 0, 200));
            al_draw_textf(font, al_map_rgb(255, 215, 0), 300, 260, ALLEGRO_ALIGN_CENTER, "TEBRIKLER %s!", oyuncuAdSoyad);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 300, 290, ALLEGRO_ALIGN_CENTER, "Gorevi %d saniyede ve %d hamlede tamamladin.", gecenSure, hamleSayisi);
            al_draw_text(font, al_map_rgb(150, 150, 150), 300, 330, ALLEGRO_ALIGN_CENTER, "Oyundan cikmak icin pencereyi kapatabilirsin.");
        }

        al_flip_display(); 

        // --- 2. ADIM: KART KARSILASTIRMA MANTIGI ---
        if (acikKartSayisi == 2 && !oyunBitti) {
            
            al_rest(0.8); 
            // Kartlarin gorsellerini gormesi icin 0.8 saniye bekle

            if (tahta[ilkSatir][ilkSutun].id == tahta[ikinciSatir][ikinciSutun].id) {
                // KARTLAR ESLESTI
                tahta[ilkSatir][ilkSutun].durum = 2;
                tahta[ikinciSatir][ikinciSutun].durum = 2;
                eslesenCiftler++; 
            } 
            else {
                // KARTLAR ESLESMEDI
                tahta[ilkSatir][ilkSutun].durum = 0;
                tahta[ikinciSatir][ikinciSutun].durum = 0;
            }
            
            acikKartSayisi = 0; 
        }

        // --- 3. ADIM: KAZANMA DURUMU KONTROLU ---
        if (eslesenCiftler == 8 && !oyunBitti) {
            // Oyun bitis aninda yapilacak tek seferlik islemler
            oyunBitti = true;
            al_stop_timer(zamanlayici); // Saati durdur

            // --- OYUN BITIS ANINDA DINAMIK LIDERLIK TABLOSUNU GUNCELLEME ---
            SkorBilgisi skorlar[100]; 
            int skorSayisi = 0;

            FILE *dosya = fopen("skor_tablosu.txt", "r");
            if (dosya != NULL) {
                char satirOkuma[256];
                while (fgets(satirOkuma, sizeof(satirOkuma), dosya) && skorSayisi < 100) {
                    if (sscanf(satirOkuma, "Oyuncu: %[^|]| Sure: %d sn | Hamle Sayisi: %d", 
                               skorlar[skorSayisi].ad, &skorlar[skorSayisi].sure, &skorlar[skorSayisi].hamle) == 3) {
                        int uzunluk = strlen(skorlar[skorSayisi].ad);
                        if (uzunluk > 0 && skorlar[skorSayisi].ad[uzunluk - 1] == ' ') {
                            skorlar[skorSayisi].ad[uzunluk - 1] = '\0';
                        }
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
                printf("\nLiderlik tablosu basariyla guncellendi ve siralandi!\n");
            }
        }

        // --- 4. ADIM: OLAY (EVENT) YAKALAMA VE ETKILESIM ---
        ALLEGRO_EVENT event;
        al_wait_for_event(kuyruk, &event); 

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            calisiyor = false; 
        } 
        else if (event.type == ALLEGRO_EVENT_TIMER) {
            gecenSure++; 
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && acikKartSayisi < 2 && !oyunBitti) {
            // Eger oyun bitmisse fare tiklamalari iptal edilir (!oyunBitti sarti eklendi)
            int mx = event.mouse.x; 
            int my = event.mouse.y; 
            
            int c = (mx - 60) / (KART_BOYUTU + BOSLUK); 
            int r = (my - 60) / (KART_BOYUTU + BOSLUK); 

            if (r >= 0 && r < SATIR && c >= 0 && c < SUTUN) {
                
                if (tahta[r][c].durum == 0) { 
                    
                    tahta[r][c].durum = 1; 
                    
                    if (acikKartSayisi == 0) {
                        ilkSatir = r; 
                        ilkSutun = c; 
                    } 
                    else {
                        ikinciSatir = r; 
                        ikinciSutun = c; 
                        hamleSayisi++; 
                    }
                    acikKartSayisi++; 
                }
            }
        }
    }

    // --- BELLEK TEMIZLIGI (MEMORY MANAGEMENT) ---
    al_destroy_bitmap(arkaPlan);
    
    for (int i = 0; i < 8; i++) {
        al_destroy_bitmap(kartResimleri[i]);
    }
    
    al_destroy_timer(zamanlayici);
    al_destroy_display(ekran);
    al_destroy_font(font);
    al_destroy_event_queue(kuyruk);

    return 0;
}