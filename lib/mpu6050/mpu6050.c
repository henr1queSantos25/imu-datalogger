#include "mpu6050.h"
#include "pico/binary_info.h"

static i2c_inst_t *i2c_port;

void mpu6050_init(i2c_inst_t *I2C_PORT) {
    i2c_port = I2C_PORT;
    mpu6050_reset();
}

void setup_I2C_mpu6050(i2c_inst_t *I2C_PORT, uint I2C_SDA, uint I2C_SCL, uint clock) {
    // INICIALIZAÇÃO DO I2C COM A FREQUÊNCIA DE CLOCK
    i2c_init(I2C_PORT, clock);
    // CONFIGURAÇÃO DO I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Define a função do pino GPIO para I2C.
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Define a função do pino GPIO para I2C.
    gpio_pull_up(I2C_SDA); // Configura pull-up para a linha de dados
    gpio_pull_up(I2C_SCL); // Configura pull-up para a linha de clock

}

// Função para resetar e inicializar o MPU6050
void mpu6050_reset() {
    // Dois bytes para reset: primeiro o registrador, segundo o dado
    uint8_t buf[] = {0x6B, 0x80};
    i2c_write_blocking(i2c_port, mpu6050_address, buf, 2, false);
    sleep_ms(100); // Aguarda reset e estabilização

    // Sai do modo sleep (registrador 0x6B, valor 0x00)
    buf[1] = 0x00;
    i2c_write_blocking(i2c_port, mpu6050_address, buf, 2, false);
    sleep_ms(10); // Aguarda estabilização após acordar
}

// Função para ler dados crus do acelerômetro, giroscópio e temperatura
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    uint8_t buffer[6];

    // Lê aceleração a partir do registrador 0x3B (6 bytes)
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_port, mpu6050_address, &val, 1, true);
    i2c_read_blocking(i2c_port , mpu6050_address, buffer, 6, false);

    for (int i = 0; i < 3; i++)
    {
        accel[i] = (buffer[i * 2] << 8) | buffer[(i * 2) + 1];
    }

    // Lê giroscópio a partir do registrador 0x43 (6 bytes)
    val = 0x43;
    i2c_write_blocking(i2c_port, mpu6050_address, &val, 1, true);
    i2c_read_blocking(i2c_port, mpu6050_address, buffer, 6, false);

    for (int i = 0; i < 3; i++)
    {
        gyro[i] = (buffer[i * 2] << 8) | buffer[(i * 2) + 1];
    }

    // Lê temperatura a partir do registrador 0x41 (2 bytes)
    val = 0x41;
    i2c_write_blocking(i2c_port, mpu6050_address, &val, 1, true);
    i2c_read_blocking(i2c_port, mpu6050_address, buffer, 2, false);

    *temp = (buffer[0] << 8) | buffer[1];
}