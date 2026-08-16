#include "rtos/LogicTask.h"
#include <atomic>

// Estrutura compartilhada entre vCommTask e vLogicTask no Core 1
struct CommState {
    std::atomic<float> rawThrottle{0.0f};
    std::atomic<float> rawTurn{0.0f};
    std::atomic<uint32_t> lastRxMs{0};
    std::atomic<bool> isConnected{false};
};

static CommState g_commState;

void vCommTask(void* pvParameters) {
    BluetoothHandler btHandler;
    btHandler.begin(Config::Bluetooth::DEVICE_NAME);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(5); // Polling de 200 Hz para baixa latência de serial

    for (;;) {
        bool connected = btHandler.isConnected();
        g_commState.isConnected.store(connected);

        RawCommandData cmd = btHandler.update();
        if (cmd.valid) {
            g_commState.rawThrottle.store(cmd.throttle);
            g_commState.rawTurn.store(cmd.turn);
            g_commState.lastRxMs.store(millis());
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void vLogicTask(void* pvParameters) {
    InputFilter filterThrottle(Config::Filter::JOYSTICK_DEADBAND, Config::Filter::EXPO_ALPHA, Config::Filter::EXPO_GAMMA);
    InputFilter filterTurn(Config::Filter::JOYSTICK_DEADBAND, Config::Filter::EXPO_ALPHA, Config::Filter::EXPO_GAMMA);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20); // 50 Hz para atualização de controle

    for (;;) {
        uint32_t now = millis();
        uint32_t lastRx = g_commState.lastRxMs.load();
        bool connected = g_commState.isConnected.load();

        bool active = connected && ((now - lastRx) <= Config::Safety::FAILSAFE_TIMEOUT_MS);

        float rawTh = g_commState.rawThrottle.load();
        float rawTr = g_commState.rawTurn.load();

        // 1. Aplicação do filtro de Joystick (Deadband + Exponencial)
        float filteredTh = filterThrottle.process(rawTh);
        float filteredTr = filterTurn.process(rawTr);

        // 2. Cinemática Arcade Drive com Normalização Dinâmica
        DriveOutput driveOut = ArcadeDrive::calculate(filteredTh, filteredTr);

        // 3. Montagem do pacote de comando
        DriveCommand cmdToSend;
        cmdToSend.leftPower    = driveOut.leftPower;
        cmdToSend.rightPower   = driveOut.rightPower;
        cmdToSend.timestampMs  = lastRx;
        cmdToSend.activeSignal = active;

        // 4. Envio para a Fila FreeRTOS do Core 0 usando xQueueOverwrite
        // (Garante envio imediato não-bloqueante e substituição do valor antigo se não consumido)
        xQueueOverwrite(xCommandQueue, &cmdToSend);

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
