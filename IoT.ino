
//LIBRARIES
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

//INTERRUPT VARIABLES 
bool wifi_connected = false;
int timeStart = 0;
unsigned long upTime;

//MACHINE VARIABLES
String mac = "";
String ip = "";

//AP AND WIFI
String hs_ssid;
String pass_ssid;

//SWITCHES AND VALUES
String s1_name,s1_value;
String s2_name,s2_value;
String s3_name,s3_value;
String s4_name,s4_value;

String s5_name,s5_value;
String s6_name,s6_value;
String s7_name,s7_value;
String s8_name,s8_value;


//WEBSERVER VARIABLES
int i = 0;
int statusCode;
String content;

String CSS = "article { background: #ff4000; padding: 1.3em; }" 
    "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
    "div { padding: 0.5em; }"
    "h1 { margin: 0.5em 0 0 0; padding: 0.5em; }"
    "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; border-radius: 10px; }"
    "select { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; border-radius: 10px; }"
    "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
    "nav { background: #1FA478; color: #fff; display: block; font-size: 1.3em; padding: 1em; }"
    "nav b { display: block; font-size: 1.5em; margin-bottom: 0.5em; } "
    "textarea { width: 100%; }";
    
String head = "<!DOCTYPE html><html>"
    "<head><title>ESP8266 IoT</title>"
    "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
    "<style>" + CSS + "</style>"
    "<meta charset=\"UTF-8\"></head>"
    "<body><nav><b>IoT Controller</b></nav><div>";
    
String foot = "</div><div align='center' class=q><a>&#169; All rights reserved JITECH I.T. SOLUTIONS.</a></div>";

//Function Declaration
bool is_connected(void);
void web_portal(void);
void read_values(void);
void write_eeprom(int f,String var);
String read_eeprom(int f,int t);
String get_uptime();

//Set DNS server credentials
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 100, 1);
DNSServer dnsServer;

//Establishing Local server at port 80 whenever required
ESP8266WebServer server(80);
    
//----------------------INITIALIZATION---------------------//
void setup(){
  
  //SERIAL BAUDRATE INITIALIZED
  Serial.begin(9600); 
  
  //EEPROM INITIALIZED
  EEPROM.begin(512); 
  
  //GPIO PINS INITIALIZED
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
 //------------------Read EEPROM values----------------//
 
  Serial.print("Reading EEPROM values");
  hs_ssid = read_eeprom(0,16);
  pass_ssid = read_eeprom(16,32);
  read_values();

 //------------------End Reading EEPROM----------------//
  

 upTime = millis();//Get Uptime
 WiFi.mode(WIFI_STA);//Set to wifi station/client mode
 WiFi.begin(hs_ssid.c_str(),pass_ssid.c_str()); // Connect to primary Wifi Hotspot
    if(is_connected()){
        wifi_connected = true;
        if (MDNS.begin("esp8266")) {
            Serial.println("MDNS responder started");
        }
        portal_page();
      }   
    else{     
          Serial.println("NodeMCU AP Mode ");
          web_portal(); //Launch Captive portal/Setup page
          wifi_connected = false;
         
      }
 server.begin();
 
 //GET WIFI MACHINE INFO
 mac = WiFi.macAddress();
 ip = WiFi.localIP().toString().c_str();
}

void loop() {   
 if(wifi_connected){ 
    read_values();    
    pin_state();
  }
  else{
    read_values();    
    pin_state();
    dnsServer.processNextRequest();
  }
  
  server.handleClient();
  MDNS.update();
                                
}
//--------------------------END INITIALIZATION--------------------------//

