/*
* Written by Mosfex : www.mosfex.com
* www.fb.com/mosfex 
* plese refer to this when using the code
*/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
const char* ssid = "mosfex.com";
const char* password = "123456789";
const char* host = "www.google.co.th";
const char* fingerprint = "1E:7A:8C:30:6C:EA:EC:F0:C2:8A:9A:BC:21:1B:00:5D:D3:A3:2A:0F";
const char* city = "Bangkok";
long counter = 0;
String forcast;
LiquidCrystal_I2C lcd(0x3F,16,2);

String get_forcast(){
  WiFiClientSecure client;
  if (!client.connect(host, 443)) { Serial.println("connection failed"); return ""; }
  client.verify(fingerprint, host);
  String url = String("/search?q=weather+")+city+"&hl=en";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "User-Agent: Mozilla/5.0 (Android 4.4; Mobile; rv:18.0) Gecko/18.0 Firefox/18.0\r\n" +
               "Accept: text/html\r\n"+
               "Connection: close\r\n\r\n");
  String forcast = ">";
  while (client.connected()) {
    String line = client.readStringUntil('>');
    if(line.startsWith("<table class=\"ts\"")){
      bool opentag = false;
      while(line.indexOf("</table") == -1){
        if(client.available()){
          char d = client.read();
          if(d == '<') opentag = true;
          if(d == '>'){ opentag = false; }          
          if(opentag == false){
            if(d == '>' && forcast[forcast.length()-1] != ' '){
              forcast += " ";
            }else if(d != '>' && d<= 127){  
              forcast += d;
            }  
          }else{
            line += d;
          }
        }
      }
    }
  }
  Serial.println(forcast);
  return forcast + "    ";
}

void setup() {
  Serial.begin(115200);  
  lcd.init();
  lcd.backlight();
  lcd.print("City : "); 
  lcd.print(city);
  WiFi.begin(ssid, password);    
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }     
  forcast = get_forcast();
  counter = millis();
}
void loop() {
  if((millis() - counter) > 60000){
    forcast = get_forcast();
    counter = millis();
  }
  lcd.setCursor(0,1);
  forcast = forcast.substring(1,forcast.length()) + forcast.substring(0,1);
  lcd.print(forcast.substring(0,16));  
  delay(400);
}

