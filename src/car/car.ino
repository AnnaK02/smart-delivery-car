#include <stdbool.h>
#include <iostream>
#include <string>
#include <sstream>
#include <WiFi.h>
#include <PubSubClient.h>
using namespace std;

#include "DriveController.cpp"
#include "connection/WifiConfig.h"
//#include "DistanceSafety.cpp"

#define ID_MQTT "IoT_Academic_Work"
#define BAUD_RATE 115200

//////////////////////////////
/* Wifi Config */
WifiConfig wifi;

/* Comunicação com MQTT */

const char* SSID        = wifi.network;   // Rede Wi-Fi
const char* PASSWORD    = wifi.password;   // Senha da rede
const char* BROKER_MQTT = "test.mosquitto.org";   // Servidor MQTT
int BROKER_PORT         = 1883; // Porta do Servidor (default: 1883)

WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient

/* Prototypes */
void initWiFi(void);
void initMQTT(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT(void);
void reconnectWiFi(void);
void VerificaConexoesWiFIEMQTT(void);

//////////////////////////////

/* Variáveis controle dos motores (ponte H) */
int pin1 = 14;
int pin2 = 27;
int pin3 = 26;
int pin4 = 25;

DriveController dir;

/* Variáveis sensor ultrassônico 
int TRIGGER = 12;
int ECHO = 35;
bool riskOfCollision = false; */

//DistanceSafety distSafe;


/* Rota */
String rota;

/* Variáveis auxiliares */
char rotaChar[10]; // Limite de 5 tempos (0-9) e 5 direcoes
/*
String gasString;
char gasChar[10];
*/

////////////////////////////

void setup() {

  Serial.begin(BAUD_RATE);

  /* MQTT */
  initWiFi();
  initMQTT();
  
   /* Ponte H */
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);

  dir.motorPins(pin1, pin2, pin3, pin4);
  dir.stopped();
  
  /* Sensor ultrassônico */
  //pinMode(TRIGGER, OUTPUT); 
  //pinMode(ECHO, INPUT);

  //distSafe.ultrasonicPins(TRIGGER, ECHO);
  
  /* ****** STATUS - SETUP COMPLETO ****** */
  
  pinMode(LED_BUILTIN, OUTPUT);
  for(int i=0; i<3; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

/////////////////////////////////////////

/* Procedimento responsável por processar a entrada (5T4L3B2R) */
void processInput(String input) {

  for (int c = 0; c < input.length() - 1; c += 2){

    
    //riskOfCollision = distSafe.calcDistanceSafety();
    
    /*if(riskOfCollision && input[c + 1] != 'B'){
      distSafe.Warning();
      dir.makeMove(5, 'S');
      return;
    } else {*/
    dir.makeMove((int)(input[c] - '0'), input[c + 1]);
    //}
  }
  dir.makeMove(5, 'S'); /* Finaliza rota */
}

void loop() {

  VerificaConexoesWiFIEMQTT(); /* Fazendo conexões com Wifi e MQTT */

  Route(); /* Recebendo input para rota */

  MQTT.loop();

  delay(3000);
}

////////////////////////////

void Route(){

  if(rota.length() > 1){
    
    rota.toCharArray(rotaChar, rota.length()+1);

    MQTT.publish("smartcarpuc/status", "On route");
    MQTT.publish("smartcarpuc/rota", rotaChar); // envio para App

    Serial.print("Rota registrada: " + rota + "\n");
    processInput(rota);

    //if(riskOfCollision) distSafe.Warning();
  } else {
    MQTT.publish("smartcarpuc/status", "Stopped");
  }

  delay(2000);
  
}

////////////////////////////////////////////////////////

/*----- Wifi & MQTT ------- */

void initWiFi(void)
{
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");

  reconnectWiFi();
}

void initMQTT(void)
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
  MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
  String msg;

  /* obtem a string do payload recebido */
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    msg += c;
  }

  rota = msg; // Atribuindo input no mqtt a rota
}

void reconnectMQTT(void)
{
  while (!MQTT.connected())
  {
    Serial.println("\n* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT))
    {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe("smartcarpuc/rota"); // le a rota inputada e devolve se ok (1)
    }
    else
    {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Havera nova tentatica de conexao em 2s");
      delay(2000);
    }
  }  
}

void VerificaConexoesWiFIEMQTT(void)
{
  if (!MQTT.connected())
    reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita

  reconnectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void reconnectWiFi(void)
{
  //se já está conectado a rede WI-FI, nada é feito.
  //Caso contrário, são efetuadas tentativas de conexão
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(SSID);
}

////////////////////////---------////////////////////////