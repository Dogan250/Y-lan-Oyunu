# Yilan-Oyunu
Bu projede esp32 s3 kartı ile yılan oyunu yaptım. Gömülü sistemler için giriş seviyesinde bir proje yapmak istedim
Nasıl Yapılır?
-# ESP32 ile OLED Ekranda Yılan Oyunu (Rotary Encoder Kontrollü) 

Bu projede, standart butonlar yerine bir **KY-040 Rotary Encoder (Döner Kodlayıcı)** kullanarak ESP32 ve 0.96 inç OLED ekran üzerinde çalışan bir Yılan oyunu geliştirdim.

##  Neden Rotary Encoder? Neden Interrupt?
Yön kontrolü için 4 farklı buton kullanmak yerine tek bir döner düğme kullandım (sağa çevirince yılan saat yönünde, sola çevirince tersine dönüyor). Oyunun kendi `delay(100)` döngüsü yüzünden çevirme hareketleri kaçmasın diye **Donanımsal Kesme (Hardware Interrupt)** kullanarak Rotary Encoder pinlerini anlık olarak dinledim. Menü geçişini ve yandığında oyunu sıfırlama işini de encoder'ın üzerindeki butona (SW) atadım.

##  Donanım & Kurulum Notlarım (ÖNEMLİ)
Devreyi kurarken en çok zorlandığım kısım ESP32'nin breadboard üzerinde hiç yer bırakmamasıydı. Bunu çözmek için ESP32'yi breadboard'un dışında tuttum. Breadboard'un şeritlerini sadece 3.3V ve GND gücünü dağıtmak için kullandım, geri kalan veri kablolarını direkt ESP32'ye çektim. 
*(ESP32 pinlerini yakmamak için KY-040'ı 5V yerine kesinlikle 3.3V ile besledim).*

### Pin Bağlantıları:
**OLED Ekran (I2C):**
*   `VCC` -> Breadboard 3.3V Hattı
*   `GND` -> Breadboard GND Hattı
*   `SDA` -> ESP32 GPIO 17
*   `SCL` -> ESP32 GPIO 18

**KY-040 Rotary Encoder:**
*   `+` -> Breadboard 3.3V Hattı
*   `GND` -> Breadboard GND Hattı
*   `CLK` -> ESP32 GPIO 4
*   `DT`  -> ESP32 GPIO 5
*   `SW`  -> ESP32 GPIO 6 (Buton)

##  Çalışma Videosu / Fotoğraflar
-jpeg resimlerinde fotoğraflar var.

##  Nasıl Çalıştırılır?
1. Repoyu klonlayın.
2. Arduino IDE üzerinden `Adafruit_GFX` ve `Adafruit_SSD1306` kütüphanelerini kurun.
3. Kodu ESP32'ye yükleyin. (ESP32-S3 vb. kullanıyorsanız Tools menüsünden `USB CDC On Boot: Enabled` yapmayı unutmayın)
