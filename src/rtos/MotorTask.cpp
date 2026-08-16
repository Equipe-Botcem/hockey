#include "rtos/MotorTask.h"

void vMotorTask(void* pvParameters) {
    // Instanciação e inicialização do driver de motor no contexto do Core 0
    MotorDriverTB6612 motorDriver;
    motorDriver.begin();

    // Estado local de comando com valores neutros
    DriveCommand currentCommand = {0.0f, 0.0f, millis(), false};
    uint32_t lastValidTimestamp = millis();

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(10); // Laço determinístico de 100 Hz (10ms)

    for (;;) {
        // Tenta ler o comando mais recente enviado pelo Core 1 sem bloquear a tarefa
        DriveCommand receivedCommand;
        if (xQueueReceive(xCommandQueue, &receivedCommand, 0) == pdTRUE) {
            currentCommand = receivedCommand;
            if (currentCommand.activeSignal) {
                lastValidTimestamp = currentCommand.timestampMs;
            }
        }

        uint32_t now = millis();
        uint32_t timeSinceLastRx = now - lastValidTimestamp;

        // --- Verificação de Segurança & Failsafe ---
        // Se exceder o timeout de 200ms ou se o sinal não estiver ativo -> Aplica Freio Ativo (Short Brake)
        if (timeSinceLastRx > Config::Safety::FAILSAFE_TIMEOUT_MS || !currentCommand.activeSignal) {
            motorDriver.applyShortBrake();
            digitalWrite(Config::Pins::LED_STATUS, LOW); // Apaga LED de status
        } else {
            // Aplica potências calculadas aos motores
            motorDriver.setSpeeds(currentCommand.leftPower, currentCommand.rightPower);
            digitalWrite(Config::Pins::LED_STATUS, HIGH); // Acende LED de status indicando operação ativa
        }

        // Aguarda a próxima iteração mantendo período exato de 10ms
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
