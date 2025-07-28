#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "mpu6050.h"
#include "SD.h"
#include "ssd1306.h"
#include "button.h"

// Definição dos pinos I2C para o MPU6050
#define I2C_PORT_MPU6050 i2c0
#define I2C_SDA_MPU6050 0
#define I2C_SCL_MPU6050 1
#define I2C_PORT_DISP i2c1
#define I2C_SDA_DISP 14
#define I2C_SCL_DISP 15
#define BUTTON_A 5
#define BUTTON_B 6
#define BUTTON_JOY 22
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13
#define BUZZER 10
ssd1306_t ssd;
bool cor = true; 
uint volatile last_time = 0; // Debounce
bool volatile gravando_dados = false; 
bool volatile montando_cartao = false; // Flag para verificar se o cartão está montado
bool volatile desmontando_cartao = false; // Flag para verificar se o cartão está desmontado

void setup();
void gpio_irq_handler(uint gpio, uint32_t events);
// Função para capturar dados do MPU6050 e salvar no arquivo *.csv
void capture_mpu6050_data_and_save();


int main() {
    setup();

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_JOY, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    mpu6050_reset(I2C_PORT_MPU6050); 

    while (true) {

            // === LIMPEZA E LAYOUT BÁSICO ===
        ssd1306_fill(&ssd, false); // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
        ssd1306_line(&ssd, 3, 25, 123, 25, cor); // Desenha uma linha
        ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 6); 
        ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 16);  
        
        ssd1306_send_data(&ssd); // Atualiza o display com as alterações


        if (gravando_dados) {
            capture_mpu6050_data_and_save(); // Captura e salva os dados do MPU6050
        } else if (montando_cartao) {
            run_mount();
            sleep_ms(2000);
        } else if (desmontando_cartao) {
            run_unmount();
            sleep_ms(2000);
        }

        sleep_ms(1000);
    }
}

void setup() {
    stdio_init_all();

    // === CONFIGURAÇÃO DO DISPLAY ===
    setup_I2C(I2C_PORT_DISP, I2C_SDA_DISP, I2C_SCL_DISP, 400 * 1000);
    setup_ssd1306(&ssd, I2C_PORT_DISP);

    // === CONFIGURAÇÃO DO MPU6050 ===
    setup_I2C_mpu6050(I2C_PORT_MPU6050, I2C_SDA_MPU6050, I2C_SCL_MPU6050, 400 * 1000); // 400 kHz
    bi_decl(bi_2pins_with_func(I2C_SDA_MPU6050, I2C_SCL_MPU6050, GPIO_FUNC_I2C));

    // === CONFIGURAÇÃO DOS PERIFÉRICOS ===
    setup_button(BUTTON_A);
    setup_button(BUTTON_B);
    setup_button(BUTTON_JOY);
}

// ========================================================================
// FUNÇÕES DE CONTROLE
// ========================================================================

/**
 * @brief Handler de interrupção para o botão A
 * @param gpio Pino do GPIO que gerou a interrupção
 * @param events Eventos que causaram a interrupção
 */
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t time = to_us_since_boot(get_absolute_time());

    if (absolute_time_diff_us(last_time, time) > 500000){
        last_time = time; // Atualiza o tempo do último evento
        if (gpio == BUTTON_A) {
            gravando_dados = !gravando_dados; 
        } else if (gpio == BUTTON_B) {
            montando_cartao = !montando_cartao;
        } else if (gpio == BUTTON_JOY) {
            desmontando_cartao = !desmontando_cartao;
        }
    }
}

// Função para capturar dados do MPU6050 e salvar no arquivo *.csv
void capture_mpu6050_data_and_save() {
    FIL file;
    FRESULT res = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);

    if (res != FR_OK) {
        ssd1306_draw_string(&ssd, "Erro ao abrir", 8, 30);
        ssd1306_draw_string(&ssd, "o arquivo", 8, 38);
        ssd1306_draw_string(&ssd, "Monte o Cartao", 8, 54);
        ssd1306_send_data(&ssd); // Atualiza o display com a mensagem de erro
        printf("\n[ERRO] Não foi possível abrir o arquivo para escrita. Monte o Cartao.\n");
        sleep_ms(2000);
        gravando_dados = false; // Para a gravação de dados
        return;
    }

    char buffer_cabecalho[100];
    sprintf(buffer_cabecalho, "numero_amostra,accel_x,accel_y,accel_z,giro_x,giro_y,giro_z\n");
    UINT bw;
    res = f_write(&file, buffer_cabecalho, strlen(buffer_cabecalho), &bw);
    if (res != FR_OK || bw != strlen(buffer_cabecalho)) {
        ssd1306_draw_string(&ssd, "Erro ao escrever arquivo", 8, 30);
        ssd1306_send_data(&ssd); // Atualiza o display com a mensagem de erro
        printf("[ERRO] Não foi possível escrever no arquivo. Monte o Cartao.\n");
        f_close(&file);
        sleep_ms(2000);
        gravando_dados = false; // Para a gravação de dados
        return;
    }

    int16_t aceleracao[3], gyro[3], temp;
    float ax, ay, az;

    ssd1306_draw_string(&ssd, "Gravando Dados", 8, 38);
    ssd1306_send_data(&ssd); // Atualiza o display com a mensagem de gravação
    while (gravando_dados) {
        int i = 0;
        mpu6050_read_raw(I2C_PORT_MPU6050, aceleracao, gyro, &temp);
        ax = aceleracao[0] / 16384.0f;
        ay = aceleracao[1] / 16384.0f;
        az = aceleracao[2] / 16384.0f;
        char buffer[256];
        sprintf(buffer, "%d,%.2f,%.2f,%.2f,%d,%d,%d\n", i + 1, ax, ay, az, gyro[0], gyro[1], gyro[2]);
        UINT bw;
        res = f_write(&file, buffer, strlen(buffer), &bw);
        if (res != FR_OK) {
            ssd1306_draw_string(&ssd, "Erro ao escrever arquivo", 8, 30);
            ssd1306_send_data(&ssd); // Atualiza o display com a mensagem de erro
            printf("[ERRO] Não foi possível escrever no arquivo. Monte o Cartao.\n");
            f_close(&file);
            sleep_ms(2000);
            gravando_dados = false; 
            return;
        }
        i++;
        sleep_ms(100);
    }
    f_close(&file);
    gravando_dados = false; // Para a gravação de dados
    printf("\nDados do MPU6050 salvos no arquivo %s.\n\n", filename);
    ssd1306_draw_string(&ssd, "Dados salvos", 16, 30);
    ssd1306_draw_string(&ssd, "com sucesso!", 16, 38);
    ssd1306_send_data(&ssd); // Atualiza o display com a mensagem de sucesso
    sleep_ms(2000);
    ssd1306_fill(&ssd, false); // Limpa o display após a mensagem
}
