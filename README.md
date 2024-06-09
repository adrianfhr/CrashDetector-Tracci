# Crash Accident Detector

This project is an Arduino-based crash accident detector using an ESP32, GPS module, accelerometer, and Firebase for real-time database updates. The system detects crashes based on sudden changes in acceleration and sends the data to a Firebase Realtime Database, including the GPS coordinates, time, and vehicle information.

## Hardware Requirements

- ESP32
- GPS Module (e.g., NEO-6M)
- Accelerometer (e.g., ADXL335)
- OLED Display (optional, for debugging and display purposes)
- Breadboard and connecting wires

## Libraries Required

- `Wire.h`
- `WiFi.h`
- `Adafruit_GFX.h`
- `Adafruit_SSD1306.h`
- `Firebase_ESP_Client.h`
- `TinyGPS++.h`

## Setup Instructions

1. **Clone the Repository:**
   ```sh
   git clone https://github.com/adrianfhr/CrashDetector-Tracci.git
   cd CrashDetector-Tracci
    ```
2. **Install the Required Libraries:**
    Ensure you have the following libraries installed in your Arduino IDE. You can install them via the Library Manager (Sketch -> Include Library -> Manage Libraries):
    - Firebase ESP Client
    - TinyGPS++
    - Adafruit GFX Library
    - Adafruit SSD1306
3. **Update the WiFi Credentials and Firebase Configuration:**
    In the `CrashAccidentDetector2.ino` file, update the following lines with your WiFi credentials, Firebase API key, email, password, and database URL:
    ```cpp
    #define WIFI_SSID "your_wifi_ssid"
    #define WIFI_PASSWORD "your_wifi_password"

    #define API_KEY "your_firebase_api_key"
    #define USER_EMAIL "your_firebase_user_email"
    #define USER_PASSWORD "your_firebase_user_password"
    #define DATABASE_URL "your_firebase_database_url"
    ```
4. **Connect the Hardware:**

    - Connect the GPS module to the ESP32:
        - RX of GPS to GPIO 16 of ESP32
        - TX of GPS to GPIO 17 of ESP32
    - Connect the accelerometer to the ESP32 analog pins:
        - X-axis to GPIO 34
        - Y-axis to GPIO 35
        - Z-axis to GPIO 32
    - (Optional) Connect the OLED display to the I2C pins of the ESP32:
        - SDA to GPIO 21
        - SCL to GPIO 22
5. **Upload the Code:**
    Open the `CrashAccidentDetector2.ino` file in the Arduino IDE, select the correct board and port, and upload the code.

## How It Works

1. **WiFi Initialization:**
    The ESP32 connects to the specified WiFi network.

2. **Firebase Authentication:**
    The ESP32 authenticates with Firebase using the provided API key, email, and password.

3. **GPS Data Acquisition:**
    The ESP32 reads the GPS coordinates from the GPS module.

4. **Acceleration Monitoring:**
    The ESP32 reads the acceleration values from the accelerometer and calculates the magnitude of changes to detect crashes.

5. **Crash Detection:**
    When a crash is detected (based on the magnitude threshold), the system records the crash data, including time, GPS coordinates, and vehicle information, and sends it to the Firebase Realtime Database.

6. **Database Update:**
    The crash data is stored in Firebase under a unique crash ID.

## Notes

- Adjust the sensitivity of the crash detection by changing the `sensitivity` and `devibrate` variables in the code.
- Ensure that the Firebase project is properly set up with Realtime Database and the required authentication methods.

## Troubleshooting

- If the ESP32 fails to connect to WiFi, check the WiFi credentials and signal strength.
- If the Firebase operations fail, ensure that the API key, email, password, and database URL are correct and that the ESP32 has internet access.
- Use the Serial Monitor to debug and check the output for any error messages.

## Contributors

- Adrian Fahri Affandi - 18221002
- Farchan Martha Adji Chandra - 18221011
- Raden Sjora Okalani - 18221014
- Nabilah Amanda Putri - 18221021
- Athira Dhyanissa Tafkir - 18221022
