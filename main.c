#include <stdio.h>
#include <allegro5/allegro.h>

int main() {
    // 1. Allegro'yu başlatıyoruz. Başarısız olursa programı durduruyoruz.
    if (!al_init()) {
        printf("Hata: Allegro baslatilamadi!\n");
        return -1;
    }

    // 2. Pencereyi (Display) temsil edecek bir pointer oluşturuyoruz.
    ALLEGRO_DISPLAY *ekran = al_create_display(800, 600);
    
    // Pencere oluşturulabildi mi diye kontrol ediyoruz.
    if (!ekran) {
        printf("Hata: Pencere olusturulamadi!\n");
        return -1;
    }

    // 3. Pencerenin başlığını ayarlayalım
    al_set_window_title(ekran, "Hafiza Karti Oyunu - YZM104");

    // 4. Arka planı belirli bir renge (örneğin koyu gri) boyuyoruz.
    // RGB renk kodları kullanıyoruz: R:50, G:50, B:50
    al_clear_to_color(al_map_rgb(50, 50, 50));

    // 5. Yaptığımız çizimlerin/boyamaların ekrana yansıması için buffer'ı çeviriyoruz.
    al_flip_display();

    // 6. Pencerenin hemen kapanmaması için 5 saniye (5.0) bekletiyoruz.
    al_rest(5.0);

    // 7. Hafıza sızıntısı (memory leak) olmaması için oluşturduğumuz ekranı yok ediyoruz.
    al_destroy_display(ekran);

    return 0;
}