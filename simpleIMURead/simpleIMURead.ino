// simple sketch to read the BNO055 
// ok now trying a different library from 7semi 



#include <Wire.h>
#include <7Semi_BNO055.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BNO055.h>
// #include <utility/imumaths.h>



// sample rate interval
// #define BNO055_SAMPLERATE_DELAY_MS (100)

// bno055 sensor object
// parameters
// device id
// i2c address
// Wire pointer
// Adafruit_BNO055 IMU = Adafruit_BNO055(55, 0x28, &Wire);

BNO055_7Semi imu;

void printCalib() {
  uint8_t sys, gyr, acc, mag;
  imu.calibBreakdown(sys, gyr, acc, mag);
  Serial.print(F("Calib SYS:"));
  Serial.print(sys);
  Serial.print(F(" G:"));
  Serial.print(gyr);
  Serial.print(F(" A:"));
  Serial.print(acc);
  Serial.print(F(" M:"));
  Serial.println(mag);
}

void setup() {
    Serial.begin(9600);

    while (!Serial) delay(10); // wait for serial


    // init imu
    if(!imu.begin()){
        Serial.println("BNO055 not detected check wiring then reset");
        while(1) { delay(1000);  }
    }

    // delay(1000);

    // IMU.setExtCrystalUse(true); // not sure what this is

    imu.setMode(Mode::NDOF);
    Serial.print(F("Calibrating"));
    if (!imu.waitCalibrated(10000, 200)){
        Serial.println(F(" - timeout"));
    } else {
        Serial.println(F(" - done"));
    }
    printCalib();

    Serial.println(F("Ready!"));
}

void loop() {

    // getting data using adafruit sensor library
    // sensors_event_t event;
    // IMU.getEvent(&event);
    float heading, roll, pitch;
    if (imu.readEuler(heading, roll, pitch)){
        Serial.println(getOrientationJson(heading, roll, pitch));
    } else {
        Serial.println("Failed to read the data from the IMU.");
    }
}

String getOrientationJson(float heading, float roll, float pitch){
    // get the data from the sensor and 
    // return it in a json string
    String eventJsonStr = "{";
    eventJsonStr += "heading:" + String(heading);
    eventJsonStr += ",roll:" + String(roll);
    eventJsonStr += ",pitch:" + String(pitch);
    eventJsonStr += "}";
    return eventJsonStr;
}
