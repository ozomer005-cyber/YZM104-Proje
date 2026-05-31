# Hafıza Kartı Oyunu - YZM104 Projesi

## Proje Hakkında
Bu proje, C programlama dili ve **Allegro 5** grafik kütüphanesi kullanılarak geliştirilmiş 4x4'lük bir Hafıza Kartı (Memory Card) oyunudur. Oyuncunun amacı, kapalı olarak verilen 16 kart (8 eşleşen çift) arasından doğru olanları en kısa sürede ve en az hamle ile bulmaktır.

## Öne Çıkan Özellikler
* **Tam Grafiksel Arayüz:** Sıkıcı konsol ekranı yerine Allegro 5 motoru ile tasarlanmış, görsellerin (.png) kullanıldığı modern arayüz.
* **Canlı Sayaçlar:** Oyun esnasında ekranda anlık olarak geçen süreyi ve yapılan hamle sayısını takip etme imkanı.
* **Dinamik Liderlik Tablosu (Leaderboard):** Oyun bitiminde elde edilen skorları `skor_tablosu.txt` dosyasına, Bubble Sort algoritması ile *en kısa sürede bitirenden en uzun sürede bitirene doğru* sıralayarak kaydetme sistemi.
* **Game Over (Oyun Bitiş) Ekranı:** Tüm eşleşmeler bulunduğunda arka planın kararmasıyla ortaya çıkan şeffaf "Tebrikler" ekranı.

## Gereksinimler
Bu projeyi kendi bilgisayarınızda derleyip çalıştırabilmek için aşağıdaki araçlara sahip olmalısınız:
* C Derleyicisi (Örn: GCC / MinGW)
* **Allegro 5 Kütüphanesi** ve şu eklentileri:
  * `allegro_primitives` (Şekil çizimleri için)
  * `allegro_font` (Yazı yazdırmak için)
  * `allegro_image` (PNG resimlerini yüklemek için)

## Kurulum ve Çalıştırma
1. Proje dosyalarını (`main.c`, `Makefile` ve tüm `.png` görsel dosyalarını) aynı klasöre alın.
2. Terminali bu klasörün dizininde açın.
3. Makefile dosyasını kullanarak projeyi tek tıkla derleyip çalıştırmak için şu komutu girin:
   ```bash
   mingw32-make run

   Nasıl Oynanır?
Oyun çalıştırıldığında siyah konsol ekranına adınızı ve soyadınızı girip Enter tuşuna basın.

Açılan pencerede farenin sol tuşunu kullanarak kapalı kartlara tıklayın.

Seçtiğiniz iki kart aynıysa açık kalır, farklıysa 0.8 saniye sonra otomatik olarak kapanır.

Toplam 8 çifti bulduğunuzda ekrana bitiş süreniz ve hamle sayınız yansır, skorunuz txt dosyasına yedeklenir. Pencereyi kapatarak oyundan çıkabilirsiniz.