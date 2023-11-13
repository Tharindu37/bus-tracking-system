//I2C Display Custom Library
#include <Custom_Display.h>

//gps code
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//GPS
TinyGPSPlus gps;  // The TinyGPS++ object
//SoftwareSerial ss(4, 5); // The serial connection to the GPS device D1,D2
SoftwareSerial ss(5, 4);
float latitude , longitude;
int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str;
int pm;

//firebase code
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#define FIREBASE_HOST "test-e2db4-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "Rf1tfDahq1g5hzmrRd9OGfbOO4WXFIC8Q2P2XFbm"

int SCL_pin = 14; //Pin D5
int SDA_pin = 12; //Pin D6
Custom_Display custom_display(SCL_pin,SDA_pin);

//web server
#include <ESP8266WebServer.h>

const char* ssid = "Bus traking system";
const char* password = "";

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);
String userId = "";

//auto connect
#include <DNSServer.h>
//#include <ESP8266WebServer.h>
#include <WiFiManager.h>

void setup(){
  Serial.begin(115200);
  ss.begin(9600);
  Serial.println();

  //I2C Display
  custom_display.begin();
  custom_display.printMessage("Setup Your Device.");


  //autoConnect wifi
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("connected...yeey :)");
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // web server
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/", HTTP_GET, handle_OnConnect);
  server.on("/get", HTTP_GET, handle_get);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  // web server end

  
}
void loop(){
  server.handleClient();
  if(userId!=""){
    while (ss.available() > 0)
    if (gps.encode(ss.read())){
      if (gps.location.isValid()){
        latitude = gps.location.lat();
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
        Serial.print("longitude:");
        Serial.println(longitude);
        Firebase.setFloat(userId+"/longitude",longitude);
        if (Firebase.failed()) {
            Serial.print("value /value failed:");
            Serial.println(Firebase.error());  
            delay(1000);
            return;
        }
        Serial.print("latitude:");
        Serial.println(latitude);
        Firebase.setFloat(userId+"/latitude",latitude);
        if (Firebase.failed()) {
            Serial.print("value /value failed:");
            Serial.println(Firebase.error());  
            delay(1000);
            return;
        }
      }
      if (gps.date.isValid()){
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();
        if (date < 10)
          date_str = '0';
        date_str += String(date);
        date_str += " / ";
        if (month < 10)
          date_str += '0';
        date_str += String(month);
        date_str += " / ";
        if (year < 10)
          date_str += '0';
        date_str += String(year);
        Serial.print("date_str:");
        Serial.println(date_str);
        Firebase.setString(userId+"/date",date_str);
        if (Firebase.failed()) {
            Serial.print("value /value failed:");
            Serial.println(Firebase.error());  
            delay(1000);
            return;
        }
      }
      if (gps.time.isValid()){
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();
        minute = (minute + 30);
        if (minute > 59){
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5) ;
        if (hour > 23)
          hour = hour - 24;
        if (hour >= 12)
          pm = 1;
        else
          pm = 0;
        hour = hour % 12;
        if (hour < 10)
          time_str = '0';
        time_str += String(hour);
        time_str += " : ";
        if (minute < 10)
          time_str += '0';
        time_str += String(minute);
        time_str += " : ";
        if (second < 10)
          time_str += '0';
        time_str += String(second);
        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";
        Serial.print("time_str:");
        Serial.println(time_str);
        Firebase.setString(userId+"/time",time_str);
        if (Firebase.failed()) {
            Serial.print("value /value failed:");
            Serial.println(Firebase.error());  
            delay(1000);
            return;
        }
      }
      if (gps.speed.isValid()) {
        float speed_kmph = gps.speed.kmph();
        Serial.print("Speed (km/h): ");
        Serial.println(speed_kmph, 6);
        Firebase.setFloat(userId+"/speed",speed_kmph);
        if (Firebase.failed()) {
            Serial.print("value /value failed:");
            Serial.println(Firebase.error());  
            delay(1000);
            return;
        }
      }
    }
    int isStop=Firebase.getBool(userId+"/isStop");
    Serial.println(isStop);
    if(isStop==1){
        custom_display.printMessage("Stop");
    }else{
      custom_display.printMessage("GO");
    }
    Serial.println();
    if (Firebase.failed()) {
            Serial.print("value /value failed:");
            Serial.println(Firebase.error());  
            delay(1000);
            return;
        }
  }
}

//web server
void handle_OnConnect() {
  server.send(200, "text/html", SendHTML());
}

void handle_get() {
  if (server.hasArg("userId")) {
    userId = server.arg("userId");
    Serial.println("User ID: " + userId);
  }
  server.send(200, "text/html", "User ID received: " + userId +
                                   "<br><a href=\"/\">Return to Home Page</a>");
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>User ID Input</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += "form {margin-bottom: 20px;} input {font-size: 16px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Real-Time Bus Traking System</h1>\n";
  ptr += "<h3>Enter your User ID</h3>\n";

  // Add userId input field
  ptr += "<form action=\"/get\">\n";
  ptr += "  User ID: <input type=\"text\" name=\"userId\">\n";
  ptr += "  <input type=\"submit\" value=\"Submit\">\n";
  ptr += "</form>\n";

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
