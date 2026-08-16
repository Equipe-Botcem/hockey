#pragma once

#include <stdint.h>

/**
 * @file PinConfig.h
 * @brief Mapeamento de pinos GPIO do ESP32 DevKit v1 para a ponte H TB6612FNG e periféricos.
 */

namespace Config {
    namespace Pins {
        // --- Motor Esquerdo (Motor A) ---
        constexpr uint8_t MOTOR_A_IN1 = 18;  ///< Direção Motor A - Entrada 1
        constexpr uint8_t MOTOR_A_IN2 = 19;  ///< Direção Motor A - Entrada 2
        constexpr uint8_t MOTOR_A_PWM = 5;   ///< PWM Motor A (Canal LEDC 0)

        // --- Motor Direito (Motor B) ---
        constexpr uint8_t MOTOR_B_IN1 = 16;  ///< Direção Motor B - Entrada 1
        constexpr uint8_t MOTOR_B_IN2 = 17;  ///< Direção Motor B - Entrada 2
        constexpr uint8_t MOTOR_B_PWM = 4;   ///< PWM Motor B (Canal LEDC 1)

        // --- Habilitação da Ponte H ---
        constexpr uint8_t MOTOR_STBY  = 23;  ///< Pino Standby da TB6612FNG (HIGH = ativo, LOW = standby)

        // --- Indicadores ---
        constexpr uint8_t LED_STATUS  = 2;   ///< LED onboard / indicador de conexão Bluetooth
    }
}
