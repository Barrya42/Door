#define RespLength 2
#include <SoftwareSerial.h>
#include <Arduino.h>



SoftwareSerial BTSerial(2, 3); // указываем пины RX и TX
int pinReley = 4;
byte helloFromPhone[] = {0x02, 0x01};
byte OpenDoorCommand[] = {0x02, 0x05};
byte Helloed[] = {0x0E, 0x00};
byte Hello[] = {0x0E, 0x01};
byte NotYetHelloed[RespLength] = {0x0E, 0x02};
bool isHelloed = false;
long openMoment;
byte request[RespLength];
long openDelay = 6000;
void ReciveRequest()
{
    while(BTSerial.available())
    {
      Serial.write(BTSerial.read());
    }
  //String str = BTSerial.readString();
  BTSerial.readBytes(request, RespLength);
  Serial.write(request,RespLength);

  //BTSerial.flush();

}



void sendHello()
{
  BTSerial.write(Hello,RespLength);

}


void sendHelloed()
{
  BTSerial.write(Helloed,RespLength);
}


void sendNotHello()
{
  BTSerial.write(NotYetHelloed,RespLength);
}


bool CompareArrays(byte* response, byte* tamplate)
{
  for(int i =0;i<=RespLength-1;i++)
  {
    if(response[i] != tamplate[i])
    return false;
  }
  return true;
}


void setup()
{
  pinMode(pinReley,OUTPUT);
  digitalWrite(pinReley,LOW);
  pinMode(13, OUTPUT);
//pinMode(2,INPUT);
//pinMode(3,OUTPUT);
Serial.begin(9600);
BTSerial.begin(9600);
}


void loop()
{
  if(millis()-openMoment >= openDelay)
  {
    digitalWrite(pinReley,LOW);
  }
  if (BTSerial.available())
  {
    if(!isHelloed)
    {
    	ReciveRequest();
      if(CompareArrays(request, helloFromPhone))
      //if("11" == "11")
      {
        sendHello();
        isHelloed = true;
      }
      else
      {
        sendNotHello();
      }
    }
    else
    {
      sendHelloed();
    }
    ReciveRequest(); // читаем из software-порта
    if(CompareArrays(request, OpenDoorCommand ))
    {
      if(isHelloed)
      {
        digitalWrite(pinReley,HIGH);
        openMoment = millis();
      }
      else
      {
        sendNotHello();
      }
    }
   Serial.write(request,RespLength);
  }

//Если вдруг с PC команду нужно отправить
if (Serial.available())
{
  int pcData = Serial.read(); // читаем из hardware-порта
  BTSerial.println(pcData); // пишем в software-порт

}

//логи
//Serial.println(request); // пишем в hardware-порт
}

 //8C3A:E3:ED8235
//18F0:E4:2F8153

//0018:e4:34bf20

