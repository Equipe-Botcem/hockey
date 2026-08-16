# Guia Passo a Passo: Como Compilar e Enviar o Código para o ESP32

Este guia detalha o processo completo para preparar o ambiente, compilar e enviar o firmware do robô **Micro Hockey 2WD** para a placa ESP32 DevKit v1, além de explicar as ligações de hardware e como realizar os testes com o aplicativo de celular.

---

## 1. Pré-Requisitos de Hardware e Conexões

### Hardware Necessário
- **MCU:** ESP32 DevKit v1 (30 pinos).
- **Cabo USB:** Micro-USB ou USB-C (deve suportar **dados**, não apenas carregamento).
- **Driver de Motor:** Ponte H TB6612FNG.
- **Motores:** 2x Micro Motores DC N20.
- **Bateria:** LiPo 2S (7.4V nominal).

### Diagrama de Mapeamento de Pinos (Pinout)

| Componente TB6612FNG | Pino ESP32 DevKit v1 | Função |
| :--- | :--- | :--- |
| **AIN1** | `GPIO 18` | Direção Motor Esquerdo A1 |
| **AIN2** | `GPIO 19` | Direção Motor Esquerdo A2 |
| **PWMA** | `GPIO 5` | PWM Canal 0 Motor Esquerdo |
| **BIN1** | `GPIO 16` | Direção Motor Direito B1 |
| **BIN2** | `GPIO 17` | Direção Motor Direito B2 |
| **PWMB** | `GPIO 4` | PWM Canal 1 Motor Direito |
| **STBY** | `GPIO 23` | Standby (HIGH = Ativo, LOW = Desligado) |
| **GND** | `GND` | Terra comum entre ESP32, TB6612 e Bateria |
| **VCC** | `3V3` ou `5V` | Alimentação lógica da TB6612 |
| **VM** | `LiPo 2S (+7.4V)` | Alimentação de força dos motores |

> [!IMPORTANT]
> **Terra Unificado (GND):** Certifique-se de unir o pino `GND` do ESP32 com o `GND` da ponte H e o polo negativo (`-`) da bateria LiPo 2S para evitar flutuação de sinal lógica.

---

## 2. Método Recomendado: Compilação via VS Code + PlatformIO

O projeto foi configurado nativamente para **PlatformIO**, que gerencia todas as dependências e o ambiente de compilação automaticamente.

