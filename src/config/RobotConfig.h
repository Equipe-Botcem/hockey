#pragma once

#include <stdint.h>
#include <freertos/FreeRTOS.h>


/**
 * @file RobotConfig.h
 * @brief Parâmetros operacionais do robô Micro Hockey 2WD.
 */

namespace Config {
    namespace PWM {
        constexpr uint32_t FREQUENCY_HZ = 20000; ///< 20 kHz para operação silenciosa dos motores N20
        constexpr uint8_t  RESOLUTION_BITS = 8;  ///< Resolução de 8 bits (valores de PWM 0 a 255)
        constexpr uint8_t  CHANNEL_LEFT = 0;     ///< Canal LEDC para Motor A (Esquerdo)
        constexpr uint8_t  CHANNEL_RIGHT = 1;    ///< Canal LEDC para Motor B (Direito)
    }

    namespace Filter {
        constexpr float JOYSTICK_DEADBAND = 0.08f; ///< Zona morta central [-0.08, 0.08]
        constexpr float EXPO_ALPHA       = 0.70f; ///< Proporção entre curva linear e exponencial (0.0 a 1.0)
        constexpr float EXPO_GAMMA       = 2.20f; ///< Curvatura exponencial (exponencial de sensitividade)
    }

    namespace Safety {
        constexpr uint32_t FAILSAFE_TIMEOUT_MS = 200; ///< Tempo limite sem sinal Bluetooth para acionar freio ativo (ms)
    }

    namespace Bluetooth {
        constexpr const char* DEVICE_NAME = "MicroHockey-ESP32"; ///< Nome exibido na busca Bluetooth
    }

    namespace RTOS {
        constexpr uint32_t MOTOR_TASK_STACK = 4096;
        constexpr uint32_t LOGIC_TASK_STACK = 4096;
        constexpr uint32_t COMM_TASK_STACK  = 4096;
        
        constexpr UBaseType_t MOTOR_TASK_PRIO = 5; ///< Alta prioridade no Core 0
        constexpr UBaseType_t LOGIC_TASK_PRIO = 3; ///< Prioridade média no Core 1
        constexpr UBaseType_t COMM_TASK_PRIO  = 4; ///< Prioridade alta no Core 1
    }
}