void pin_state(void){ //PIN OUTS
  
    if(s1_value.toInt() == 1){digitalWrite(D1, HIGH);}else if(s1_value.toInt() == 0){digitalWrite(D1, LOW);}
    if(s2_value.toInt() == 1){digitalWrite(D2, HIGH);}else if(s2_value.toInt() == 0){digitalWrite(D2, LOW);}
    if(s3_value.toInt() == 1){digitalWrite(D3, HIGH);}else if(s3_value.toInt() == 0){digitalWrite(D3, LOW);}
    if(s4_value.toInt() == 1){digitalWrite(D4, HIGH);}else if(s4_value.toInt() == 0){digitalWrite(D4, LOW);}
    if(s5_value.toInt() == 1){digitalWrite(D5, HIGH);}else if(s5_value.toInt() == 0){digitalWrite(D5, LOW);}
    if(s6_value.toInt() == 1){digitalWrite(D6, HIGH);}else if(s6_value.toInt() == 0){digitalWrite(D6, LOW);}
    if(s7_value.toInt() == 1){digitalWrite(D7, HIGH);}else if(s7_value.toInt() == 0){digitalWrite(D7, LOW);}
    if(s8_value.toInt() == 1){digitalWrite(D0, HIGH);}else if(s8_value.toInt() == 0){digitalWrite(D0, LOW);}
  }

void read_values(void){ //Read EEPROM VALUES
     //Relay 1 Name
      s1_name = read_eeprom(32,48);
      s2_name = read_eeprom(48,64);
      s3_name = read_eeprom(64,80);
      s4_name = read_eeprom(80,96);
 
     // Relay 1 Values  
      s1_value = read_eeprom(96,97);
      s2_value = read_eeprom(97,98);
      s3_value = read_eeprom(98,99);
      s4_value = read_eeprom(99,100); 
      
    //Relay 2 Name
      s5_name = read_eeprom(100,116);
      s6_name = read_eeprom(116,132);
      s7_name = read_eeprom(132,148);
      s8_name = read_eeprom(148,164);
      
     //Relay 2 Values 
      s5_value = read_eeprom(164,165);
      s6_value = read_eeprom(165,166);
      s7_value = read_eeprom(166,167);
      s8_value = read_eeprom(167,168); 
  }

//--------------Check Existing Current Connection---------------//

bool is_connected(void){
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED){
      return true;
    }
    
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}
//-----------------------------------------------------------//


//------------------------Uptime Function--------------------//
String get_uptime() { 
  unsigned long currentMillis;
  unsigned long seconds;
  unsigned long minutes;
  unsigned long hours;
  unsigned long days;
  String readableTime;

  currentMillis = millis();
  
  seconds =  currentMillis / 1000;
  minutes = seconds / 60;
  hours = minutes / 60;
  days = hours / 24;

  seconds %= 60;
  minutes %= 60;
  hours %= 24;

  if (days > 0) {
    readableTime = String(days) + " day(s) ";
  }

  if (hours > 0) {
     if (hours < 10) {
        readableTime += "0"+ String(hours) + ":";
      }
      
      else{
        readableTime += String(hours) + ":";  
      }
  }

  if (hours <= 0) {
    readableTime += "00:";
  }

  if (minutes < 10) {
    readableTime += "0";
  }
  readableTime += String(minutes) + ":";

  if (seconds < 10) {
    readableTime += "0";
  }
  readableTime += String(seconds);

  return readableTime;
}
//---------------------------------------------------------//


//---------------Read/Write to EEPROm functions------------//
void write_eeprom(int f, String var ){
  for (int i = 0; i < var.length(); ++i)
        {
          EEPROM.write(f + i, var[i]);
        }
  EEPROM.commit(); 
}
//End write eeprom

String read_eeprom(int f, int t){
  String var;
  for (int i = f; i < t; ++i){
        var += char(EEPROM.read(i));
      }  
  return var;
}
//------------------End Read/Write EEPROM------------------//


//-------------------Load WIFI Portal Setup----------------// 
void web_portal(){ 
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("IoT Controller","");
  dnsServer.start(DNS_PORT, "*", apIP);
    
  server.onNotFound([]() {
      server.sendHeader("Location", String("http://esp8266.local/"), true);
      server.send (302, "text/plain", "");  
    });
  
  portal_page();
}
//---------------------------------------------------------//

