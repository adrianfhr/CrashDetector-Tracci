#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Firebase_ESP_Client.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <time.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a HardwareSerial object to communicate with the GPS module
HardwareSerial SerialGPS(1); // Use Serial1 for ESP32

#include <Wire.h>
#include <TinyGPS++.h>
#include <math.h>

// Insert your network credentials
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

#define API_KEY "your_firebase_api_key" //API KEY FIREBASE

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "your_firebase_user_email"
#define USER_PASSWORD "your_firebase_user_password"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "your_firebase_database_url"

#define RX_GPS 16
#define TX_GPS 17

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;
String carID = "-NyGzbU4PBewI8RfMeBE";
String crashID;
String date;
String model = "Honda Brio"; 
String platNomor = "B 1234 BC";
String newCollection;

// Variable locations
float lat = -6.925764613672474;
float lgt = 107.76738840816121;

// Variables to save database paths



String databasePath;
String lgtPath;
String latPath;
String createdAtPath;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 2000;


#define xPin 34
#define yPin 35
#define zPin 32

byte updateflag;
int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 0, magnitude = 0, sensitivity = 30, devibrate = 75;
double angle;
//Used to run impact routine every 2mS.
unsigned long time1;


// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Write integer values to the database
void sendInt(String path, int value){
  if (Firebase.RTDB.setInt(&fbdo, path.c_str(), value)){
    Serial.print("Writing value: ");
    Serial.print (value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

// Write integer values to the database
void sendFloat(String path, float value){
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)){
    Serial.print("Writing value: ");
    Serial.print (value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}
// Write integer values to the database
void sendString(String path, String value){
  if (Firebase.RTDB.setString(&fbdo, path.c_str(), value)){
  Serial.print("Writing value: ");
  Serial.print (value);
  Serial.print(" on the following path: ");
  Serial.println(path);
  Serial.println("PASSED");
  Serial.println("PATH: " + fbdo.dataPath());
  Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

void initTime() {
  const long gmtOffset_sec = 7 * 3600; // WIB is UTC+7
  const int daylightOffset_sec = 0; // No daylight saving time in WIB

  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}


String getCurrentTime() {
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  char buffer[80];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}


void setup() {
  
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, RX_GPS, TX_GPS);
  delay(1000);

  initWiFi();
  initTime();

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(zPin, INPUT);

  
  //Firebase setUp
  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
  time1 = micros();

}


void loop() {

  //Read location
  if (SerialGPS.available()) {
    while (SerialGPS.available()) {
      if (gps.encode(SerialGPS.read())) {
        // If valid data is available, print the GPS information
        if (gps.location.isValid()) {
          lat = gps.location.lat();
          lgt = gps.location.lng();
          Serial.print("Latitude: ");
          Serial.println(lat);
          Serial.print("Longitude: ");
          Serial.println(lgt);
        }
      }
    }
  }


  if (Firebase.ready()){
    // sendDataPrevMillis = millis();
    if (micros() - time1 > 1999) Impact();
      
      

  //--------------------------------------------------------------
    if(updateflag > 0) 
    {
      updateflag=0;
      Serial.print("Impact detected!!");
      Serial.print("Magnitude:"); Serial.print(magnitude);
      Serial.print("\t Angle:"); Serial.print(angle, 2);
      Serial.print("\t Car Direction: NaN");
      Serial.println();

      if (Firebase.RTDB.pushString(&fbdo, "/crash", "initial_data")) {
        newCollection = fbdo.pushName();  // Mendapatkan ID koleksi baru
        Serial.println("Data pushed successfully");
        Serial.print("Pushed data key: ");
        Serial.println(newCollection);

        // Memperbarui jalur database
        databasePath = "/crash/" + newCollection;

        // Update database path for sensor readings
        latPath = databasePath + "/latitude";
        lgtPath = databasePath + "/longitude";
        
        date = getCurrentTime();
        
        sendString((databasePath + "/carID"), carID);
        sendString((databasePath + "/crashID"), newCollection);
        sendString((databasePath + "/date"), date);
        sendFloat(latPath, lat);
        sendFloat(lgtPath, lgt);
        sendString((databasePath + "/model"), model);
        sendString((databasePath + "/platNomor"), platNomor);
        sendString(("/admins/" + uid + "/status"), newCollection);

        

      } else {
        Serial.println("Failed to push data");
        Serial.println(fbdo.errorReason());
      }
    }


  }
  delay(1000);
}

void Impact()
{
  //--------------------------------------------------------------
  time1 = micros(); // resets time value
  //--------------------------------------------------------------
  int oldx = xaxis; //store previous axis readings for comparison
  int oldy = yaxis;
  int oldz = zaxis;

  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);

  Serial.print("X: ");
  Serial.print(xaxis);
  Serial.print(" Y: ");
  Serial.print(yaxis);
  Serial.print(" Z: ");
  Serial.println(zaxis);

  
  //--------------------------------------------------------------
  //loop counter prevents false triggering. Vibration resets if there is an impact. Don't detect new changes until that "time" has passed.
  vibration--; 
  //Serial.print("Vibration = "); Serial.println(vibration);
  if(vibration < 0) vibration = 0;                                
  //Serial.println("Vibration Reset!");
  
  if(vibration > 0) return;
  //--------------------------------------------------------------
  deltx = xaxis - oldx;                                           
  delty = yaxis - oldy;
  deltz = zaxis - oldz;
  
  //Magnitude to calculate force of impact.
  magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));
  Serial.print("Magnitude; ");
  Serial.print(magnitude);
  Serial.print("  Sensi: ");
  Serial.println(sensitivity);
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  if (magnitude >= sensitivity) //impact detected
  {
    updateflag=1;
    // reset anti-vibration counter
    vibration = devibrate;
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  else
  {
    //if (magnitude > 15)
      //Serial.println(magnitude);
    //reset magnitude of impact to 0
    magnitude=0;
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}




