#include <WiFi.h>
#include <SoftwareSerial.h>



// Wi-Fi
const char* ssid = "Baker"; //oasis-asus
const char* password = "bakerchen"; // 19911991

const uint16_t port = 8888;
const char* host = "172.20.10.5"; //"172.20.10.3" 



// Parameters
const float init_degree = 216; //216
const float scaling = 5;
const float degree_per_scale = 7.2;
const float value_per_scale = 0.1;
const float threshold = 1.0;

WiFiServer server(80);

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200); // PC
  Serial2.begin(115200); // EM9D
  delay(10);

  Serial.println("Connect to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("...");
  }
  Serial.println("");
  Serial.println("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiClient client;
  if (!client.connect(host, port)){
    Serial.println("Connetction to host failed");
    delay(4000);
    return;
  }
  
  String buf = ""; // String Buffer
  float pred = 0; // Prediction
  float map_degree = 0;
  float gauge_val = 0;
  
    while(Serial2.available() > 0){
      char receivedData = Serial2.read();
      // Serial.print(receivedData);
      
      buf = buf + receivedData;
      if(receivedData == '\n'){
        client.print(buf);
        pred = buf.toFloat();
        map_degree = int(pred * scaling + 360 - init_degree) % 360;
        gauge_val = (map_degree* value_per_scale)/degree_per_scale;
        
        Serial.print("Prediction: ");
        Serial.println(pred);
        
        Serial.print("Gauge Value: ");
        Serial.println(gauge_val);

        String buf = "";
        Serial.println("------------------");

        if(gauge_val > threshold){
          digitalWrite(2, HIGH);
        }
        else{
          digitalWrite(2, LOW);
        }
      }
  }
}
