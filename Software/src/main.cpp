// Version 1.1 v. 30.03.2024 Temperaturanzeige nur, wenn Energie ein
// Version 1.2 v. 02.04.2024 now with daylightsaving for the correct location, using RTC for timecount
// Version 1.3 v. 02.04.2024 correct text on display
// Version 1.4 v. 03.04.2024 char color changes while button pressed



// install pioasm
// https://gist.github.com/hexeguitar/f4533bc697c956ac1245b6843e2ef438
// install toolchain 
// https://arduino-pico.readthedocs.io/en/latest/platformio.html#examples

// DEGUGINFO
//#define __serial_DEBUG
#define __mqtt_DEBUG

#include <Arduino.h>

#include <CPU.h>


#include <GFXMatrix.h>
#include <WiFi.h>



#include <ArduinoOTA.h>
#include <PubSubClient.h>

// time stuff
#include <NTPClient.h>
#include <hardware/rtc.h>
#include <pico/util/datetime.h>

#include <watch.h>


#define SSI_ID  "your_SSID"
#define SSI_PWD "your_password"


WiFiUDP udpSocket;

#define NTP_SERVER1       "ptbtime1.ptb.de"
#define NTP_SERVER2       "europe.pool.ntp.org"
#define TIME_ZONE_BERLIN  "TZ","CET-1CEST,M3.5.0,M10.5.0/3"
// Define NTP Client to get time
NTPClient ntpClient(udpSocket, NTP_SERVER1);

WiFiClient espClient;
PubSubClient client(espClient);
GFXMatrix matrix(64, 64);
CPU cpu;
Watch my_Watch(&matrix);



#include <drawpicture.h>


const char* ssid = SSI_ID;
const char* password = SSI_PWD;
const char* mqtt_server = "VM2-MQTT";

#define MSG_BUFFER_SIZE	(100)
char msg[MSG_BUFFER_SIZE];
char dt[MSG_BUFFER_SIZE];



String Version    = "V1.40  ";
String AppName    = "IoT-LED-Matrix";

// mqtt publish values

const char* mqtt_pub_Val1             = "/openHAB/LED_Matrix/CPU_Temperature";
const char* mqtt_pub_RSSI             = "/openHAB/LED_Matrix/RSSI";
const char* mqtt_pub_Version          = "/openHAB/LED_Matrix/Version";
const char* mqtt_pub_Debug            = "/openHAB/LED_Matrix/Debug";
const char* mqtt_pub_lastconnect      = "/openHAB/LED_Matrix/lastReconnect";
const char* mqtt_pub_lastmsg          = "/openHAB/LED_Matrix/lastMessage";
const char* mqtt_pub_brightness       = "/openHAB/LED_Matrix/percentbrightness";

const char* mqtt_pub_energycontrol_c  = "/openHAB/gardenpower/switch/set_Power";                  // openhab command topic
const char* mqtt_pub_whirlcontrol_c   = "/openHAB/hot_tub_control/switch/set_power2";             // openhab command topic
const char* mqtt_pub_filtercontrol_c  = "/openHAB/hot_tub_control/switch/set_power1";             // openhab command topic

// mqtt subscribe values
const char* mqtt_sub_Val1         = "/openHAB/house/tempair";
const char* mqtt_sub_Val2         = "/openHAB/hot_tub_control/sensor/temperatur_vorlauf/state";
const char* mqtt_sub_Val3         = "/openHAB/hot_tub_control/sensor/temperatur_ruecklauf/state";
const char* mqtt_sub_Val4         = "/openHAB/hot_tub_control/switch/relay_output1/state";
const char* mqtt_sub_Val5         = "/openHAB/hot_tub_control/switch/relay_output2/state";
const char* mqtt_sub_Val6         = "/openHAB/gardenpower/switch/relay_output/state";
const char* mqtt_sub_Val7         = "/openHAB/house/brightness";


// IO
#define INPUT_BUTTON_FILTER 4   // IO GP4
#define INPUT_BUTTON_WHIRL  3   // IO GP3
#define INPUT_BUTTON_ENERGY 5   // IO GP5


