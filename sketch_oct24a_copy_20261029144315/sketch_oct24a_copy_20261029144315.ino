#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ESP32 Pin bağlantılarım
// OLED ekran I2C pinleri
#define SDA_PIN 17
#define SCL_PIN 18

// KY-040 Rotary Encoder pinleri (Voltajı 3.3V verdim pinler yanmasın diye)
#define CLK_PIN 4
#define DT_PIN 5
#define SW_PIN 6  // Üzerindeki butona basma pini

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Yılanın ayarları
#define GRID_SIZE 4 // Yılan 4x4 piksellik karelerden oluşuyor
#define MAX_SNAKE 100

int snakeX[MAX_SNAKE];
int snakeY[MAX_SNAKE];
int snakeLength = 3;
int foodX, foodY;
bool gameOver = false;
int score = 0;

// Yılanın yönü: 0=Yukarı, 1=Sağ, 2=Aşağı, 3=Sol
volatile int direction = 1; 
volatile unsigned long lastInterruptTime = 0;

// Normal loop içinde okumaya kalkarsam delay(100) yüzünden dönüşleri kaçırıyordu.
// O yüzden Rotary Encoder çevrildiğinde anında tetiklenen interrupt (kesme) yazdım.
void IRAM_ATTR readEncoder() {
  unsigned long interruptTime = millis();
  // 50ms debounce koydum ki elim titrediğinde 2 kere dönmüş algılamasın
  if (interruptTime - lastInterruptTime > 50) { 
    if (digitalRead(DT_PIN) == digitalRead(CLK_PIN)) {
      direction = (direction + 1) % 4; // Sağa çevrildi
    } else {
      direction = (direction + 3) % 4; // Sola çevrildi
    }
    lastInterruptTime = interruptTime;
  }
}

void spawnFood() {
  // Ekranın dışına taşmayacak rastgele bir yere yem at
  foodX = random(0, (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
  foodY = random(0, (SCREEN_HEIGHT / GRID_SIZE)) * GRID_SIZE;
}

void resetGame() {
  snakeLength = 3;
  direction = 1;
  score = 0;
  gameOver = false;
  
  // Yılanı ekranın ortasından başlat
  for (int i = 0; i < snakeLength; i++) {
    snakeX[i] = (SCREEN_WIDTH / 2) - (i * GRID_SIZE);
    snakeY[i] = SCREEN_HEIGHT / 2;
  }
  spawnFood();
}

void setup() {
  Serial.begin(115200);

  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  pinMode(SW_PIN, INPUT_PULLUP);

  // CLK pininde voltaj düştüğü an (FALLING) kesmeyi tetikle
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), readEncoder, FALLING);

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED hatası! Kablolarda temassızlık olabilir.");
    while (true);
  }

  // Rastgele sayıların hep aynı sırayla gelmemesi için boş pinden gürültü okuyoruz
  randomSeed(analogRead(0)); 
  resetGame();
}

void loop() {
  display.clearDisplay();

  if (gameOver) {
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 20);
    display.print("OYUN BITTI");
    display.setTextSize(1);
    display.setCursor(15, 45);
    display.print("Skor: ");
    display.print(score);
    display.display();

    // Yanınca rotary encoder'ın butonuna basarsam oyunu sıfırla
    if (digitalRead(SW_PIN) == LOW) {
      delay(200); // Butona basınca menü sapıtmasın diye ufak bekleme
      resetGame();
    }
    return;
  }

  // Yılanın kuyruğunu başın gittiği yere doğru kaydır
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  // Yöne göre sadece yılanın başını (0. indeksi) hareket ettir
  if (direction == 0) snakeY[0] -= GRID_SIZE; // Yukarı
  if (direction == 1) snakeX[0] += GRID_SIZE; // Sağ
  if (direction == 2) snakeY[0] += GRID_SIZE; // Aşağı
  if (direction == 3) snakeX[0] -= GRID_SIZE; // Sol

  // Duvara çarpma kontrolü
  if (snakeX[0] < 0 || snakeX[0] >= SCREEN_WIDTH || snakeY[0] < 0 || snakeY[0] >= SCREEN_HEIGHT) {
    gameOver = true;
  }

  // Kendi kendine çarpma kontrolü
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver = true;
    }
  }

  // Yemi yeme kontrolü
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    if (snakeLength < MAX_SNAKE) {
      snakeLength++;
    }
    score += 10;
    spawnFood();
  }

  // Yılanın vücudunu çizdir
  for (int i = 0; i < snakeLength; i++) {
    display.fillRect(snakeX[i], snakeY[i], GRID_SIZE, GRID_SIZE, SSD1306_WHITE);
  }

  // Yemi çizdir
  display.fillRect(foodX + 1, foodY + 1, GRID_SIZE - 2, GRID_SIZE - 2, SSD1306_WHITE);

  // Puanı sol üste yaz
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(score);

  display.display();

  // Oyunun hızı (100 ms'de bir frame güncelleniyor)
  delay(100); 
}