#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ws2812.pio.h"
#include "hardware/pwm.h"
#include "ssd1306.h"
#include "font.h"
#include "semphr.h"

#define vermelho 13
#define verde 11
#define azul 12
#define PWM_WRAP 4095
#define PWM_CLK_DIV 30.52f
#define buzzer1 10
#define buzzer2 21
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define botaoa 5
#define botaob 6
#define selec 22

ssd1306_t ssd;
SemaphoreHandle_t xEntradaSem;
SemaphoreHandle_t xSaidaSem;
SemaphoreHandle_t xResetSem;
SemaphoreHandle_t xDisplayMutex;
uint16_t eventos_incrementados = 0;
bool global = false;
bool global1 = false;
bool global2 = false;
bool sound = false;
bool display_ocupado = false;  // Flag para controlar quando o display está ocupado
char buffer[32];

// ISR do botão A (incrementa o semáforo de contagem)
void gpio_callback(uint gpio, uint32_t events)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if(gpio == botaoa){
        xSemaphoreGiveFromISR(xEntradaSem, &xHigherPriorityTaskWoken);
    }
    else if(gpio == botaob){
        xSemaphoreGiveFromISR(xSaidaSem, &xHigherPriorityTaskWoken);
    }
    else if(gpio == selec){
        xSemaphoreGiveFromISR(xResetSem, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void pwm_init_gpio(uint gpio, uint wrap, float clkdiv) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, wrap);
    pwm_config_set_clkdiv(&config, clkdiv);
    pwm_init(slice_num, &config, true);
}

void controlar_leds() {
        if (eventos_incrementados >= 1 && eventos_incrementados <= 6) {
            // Verde: 1-6 pessoas (capacidade normal)
            gpio_put(verde, true);
            gpio_put(vermelho, false);
            gpio_put(azul, false);
        }
        else if (eventos_incrementados == 7) {
            // Amarelo (verde + vermelho): 7 pessoas (quase lotado)
            gpio_put(verde, true);
            gpio_put(vermelho, true);
            gpio_put(azul, false);
        }
        else if (eventos_incrementados == 0) {
            // Azul: 0 pessoas (vazio)
            gpio_put(verde, false);
            gpio_put(vermelho, false);
            gpio_put(azul, true);
        }
        else if (eventos_incrementados == 8) {
            if(eventos_incrementados > 8){// Vermelho: 8 pessoas (lotado)
            pwm_set_gpio_level(buzzer1, 125); // 50%
            pwm_set_gpio_level(buzzer2, 125); // 50%
            vTaskDelay(pdMS_TO_TICKS(400));
            pwm_set_gpio_level(buzzer1, 0);   // silencia
            pwm_set_gpio_level(buzzer2, 0);   // silencia
            vTaskDelay(pdMS_TO_TICKS(800));
            }
            gpio_put(verde, false);
            gpio_put(vermelho, true);
            gpio_put(azul, false);
        }
}

void vTaskEntrada(void *params){
    while(true){
        // Espera pelo semáforo de entrada
        if (xSemaphoreTake(xEntradaSem, portMAX_DELAY) == pdTRUE){
            if(global == true){
                // Pega o mutex do display
                if (xSemaphoreTake(xDisplayMutex, portMAX_DELAY) == pdTRUE) {
                    display_ocupado = true;  // Marca display como ocupado
                    
                    if(eventos_incrementados < 8){
                        eventos_incrementados++;
                        // Atualiza display com a nova contagem
                        ssd1306_fill(&ssd, 0);
                        sprintf(buffer, "Pessoas: %d", eventos_incrementados);
                        ssd1306_draw_string(&ssd, "Entrada ", 5, 10);
                        ssd1306_draw_string(&ssd, "Recebida!", 5, 19);
                        ssd1306_draw_string(&ssd, buffer, 5, 44);
                        ssd1306_send_data(&ssd);
                        
                        // Controla os LEDs
                        controlar_leds();
                        
                        // Simula tempo de processamento
                        vTaskDelay(pdMS_TO_TICKS(1500));

                        // Retorna à tela de espera
                        ssd1306_fill(&ssd, 0);
                        ssd1306_draw_string(&ssd, "Aguardando ", 5, 10);
                        ssd1306_draw_string(&ssd, " Interacao...", 5, 19);
                        sprintf(buffer, "Pessoas: %d", eventos_incrementados);
                        ssd1306_draw_string(&ssd, buffer, 5, 44);
                        ssd1306_send_data(&ssd);
                    }
                    else{
                        pwm_set_gpio_level(buzzer1, 125); // 50%
                        pwm_set_gpio_level(buzzer2, 125); // 50%
                        vTaskDelay(pdMS_TO_TICKS(400));
                        pwm_set_gpio_level(buzzer1, 0);   // silencia
                        pwm_set_gpio_level(buzzer2, 0);   // silencia
                        vTaskDelay(pdMS_TO_TICKS(800));
                        ssd1306_fill(&ssd, 0);
                        ssd1306_draw_string(&ssd, "Limite Max.", 5, 20);
                        ssd1306_draw_string(&ssd, "atingido!", 5, 30);
                        sprintf(buffer, "Pessoas: %d", eventos_incrementados);
                        ssd1306_draw_string(&ssd, buffer, 5, 44);
                        ssd1306_send_data(&ssd);
                        
                        vTaskDelay(pdMS_TO_TICKS(1500));

                        ssd1306_fill(&ssd, 0);
                        ssd1306_draw_string(&ssd, "Aguardando ", 5, 10);
                        ssd1306_draw_string(&ssd, " Interacao...", 5, 19);
                        sprintf(buffer, "Pessoas: %d", eventos_incrementados);
                        ssd1306_draw_string(&ssd, buffer, 5, 44);
                        ssd1306_send_data(&ssd);
                    }
                    
                    display_ocupado = false;  // Libera o display
                    global = false;
                    xSemaphoreGive(xDisplayMutex);
                }
            }
        }
    }
}

