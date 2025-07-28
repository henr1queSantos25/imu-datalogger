#include "mpu6050.h"
#include "pico/binary_info.h"

// === VARIÁVEIS GLOBAIS ===
static i2c_inst_t *i2c_port;

// ========================================================================
// FUNÇÕES DE INICIALIZAÇÃO
// ========================================================================

/**
 * @brief Inicializa o MPU6050 e armazena o ponteiro do I2C
 * @param I2C_PORT Instância do I2C utilizada
 */
void mpu6050_init(i2c_inst_t *I2C_PORT) {
    i2c_port = I2C_PORT;
    mpu6050_reset();
}

/**
 * @brief Configura os pinos e inicializa o barramento I2C para o MPU6050
 * @param I2C_PORT Instância do I2C
 * @param I2C_SDA Pino SDA
 * @param I2C_SCL Pino SCL
 * @param clock Frequência do barramento I2C
 */
void setup_I2C_mpu6050(i2c_inst_t *I2C_PORT, uint I2C_SDA, uint I2C_SCL, uint clock) {
    i2c_init(I2C_PORT, clock);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

// ========================================================================
// FUNÇÕES DE CONTROLE DO SENSOR
// ========================================================================

/**
 * @brief Reseta e inicializa o MPU6050
 */
void mpu6050_reset() {
    uint8_t buf[] = {0x6B, 0x80};
    i2c_write_blocking(i2c_port, mpu6050_address, buf, 2, false);
    sleep_ms(100);

    buf[1] = 0x00;
    i2c_write_blocking(i2c_port, mpu6050_address, buf, 2, false);
    sleep_ms(10);
}

// ========================================================================
// FUNÇÕES DE LEITURA DE DADOS
// ========================================================================

/**
 * @brief Lê dados crus do acelerômetro, giroscópio e temperatura do MPU6050
 * @param accel Array para armazenar dados do acelerômetro
 * @param gyro Array para armazenar dados do giroscópio
 * @param temp Ponteiro para armazenar o dado de temperatura
 */
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    uint8_t buffer[6];
    uint8_t val;

    // Leitura do acelerômetro
    val = 0x3B;
    i2c_write_blocking(i2c_port, mpu6050_address, &val, 1, true);
    i2c_read_blocking(i2c_port, mpu6050_address, buffer, 6, false);
    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8) | buffer[(i * 2) + 1];
    }

    // Leitura do giroscópio
    val = 0x43;
    i2c_write_blocking(i2c_port, mpu6050_address, &val, 1, true);
    i2c_read_blocking(i2c_port, mpu6050_address, buffer, 6, false);
    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8) | buffer[(i * 2) + 1];
    }

    // Leitura da temperatura
    val = 0x41;
    i2c_write_blocking(i2c_port, mpu6050_address, &val, 1, true);
    i2c_read_blocking(i2c_port, mpu6050_address, buffer, 2, false);
    *temp = (buffer[0] << 8) | buffer[1];
}