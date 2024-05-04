#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <driver/adc.h>
#include <driver/dac.h>

#define ESP_INTR_FLAG_DEFAULT 0
#define CONFIG_BUTTON_PIN 0
#define CONFIG_LED_PIN 2

/*

/////////////////////////////////
///////////TAREAS////////////////
/////////////////////////////////

TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;

void task1 (void *arg)
{
    int c=0;
    while(1)
    {
        c++;
        printf("Hello from task 1\t times:%d\n",c);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if(c == 10)
        {
            vTaskSuspend(myTask2Handle);
            printf("Task 2 deleted\n");
        }
        if(c == 15)
        {
            vTaskResume(myTask2Handle);
            vTaskDelete(myTask1Handle);
        }
    }
}

void task2 (void *arg)
{
    while(1)
    {
        printf("Hello from task 2\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main() 
{
    xTaskCreate(task1,"task1",4096, NULL, 10, &myTask1Handle);
    xTaskCreatePinnedToCore(task2,"task2",4096, NULL, 10, &myTask2Handle,1);
}*/

/*
TaskHandle_t ISR = NULL;

void IRAM_ATTR button_isr_handler(void *arg)
{
    xTaskResumeFromISR(ISR);
}

void button_task(void *arg)
{
    bool led_status = false;
    while(1)
    {
        vTaskSuspend(NULL);
        led_status = !led_status;
        gpio_set_level(CONFIG_LED_PIN,led_status);
        printf("Button pressed!\t status:%i\n",led_status);
    }
}

void app_main()
{
    gpio_pad_select_gpio(CONFIG_BUTTON_PIN);
    gpio_pad_select_gpio(CONFIG_LED_PIN);

    //Definir modo de operacion E/S
    gpio_set_direction(CONFIG_BUTTON_PIN,GPIO_MODE_INPUT);
    gpio_set_direction(CONFIG_LED_PIN,GPIO_MODE_OUTPUT);

    //Falling edge para el ISR
    gpio_set_intr_type(CONFIG_BUTTON_PIN,GPIO_INTR_NEGEDGE);

    //Configuraciones por defecto del ISR
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    //Attach el ISR al pin
    gpio_isr_handler_add(CONFIG_BUTTON_PIN, button_isr_handler,NULL);

    //Crear la tarea
    xTaskCreate(button_task, "Button Task", 4096, NULL, 10, &ISR);
}*/

#include "esp_log.h"


#define TASK1_TAG "Tarea 1"
#define TASK2_TAG "Tarea 2"
#define Boton 0
#define LED 2

bool status = 0;
int c=0;
int adc = 0;

TaskHandle_t botonTaskHandle = NULL;
TaskHandle_t ADCTaskHandle = NULL;
TaskHandle_t DACTaskHandle = NULL;

void botonTask(void *arg)
{
    while(true)
    {
        gpio_set_level(LED,1);
        vTaskDelay(25/portTICK_RATE_MS);
        gpio_set_level(LED,0);
        vTaskDelay(500/portTICK_RATE_MS);
        ESP_LOGI(TASK1_TAG, "ADC:%d\n",adc);
        ESP_LOGW(TASK2_TAG, "DAC:%d\n",c);
    }
}

void ADCTask(void *arg)
{
    while (true)
    {
        adc = adc1_get_raw(ADC1_CHANNEL_0);
        vTaskDelay(50/portTICK_RATE_MS);
    }
}

void DACTask(void *arg)
{
    while (true)
    {
        c++;
        dac_output_voltage(DAC_CHANNEL_1,c);
        if(c==255)
        {
            c = 0;
        }
        vTaskDelay(20/portTICK_RATE_MS);
    }
    
}


void app_main()
{
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED,GPIO_MODE_OUTPUT);

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_0);

    dac_output_enable(DAC_CHANNEL_1);

    xTaskCreate(botonTask,"Tarea Boton",4096, NULL, 1, &botonTaskHandle);
    xTaskCreate(ADCTask,"Tarea ADC",4096, NULL,2, &ADCTaskHandle);
    xTaskCreate(DACTask,"Tarea DAC",4096, NULL,2, &DACTaskHandle);
}
