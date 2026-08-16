#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include "config/PinConfig.h"
#include "config/RobotConfig.h"
#include "rtos/SharedData.h"
#include "rtos/MotorTask.h"
#include "rtos/LogicTask.h"

// Definição da Fila FreeRTOS declarada em SharedData.h
QueueHandle_t xCommandQueue = nullptr;

void setup() {
    // Inicialização da porta serial para depuração
    Serial.begin(115200);
    delay(500);

    Serial.println(F("=============================================="));
    Serial.println(F(" Robô Micro Hockey 2WD - Firmware Initializing "));
    Serial.println(F(" Summit Robótica 2026 - ESP32 Dual Core RTOS   "));
    Serial.println(F("=============================================="));

    // Configuração do pino LED indicador de status
    pinMode(Config::Pins::LED_STATUS, OUTPUT);
    digitalWrite(Config::Pins::LED_STATUS, LOW);

    // Criação da fila FreeRTOS com capacidade para 1 mensagem (usada com xQueueOverwrite)
    xCommandQueue = xQueueCreate(1, sizeof(DriveCommand));
    if (xCommandQueue == nullptr) {
        Serial.println(F("[ERRO CRÍTICO] Falha ao criar xCommandQueue!"));
        while (1) {
            digitalWrite(Config::Pins::LED_STATUS, !digitalRead(Config::Pins::LED_STATUS));
            delay(100);
        }
    }

    // Inicialização dos valores padrão na fila
    DriveCommand initialCmd = {0.0f, 0.0f, millis(), false};
    xQueueOverwrite(xCommandQueue, &initialCmd);

    // --- Criação das Tarefas FreeRTOS ---

    // Core 0: Tarefa de controle de motores (Alta prioridade, controle determinístico do hardware LEDC)
    xTaskCreatePinnedToCore(
        vMotorTask,
        "MotorTask",
        Config::RTOS::MOTOR_TASK_STACK,
        nullptr,
        Config::RTOS::MOTOR_TASK_PRIO,
        nullptr,
        0 // Core 0
    );
    Serial.println(F("[RTOS] Tarefa 'vMotorTask' fixada ao Core 0."));

    // Core 1: Tarefa de recepção Bluetooth
    xTaskCreatePinnedToCore(
        vCommTask,
        "CommTask",
        Config::RTOS::COMM_TASK_STACK,
        nullptr,
        Config::RTOS::COMM_TASK_PRIO,
        nullptr,
        1 // Core 1
    );
    Serial.println(F("[RTOS] Tarefa 'vCommTask' fixada ao Core 1."));

    // Core 1: Tarefa de lógica de controle (Filtros, Deadband, Expo, Cinemática)
    xTaskCreatePinnedToCore(
        vLogicTask,
        "LogicTask",
        Config::RTOS::LOGIC_TASK_STACK,
        nullptr,
        Config::RTOS::LOGIC_TASK_PRIO,
        nullptr,
        1 // Core 1
    );
    Serial.println(F("[RTOS] Tarefa 'vLogicTask' fixada ao Core 1."));

    Serial.println(F("[INFO] Firmware inicializado com sucesso. Aguardando conexão Bluetooth..."));
}

void loop() {
    // O laço principal é mantido inativo, delegando todo o controle ao escalonador FreeRTOS
    vTaskDelay(pdMS_TO_TICKS(1000));
}
