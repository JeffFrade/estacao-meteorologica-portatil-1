// Usar ESP32 (ESP32-WROOM-Da Module)

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>
#include <DHT.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Hostmane = Nome da sua placa ESP32 dentro da sua rede
const char* HOSTNAME = "estacao_meteorologica_esp";

// Preencher SSID com o nome da sua rede Wi-Fi e PASSWORD com a senha (Se houver).
const char* SSID = "nome_rede"; // rede wifi
const char* PASSWORD = "senha_rede"; // senha da rede wifi

// LED
#define redLed 13
#define greenLed 33
#define blueLed 12

// DHT
#define dhtPin 18
// Caso queira utilizar um DHT11, basta comentar a linha abaixo (25) e descomentar a linha 26
#define dhtType DHT22
// #define dhtType DHT11

// BMP
// Caso apresente problemas ao inicializar, trocar 0x76 por 0x77
#define addr 0x76

// OLED Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin
#define SCREEN_ADDRESS 0x3C

void initSerial();
void initWiFi();

Adafruit_BMP280 bmp;
DHT dht(dhtPin, dhtType);
BH1750 bh;
SoftwareSerial gpsSerial(16, 17); // RX, TX
TinyGPSPlus gps;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  initSerial();
  Serial.println(F("Inicializando estação meteorológica"));

  dht.begin();

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, HIGH);
  digitalWrite(blueLed, HIGH);

  while (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Erro ao inicializar display"));
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    
    digitalWrite(redLed, HIGH);
  }

   displayHeader();
   display.setTextSize(1);
   display.println("Inicializando...");
   display.display();

  while (!bmp.begin(addr)) {
    Serial.println(F("Erro ao iniciar o BMP280"));

    displayHeader();
    display.setTextSize(2);
    display.println("BMP280");
    display.display();

    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    
    digitalWrite(redLed, HIGH);
    delay(2000);
    digitalWrite(redLed, LOW);
    delay(2000);
  }

  while (!bh.begin()) {
    Serial.println(F("Erro ao iniciar o BH1750"));

    displayHeader();
    display.setTextSize(2);
    display.println("BH1750");
    display.display();

    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    
    digitalWrite(redLed, HIGH);
    delay(2500);
    digitalWrite(redLed, LOW);
    delay(2500);
  }
  
  initWiFi();
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(blueLed, LOW);
}

void initSerial() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
}

void initWiFi() {
  delay(10);
  Serial.println("Conectando-se em: " + String(SSID));
  
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(SSID, PASSWORD);

  displayHeader();
  display.setTextSize(1);
  display.println("Conectando Wi-Fi");
  display.display();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");

    displayHeader();
    display.setTextSize(1);
    display.println("Conectando...");
    display.display();
  }

  displayHeader();
  display.setTextSize(1);
  display.println("Conectado!");
  display.display();

  delay(2000);
  
  Serial.println();
  Serial.print("Conectado na Rede " + String(SSID) + " | IP => ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.println("Iniciando coletas");

  // Caso o roteador caia, a cada execução ele veriica se está conectado, se não estiver quando o roteador voltar, ele volta a conectar sozinho, sem necessidade de reiniciar.
  conectaWiFi();
  gpsAvailable();

  Serial.println("---------------------------------");

  digitalWrite(blueLed, HIGH);

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  Serial.print(F("Temperatura: "));
  Serial.print(temperature);
  Serial.println(F(" ºC"));

  Serial.print(F("Umidade: "));
  Serial.print(humidity);
  Serial.println(F(" %"));
  
  Serial.print(F("Pressão: "));
  Serial.print(bmp.readPressure());
  Serial.println(F(" Pa"));

  Serial.print(F("Altitude: "));
  Serial.print(bmp.readAltitude());
  Serial.println(F(" m"));

  Serial.print(F("Luminosidade: "));
  Serial.print(bh.readLightLevel());
  Serial.println(F(" lux"));

  Serial.print(F("Latitude: "));
  Serial.println(String(gps.location.lat(), 5));

  Serial.print(F("Longitude: "));
  Serial.println(String(gps.location.lng(), 5));

  Serial.print(F("Velocidade: "));
  Serial.print(gps.speed.kmph());
  Serial.println(F(" km/h"));

  digitalWrite(blueLed, LOW);

  displayHeader();
  display.setTextSize(1);
  display.println("Temperatura:");
  display.setTextSize(2);
  display.println(String(temperature, 1) + " C");

  display.setTextSize(1);
  display.println("Umidade:");
  display.setTextSize(2);
  display.println(String(humidity, 2) + "%");

  Serial.println("---------------------------------");

  digitalWrite(greenLed, HIGH);
  delay(1000);
  digitalWrite(greenLed, LOW);
}

void conectaWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
     return;
  }

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
  }
}