void vTaskSaida(void *params){
    while(true){
        // Espera pelo semáforo de saída
        if (xSemaphoreTake(xSaidaSem, portMAX_DELAY) == pdTRUE){
            if(global1 == true){
                // Pega o mutex do display
                if (xSemaphoreTake(xDisplayMutex, portMAX_DELAY) == pdTRUE) {
                    display_ocupado = true;  // Marca display como ocupado
                    
                    if (eventos_incrementados > 0) {
                        eventos_incrementados--;
                        // Atualiza display com a nova contagem
                        ssd1306_fill(&ssd, 0);
                        sprintf(buffer, "Pessoas: %d", eventos_incrementados);
                        ssd1306_draw_string(&ssd, "Pessoa ", 5, 10);
                        ssd1306_draw_string(&ssd, "Saindo!", 5, 19);
                        ssd1306_draw_string(&ssd, buffer, 5, 44);
                        ssd1306_send_data(&ssd);
                        
                        // Controla os LEDs
                        controlar_leds();
                        
                        // Simula tempo de processamento
                        vTaskDelay(pdMS_TO_TICKS(1500));

                        // Retorna à tela de espera
                        ssd1306_fill(&ssd, 0);
                        ssd1306_draw_string(&ssd, "Aguardando ", 5, 10);
                        ssd1306_draw_string(&ssd, "  Interacao...", 5, 19);
                        sprintf(buffer, "Pessoas: %d", eventos_incrementados);
                        ssd1306_draw_string(&ssd, buffer, 5, 44);
                        ssd1306_send_data(&ssd);
                    }
                    else {
                        ssd1306_fill(&ssd, 0);
                        ssd1306_draw_string(&ssd, "Limite Min.", 5, 20);
                        ssd1306_draw_string(&ssd, "atingido!", 5, 30);
                        sprintf(buffer, "Pessoas: %d", eventos_incrementados);
                        ssd1306_draw_string(&ssd, buffer, 5, 44);
                        ssd1306_send_data(&ssd);
                        
                        vTaskDelay(pdMS_TO_TICKS(1500));

                        ssd1306_fill(&ssd, 0);
                        ssd1306_draw_string(&ssd, "Aguardando ", 5, 10);
                        ssd1306_draw_string(&ssd, " Interacao...", 5, 19);
                        sprintf(buffer, "Pessoas: %d", eventos_incrementados);
                        ssd1306_draw_string(&ssd, buffer, 5, 44);
                        ssd1306_send_data(&ssd);
                    }
                    
                    display_ocupado = false;  // Libera o display
                    global1 = false;
                    xSemaphoreGive(xDisplayMutex);
                }
           }
        }
    }
}

