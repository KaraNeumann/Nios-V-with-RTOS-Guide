// Note this code was originally created by generative AI (https://claude.com/),
// and then heavily modified.

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"

#define TASK_STACKSIZE  2048


/* Task priority setting : Task1 > Task2 */
#define TASK1_PRIORITY  (tskIDLE_PRIORITY + 2)
#define TASK2_PRIORITY  (tskIDLE_PRIORITY + 1)

// Semaphores to ensure tasks don't fight over access to I/O
SemaphoreHandle_t sdram_mutex;
SemaphoreHandle_t sw_mutex;
SemaphoreHandle_t juart_mutex;
SemaphoreHandle_t ledr_mutex;

// Sleep time for each task in milliseconds
int t1_ms_slept = 1000 / portTICK_PERIOD_MS;
int t2_ms_slept = 2000 / portTICK_PERIOD_MS;

// The context value for the edge capture ISR, unused
int edge_val = 0;


// Define the first task
void prvTask1(void* pvParameters)
{
    int ms_slept = (*((int *)pvParameters));

    printf("Hello from task1\n");

    while (1)
    {

        xSemaphoreTake(sw_mutex, portMAX_DELAY); // Wait indefinitely to get the SW Mutex
        uint16_t sw_val = IORD_ALTERA_AVALON_PIO_DATA(PIO_SW_BASE); // Read the SW value
        xSemaphoreGive(sw_mutex); // Release the MUTEX


        xSemaphoreTake(sdram_mutex, portMAX_DELAY);
        xSemaphoreTake(juart_mutex, portMAX_DELAY);

        printf("Task 1 is writing to SDRAM value=%x\n\n", sw_val);
        IOWR(CORE_SDRAM_AXI4_0_BASE, 0, sw_val); // Write to the SDRAM base address, with an offset of 0
        xSemaphoreGive(sdram_mutex);
        xSemaphoreGive(juart_mutex);

        vTaskDelay(ms_slept);

    }
}

void prvTask2(void* pvParameters)
{
    int ms_slept = (*((int *)pvParameters));

    printf("Hello from task2\n");


    while (1)
    {

        xSemaphoreTake(sdram_mutex,portMAX_DELAY);
        uint16_t sdramVal = IORD(CORE_SDRAM_AXI4_0_BASE, 0);  // Read from the SDRAM base address, with an offset of 0
        xSemaphoreGive(sdram_mutex);

        xSemaphoreTake(juart_mutex, portMAX_DELAY);
        printf("Task 2 is reading from SDRAM value=%x\n\n", sdramVal);
        xSemaphoreGive(juart_mutex);

        xSemaphoreTake(ledr_mutex, portMAX_DELAY);
        IOWR_ALTERA_AVALON_PIO_DATA(PIO_LEDR_BASE, sdramVal); // Write to the LEDR PIO
        xSemaphoreGive(ledr_mutex);

        vTaskDelay(ms_slept);

    }
}

// This function runs whenever the KEY interrupt is triggered
void pio_key_isr(void* context)
{

	// Get the edge capture value from the PIO
    uint16_t edge_cap_val = IORD_ALTERA_AVALON_PIO_EDGE_CAP(PIO_KEY_BASE);

    // Set the edge capture register to 0, so the Interrupt doesn't keep triggering
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_KEY_BASE, 0);


    printf("KEY Interrupt Triggered!\n");
	printf("KEY value=: %x\n\n", edge_cap_val);


	// Return priority to the RTOS
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}




int main(void)
{
    printf("Hello FreeRTOS from main...\n\n");

    // Init all the mutexes
    sdram_mutex = xSemaphoreCreateMutex();
    sw_mutex = xSemaphoreCreateMutex();
    juart_mutex = xSemaphoreCreateMutex();
    ledr_mutex = xSemaphoreCreateMutex();


    // Init the tasks
    if (pdFAIL == xTaskCreate( prvTask1, "Task1", TASK_STACKSIZE, &t1_ms_slept, TASK1_PRIORITY, NULL )){
        printf("Task1 creation fail!\n");
    }
    if (pdFAIL == xTaskCreate( prvTask2, "Task2", TASK_STACKSIZE,
                              &t2_ms_slept, TASK2_PRIORITY, NULL )){
        printf("Task2 creation fail!\n");
    }


    // Init the ISR for the KEY

    // First, clear the edge capture
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_KEY_BASE, 0x00);
    // Next, set the IRP mask to accept all interrupt on all 3 KEYs
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_KEY_BASE, 0xF);
    // Register our pio_key_isr() function to run when the interrupt is triggered
    alt_ic_isr_register(PIO_KEY_IRQ_INTERRUPT_CONTROLLER_ID, PIO_KEY_IRQ, pio_key_isr, &edge_val, 0x00);

    // Start the RTOS
    vTaskStartScheduler();

    for( ;; );
}

