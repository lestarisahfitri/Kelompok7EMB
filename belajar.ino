#include <WiFi.h>
#include <HTTPClient.h>

#define TRIGGER_PIN 5 // Jumper pin trigger yang digunakan
#define ECHO_PIN 18   // Jumper pin echo yang digunakan

// menghubungkan dengan wifi
const char* ssid = "aku"; 
const char* password = "sampai12";

//menghubungkan dengan server
const char* host = "192.168.43.8";
WiFiClient client;
const int httpPort = 80;
String url;
long duration, distance;
unsigned long timeout;
  
void setup() {
  Serial.begin(9600);
  delay(10);
  
//melakukan inisialisasi termasuk mengatur pin dan menghubungkan ke jaringan WiFi
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // mulai menghubungkan ke jaringan WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void baca_jarak(){
  digitalWrite(TRIGGER_PIN, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration/2) / 29.1; //menghitung jarak dengan membagi durasi (waktu tempuh) sinyal ultrasonik dengan 2, karena sinyal pergi dan kembali. Nilai 29.1 adalah faktor konversi yang bergantung pada kecepatan suara di udara dan faktor skalanya untuk mengubah durasi menjadi jarak dalam sentimeter.
  Serial.print(distance);
  Serial.println(" cm");
}

void loop() {
  Serial.print("baca jarak ");
  baca_jarak();
  
  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    //return;
  }

// sekarang membuat URI untuk permintaan 
  url = "/belajar/index.php/simpan/sensor?data=";
  url += distance;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);

// Ini akan mengirimkan permintaan ke server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

// Baca semua baris balasan dari server dan cetak ke Serial

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
  Serial.println();
  delay(5000);
}
