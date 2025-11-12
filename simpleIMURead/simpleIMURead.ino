// simple sketch to read the BNO055 
// use to learn the adafruit sensor library 
// and to eventually port into the freertos sketch


#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <ArduinoJson.h>

// sample rate interval
#define BNO055_SAMPLERATE_DELAY_MS (100)

// bno055 sensor object
// parameters
// device id
// i2c address
// Wire pointer
Adafruit_BNO055 IMU = Adafruit_BNO055(55, 0x28, &Wire);

void setup() {
    Serial.begin(9600);

    while (!Serial) delay(10); // wait for serial

    // init imu
    if(!IMU.begin()){
        Serial.println("BNO055 not detected check wiring then reset");
        while(1);
    }

    delay(1000);

    IMU.setExtCrystalUse(true); // not sure what this is
}

void loop() {

    // getting data using adafruit sensor library
    sensors_event_t event;
    IMU.getEvent(&event);
    

    Serial.println(getOrientationJson(event));

}

String getOrientationJson(sensors_event_t event){
    // get the data from the sensor and 
    // return it in a json string
    JsonDocument eventJson;
    eventJson["orientation"]["x"] = event.orientation.x;
    eventJson["orientation"]["y"] = event.orientation.y;
    eventJson["orientation"]["z"] = event.orientation.z;
    String eventJsonStr;
    serializeJson(eventJson, eventJsonStr);
    return eventJsonStr;
}
