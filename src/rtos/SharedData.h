#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

/**
 * @struct DriveCommand
 * @brief Estrutura de mensagem transferida via FreeRTOS Queue do Core 1 para o Core 0.
 */
struct DriveCommand {
    float leftPower;       ///< Potência calculada para motor esquerdo [-1.0, 1.0]
    float rightPower;      ///< Potência calculada para motor direito [-1.0, 1.0]
    uint32_t timestampMs;  ///< Timestamp (millis) da geração da mensagem para verificação de failsafe
    bool activeSignal;     ///< Flag indicando se a comunicação Bluetooth está ativa e enviando comandos válidos
};

/**
 * Declaração global das Queues e Mutexes do FreeRTOS
 */
extern QueueHandle_t xCommandQueue;
