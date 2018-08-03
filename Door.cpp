#include <SoftwareSerial.h>
#include <Arduino.h>

SoftwareSerial BTSerial(2, 3); // указываем пины RX и TX
int pinReley = 13;
int pinDeviceConnected = 4;
bool DEBUG = true;

const String REQUEST_OPEN_DOOR = "54141f5a-fb34-494e-a9b4-516689876310";
const String REQUEST_HELLO = "3dfe191f-15f2-43cf-b2ac-4ac6d1c928fc";
const String REQUEST_CLOSE_DOOR = "c6361166-4c22-4bd9-b682-49812110f605";
String Ok = "200:OK";
String Helloed = "501:Helloed";
String NotYetHelloed = "501:NotHelloed";
bool isHelloed = false;

long openMoment;
long openDelay = 6000;

void PrintDebug(String message)
{
	if (DEBUG)
		Serial.println(message);
}

String ReciveRequest()
{
	String res = "";
	if (BTSerial.available())
	{
		res = BTSerial.readString();

		PrintDebug("recive request: " + res);
	}
	return res;
}

//bool CompareArrays(byte* response, byte* tamplate)
//{
//	for (int i = 0; i <= RespLength - 1; i++)
//	{
//		if (response[i] != tamplate[i])
//			return false;
//	}
//	return true;
//}

void CloseDoorByTimer(bool force)
{
	if (millis() - openMoment >= openDelay || force)
	{
		digitalWrite(pinReley, LOW);
	}
}

void OpenDoor()
{
	digitalWrite(pinReley, HIGH);
}

void CheckConnected()
{
	if(digitalRead(pinDeviceConnected) == LOW && isHelloed)
	{
		isHelloed = false;
		PrintDebug("device DISCONNECTED, ""isHelloed = false""");
	}
	if(digitalRead(pinDeviceConnected) == HIGH)
	{
		PrintDebug("device CONNECTED");
	}
}

void SendResponse(String resp)
{
	BTSerial.print(resp);
}
void setup()
{
	pinMode(pinReley, OUTPUT);
	digitalWrite(pinReley, LOW);
	pinMode(pinDeviceConnected,INPUT);
//pinMode(2,INPUT);
//pinMode(3,OUTPUT);
	Serial.begin(9600);
	BTSerial.begin(38400);
}

void loop()
{
	CloseDoorByTimer(false);
	CheckConnected();

	String res = ReciveRequest();

	if (res.equals(""))
	{
		//PrintDebug("empty request");
	}
	else if (res.equals(REQUEST_HELLO))
	{
		//телефон пробует поздороваться
		if (isHelloed)
		{
			PrintDebug("Hello request and we are HELLOED");
			SendResponse(Helloed);
		}
		else
		{
			PrintDebug("Hello request and we are NOT HELLOED");
			isHelloed = true;
			SendResponse(Ok);
		}
	}
	else if (res.equals(REQUEST_CLOSE_DOOR))
	{
		//Телефон рпосит открыть дверь
		if (isHelloed)
		{
			PrintDebug("Close request and we HELLOED");
			CloseDoorByTimer(true);
			SendResponse(Ok);
		}
		else
		{
			PrintDebug("Close request and we are NOT HELLOED");
			SendResponse(NotYetHelloed);
		}
	}
	else if (res.equals(REQUEST_OPEN_DOOR))
	{
		if (isHelloed)
		{
			PrintDebug("Open request and we are HELLOED");
			OpenDoor();
			SendResponse(Ok);
		}
		else
		{
			PrintDebug("Open request and we are NOT HELLOED");
			SendResponse(NotYetHelloed);
		}
	}
	else
	{
		PrintDebug("Unknown command: " + res);
		SendResponse("500:Unknown command");
	}

//Если вдруг с PC команду нужно отправить
	if (Serial.available())
	{
		BTSerial.println(Serial.readString()); // пишем в software-порт

	}
}

//логи
//Serial.println(request); // пишем в hardware-порт

//8C3A:E3:ED8235
//18F0:E4:2F8153

//0018:e4:34bf20