// Task para gerenciar a tela de espera
void vTaskDisplay(void *params){
    while(true){
        // Só atualiza o display se não estiver ocupado com outras mensagens
        if (!display_ocupado && xSemaphoreTake(xDisplayMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            ssd1306_fill(&ssd, 0);
            ssd1306_draw_string(&ssd, "Aguardando ", 5, 10);
            ssd1306_draw_string(&ssd, " Interacao...", 5, 19);
            sprintf(buffer, "Pessoas: %d", eventos_incrementados);
            ssd1306_draw_string(&ssd, buffer, 5, 44);
            ssd1306_send_data(&ssd);
            xSemaphoreGive(xDisplayMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Atualiza a tela a cada 2 segundos
    }
}

void vTaskReset(void *params){
    while(true){
        if(xSemaphoreTake(xResetSem, portMAX_DELAY) == pdTRUE){
            if(xSemaphoreTake(xDisplayMutex, portMAX_DELAY) == pdTRUE){
                display_ocupado = true;  // Marca display como ocupado
                
                eventos_incrementados = 0;
                pwm_set_gpio_level(buzzer1, 125); // 50%
                pwm_set_gpio_level(buzzer2, 125); // 50%
                vTaskDelay(pdMS_TO_TICKS(100));
                pwm_set_gpio_level(buzzer1, 0);   // silencia
                pwm_set_gpio_level(buzzer2, 0);   // silencia   
                vTaskDelay(pdMS_TO_TICKS(200));
                pwm_set_gpio_level(buzzer1, 125); // 50%
                pwm_set_gpio_level(buzzer2, 125); // 50%
                vTaskDelay(pdMS_TO_TICKS(100));
                pwm_set_gpio_level(buzzer1, 0);   // silencia
                pwm_set_gpio_level(buzzer2, 0);   // silencia   
                vTaskDelay(pdMS_TO_TICKS(100));
                controlar_leds();
                ssd1306_fill(&ssd, 0);
                ssd1306_draw_string(&ssd, "Reiniciado", 1, 10);
                ssd1306_draw_string(&ssd, " pelo Admin.", 5, 19);
                sprintf(buffer, "Pessoas: %d", eventos_incrementados);
                ssd1306_draw_string(&ssd, buffer, 5, 44);
                ssd1306_send_data(&ssd);    
                vTaskDelay(pdMS_TO_TICKS(1500)); 
                
                // Volta à tela de espera
                ssd1306_fill(&ssd, 0);
                ssd1306_draw_string(&ssd, "Aguardando ", 5, 10);
                ssd1306_draw_string(&ssd, " Interacao...", 5, 19);
                sprintf(buffer, "Pessoas: %d", eventos_incrementados);
                ssd1306_draw_string(&ssd, buffer, 5, 44);
                ssd1306_send_data(&ssd);
                
                display_ocupado = false;  // Libera o display
                xSemaphoreGive(xDisplayMutex);
            }
        }
    }
}

// ISR para BOOTSEL e botão de evento
void gpio_irq_handler(uint gpio, uint32_t events)
{
    if (gpio == botaob)
    {
        global1 = !global1;
        gpio_callback(gpio, events);

    }
    else if (gpio == botaoa)
    {   
        global = !global;
        gpio_callback(gpio, events);
    }
    else if(gpio == selec){
        global2 = !global2;
        gpio_callback(gpio, events);
 
    }
}

int main()
{
    stdio_init_all();

    // Inicialização do display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Configura os botões
    gpio_init(botaoa);
    gpio_set_dir(botaoa, GPIO_IN);
    gpio_pull_up(botaoa);

    gpio_init(botaob);
    gpio_set_dir(botaob, GPIO_IN);
    gpio_pull_up(botaob);

    gpio_init(selec);
    gpio_set_dir(selec, GPIO_IN);
    gpio_pull_up(selec);
    
    // Configura os LEDs
    gpio_init(vermelho);
    gpio_set_dir(vermelho, GPIO_OUT);
    gpio_init(verde);
    gpio_set_dir(verde, GPIO_OUT);
    gpio_init(azul);
    gpio_set_dir(azul, GPIO_OUT);
    pwm_init_gpio(buzzer1, 500, 125.0f);
    pwm_init_gpio(buzzer2, 500, 125.0f);
    

    controlar_leds();
    
    gpio_set_irq_enabled_with_callback(botaoa, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(botaob, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(selec, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    

    xEntradaSem = xSemaphoreCreateCounting(10, 0);
    xSaidaSem = xSemaphoreCreateCounting(10, 0);
    xDisplayMutex = xSemaphoreCreateMutex();
    xResetSem = xSemaphoreCreateBinary();


    xTaskCreate(vTaskEntrada, "EntradaTask", 512, NULL, 2, NULL);
    xTaskCreate(vTaskSaida, "SaidaTask", 512, NULL, 2, NULL);
    xTaskCreate(vTaskDisplay, "DisplayTask", 512, NULL, 1, NULL);
    xTaskCreate(vTaskReset, "ResetTask", 512, NULL, 3, NULL);
    vTaskStartScheduler();
    panic_unsupported();
}