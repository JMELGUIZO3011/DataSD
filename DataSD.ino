#include<DS1302.h>
#include<SoftwareSerial.h>
#include<SD.h>

/*
Conexiones de tarjeta SD

MISO -> 12
MOSI -> 11
SCK -> 13
CS -> 10

Conexiones de reloj en tiempo real

CLK -> 4
DAT -> 5 
RST -> 6

*/

//Mensaje de solicitud de datos 
uint8_t b0 = 0xFF;
uint8_t b1 = 0x4E;
uint8_t b2 = 0x57;
uint8_t b3 = 0x00;
uint8_t b4 = 0x13;
uint8_t b5 = 0x01; 
uint8_t b6 = 0x10; 
uint8_t b7 = 0x00;
uint8_t b8 = 0x00;
uint8_t b9 = 0x06;
uint8_t b10 = 0x03;
uint8_t b11 = 0x00;
uint8_t b12 = 0x00;
uint8_t b13 = 0x00;
uint8_t b14 = 0x00;
uint8_t b15 = 0x00;
uint8_t b16 = 0x00;
uint8_t b17 = 0x68;
uint8_t b18 = 0x00;
uint8_t b19 = 0x00;
uint8_t b20 = 0x01;
uint8_t b21 = 0x3A; 


SoftwareSerial Serial2(3, 2);
const long intervalo = 2000;
unsigned long tiempoAnterior = 0;
int voltage;
float V;
float I;
File datos; 

DS1302 rtc(6, 5, 4); //CLK, DAT, RST 
Time t;

void setup()
{
  //Iniciar el reloj y desactivar la proteccion de escritura
  //rtc.halt(false);
  //rtc.writeProtect(false);
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.println("Iniciando SD");
  if(!SD.begin(10))
  {
    Serial.println("Error al iniciar");
   while(1);
  }
  Serial.println("Iniciado Correctamente");
  //rtc.setDOW(FRIDAY);
  //rtc.setTime(12, 22, 00);
  //rtc.setDate(2, 2, 2024);
  Serial2.write(b0);

  delay(4000);
}

void mensaje()
{
  Serial2.write(b1);
  Serial2.write(b2);
  Serial2.write(b3);
  Serial2.write(b4);
  Serial2.write(b5);
  Serial2.write(b6);
  Serial2.write(b7);
  Serial2.write(b8);
  Serial2.write(b9);
  Serial2.write(b10);
  Serial2.write(b11);
  Serial2.write(b12);
  Serial2.write(b13);
  Serial2.write(b14);
  Serial2.write(b15);
  Serial2.write(b16);
  Serial2.write(b17);
  Serial2.write(b18);
  Serial2.write(b19);
  Serial2.write(b20);
  Serial2.write(b21);
}

void tiempo()
{
  //Serial.print(rtc.getDOWStr());
  //Serial.print(" ");
  //Serial.print(rtc.getDateStr());
  //Serial.print(" -- ");
  Serial.print(rtc.getTimeStr());
  Serial.print(",");
}

void loop()
{
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnterior >= intervalo) 
  {
    tiempoAnterior = tiempoActual;
    mensaje();
    //Serial.println(rtc.getTimeStr());
  }

  if (Serial2.available() > 0)
  {
    byte dato = Serial2.read();

    if (dato == 0x83 && Serial2.available() >= 2)
    {
      byte bytes[2];
      Serial2.readBytes(bytes, 2);
      voltage = (bytes[0] << 8) | bytes[1];
     //if(!isnan(voltage))
      //{
        V = voltage * 0.01;

      // Abrir el archivo para escribir
        datos = SD.open("datalog.txt", FILE_WRITE);
        if (datos)
        {
          Serial.print(rtc.getTimeStr());
          Serial.print(" ");
          Serial.print(V);
          Serial.print(" ");
          datos.print(rtc.getTimeStr());
          datos.print(";");
          datos.print(V);
          datos.print(";");

        // Cerrar el archivo después de escribir
          datos.close();
        }
     //}
    }
    if (dato == 0x84 && Serial2.available() >= 2)
    {
      byte bytes[2];
      Serial2.readBytes(bytes, 2);
      int corriente = (bytes[0] << 8) | bytes[1];
      float current = (10000 - corriente) * 0.01;
      I = round(current * 10.0) / 10.0;

      datos = SD.open("datalog.txt", FILE_WRITE);
      if (datos)
      {
        Serial.print(I);
        Serial.print(" ");
        datos.print(I * -1);
        datos.print(";");

        // Cerrar el archivo después de escribir
        datos.close();
      }
    }

    if (dato == 0x85 && Serial2.available() >= 1)
    {
      byte byte4 = Serial2.read();
      if(byte4 > 0 && !isnan(byte4))
      {
        datos = SD.open("datalog.txt", FILE_WRITE);
        if (datos)
        {
          Serial.println(byte4);
          datos.println(byte4);
        }
        // Cerrar el archivo después de escribir
        datos.close();
      }
    }
  }
}
