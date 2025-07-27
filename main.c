#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"


#include "mpu6050.h"
#include "SD.h"

// Definição dos pinos I2C para o MPU6050
#define I2C_PORT i2c0 
#define I2C_SDA 0
#define I2C_SCL 1


// Função para capturar dados do MPU6050 e salvar no arquivo *.csv
void capture_mpu6050_data_and_save();


int main() {
    stdio_init_all();

    // Inicializa o I2C
    setup_I2C_mpu6050(I2C_PORT, I2C_SDA, I2C_SCL, 400 * 1000); // 400 kHz
    // Declara os pinos como I2C na Binary Info
    bi_decl(bi_2pins_with_func(I2C_SDA, I2C_SCL, GPIO_FUNC_I2C));

    mpu6050_reset(I2C_PORT); 

    while (true) {
        
        
        sleep_ms(1000);
    }
}

// Função para capturar dados do MPU6050 e salvar no arquivo *.csv
void capture_mpu6050_data_and_save() {
    printf("\nCapturando dados do MPU6050. Aguarde finalização...\n");
    FIL file;
    FRESULT res = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);

    if (res != FR_OK) {
        printf("\n[ERRO] Não foi possível abrir o arquivo para escrita. Monte o Cartao.\n");
        return;
    }

    char buffer_cabecalho[100];
    sprintf(buffer_cabecalho, "numero_amostra,accel_x,accel_y,accel_z,giro_x,giro_y,giro_z\n");
    UINT bw;
    res = f_write(&file, buffer_cabecalho, strlen(buffer_cabecalho), &bw);
    if (res != FR_OK || bw != strlen(buffer_cabecalho)) {
        printf("[ERRO] Não foi possível escrever no arquivo. Monte o Cartao.\n");
        f_close(&file);
        return;
    }

    int16_t aceleracao[3], gyro[3], temp;
    float ax, ay, az;
    for (int i = 0; i < 128; i++) {
        mpu6050_read_raw(I2C_PORT, aceleracao, gyro, &temp);
        ax = aceleracao[0] / 16384.0f;
        ay = aceleracao[1] / 16384.0f;
        az = aceleracao[2] / 16384.0f;
        char buffer[256];
        sprintf(buffer, "%d,%.2f,%.2f,%.2f,%d,%d,%d\n", i + 1, ax, ay, az, gyro[0], gyro[1], gyro[2]);
        UINT bw;
        res = f_write(&file, buffer, strlen(buffer), &bw);
        if (res != FR_OK)
        {
            printf("[ERRO] Não foi possível escrever no arquivo. Monte o Cartao.\n");
            f_close(&file);
            return;
        }
        sleep_ms(100);
    }
    f_close(&file);
    printf("\nDados do MPU6050 salvos no arquivo %s.\n\n", filename);
}