#define POS_INFORECT_LI     1
#define POS_INFORECT_MI     22
#define POS_INFORECT_RE     43
#define ENERGY_CHAR         'E'
#define WHIRLPUMP_CHAR      'W'
#define FILTERPUMP_CHAR     'F'


// globals
float   cpu_temp        = 0.0f;   // temperature 2040 CPU
float   air_temp        = 0.0f;   // temperature air 
float   tub_temp_up     = 0.0f;   // temperature hot tub surface
float   tub_temp_down   = 0.0f;   // temperature hot tub bottom
int     rssi            = 0;      // field force 
int     env_brightness  = 0;      // brightness environment


String  filter          = "-";
String  whirl           = "-";
String  power           = "-";

u_int16_t second        = 0;
u_int16_t minute        = 0;
u_int16_t hour          = 0;

bool      switchdisplay = false;

#define   MAXCPUTEMP    50
bool      temp_error    = false;   // CPU-TEMP to high

int       percent_displaybrightness = 0;


uint      button_whirl_pump   = 0;
uint      button_energy       = 0;
uint      button_filter_pump  = 0;

time_t    systemtime          = 0;      // epoch time
struct tm *timeinfo;                    // cpp time

// time struct for RTC
datetime_t t = {
        .year  = 2120,
        .month = 06,
        .day   = 05,
        .dotw  = 1, // 0 is Sunday, so 5 is Friday
        .hour  = 0,
        .min   = 0,
        .sec   = 0
    };




#define   SCEDULE_SYSTEMVALUES 5000     // 5  seconds
#define   SCEDULE_TIME         1000     // 1  second
#define   SCEDULE_INFO         10000    // 10 seconds
#define   SCEDULE_WRITEOUT     1000     // 1  second
#define   SCEDULE_RR           120000   // 2 minutes
#define   SCEDULE_INPUT        100     //  0.1  second 

unsigned long thread_get_sysvalues  = 0;
unsigned long thread_get_time       = 0;
unsigned long thread_writeinfo      = 0;
unsigned long thread_writeout       = 0;
unsigned long thread_roundrobin     = 0;
unsigned long thread_readinput      = 0;

#define BLACK     0x0000
#define BLUE      0x001F
#define RED       0xF800
#define GREEN     0x07E0
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define WHITE     0xFFFF
#define GRAY1     0x632c


#define RECT      20
#define DISPLAY_HEIGT 64

String HOSTNAME = "IoT-LEDMatrix";

#include <helpers.h>


void write_temperatures(bool force);


void get_sys_values() {

  long now = millis();
  if (now - thread_get_sysvalues < SCEDULE_SYSTEMVALUES) return;

  cpu_temp  =   cpu.getTemperature();
  rssi      =   WiFi.RSSI();

  if (cpu_temp > MAXCPUTEMP) temp_error = true; else temp_error = false;

  

  // publish rssi
  snprintf (msg, MSG_BUFFER_SIZE,"%d", rssi);
  client.publish(mqtt_pub_RSSI, msg);
  // publish Temperature
  snprintf (msg, MSG_BUFFER_SIZE,"%.1f", cpu_temp);
  client.publish(mqtt_pub_Val1, msg);
  
  client.publish(mqtt_pub_lastmsg,dt);

  snprintf (msg, MSG_BUFFER_SIZE,"%d", percent_displaybrightness);
  client.publish(mqtt_pub_brightness, msg);

  thread_get_sysvalues = now;

}

// switch between clock Page and temperatur page
void roundrobin() {

  long now = millis();
  if (now - thread_roundrobin < SCEDULE_RR ) return;
  
  switchdisplay = !switchdisplay;
  
  ntpClient.update();

  thread_roundrobin = now;

}





