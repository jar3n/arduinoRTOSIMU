#include <Arduino_FreeRTOS.h>
#include <projdefs.h>
#include <task.h>
#include <queue.h>

// IMU libraries
#include <Wire.h>
#include <7Semi_BNO055.h>

// function headers for blink and analogread example
void TaskSensorRead(void *pvParameters);
void TaskPublish(void *pvParameters);

QueueHandle_t dataQueue;

BNO055_7Semi imu;

// 3 floats in the array
#define QUEUE_ARRAY_SIZE 3


void setup() {
    Serial.begin(9600);

    while(!Serial) delay(10);


    // setting up the imu at the start 
    // because it has to calibrate
    if(!imu.begin()){
        Serial.println("BMO055 failed to start!!!!");
        while(1) { delay(1000); }
    }
    imu.setMode(Mode::NDOF);
    Serial.print(F("Calibrating IMU"));
    if (!imu.waitCalibrated(10000, 200)){
        Serial.println(F(" - timeout"));
    } else {
        Serial.println(F(" - done"));
    }

    Serial.println("Now setting up tasks");


    // instantiating a message buffer
    dataQueue = xQueueCreate(3, sizeof(float)*QUEUE_ARRAY_SIZE);

    if (dataQueue != NULL){
        // here the tasks are set up to run independently
        // first set up blink
        // parameters:
        // task function 
        // human readable task name
        // allocated stack size 
        // NULL
        // priority
        // NULL
        // NOTE: do not increase over 128 because it causes a heap overflow
        // (noted by the fast blinking led and lack of serial printing)
        xTaskCreate(TaskSensorRead, "SensorRead", 128, NULL, 1, NULL);

        xTaskCreate(TaskPublish, "PublishData", 128, NULL, 2, NULL);
    } else {
        Serial.println("ERROR UNABLE TO CREATE THE MESSAGE BUFFER NEED MORE SPACE LIKELY.");
    }

}

void loop() {
    // this is the idle task when there is no task to do in the rtos
    // for this example i will leave this empty
    // but appearantly you can reduce the battery consumption by putting the arduino
    // to sleep not necessary when you have it plugged in to the computer though

}

// task implementations
void TaskSensorRead(void *pvParameters){
    (void) pvParameters;

    float imuData[QUEUE_ARRAY_SIZE];

    for(;;){
        if (dataQueue != NULL){

            // get the data using the adafruit sensor 
            // interface then make it json
            float heading, roll, pitch;
            if (imu.readEuler(heading, roll, pitch)){
                imuData[0] = heading;
                imuData[1] = roll;
                imuData[2] = pitch;

                // check the message was sent
                if (xQueueSendToBack(dataQueue, (void *) imuData, portMAX_DELAY) != pdPASS){
                    Serial.println("UNABLE TO SEND FULL MESSAGE!!!!");
                } 
            }
        }
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


void TaskPublish(void *pvParameters){
    (void) pvParameters;
    
    size_t bytesReceived;
    float imuData[QUEUE_ARRAY_SIZE];

    for(;;){
        if (dataQueue != NULL){
            // appearantly the portMAX_DELAY means block indefinitely until a message is received
            if(xQueueReceive(dataQueue, (void *) imuData, portMAX_DELAY) != pdPASS){
                Serial.println("Not able to get data from queue");
            } else {
                Serial.println(getOrientationJson(imuData[0], imuData[1], imuData[2]));
            }
        }
    }
}