### Passo 1: Instalar o VS Code e a Extensão PlatformIO
1. Baixe e instale o [Visual Studio Code](https://code.visualstudio.com/).
2. Abra o VS Code, vá na aba **Extensions** (`Ctrl + Shift + X`).
3. Procure por **PlatformIO IDE** e clique em **Install**.
4. Reinicie o VS Code após a instalação ser concluída.

### Passo 2: Abrir o Projeto
1. No VS Code, vá em `File` > `Open Folder...`.
2. Selecione a pasta do projeto: `c:\Users\lucas\hockey`.
3. Aguarde o PlatformIO carregar o projeto (uma barra de status azul aparecerá na parte inferior).

### Passo 3: Compilar o Firmware
1. Clique no ícone de **Checkmark** (`✓` Build) no rodapé do PlatformIO ou pressione `Ctrl + Alt + B`.
2. O PlatformIO fará o download da plataforma `espressif32` e compilará todos os arquivos da pasta `src/`.

### Passo 4: Gravar no ESP32 (Upload)
1. Conecte o ESP32 ao computador via cabo USB.
2. Clique no ícone de **Seta para a direita** (`→` Upload) no rodapé do PlatformIO ou pressione `Ctrl + Alt + U`.
3. *(Opcional)* Se a mensagem `Connecting......` aparecer no terminal sem progredir, mantenha pressionado o botão **BOOT** na placa ESP32 até o upload iniciar.

### Passo 5: Monitor Serial
1. Clique no ícone de **Tomada/Plug** (`Monitor`) no rodapé do PlatformIO.
2. O baudrate será automaticamente configurado em `115200`. Você verá as mensagens de inicialização do FreeRTOS e das tarefas.

---

## 3. Método Alternativo: Compilação via Arduino IDE (2.x)

Caso prefira utilizar o **Arduino IDE**, siga estes passos:

### Passo 1: Configurar a Placa ESP32 no Arduino IDE
1. Abra o **Arduino IDE**.
2. Vá em `File` > `Preferences`.
3. No campo **Additional Boards Manager URLs**, adicione a URL:
   ```text
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Vá em `Tools` > `Board` > `Boards Manager...`, procure por **esp32** e instale a versão mais recente da Espressif.

### Passo 2: Configurar Parâmetros da Placa
1. Vá em `Tools` > `Board` > `esp32` > Selecione **DOIT ESP32 DEVKIT V1**.
2. Vá em `Tools` > `Partition Scheme` > Selecione **Huge APP (3MB No OTA/1MB SPIFFS)** ou **Default 4MB with spiffs** (para garantir espaço suficiente para a biblioteca Bluetooth).
3. Selecione a porta COM correspondente ao seu ESP32 em `Tools` > `Port`.

### Passo 3: Estrutura da Pasta para Arduino IDE
Se usar o Arduino IDE, renomeie `src/main.cpp` para `hockey.ino` e mantenha a pasta `src/` no mesmo diretório para que a IDE reconheça os cabeçalhos (`.h`) e implementações (`.cpp`).

---

## 4. Como Testar e Controlar o Robô via Bluetooth

### 1. Nome do Dispositivo Bluetooth
- O Bluetooth do robô iniciará com o nome: **`MicroHockey-ESP32`**.

### 2. Pareamento no Smartphone
1. Ligue a alimentação do ESP32 e o robô.
2. Abra o Bluetooth do smartphone (Android ou iOS).
3. Procure por **`MicroHockey-ESP32`** e faça o pareamento (código padrão `0000` ou `1234` se solicitado).

### 3. Aplicativos Recomendados
Você pode usar qualquer aplicativo de controle de robô Bluetooth (como *Serial Bluetooth Terminal*, *Dabble*, *RoboRemo*, ou um app customizado em Flutter/React Native).

### 4. Formatos de Comando Aceitos pelo Firmware

O código suporta dois formatos de pacote automaticamente:

#### Formato A: Protocolo Binário Compacto (Recomendado para baixa latência)
Pacote fixo de **5 bytes**:
- `Byte 0:` `0xAA` (Header 1)
- `Byte 1:` `0x55` (Header 2)
- `Byte 2:` `int8_t` Aceleração (-100 a +100)
- `Byte 3:` `int8_t` Curva/Giro (-100 a +100)
- `Byte 4:` `uint8_t` Checksum (`Byte0 ^ Byte1 ^ Byte2 ^ Byte3`)

#### Formato B: Protocolo ASCII (Útil para testes com terminal)
Envie valores decimais em texto no formato `Aceleração,Curva\n`:
- **Exemplo 1 (Frente 50%, sem curva):** `0.5,0.0`
- **Exemplo 2 (Giro máximo para esquerda):** `0.0,-1.0`
- **Exemplo 3 (Ré 100%):** `-1.0,0.0`
- **Exemplo 4 (Neutro/Parar):** `0.0,0.0`

---

## 5. Verificação de Failsafe (Segurança)

Para testar o Failsafe de segurança:
1. Conecte o smartphone ao robô e envie comandos para mover os motores.
2. **Desligue o Bluetooth do celular** ou encerre o aplicativo subitamente.
3. **Resultado Esperado:** Dentro de **200ms**, o laço no Core 0 detectará o timeout de comunicação e acionará o **Freio Ativo** (`IN1=HIGH`, `IN2=HIGH`, `PWM=0`), travando os motores instantaneamente para evitar deslizamento na arena.
