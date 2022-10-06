#include <ESP8266WiFi.h> //nodemcu
#include <PubSubClient.h> //mqtt
#include <SoftwareSerial.h> //gsm
SoftwareSerial mySerial(D6, D7);//rx,tx
#define echoPin D1 // Echo Pin (OUTPUT pin in RB URF02)
#define trigPin D2 // Trigger Pin (INPUT pin in RB URF02)
int maximumRange = 350; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance;
String dis;
int m=0;
int p=0;
const char* ssid = "Sush";
const char* password = "Sushritha";
const char* mqtt_server = "test.mosquitto.org";
//const char* mqtt_server = "iot.eclipse.org";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(100);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "nwm";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      //once connected to MQTT broker, subscribe command if any
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  mySerial.begin(9600);
   pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 Serial.println("   ");
 Serial.println("           WASTEBIN MANAGEMENT ");
}
void loop() {
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg >0) {
    lastMsg = now;
digitalWrite(trigPin, LOW); 
 delayMicroseconds(2); 
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10); 
 digitalWrite(trigPin, LOW);
 duration = pulseIn(echoPin, HIGH);
 distance = duration/58.2; //Calculate the distance (in cm) based on the speed of sound.
 dis=duration/58.2;
 dis +="Cms";
 char message[11];
 Serial.print("level=");
 Serial.println(distance); // distance in cm  
 delay(50); //Delay 50 ms
 dis.toCharArray(message, 11);
  client.publish("nwm1", message);
 if((distance>1)&&(distance<10)&&(m==0))
{
 Serial.println("GARBAGE FILLED"); 
 client.publish("nwm2", "GARBAGE FILLED");
 m=1;
 p=0;
}
if(distance>10)
{ 
 Serial.println("GARBAGE LEVEL NORMAL");
 client.publish("nwm2", "GARBAGE LEVEL NORMAL");
 m=0;
}
if((m==1)&&(p==0))
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
    delay(500);  
   mySerial.println("AT+CMGS=\"+918800799686\"\r"); // 
   delay(500);
   mySerial.print("ALERT:GARBAGE LEVEL FILLED  ");// The SMS text you want to send
   mySerial.println("");
   delay(100);
    mySerial.println((char)26);// ASCII code of CTRL+Z
   delay(1000);
   p=1;
   Serial.println("message sent");
}
delay(1000);
}
}
