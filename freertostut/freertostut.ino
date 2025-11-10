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
void TaskSensorRead(void *pvParameters);
void TaskPublish(void *pvParameters);

// adding a queue for the temperature data
// to separate the getting of the data from publishing the data
QueueHandle_t dataQueue;
unsigned int dataQueueSize = 10;

void setup() {
    Serial.begin(9600);
    // instantiating a queue
    dataQueue = xQueueCreate(dataQueueSize, sizeof(double));

    if (dataQueue !=NULL){
        // here the tasks are set up to run independently
        // first set up blink
        // parameters:
        // task function 
        // human readable task name
        // allocated stack size 
        // NULL
        // priority
        // NULL
        xTaskCreate(TaskBlink, "Blink", 128,  NULL, 3, NULL);

        xTaskCreate(TaskSensorRead, "SensorRead", 128, NULL, 1, NULL);

        xTaskCreate(TaskPublish, "PublishData", 128, NULL, 2, NULL);
    } else {
        Serial.println("ERROR UNABLE TO CREATE THE DATA QUEUE NEED MOR ESPACE LIKELY.");
    }

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

    // the LED is on when there is 
    // stuff in the queue 
    // and off when there is nothing in the queue

    // set up stuff happens here
    pinMode(13, OUTPUT);

    // here is the forever loop
    for(;;){
        unsigned int numQueueFreeSpaces = uxQueueSpacesAvailable(dataQueue);
        if (numQueueFreeSpaces == dataQueueSize){
            // queue is empty so led is off
            digitalWrite(13, LOW);
        } else{
            digitalWrite(13, HIGH);
        }
        vTaskDelay(1); // for cleanness?? was in example maybe unecessary
        // digitalWrite(13, HIGH);
        // vTaskDelay(1000/portTICK_PERIOD_MS); // use rtos delay to delay one sec
        // digitalWrite(13, LOW);
        // vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void TaskSensorRead(void *pvParameters){
    (void) pvParameters;

    Adafruit_HDC302x hdc = Adafruit_HDC302x();
    double temp = 0.0;
    double hum = 0.0;

    hdc.begin(0x44, &Wire);

    for(;;){
        if (dataQueue != NULL){
            if (uxQueueSpacesAvailable(dataQueue) > 0){
                hdc.readTemperatureHumidityOnDemand(temp, hum, TRIGGERMODE_LP0);
                // put data in queue here
                xQueueSendToBack(dataQueue, (void *) &temp, 0);
            }
            // maybe remove an element in the queue if there is no available space?
            // eh, by the time the task runs again it will have newer data
            // so its not important that every data point gets into the queue I think
            
            // delay one rtos tick which is 15 milliseconds 
            // this is so the task is not constantly reading the data
            vTaskDelay(200/portTICK_PERIOD_MS); 
        }
        

    }
}

void TaskPublish(void *pvParameters){
    (void) pvParameters;

    double temp;

    for(;;){
        if (dataQueue != NULL){
            unsigned int dataInQueue = dataQueueSize - (unsigned int)(uxQueueSpacesAvailable);
            if (dataInQueue > 0){
                // read from the queue
                // args for this
                // queue handle to read from 
                // pointer to buffer to read into
                // time in rtos ticks to block waiting for queue to be nonempty
                xQueueReceive(dataQueue, &(temp), 0);
                Serial.println("temperature: " + String(temp) + " C");
            }
            vTaskDelay(500/portTICK_PERIOD_MS); // only print every second
        }
    }
}