void get_time() {

  long now = millis();
  if (now - thread_get_time < SCEDULE_TIME) return;

  rtc_get_datetime(&t);
  hour = t.hour;
  minute = t.min;
  second = t.sec;

 
  //calculate brightness depend on Openhab value 0 - 5000 equal to 10% to 80 %  
  if(env_brightness < 0 || env_brightness > 5000 ) env_brightness = 0;
  #define MAX_BRIGHTNESS 5000
  // calculate percent brightness of the current time of day
  int br_percent  = (int)(100.0f * env_brightness / MAX_BRIGHTNESS) ;  

  //snprintf(msg,MSG_BUFFER_SIZE,"ist Helligkeit %% %d",br_percent);
  //DebugInfo(msg);


  percent_displaybrightness = (int)(0.8f * br_percent + 20.0f);
  // if CPU temperature too high, set brigtness to 10 % else like calculated
  if (temp_error) percent_displaybrightness = 10; 

  //snprintf(msg,MSG_BUFFER_SIZE,"set Helligkeit %d",percent_displaybrightness);
  //DebugInfo(msg);

  matrix.setBrightness(percent_displaybrightness);
  

  thread_get_time = now;

}

void inforect(int x, int y, int w, uint16_t bkcolor, char mychar, uint16_t chrcolor) {

  matrix.drawRect(x, DISPLAY_HEIGT - w -1, w, w, WHITE);
  matrix.fillRect(x+1, DISPLAY_HEIGT - w , w-2, w-2, bkcolor);
  matrix.drawChar(x + 5, y +3, mychar, chrcolor, BLACK, 2);

}


void draw_clock() {

    long now = millis();
    if (now - thread_writeout < SCEDULE_WRITEOUT) return;
  
    
    matrix.setTextSize(1);
    matrix.clear();

    my_Watch.showtime(hour, minute, second);
    
    matrix.display();

  thread_writeout = now;

}


void get_inputs() {

    long now = millis();
    if (now - thread_readinput< SCEDULE_INPUT) return;
  

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

    button_energy =       digitalRead(INPUT_BUTTON_ENERGY);
    button_whirl_pump =   digitalRead(INPUT_BUTTON_WHIRL);
    button_filter_pump =  digitalRead(INPUT_BUTTON_FILTER);
    
    
    //Serial.print("Energy : "); Serial.println(button_energy);
    //Serial.print("whirl : "); Serial.println(button_whirl_pump);
    //Serial.print("filter : "); Serial.println(button_filter_pump);


    // toggle energy when button pressed

    if (button_energy == 1) {
      switchdisplay = false;
      write_temperatures(true);
      matrix.drawChar(POS_INFORECT_LI + 5, DISPLAY_HEIGT - RECT +2, ENERGY_CHAR, YELLOW , BLACK, 2);
      matrix.display();
      if (power == "ON") {
        client.publish(mqtt_pub_energycontrol_c, "OFF"); 
      
      }  
      else {
        client.publish(mqtt_pub_energycontrol_c, "ON"); 
      }
      while (digitalRead(INPUT_BUTTON_ENERGY) == 1);     
      matrix.drawChar(POS_INFORECT_LI + 5, DISPLAY_HEIGT - RECT +2, ENERGY_CHAR, GRAY1 , BLACK, 2);
      matrix.display();

    }  

    if (button_whirl_pump == 1) {
      switchdisplay = false;
      write_temperatures(true);
      matrix.drawChar(POS_INFORECT_MI + 5, DISPLAY_HEIGT - RECT +2, WHIRLPUMP_CHAR, YELLOW , BLACK, 2);
      matrix.display();
      if (whirl == "ON") {
        client.publish(mqtt_pub_whirlcontrol_c, "OFF"); 
      
      }  
      else {
        client.publish(mqtt_pub_whirlcontrol_c, "ON"); 
           
      }
      while (digitalRead(INPUT_BUTTON_WHIRL) == 1);     
      matrix.drawChar(POS_INFORECT_MI + 5, DISPLAY_HEIGT - RECT +2, WHIRLPUMP_CHAR, GRAY1 , BLACK, 2);
      matrix.display();

    }  


    if (button_filter_pump == 1) {
      switchdisplay = false;
      write_temperatures(true);
      matrix.drawChar(POS_INFORECT_RE + 5, DISPLAY_HEIGT - RECT +2, FILTERPUMP_CHAR, YELLOW , BLACK, 2);
      matrix.display();
      if (filter == "ON") {
        client.publish(mqtt_pub_filtercontrol_c, "OFF"); 
      
      }  
      else {
        client.publish(mqtt_pub_filtercontrol_c, "ON"); 
     
      }
      while (digitalRead(INPUT_BUTTON_FILTER) == 1);
      matrix.drawChar(POS_INFORECT_RE + 5, DISPLAY_HEIGT - RECT +2, FILTERPUMP_CHAR, GRAY1 , BLACK, 2);     
      matrix.display();

    }  
  
  thread_readinput = now;

}






