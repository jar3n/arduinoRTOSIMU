#include <Arduino_FreeRTOS.h>
// #include <croutine.h>
#include <event_groups.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOSVariant.h>
#include <list.h>
#include <mpu_wrappers.h>
#include <portable.h>
#include <portmacro.h>
#include <projdefs.h>
#include <queue.h>
#include <semphr.h>
// #include <StackMacros.h>
#include <task.h>
#include <timers.h>

// include for the hdc temp sensor 
// trying to make the task read the sensor instead of simple 
// analog read
#include <Adafruit_HDC302x.h>

// function headers for blink and analogread example
void TaskBlink(void *pvParameters);
void TasSensorRead(void *pvParameters);

void setup() {

    // here the tasks are set up to run independently
    // first set up blink
    // parameters:
    // task function 
    // human readable task name
    // allocated stack size 
    // NULL
    // priority
    // NULL
    xTaskCreate(TaskBlink, "Blink", 128,  NULL, 2, NULL);

    xTaskCreate(TaskSensorRead, "SensorRead", 128, NULL, 1, NULL);
}

void loop() {
    // this is the idle task when there is no task to do in the rtos
    // for this example i will leave this empty
    // but appearantly you can reduce the battery consumption by putting the arduino
    // to sleep not necessary when you have it plugged in to the computer though

}

// task implementations
void TaskBlink(void *pvParameters){
    (void) pvParameters;

    // set up stuff happens here
    pinMode(13, OUTPUT);

    // here is the forever loop
    for(;;){
        digitalWrite(13, HIGH);
        vTaskDelay(1000/portTICK_PERIOD_MS); // use rtos delay to delay one sec
        digitalWrite(13, LOW);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void TaskSensorRead(void *pvParameters){
    Serial.begin(9600);

    Adafruit_HDC302x hdc = Adafruit_HDC302x();
    double temp = 0.0;
    double hum = 0.0;

    hdc.begin(0x44, &Wire);

    for(;;){
        hdc.readTemperatureHumidityOnDemand(temp, hum, TRIGGERMODE_LP0);
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.print(" C, Humidity: ");
        Serial.print(hum);
        Serial.println(" %");
        vTaskDelay(1500/portTICK_PERIOD_MS); // delay one rtos tick which is 15 milliseconds 
    }
}
