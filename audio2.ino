#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ESP8266WiFi.h>
#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>


//variables de DFPlayer
#define PIN_BUSY D3

SoftwareSerial mp3Serial(D1, D2); // RX, TX
///variables de sensor de proximidad
const int triggerPin = 16; 
const int echoPin = 13; 

long duracion;
float distancias;


///variables de led rgb
const int output5 = 15;////dudoso
const int output4 = 14;
const int output3 = 12;

//variables de wifi

#define WLAN_SSID  "Erick1"
#define WLAN_PASS  "erick5599"

const char* resource2 = "https://maker.ifttt.com/trigger/levantar/with/key/bHOg8QsihwicLNLQFA9dD5";
const char* server = "maker.ifttt.com";
#define AIO_SERVER  "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define IO_USERNAME  "Erick99"
#define IO_KEY       "aio_ArKa63UDLjA9YFGLdZlgOgMxR1j8"  
void MQTT_connect();

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client,AIO_SERVER,AIO_SERVERPORT,IO_USERNAME,IO_KEY);





void setup () {
  pinMode(PIN_BUSY, INPUT);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin (115200);
  Serial.println("Setting up software serial");
  mp3Serial.begin (9600);
  Serial.println("Setting up mp3 player");
  mp3_set_serial (mp3Serial);
  // Delay is required before accessing player. From my experience it's ~1 sec
  delay(1000);
  mp3_set_volume (25);
   Serial.print("conectando a...");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID,WLAN_PASS);
  /*
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  */
  Serial.println();

  Serial.println("Wifi conectado Direccion IP");
  Serial.println("IP ADRESS: ");
  Serial.println(WiFi.localIP());
}

///variables del ciclo
int contador = 0;
float distan;
float tiempo;
float distancia_fija;
boolean robot = false;
float antiguo = 0;
void loop () {
  Serial.println(WiFi.status());
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("desconectado");
    WiFi.begin(WLAN_SSID,WLAN_PASS);
  }
  tiempo = millis();
  Serial.println(tiempo);
  if (tiempo >= 15000) {
    Serial.println("el robot es");
    Serial.println(robot);
    if (robot == false) {
      distan = distancia();
      Serial.println(distan);
      robot = true;
      antiguo = 15000;
      
   }
  }
  else {
    Serial.println("menos de 10 segundos");
    distan = distancia();
    Serial.println(distan);
    distancia_fija = distan;
    Serial.println("distancia fija:"+String(distancia_fija));

    boolean busy = digitalRead(PIN_BUSY);
    Serial.println(busy);
    if (busy) {
      rgb_col(0, 255, 0);
      mp3_play(1);
      delay(5000);
    }
  }
  
 

  if (robot) {
    Serial.println("distancia fija:"+String(distancia_fija));
    distan = distancia();
    Serial.println(distan);
    if (distan > distancia_fija) {
      float porc = distancia_fija * 0.50;
      float distancia_caso=distancia_fija+porc;
      if (distan>=distancia_caso) {
        boolean busy = digitalRead(PIN_BUSY);
        Serial.println(busy);
        if (busy) {
          rgb_col(255, 0, 0);
          mp3_play(2);///oye no te muevas de tu lugar
          makeIFTTTRequestEmail();
        }
        else{
          int varrandom = random(3, 5);
          boolean busy = digitalRead(PIN_BUSY);
          Serial.println(busy);
          if (busy) {
            Serial.println("esto es random");
            mp3_play(varrandom);///aleatorio motivacion
            rgb_col(0, 0, 255);
          }
        }
      }
    }
    else {
      tiempo = millis();
      if ((tiempo - antiguo) >= 10000) {
        int varrandom = random(3, 5); //////aun no se cuantos audios seran
        Serial.println("el random es:"+String(varrandom));
        boolean busy = digitalRead(PIN_BUSY);
        Serial.println(busy);
        if (busy) {
          Serial.println("esto es random");
          mp3_play(varrandom);///aleatorio motivacion
          rgb_col(0, 0, 255);
        }
        antiguo = antiguo + (tiempo - antiguo);
      }
      else {
        int varrandom = random(3, 5); //////aun no se cuantos audios seran
        boolean busy = digitalRead(PIN_BUSY);
        Serial.println(busy);
        if (busy) {
          mp3_play(varrandom);///aleatorio motivacion
          rgb_col(0, 0, 255);
        }

      }



    }
  }
  
}





float distancia() {

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  // Iniciar elproceso de envío de ultrasonido
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // https://www.arduino.cc/reference/en/language/functions/advanced-io/pulsein/
  duracion = pulseIn(echoPin, HIGH);

  Serial.print("Duración: ");
  Serial.println(duracion);

  // Calcular la distancia
  distancias = duracion * 0.034 / 2;

  Serial.print("Distancia: ");
  Serial.println(distancias);
  return distancias;


}

void rgb_col(int red, int green, int blue) {
  analogWrite(output5, red); //para recibir valores analogos
  analogWrite(output3, green);
  analogWrite(output4, blue);
}


void makeIFTTTRequestEmail() {
  Serial.print("Connecting to "); 
  Serial.print(server);
  
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if(!!!client.connected()) {
     Serial.println("Failed to connect, going back to sleep");
  }
  Serial.print("Request resource2: "); 
  Serial.println(resource2);
  client.print(String("GET ") + resource2 + 
                  " HTTP/1.1\r\n" +
                  "Host: " + server + "\r\n" + 
                  "Connection: close\r\n\r\n");
                  
  int timeout = 5 * 10; // 5 seconds             
  if(!!!client.available()) {
     Serial.println("No response, going back to sleep");
  }
  
  Serial.println("\nclosing connection");
  client.stop();
}



////METODO PARA COMPRABAR CONECCION
void MQTT_connect() {
  int8_t ret;

  if(mqtt.connected()) {
    return;
  }

  Serial.print("Conectando a MQTT... ");

  uint8_t retries = 3;
  /*
  while ((ret = mqtt.connect())!=0){
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintentando conexion en 5 segundos...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    Serial.println("Retry "+ retries);
    if (retries == 0) {
      while(1);
    }
  }
  */
  
  //Serial.println("MQTT CONECTADO!");
}
