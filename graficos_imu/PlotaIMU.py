import pandas as pd
import matplotlib.pyplot as plt

# Carrega os dados do CSV
data = pd.read_csv('mpu6050_movimentando.csv')

# Cria figura com 2 subplots (um acima do outro)
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8), sharex=True)

# --- Gráfico do Acelerômetro ---
ax1.plot(data['numero_amostra'], data['accel_x'], label='Accel X', color='red')
ax1.plot(data['numero_amostra'], data['accel_y'], label='Accel Y', color='green')
ax1.plot(data['numero_amostra'], data['accel_z'], label='Accel Z', color='blue')
ax1.set_title('Acelerômetro (MPU6050)')
ax1.set_ylabel('Aceleração (g)')
ax1.grid(True)
ax1.legend()

# --- Gráfico do Giroscópio ---
ax2.plot(data['numero_amostra'], data['giro_x'], label='Giro X', color='purple')
ax2.plot(data['numero_amostra'], data['giro_y'], label='Giro Y', color='orange')
ax2.plot(data['numero_amostra'], data['giro_z'], label='Giro Z', color='brown')
ax2.set_title('Giroscópio (MPU6050)')
ax2.set_xlabel('Número da Amostra')
ax2.set_ylabel('Velocidade Angular (°/s)')
ax2.grid(True)
ax2.legend()

# Ajusta o layout para evitar sobreposição
plt.tight_layout()
plt.show()