//--------------------HTTP Configuration page--------------// 
void portal_page()
{
 {
    server.on("/setup", []() { 
        
      String ssid_hs = hs_ssid.c_str();
      String ssid_pass = pass_ssid.c_str();
            
      content = head;
      content += "<h2 align='center'>WIFI SETTINGS</h2><hr>";
      content += "<div><form method='post' action='save'>";
      content += "<label>IPV4:</label><input name='ip' value='"+ ip +"' disabled>";
      content += "<br><label>MAC:</label><input name='mac' value='"+ mac +"' disabled>";
      content += "<br><label>SSID:</label><input name='ssid' value='"+ ssid_hs +"' length=16 placeholder ='16 Characters only' required>";
      content += "<br><label>*KEY:</label><input type='password' name='ssid_hs_pass' value='"+ ssid_pass +"' length=16 placeholder ='16 Characters only'><hr>"; 
      content += "<input type='submit' value='SAVE SETTINGS'></form></div>";
      content += foot;
      server.send(200, "text/html", content);
    });

    server.on("/save", []() {
      
      String ssid_hs = server.arg("ssid");
      String ssid_pass = server.arg("ssid_hs_pass");
         
      if (ssid_hs.length() > 0) { 
        Serial.println("clearing eeprom");//Clear EEPROM
        for (int i = 0; i < 32; ++i) {
          EEPROM.write(i, 0);
        }
          
        Serial.println("write values to eeprom");
        write_eeprom(0,ssid_hs);
        write_eeprom(16,ssid_pass);     
        content = "{\"Success\":\"Credentials saved to EEPROM... Rebooting controller to apply settings\"}";
        statusCode = 200;     
      }
      
      else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);

      if(statusCode == 200){
          for(i=9;i>=0;i--){         
                 Serial.println(i);           
                 delay(1000);
              }
          ESP.reset(); //Reboot NodeMCU
        }
    });

      server.on("/control", []() { 
        
      read_values();  
      String s1n = s1_name.c_str();
      String s2n = s2_name.c_str();
      String s3n = s3_name.c_str();
      String s4n = s4_name.c_str();
      String s5n = s5_name.c_str();
      String s6n = s6_name.c_str();
      String s7n = s7_name.c_str();
      String s8n = s8_name.c_str();
      
      String s1v = s1_value.c_str();
      String s2v = s2_value.c_str();
      String s3v = s3_value.c_str();
      String s4v = s4_value.c_str(); 
      String s5v = s5_value.c_str();
      String s6v = s6_value.c_str();
      String s7v = s7_value.c_str();
      String s8v = s8_value.c_str();
      String label = "";
      
      content = head;
      content += "<h2 align='center'>Uptime: "+ get_uptime() +"</h2><hr>";
      content += "<div><form method='post' action='apply'>";
      content += "<table width='100%'><thead><tr><th>RELAY</th><th>DEVICES</th><th>STATE</th></tr></thead><tbody>";
      if(s1v=="1"){label = "N.O.";}else{label="N.C.";}
      content += "<tr><td>[ 1 ]</td><td><input name='s1' value='"+ s1n +"'></td><td><select name='s1_v'><option value='"+ s1v +"'>"+ label +"</option><option value='0'>N.C.</option><option value='1'>N.O.</option></select></td></tr>";
      if(s2v=="1"){label = "N.O.";}else{label="N.C.";}
      content += "<tr><td>[ 2 ]</td><td><input name='s2' value='"+ s2n +"'></td><td><select name='s2_v'><option value='"+ s2v +"'>"+ label+"</option><option value='0'>N.C.</option><option value='1'>N.O.</option></select></td></tr>";
      if(s3v=="1"){label = "N.O.";}else{label="N.C.";}
      content += "<tr><td>[ 3 ]</td><td><input name='s3' value='"+ s3n +"'></td><td><select name='s3_v'><option value='"+ s3v +"'>"+ label +"</option><option value='0'>N.C.</option><option value='1'>N.O.</option></select></td></tr>";
      if(s4v=="1"){label = "N.O.";}else{label="N.C.";}
      content += "<tr><td>[ 4 ]</td><td><input name='s4' value='"+ s4n +"'></td><td><select name='s4_v'><option value='"+ s4v+"'>"+ label +"</option><option value='0'>N.C.</option><option value='1'>N.O.</option></select></td></tr>";
      if(s5v=="1"){label = "N.O.";}else{label="N.C.";}
      content += "<tr><td>[ 5 ]</td><td><input name='s5' value='"+ s5n +"'></td><td><select name='s5_v'><option value='"+ s5v +"'>"+ label +"</option><option value='0'>N.C.</option><option value='1'>N.O.</option></select></td></tr>";
      if(s6v=="1"){label = "N.O.";}else{label="N.C.";}
      content += "<tr><td>[ 6 ]</td><td><input name='s6' value='"+ s6n +"'></td><td><select name='s6_v'><option value='"+ s6v +"'>"+ label +"</option><option value='0'>N.C.</option><option value='1'>N.O.</option></select></td></tr>";
      if(s7v=="1"){label = "N.O.";}else{label="N.C.";}
      content += "<tr><td>[ 7 ]</td><td><input name='s7' value='"+ s7n +"'></td><td><select name='s7_v'><option value='"+ s7v +"'>"+ label +"</option><option value='0'>N.C.</option><option value='1'>N.O.</option></select></td></tr>";
      if(s8v=="1"){label = "N.O.";}else{label="N.C.";}
      content += "<tr><td>[ 8 ]</td><td><input name='s8' value='"+ s8n +"'></td><td><select name='s8_v'><option value='"+ s8v +"'>"+ label +"</option><option value='0'>N.C.</option><option value='1'>N.O.</option></select></td></tr>"; 
      content += "</tbody></table><hr>";     
      content += "<input type='submit' value='APPLY'></form></div>";   
      content += foot; 
       
      server.send(200, "text/html", content);
      
      });
    
      server.on("/apply", []() { 
           
      String s1_n = server.arg("s1");
      String s2_n = server.arg("s2");
      String s3_n = server.arg("s3");
      String s4_n = server.arg("s4");
      String s5_n = server.arg("s5");
      String s6_n = server.arg("s6");
      String s7_n = server.arg("s7");
      String s8_n = server.arg("s8");
      
      String s1_v = server.arg("s1_v");
      String s2_v = server.arg("s2_v");
      String s3_v = server.arg("s3_v");
      String s4_v = server.arg("s4_v");      
      String s5_v = server.arg("s5_v");
      String s6_v = server.arg("s6_v");
      String s7_v = server.arg("s7_v");
      String s8_v = server.arg("s8_v");
      
      Serial.println("Erasing values from eeprom");
      for (int i = 32; i < 168; ++i) {
          EEPROM.write(i, 0);
        }
      Serial.println("write values to eeprom"); 
      
      //4 Channel relay #1
      write_eeprom(32,s1_n);   
      write_eeprom(48,s2_n); 
      write_eeprom(64,s3_n);
      write_eeprom(80,s4_n); 
      write_eeprom(96,s1_v);
      write_eeprom(97,s2_v);
      write_eeprom(98,s3_v); 
      write_eeprom(99,s4_v); 

      //4 Channel relay #2
      write_eeprom(100,s5_n);   
      write_eeprom(116,s6_n); 
      write_eeprom(132,s7_n);
      write_eeprom(148,s8_n); 
      write_eeprom(164,s5_v);
      write_eeprom(165,s6_v);
      write_eeprom(166,s7_v); 
      write_eeprom(167,s8_v);  

      String loader = ".loader {margin: auto;top: 40%;border: 16px solid #1FA478;border-radius: 50%;border-top: 16px solid blue;border-bottom: 16px solid green;width: 120px;height: 120px;-webkit-animation: spin 2s linear infinite;animation: spin 2s linear infinite;}"
                      "@-webkit-keyframes spin {0% { -webkit-transform: rotate(0deg); }100% { -webkit-transform: rotate(360deg); }}"
                      "@keyframes spin {0% { transform: rotate(0deg); }100% { transform: rotate(360deg); }}";
                      
      String breaker = "<br><br><br><br><br><br>";
      
      String refresh = "<!DOCTYPE html><html><head><meta name='viewport' content=\'width=device-width,initial-scale=1\'><style>"+ loader +"</style><script>setTimeout(function(){window.location.href = '/';}, 1000);</script></head><body>"+ breaker +"<div class='loader'></div></body></html>";
      
      server.send(200, "text/html", refresh); 
         
      });       
  } 
}

