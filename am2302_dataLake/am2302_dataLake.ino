#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

const char* ssid = "ModraZaba";
const char* password = "683lUqr;";
 
// DHT Sensor
uint8_t DHTPin = D8; 
               
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

float Temperature;
float Humidity;
 
void setup() {
  Serial.begin(115200);
  delay(100);
  
  pinMode(DHTPin, INPUT_PULLUP);
  delayMicroseconds(50);

  dht.begin();              

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
}

void loop() {
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
  
    Temperature = dht.readTemperature(); // Gets the values of the temperature
    Humidity = dht.readHumidity(); // Gets the values of the humidity 
  
    String body = "{\"id_sensor\":1,\"value\":";
    body += (int)Temperature;
    body += "}"; 
  
    HTTPClient http;
//    http.begin("http://192.168.1.88/dataLake/create.php");
    http.begin("http://421.cz/dataLake/create.php");
    http.addHeader("Content-Type", "application/json");
    http.POST(body);
  
    String payload = http.getString();
    http.end();

    Serial.println(payload);
  } else {
    Serial.println("Error in WiFi connection");   
  }
 
  delay(30000);  //Send a request every 30 seconds
}

String getRequest(float Temperaturestat,float Humiditystat){
  String body = "{\"id_sensor\":1,\"value\":";
  body += (int)Temperaturestat;
  body += "}"; 
  
  String s = "POST /dataLake/create.php HTTP/1.1 \n";
  s += "Host: localhost\n";
  s += "Content-Type: application/json\n";
  s += "User-Agent: PostmanRuntime/7.13.0\n";
  s += "Accept: */*\n";
  s += "Cache-Control: no-cache";
  s += "accept-encoding: gzip, deflate";
  s += "content-length: ";
  s += body.length();
  s += "\n";
  s += "Connection: keep-alive\n";
  s += body;
  return s;
}

String SendHTML(float Temperaturestat,float Humiditystat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Report</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 NodeMCU Weather Report</h1>\n";
  
  ptr +="<p>Temperature: "+ floatToString(Temperaturestat, 1);
  ptr +="&deg;C</p>";
  ptr +="<p>T: ";
  ptr += (int)Temperaturestat;
  ptr += "&deg;C";
  ptr +="<p>Humidity: ";
  ptr +=(int)Humiditystat;
  ptr +="%</p>";
  ptr +="<p>HTTP STORE: ";
  ptr += getRequest(Temperaturestat, Humiditystat);
  ptr += "</p>\n";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

String floatToString(float val, int decimalPlaces) {
  int valInt = (int) val;
  float valDec = val - (float) valInt;
  for (int i = 0; i < decimalPlaces; i++) {
    valDec = valDec * 10;
  }
  String str = "";
  str += valInt;
  str += ",";
  str += (int)valDec;
  return str;
}