void write_temperatures(bool force) {

  long now = millis();  
  if (!force) if (now - thread_writeout < SCEDULE_WRITEOUT) return;
  
  
  #define CHAR_HEIGHT   8
  #define XPOS          2

  int ypos = 1;
   

  matrix.setTextSize(1);
  matrix.clear();

  // write out current time
  matrix.setCursor(8,ypos);

  matrix.setTextColor(MAGENTA);
  snprintf(dt,MSG_BUFFER_SIZE,"%02d:%02d:%02d\n",hour,minute,second);
   
  matrix.println(dt);
  
  // write out Temperature air
  ypos+= CHAR_HEIGHT+4;
  matrix.setCursor(XPOS,ypos);
  matrix.setTextColor(WHITE);
  snprintf(msg,MSG_BUFFER_SIZE,"L: %.1f ",air_temp); 
  matrix.print(msg);  matrix.write(0xF8);  matrix.write(0x43); 

  // write out Temperature up

  ypos+= CHAR_HEIGHT+4;
  matrix.setCursor(XPOS,ypos);
  if (tub_temp_up <= 29) matrix.setTextColor(BLUE);
    else if ((tub_temp_up > 29) && (tub_temp_up <=32)) matrix.setTextColor(MAGENTA);
      else if ((tub_temp_up > 32) && (tub_temp_up <=34)) matrix.setTextColor(YELLOW);
        else if ((tub_temp_up > 34) && (tub_temp_up <=36)) matrix.setTextColor(GREEN);
          else if (tub_temp_up > 36) matrix.setTextColor(RED);
  if (power == "ON") snprintf(msg,MSG_BUFFER_SIZE,"O: %.1f ",tub_temp_up); else snprintf(msg,MSG_BUFFER_SIZE,"O: ---- ");        
  matrix.print(msg);  matrix.write(0xF8);  matrix.write(0x43);

  // write out Temperature down
  ypos+= CHAR_HEIGHT;
  matrix.setCursor(XPOS,ypos);
  if (tub_temp_down <= 29) matrix.setTextColor(BLUE);
    else if ((tub_temp_down > 29) && (tub_temp_down <=32)) matrix.setTextColor(MAGENTA);
      else if ((tub_temp_down > 32) && (tub_temp_down <=34)) matrix.setTextColor(YELLOW);
        else if ((tub_temp_down > 34) && (tub_temp_down <=36)) matrix.setTextColor(GREEN);
          else if (tub_temp_down > 36) matrix.setTextColor(RED);
  if (power == "ON") snprintf(msg,MSG_BUFFER_SIZE,"U: %.1f ",tub_temp_down); else snprintf(msg,MSG_BUFFER_SIZE,"U: ---- ");        
  matrix.print(msg);  matrix.write(0xF8);  matrix.write(0x43);


  // Power off / on
  if (power == "ON") inforect(POS_INFORECT_LI, DISPLAY_HEIGT - RECT -1, RECT, GREEN, ENERGY_CHAR, BLACK);
      else {
          inforect(1, DISPLAY_HEIGT - RECT -1, RECT, BLACK, ENERGY_CHAR, GRAY1);
          filter = "OFF";
          whirl = "OFF";
      }
      
  // Whirl off / on
  if (whirl == "ON") inforect(POS_INFORECT_MI, DISPLAY_HEIGT - RECT -1, RECT, GREEN, WHIRLPUMP_CHAR, BLACK);
      else inforect(22, DISPLAY_HEIGT - RECT -1, RECT, BLACK, WHIRLPUMP_CHAR,GRAY1);  

  // Filter off / on
  if (filter == "ON" ) inforect(POS_INFORECT_RE, DISPLAY_HEIGT - RECT -1, RECT, RED, FILTERPUMP_CHAR, BLACK);
      else inforect(43, DISPLAY_HEIGT - RECT -1, RECT, BLACK, FILTERPUMP_CHAR, GRAY1);

  // show error if cputemperature too high
  if (temp_error) matrix.drawChar(0, 0, 'T', RED, BLACK, 1);
    
  matrix.display();

  thread_writeout = now;

}



