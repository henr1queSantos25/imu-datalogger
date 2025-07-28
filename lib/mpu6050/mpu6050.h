#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Endereço padrão do MPU6050
static int mpu6050_address = 0x68;


// Configuração do I2C para o MPU6050
void setup_I2C_mpu6050(i2c_inst_t *I2C_PORT, uint I2C_SDA, uint I2C_SCL, uint clock);

// Função para resetar e inicializar o MPU6050
void mpu6050_reset();

void mpu6050_init(i2c_inst_t *I2C_PORT);

// Função para ler dados crus do acelerômetro, giroscópio e temperatura
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);