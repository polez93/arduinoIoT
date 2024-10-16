#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <SHT1x.h>

// Definir los pines para el SHT10
#define dataPin 2
#define clockPin 3

// Crear una instancia del sensor
SHT1x sht1x(dataPin, clockPin);

float tempC = 0;
float humidity = 0;

const int ledwifi = 10;
const int ledinternet = 11;
const int ledservidor = 12;

const char* ssid = "hamster";     // Reemplaza con el nombre de tu red WiFi
const char* password = "k4k4deperr0"; // Reemplaza con la contraseña de tu red WiFi

const char* serverAddress = "146.190.222.135"; // Reemplaza con la IP o dominio de tu servidor Node.js
int port = 3000;                            // El puerto en el que escucha tu servidor Node.js

WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, serverAddress, port);

void setup() {
  // Inicializar el pin del LED como salida
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledwifi, OUTPUT);
  pinMode(ledinternet, OUTPUT);
  pinMode(ledservidor, OUTPUT);



  // Iniciar comunicación serial para depuración
  Serial.begin(9600);
  //while (!Serial);
  
  // Conectar a la red WiFi
  Serial.print("Conectando a ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {

    digitalWrite(ledwifi, HIGH);
    delay(500);
    digitalWrite(ledwifi, LOW);
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a WiFi");
  // Imprimir la dirección IP
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Inicializando el sensor SHT10...");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conexión WiFi perdida, reiniciando...");
    // Parpadear el LED (rojo) antes de reiniciar
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
    // Reiniciar el Arduino
    //NVIC_SystemReset();
  }   
  tempC = sht1x.readTemperatureC();
  humidity = sht1x.readHumidity();
  


  //Imprimir los resultados en el monitor serie
  Serial.print("Temperatura: ");
  Serial.print(tempC);
  Serial.print(" C");
  Serial.print(" - Humedad: ");
  Serial.print(humidity);
  Serial.println(" %");
  int sensor_id = 11;
  int sensor_id_2 = 12;


  // Crear la carga útil JSON
  String data = "[{\"sensorValue\":" + String(humidity)+ ", \"sensor_id\":" + String(sensor_id) + "}, {\"sensorValue\":"+ String(tempC)+ ", \"sensor_id\":" + String(sensor_id_2) +"}]";
  //String data = "{\"sensorValue\":" + String(humidity) + ",\"sensor_id\":" + String(sensor_id) + "}";
  Serial.println(data);

  // Enviar la solicitud HTTP POST
  client.beginRequest();
  client.post("/receive-data");
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", data.length());
  client.beginBody();
  client.print(data);
  client.endRequest();

  // Leer la respuesta del servidor
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  if(statusCode != 200){
    for (int i = 0; i < 5; i++) {
      digitalWrite(ledinternet, HIGH);
      delay(200);
      digitalWrite(ledinternet, LOW);
      delay(200);
    }
  }else{
    digitalWrite(ledservidor, HIGH);
      delay(1000);
      digitalWrite(ledservidor, LOW);
  }

  delay(10000); // Esperar 10 segundos antes de enviar la siguiente lectura
}

