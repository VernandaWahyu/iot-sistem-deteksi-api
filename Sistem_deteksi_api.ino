#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// Menentukan pin sensor flame, pin relay, dan pin buzzer
const int flamePin = D2; // Pin digital untuk sensor flame
const int relayPin = D3; // Pin digital untuk relay
const int buzzerPin = D1; // Pin digital untuk buzzer
const int smokePin = A0; // Pin analog untuk sensor asap

// API key dan informasi jaringan WiFi
String apiKey = "1IV9R5AMQUIDEE6D";
const char* ssid = "Infinix NOTE 40";
const char* pass = "123456781";
const char* server = "api.thingspeak.com";

// Threshold sensor asap
int thresholdMQ2 = 600;

// Objek WiFiClient
WiFiClient client;

void setup() {
    Serial.begin(115200);

    // Mengatur mode pin
    pinMode(buzzerPin, OUTPUT);
    pinMode(flamePin, INPUT);
    pinMode(relayPin, OUTPUT);

    // Menghubungkan ke jaringan WiFi
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected"); // Terhubung ke jaringan WiFi
}

void loop() {
    // Membaca nilai sensor flame dan sensor asap
    int flameStatus = digitalRead(flamePin);
    int smokeLevel = analogRead(smokePin);

    // Cetak nilai sensor
    Serial.print("Smoke level: ");
    Serial.print(smokeLevel);
    Serial.print(" | Flame status: ");
    Serial.println(flameStatus);

    // Cek jika nilai sensor asap melampaui threshold
    if (smokeLevel > thresholdMQ2) {
        tone(buzzerPin, 1000); // Aktifkan buzzer
    } else {
        noTone(buzzerPin); // Matikan buzzer
    }

    // Jika ada api yang terdeteksi
    if (flameStatus == 1) {
        digitalWrite(relayPin,LOW); // Aktifkan relay untuk menyalakan pompa air
        Serial.println("Api terdeteksi! Menyalakan pompa air...");
    } else {
        digitalWrite(relayPin,HIGH); // Matikan relay jika tidak ada api yang terdeteksi
    }

    // Mengirim data ke ThingSpeak
    if (client.connect(server, 80)) {
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(smokeLevel);
        postStr += "&field2=";
        postStr += String(flameStatus);
        postStr += "\r\n";

        // Mengirim permintaan POST ke ThingSpeak
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: " + String(postStr.length()) + "\n\n");
        client.print(postStr);
        
        // Beri jeda untuk memastikan data terkirim
        delay(1000);
        
        Serial.println("Data berhasil dikirim ke ThingSpeak.");
    } else {
        Serial.println("Gagal terhubung ke ThingSpeak.");
    }

    // Tutup koneksi
    client.stop();

    // Jeda singkat sebelum loop berikutnya
    delay(500);
}