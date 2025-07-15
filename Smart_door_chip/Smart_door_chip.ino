#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FS.h> 
#include <NTPClient.h>
#include <WiFiUdp.h> 

#include <SoftwareSerial.h>

SoftwareSerial btSerial(D7, D6);

class NetworkDetails {
public:
  //packing these in a class, made public because memeber variables are static and constant
  //Also, they're not more safe in private becuase the values are not changed anyways 
  //Constexpr becomes into machine code immediately, doesn't use RAM or run-time (CHANGED)
  static const char* m_ssid;
  static const char* m_password; 
  static const char* m_serverURL; 
};

const char* NetworkDetails::m_ssid = ""; //PUT YOUR SSID
const char* NetworkDetails::m_password = ""; //PUT YOUR WIFI PASSWORD
const char* NetworkDetails::m_serverURL = "";  //PUT SERVER-URL

WiFiUDP ntpUDP;
NTPClient time_client(ntpUDP, "pool.ntp.org", 3600 * 2, 60'000);


 void retrieveMobileData(String activity){
  if (btSerial.available()) {
    delay(100); // Delay to give time for characters to be retrieved
    Serial.print("Fra HC-05: ");
    while (btSerial.available()) {
      char c = btSerial.read();
      activity += c;
      Serial.print(c);
    }
    Serial.println();
  }
 }

 String format_timestamp(NTPClient& time_client){
  String year = String(1970 + time_client.getEpochTime() / 31556926);
  String month = String((time_client.getEpochTime() / 2629743) % 12 + 1);
  String day = String((time_client.getEpochTime() / 86400) % 31 + 1);
  String time = time_client.getFormattedTime(); 

  return year + "-" + month + "-" + day + " " + time;
}

void sendToServer(String activity) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    
    http.begin(client, NetworkDetails::m_serverURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Lost WiFi-connection...");
    }

    String postData = "activity=" + activity;
    int httpResponseCode = http.POST(postData);
    //HAD Serial.println("HTTP Error: " + http.errorToString(httpResponseCode));
  

  if (httpResponseCode > 0) {
    Serial.println("HTTP return code: " + String(httpResponseCode));
  } else {
    Serial.println("Failed sending message to server...");
  }

  http.end();
} else {
  Serial.println("Not connected to WiFi...");
  }
}


void runCode() {
  time_client.update();
  String formatted_time = format_timestamp(time_client);
  String current_time = time_client.getFormattedTime();

  String activity;
  retrieveMobileData(activity); 

  String log_entry = "[" + formatted_time + "] " + activity;  
  Serial.println("Sending data to the webserver ...");
  delay(5000);
  sendToServer(log_entry);

  String return_message = "RETURN MESSAGE FOR ANDROID";
  for (int i=0; i < return_message.length(); i++){
    btSerial.print(return_message[i]);
  }
  btSerial.println("");

  delay(10'000);
  
}

//--------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(5000);
  btSerial.begin(9600);

  if (!SPIFFS.begin()){
    Serial.println("SPIFFS initialisation failed...");
    return;
  }
  Serial.println("SPIFFS has initialised!");

  WiFi.begin(NetworkDetails::m_ssid, NetworkDetails::m_password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  //Eventual additional logs with connection status

  time_client.begin();

}




void loop() {
  
  runCode();

}

