#include <Arduino.h>
#include <WiFi.h>


#include <ArduinoOTA.h>
#include <PubSubClient.h>




/******************************************************************************************************************
* Sendet Degug information auf den MQTT-Kanal
* void mqttDebugInfo(String load ) 
*******************************************************************************************************************/

void DebugInfo(String load ) {
     #ifdef __mqtt_DEBUG

        load.toCharArray(msg,150);
        client.publish(mqtt_pub_Debug, msg);
     #endif
     #ifdef __serial_DEBUG
        Serial.println(load);
     #endif
        
     }





//**************************************************************************
//                          attach WiFi
//**************************************************************************

void attach_wifi() {

    WiFi.mode(WIFI_STA); // Optional
    WiFi.setTimeout(20000);
    WiFi.begin(ssid, password);
    Serial.print("\nConnecting ");

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
 
  
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        rp2040.restart();
    }

    


}


//**************************************************************************
//                          handle mqtt
//**************************************************************************



/******************************************************************************************************************
  *  void callback(char* topic, byte* payload, unsigned int length) void set_WarmColor()
  *  wird über mqtt subscribe getriggert, sobald von OPENHAB rom Event gemeldet wird
  *  
*******************************************************************************************************************/
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
        {
            // erst mal alles in Strings verwandeln für die Ausgabe und den Vergleich
            String str_topic=String(topic);
            
            char char_payload[20];
            unsigned int i=0;
            for (i = 0; i < length; i++) { char_payload[i]=(char)payload[i];}
            char_payload[i]=0;
            String str_payload=String(char_payload);
                
            String target_rpn=String();
            String str_mqtt_openHab_command=String(mqtt_pub_Val1);

            String out="Message arrived, Topic : [" + str_topic + "] Payload : [" + str_payload + "]";
              
            DebugInfo(out);
            

            //
            //---- Kommando abfragen
            //
                       
            if (str_topic == mqtt_sub_Val1) air_temp = str_payload.toFloat();
            else 
                if (str_topic == mqtt_sub_Val2) tub_temp_up = str_payload.toFloat();
                else 
                    if (str_topic == mqtt_sub_Val3) tub_temp_down = str_payload.toFloat();
                      else 
                        if (str_topic == mqtt_sub_Val4) filter = str_payload;
                          else 
                            if (str_topic == mqtt_sub_Val5) whirl = str_payload;
                              else 
                                if (str_topic == mqtt_sub_Val6) power = str_payload;
                                  else 
                                    if (str_topic == mqtt_sub_Val7) env_brightness = str_payload.toFloat();

                                
                  
                  
                  
                    
       }











void attach_mqtt() {

    IPAddress ip = WiFi.localIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    String MQTT_HostName=String(HOSTNAME)+"_"+ipStr;
    char MQTT_HostNameChar[MQTT_HostName.length()];

  
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(ipStr);

    MQTT_HostName.toCharArray(MQTT_HostNameChar,sizeof(MQTT_HostNameChar)); 
    client.setServer(mqtt_server, 1883);
    client.setCallback(mqtt_callback);

    int loop = 0;
    while (!client.connected()) {
                client.connect((char*)MQTT_HostNameChar);
                Serial.print(".");
                loop++;
                if (loop > 2) break;
    }
    if (loop > 2) {

        Serial.println("");
        Serial.println("NQTT-Server not found ...");
        delay(2000);
    }
    else  
     {
       Serial.println("");
       Serial.print("MQTT found Adress: ");
       Serial.println(MQTT_HostNameChar);
     }

}






void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_pub_lastconnect, msg);
      
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}




/******************************************************************************************************************
  * void mqtt_subscribe()
  * Trägt sich für Botschaften ein
*******************************************************************************************************************/
void mqtt_subscribe()
      {
              // ... and subscribe
              client.subscribe(mqtt_sub_Val1);
              client.subscribe(mqtt_sub_Val2);
              client.subscribe(mqtt_sub_Val3);
              client.subscribe(mqtt_sub_Val4);
              client.subscribe(mqtt_sub_Val5);
              client.subscribe(mqtt_sub_Val6);
              client.subscribe(mqtt_sub_Val7);
       


                            
      }






//**************************************************************************
//                          handle OTA
//**************************************************************************

void init_OTA() {
ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
   });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();



}

