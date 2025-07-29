# IMU DATALOGGER: Sistema Embarcado de Captura e Análise de Dados Inerciais

O **IMU DATALOGGER** é um sistema embarcado completo e portátil desenvolvido para a plataforma **BitDogLab** com o microcontrolador **Raspberry Pi Pico W (RP2040)**. O sistema realiza captura, processamento e armazenamento em tempo real de dados de movimento provenientes do sensor inercial **MPU6050**, registrando informações de aceleração linear e velocidade angular nos três eixos espaciais. Com interface intuitiva baseada em LEDs RGB, display OLED e controle por botões, o projeto oferece uma solução completa para análise de movimento, incluindo ferramenta de visualização gráfica em Python para interpretação posterior dos dados coletados.

---

## Vídeo de Demonstração

[Assista à demonstração do IMU DATALOGGER](#)

---

## Funcionalidades Principais

- **Captura de Dados Inerciais em Tempo Real**: Leitura contínua de aceleração linear e velocidade angular utilizando o sensor MPU6050 de 6 eixos (3 acelerômetro + 3 giroscópio).
- **Armazenamento Seguro em MicroSD**:
  - Sistema de arquivos FAT32 com montagem/desmontagem controlada manualmente
  - Formato CSV padronizado para compatibilidade com ferramentas de análise
  - Proteção contra corrupção de dados através de controle de estado
- **Interface de Usuário Intuitiva**:
  - **Display OLED SSD1306**: Exibição de status do sistema e estados operacionais
  - **LED RGB**: Indicação visual clara dos estados (espera, pronto, gravando)
  - **Controle por Botões**: Três botões físicos para controle completo do sistema
  - **Feedback Sonoro**: Buzzer PWM para confirmação de início/fim de gravação
- **Conversão de Unidades Físicas**: 
  - Aceleração convertida para unidades de gravidade (g)
  - Velocidade angular convertida para graus por segundo (°/s)
- **Ferramenta de Análise Python**: Script dedicado para visualização gráfica dos dados coletados
- **Máquina de Estados Robusta**: Controle confiável com tratamento de interrupções e debounce
- **Operação Portátil**: Sistema autônomo alimentado via USB, ideal para coleta de dados em campo

---

## Tecnologias Utilizadas

- **Linguagem de Programação**: C (firmware) + Python (análise de dados)
- **Microcontrolador**: Raspberry Pi Pico W (RP2040)
- **Sensor Principal**: MPU6050 (acelerômetro e giroscópio integrados 6-DOF)
- **Armazenamento**: Cartão MicroSD com sistema de arquivos FAT32
- **Comunicação**: 
  - I2C para sensores e display
  - SPI para interface com cartão SD
- **Componentes de Interface**:
  - Display OLED SSD1306 128x64 (I2C) para feedback visual
  - LED RGB individual para indicação de estado
  - Buzzer com controle PWM para feedback sonoro
  - Três push buttons com tratamento por interrupção
- **Bibliotecas Principais**:
  - Pico SDK para acesso ao hardware (GPIO, I2C, SPI, PWM)
  - FatFs para gerenciamento do sistema de arquivos SD
  - Bibliotecas personalizadas para controle dos periféricos
- **Ferramentas de Análise**:
  - Python com pandas para processamento de dados CSV
  - matplotlib para geração de gráficos científicos
  - Interface gráfica para visualização simultânea dos 6 eixos

---

## Sensor MPU6050 - Especificações Técnicas

### Acelerômetro
- **Faixas Configuráveis**: ±2g, ±4g, ±8g, ±16g
- **Faixa Utilizada**: ±2g (sensibilidade: 16.384 LSB/g)
- **Resolução**: 16 bits
- **Precisão**: Alta estabilidade para medições de movimento

### Giroscópio
- **Faixas Configuráveis**: ±250°/s, ±500°/s, ±1000°/s, ±2000°/s
- **Faixa Utilizada**: ±250°/s (sensibilidade: 131 LSB/°/s)
- **Resolução**: 16 bits
- **Aplicação**: Medição de velocidade angular em três eixos

### Interface e Comunicação
- **Protocolo**: I2C (endereço padrão: 0x68)
- **Tensão de Operação**: 3.3V
- **Taxa de Amostragem**: Configurável até 8kHz
- **Filtro Digital**: Low-pass filter interno para redução de ruído

---

## Como Funciona

### Estados Operacionais do Sistema

#### Estado de Inicialização
- **LED RGB**: Amarelo (aguardando montagem do cartão)
- **Display**: "IMU DATALOGGER" (tela inicial)
- **Ação Requerida**: Inserir cartão SD e pressionar Botão B para montagem

#### Estado Pronto para Gravação
- **LED RGB**: Verde (cartão montado com sucesso)
- **Display**: Confirmação de cartão montado
- **Ação Disponível**: Pressionar Botão A para iniciar gravação

#### Estado de Gravação Ativa
- **LED RGB**: Vermelho (gravação em andamento)
- **Display**: "Gravando Dados" (mensagem fixa)
- **Buzzer**: Sinal sonoro de início
- **Processo**: Captura contínua e armazenamento em CSV
- **Ação Disponível**: Pressionar Botão A novamente para parar

#### Estado de Finalização
- **Buzzer**: Sinal sonoro de conclusão
- **Display**: "Dados salvos com sucesso!"
- **Arquivo**: CSV criado com cabeçalho e dados estruturados
- **Retorno**: Volta ao estado pronto (LED verde)

### Controle por Botões

| Botão | Função Principal | Estado Necessário |
|-------|------------------|-------------------|
| **Botão A** | Iniciar/Parar gravação | Cartão montado |
| **Botão B** | Montar cartão SD | Cartão inserido, sistema em espera |
| **Botão do Joystick** | Desmontar cartão SD | Sistema parado, não gravando |

### Processamento de Dados Inerciais

#### Aquisição de Dados Brutos
```c
// Leitura dos registradores do MPU6050
int16_t aceleracao[3]; // X, Y, Z (valores brutos)
int16_t gyro[3];       // X, Y, Z (valores brutos)
mpu6050_read_raw(aceleracao, gyro, &temp);
```

#### Conversão para Unidades Físicas
```c
// Conversão do acelerômetro (±2g, 16384 LSB/g)
dados->ax = aceleracao[0] / 16384.0f; // Resultado em 'g'
dados->ay = aceleracao[1] / 16384.0f;
dados->az = aceleracao[2] / 16384.0f;

// Conversão do giroscópio (±250°/s, 131 LSB/°/s)
dados->gx = gyro[0] / 131.0f; // Resultado em '°/s'
dados->gy = gyro[1] / 131.0f;
dados->gz = gyro[2] / 131.0f;
```

#### Estrutura de Dados CSV
```csv
numero_amostra,accel_x,accel_y,accel_z,giro_x,giro_y,giro_z
1,-0.12,0.98,-0.05,1.23,-0.45,2.67
2,-0.11,0.99,-0.04,1.19,-0.48,2.71
...
```

---

## Configuração do Hardware

| Componente | Interface | Pinos do RP2040 | Função |
|------------|-----------|----------------|--------|
| MPU6050 | I2C0 | GP0 (SDA), GP1 (SCL) | Sensor inercial 6-DOF |
| Display OLED SSD1306 | I2C1 | GP14 (SDA), GP15 (SCL) | Interface visual do usuário |
| Cartão MicroSD | SPI0 | GP16 (MISO), GP19 (MOSI), GPIO18 (SCK), CS (17) | Armazenamento de dados |
| LED RGB | GPIO | GP11 (Verde), GP12 (Azul), GP13 (Vermelho) | Indicação de estado |
| Buzzer | PWM | GP10 | Feedback sonoro |
| Botão A | GPIO com IRQ | GP5 | Controle de gravação |
| Botão B | GPIO com IRQ | GP6 | Montagem do cartão |
| Botão Joystick | GPIO com IRQ | GP22 | Desmontagem do cartão |

### Configuração I2C
- **I2C0 (MPU6050)**: Velocidade 400kHz para comunicação rápida com sensor
- **I2C1 (Display)**: Velocidade 400kHz para atualização fluida do display
- **Pull-ups**: Resistores externos de 4.7kΩ recomendados

### Configuração SPI (MicroSD)
- **Interface**: Gerenciada pela biblioteca FatFs_SPI
- **Velocidade**: Otimizada para gravação contínua
- **Formato**: Sistema de arquivos FAT32 para máxima compatibilidade

---

## Ferramenta de Análise Python

### Funcionalidades do Script
O script `PlotaIMU.py` oferece visualização científica completa dos dados coletados:

#### Características Principais
- **Carregamento Automático**: Leitura direta de arquivos CSV gerados pelo sistema
- **Gráficos Duais**: Visualização simultânea de acelerômetro e giroscópio
- **Cores Diferenciadas**: Cada eixo (X, Y, Z) com cor única para fácil identificação
- **Grade e Legendas**: Interface científica com grid e legendas claras
- **Escala Compartilhada**: Eixo X sincronizado entre os dois gráficos

#### Estrutura dos Gráficos
```python
# Gráfico Superior: Acelerômetro
- Eixo X: Vermelho
- Eixo Y: Verde  
- Eixo Z: Azul
- Unidade: g (gravidade)

# Gráfico Inferior: Giroscópio  
- Eixo X: Roxo
- Eixo Y: Laranja
- Eixo Z: Marrom
- Unidade: °/s (graus por segundo)
```

#### Dependências Python
```bash
pip install pandas matplotlib
```

#### Uso da Ferramenta
```bash
python PlotaIMU.py
```

## Estrutura do Repositório

- **`main.c`**: Código-fonte principal com lógica de controle e máquina de estados
- **`CMakeLists.txt`**: Configuração do sistema de build com Pico SDK
- **`lib/`**: Bibliotecas modulares organizadas por funcionalidade
  - **`mpu6050/`**: Biblioteca completa para sensor inercial
    - **`mpu6050.h` e `mpu6050.c`**: Interface I2C e processamento de dados
  - **`SD/`**: Sistema de gerenciamento do cartão MicroSD
    - **`SD.h` e `SD.c`**: Montagem, desmontagem e operações de arquivo
  - **`ssd1306/`**: Controle do display OLED
    - **`ssd1306.h` e `ssd1306.c`**: Funções gráficas e de texto
  - **`push_button/`**: Sistema robusto de controle de botões
    - **`button.h` e `button.c`**: Interrupções, debounce e tratamento de eventos
  - **`rgb/`**: Controle do LED RGB
    - **`rgb.h` e `rgb.c`**: Funções para cores e padrões visuais
  - **`buzzer/`**: Sistema de feedback sonoro
    - **`buzzer.h` e `buzzer.c`**: Controle PWM para geração de tons
  - **`FatFs_SPI/`**: Biblioteca externa para sistema de arquivos
  - **`hw_config.c`**: Configuração de hardware centralizada
- **`graficos_imu/`**: Ferramentas de análise
  - **`PlotaIMU.py`**: Script principal de visualização
  - **Exemplos de arquivos CSV**: Dados de demonstração
- **`README.md`**: Documentação completa do projeto

---

## Fluxo de Operação Detalhado

### 1. Inicialização do Sistema
```c
setup(); // Configuração de todos os periféricos
setup_interrupcoes(); // Configuração de botões com debounce
```
- **Configuração I2C**: Inicialização das duas interfaces I2C
- **Reset do MPU6050**: Configuração de registradores e calibração
- **Inicialização do Display**: Limpeza e exibição da tela inicial
- **Setup dos GPIOs**: Configuração de botões, LEDs e buzzer
- **Estado Inicial**: LED amarelo, aguardando montagem do cartão

### 2. Montagem do Cartão SD
```c
// Ativado por interrupção do Botão B
run_mount(); // Função da biblioteca SD
```
- **Detecção do Cartão**: Verificação de presença física
- **Montagem FAT32**: Inicialização do sistema de arquivos
- **Verificação de Integridade**: Teste de escrita/leitura
- **Feedback Visual**: LED muda para verde (pronto para usar)

### 3. Processo de Gravação
```c
capture_mpu6050_data_and_save(); // Loop principal de captura
```

#### Criação do Arquivo CSV
```c
FIL file;
f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);
f_write(&file, cabecalho, strlen(cabecalho), &bytes_written);
```

#### Loop de Captura de Dados
```c
while (gravando_dados) {
    mpu6050_read_raw(aceleracao, gyro, &temp);
    processar_dados_imu(&dados_imu, aceleracao, gyro);
    sprintf(buffer, "%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", ...);
    f_write(&file, buffer, strlen(buffer), &bytes_written);
    sleep_ms(100); // Taxa de amostragem: 10Hz
}
```

### 4. Finalização e Desmontagem
```c
f_close(&file); // Fechamento seguro do arquivo
run_unmount(); // Desmontagem segura do cartão
```
- **Fechamento do Arquivo**: Garantia de gravação de todos os dados
- **Desmontagem Segura**: Prevenção de corrupção de dados
- **Reset do Estado**: Sistema retorna ao estado inicial

---

## Tratamento de Interrupções e Debounce

### Sistema de Debounce por Temporização
```c
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    
    // Debounce de 500ms para evitar múltiplos triggers
    if (absolute_time_diff_us(last_time, current_time) > 500000) {
        last_time = current_time;
        
        // Processamento da interrupção baseado no GPIO
        if (gpio == BUTTON_A && condições_válidas) {
            gravando_dados = !gravando_dados;
        }
        // ... outros botões
    }
}
```

### Máquina de Estados com Flags Booleanas
```c
// Flags de controle global
volatile bool gravando_dados = false;
volatile bool montando_cartao = false;
volatile bool desmontando_cartao = false;

// Verificação de estados mutuamente exclusivos
if (gpio == BUTTON_A && !montando_cartao && !desmontando_cartao) {
    // Permite iniciar/parar gravação apenas se não há outras operações
}
```

---

## Aplicações Práticas

### Monitoramento de Atividades Físicas
- **Análise de Caminhada**: Estudo de padrões de passada, detecção de irregularidades
- **Exercícios**: Monitoramento de forma e técnica em atividades físicas
- **Reabilitação**: Acompanhamento de progresso em fisioterapia
- **Detecção de Quedas**: Sistema de alerta para idosos

### Pesquisa e Desenvolvimento
- **Biomecânica**: Estudos científicos de movimento humano
- **Robótica**: Coleta de dados para algoritmos de controle
- **Realidade Virtual**: Captura de movimentos para interfaces imersivas
- **Jogos**: Desenvolvimento de controles baseados em movimento

### Monitoramento Industrial
- **Análise de Vibrações**: Detecção precoce de falhas em máquinas
- **Controle de Qualidade**: Verificação de produtos durante transporte
- **Manutenção Preditiva**: Monitoramento de equipamentos críticos
- **Segurança**: Detecção de movimentos anômalos em estruturas

### Educação e Ensino
- **Física Experimental**: Demonstração prática de conceitos de movimento
- **Engenharia**: Projetos práticos de sistemas embarcados
- **Matemática**: Visualização de funções trigonométricas através de movimento
- **Programação**: Ensino de estruturas de dados e processamento de sinais

---

## Objetivos Alcançados

- **Sistema de Aquisição Profissional**: Datalogger robusto para aplicações científicas e industriais
- **Interface Intuitiva**: Operação simples através de feedback visual e sonoro claro
- **Armazenamento Confiável**: Sistema seguro de gravação com proteção contra corrupção
- **Precisão de Medição**: Conversão precisa para unidades físicas padrão
- **Portabilidade**: Sistema compacto e autônomo para uso em campo
- **Compatibilidade Universal**: Formato de dados aberto para máxima interoperabilidade
- **Ferramenta de Análise**: Script Python completo para visualização científica
- **Arquitetura Modular**: Código bem estruturado para fácil manutenção e expansão
- **Tratamento Robusto de Eventos**: Sistema de controle confiável com debounce adequado
- **Documentação Completa**: Guia abrangente para uso e desenvolvimento
- **Aplicabilidade Multidisciplinar**: Solução versátil para diversas áreas de pesquisa
- **Baixo Custo**: Implementação econômica com componentes acessíveis
- **Código Aberto**: Disponibilização completa para comunidade técnica e acadêmica

---

## Desenvolvido por

Henrique Oliveira dos Santos  

[![LinkedIn](https://img.shields.io/badge/LinkedIn-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/dev-henriqueo-santos/)