void write_info() {

long now = millis();
  if (now - thread_writeinfo < SCEDULE_INFO) return;
  
    snprintf(dt,MSG_BUFFER_SIZE," Time                    : %02d:%02d:%02d", hour, minute, second);
    DebugInfo(dt);    
    snprintf(msg,MSG_BUFFER_SIZE, "Temperatur Luft        : %.1f °C",air_temp);
    DebugInfo(msg);
    snprintf(msg,MSG_BUFFER_SIZE, "Helligkeit Umwelt      : %d" ,env_brightness);
    DebugInfo(msg);
    snprintf(msg,MSG_BUFFER_SIZE, "Temperatur HotTub Oben : %.1f °C",tub_temp_up);
    DebugInfo(msg);
    snprintf(msg,MSG_BUFFER_SIZE, "Temperatur HotTub Unten: %.1f °C",tub_temp_down);
    DebugInfo(msg);
    String myString =             "Schalter Filterpumpe   : " + filter;
    DebugInfo(myString);
    myString=                     "Schalter Whirlpumpe    : " + whirl;
    DebugInfo(myString);
    myString =                    "Schalter Strom für Pool: " + power;
    DebugInfo(myString);
    DebugInfo(                    "Temperatur CPU         : " + String(cpu_temp) + " °C");
    DebugInfo(                    "RSS                    : " + String(rssi) + " db");

  thread_writeinfo = now;



}







void setup() {
  

 
  //pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(19200);
  Serial.println("Booting");

  // init inputs
  pinMode(INPUT_BUTTON_ENERGY,INPUT); pinMode(INPUT_BUTTON_WHIRL,INPUT); pinMode(INPUT_BUTTON_FILTER,INPUT);
  pinMode(LED_BUILTIN,OUTPUT);

  matrix.begin();
  matrix.clear();
  matrix.cp437(true);
  matrix.setTextSize(1);
  matrix.setTextColor(GREEN);
  matrix.setCursor(0,0);
  matrix.println("booting");
  matrix.display();
  matrix.setCursor(0,8);
  matrix.println("WiFi");
  matrix.display();

  cpu.begin();  // Initialize the CPU temperature sensor
  
  attach_wifi();
  attach_mqtt();
  mqtt_subscribe();

  // set correct timezone Berlin daylightsaving
  setenv(TIME_ZONE_BERLIN,1),
  tzset();

  // setup ntp and RTC
  ntpClient.begin();
  ntpClient.update();
  systemtime = ntpClient.getEpochTime();
  timeinfo = localtime( &systemtime );
  // intit realtimeclock
  rtc_init(); 
  t.hour = timeinfo->tm_hour;
  t.min  = timeinfo->tm_min;
  t.sec  = timeinfo->tm_sec;
  rtc_set_datetime(&t);

  matrix.setCursor(0,16);
  matrix.println("MQTT");
  matrix.display();
  
  // publish version
  String out=Version+AppName;
  DebugInfo("Version: "+out);
  out.toCharArray(msg,75);
  client.publish(mqtt_pub_Version, msg);
  client.publish(mqtt_pub_lastconnect,dt);


  matrix.setCursor(0,24);
  matrix.println("V "+Version);
  matrix.display();
  delay(2000);

  init_OTA();

  matrix.clear();
  drawImage(0,0);
  matrix.display();

  delay(20000);

}



void loop() {
  
    roundrobin();

    if (!switchdisplay) write_temperatures(false); else draw_clock();
    //draw_clock();
    
    get_sys_values();

    get_time();

    write_info();

    if (!client.connected()) {
      reconnect();
    }
  
    client.loop();

    ArduinoOTA.handle();

    get_inputs();
    
  }

