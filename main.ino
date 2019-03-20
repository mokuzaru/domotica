#include <DHT.h>
#include <DHT_U.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

/*
 * Pin conectors
 * 
 * RFID ( SS_ = 53, SCK = 52, MOSI = 51, MISO = 50, irq = '', GND = GND, RST = 9, 3.3 =3.3) )
 * 
 * servodriver ((gnd & oe) = GND, scl = 21, sda = 20, vcc = 5v, +v = '')
 * 
 * mq135 (d0 = 2)
 * 
 * dht (pin =3)
 * 
*/

//gobales
int mq = 2;
char l = 'w';
#define DHTPIN 3


//codigo rfid 35 A2 2E 83

//rfid shit

#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);

//dht 11  shit

#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);
float temp, hum;


//Objetos

Adafruit_PWMServoDriver servos  = Adafruit_PWMServoDriver (0x40);

unsigned int pos0=172; // ancho de pulso en cuentas para pocicion 0°
unsigned int pos180=565; // ancho de pulso en cuentas para la pocicion 180°

void setup() {
  Serial.begin(9600);
  servos.begin();  
  servos.setPWMFreq(60); //Frecuecia PWM de 60Hz o T=16,66ms
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(mq, INPUT);
}

void loop() {

  // Revisamos si hay nuevas tarjetas  presentes
  if ( mfrc522.PICC_IsNewCardPresent()) 
        {  
      //Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()) 
            {
                  // Enviamos serialemente su UID
                  Serial.print("Card UID:");
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          Serial.print(mfrc522.uid.uidByte[i], HEX);   
                  } 
                  l = 'c';
                  Serial.println();
                  // Terminamos la lectura de la tarjeta  actual
                  mfrc522.PICC_HaltA();         
              }      
    }

    l = 'k';
    hum = dht.readHumidity();
    temp = dht.readTemperature();
    
    if(isnan(hum) || isnan (temp)){
      Serial.println("Fallo!");
      return;
    }else{
      Serial.print (temp);
      Serial.print (hum);
      l = 'T';
    }

    boolean humo = digitalRead(mq);
    if ( mq){
      Serial.print ("good air");
    }else{
      l = 'q';
    }

  switch (1){
    case 'c':
      for (int duty = pos0; duty < pos180; duty=duty+10) {
        int n = 1; //el servo de la cochera
        servos.setPWM(n,0,duty);      
      }
      delay(2000);//configurar el triger para cerrar la puerta
      for (int duty = pos180; duty > pos0; duty=duty-10) {
          int n = 1; //servo de la cochera abajo
          servos.setPWM(n,0,duty); 
      }
    break;

    case 'l':
      //start a fan or something
    break;

    case 'T':
      //Start something
    break;
  }

}
