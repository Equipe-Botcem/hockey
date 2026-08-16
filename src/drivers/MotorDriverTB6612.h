#pragma once

#include <Arduino.h>
#include <esp_arduino_version.h>
#include "config/PinConfig.h"
#include "config/RobotConfig.h"

/**
 * @class MotorDriverTB6612
 * @brief Driver para a Ponte H TB6612FNG utilizando os periféricos LEDC do ESP32.
 */
class MotorDriverTB6612 {
public:
    MotorDriverTB6612();
    ~MotorDriverTB6612() = default;

    /**
     * @brief Inicializa os pinos GPIO e os canais PWM LEDC do ESP32.
     */
    void begin();

    /**
     * @brief Define a velocidade dos motores esquerdo e direito.
     * @param leftPower Potência do motor esquerdo [-1.0f, 1.0f].
     * @param rightPower Potência do motor direito [-1.0f, 1.0f].
     */
    void setSpeeds(float leftPower, float rightPower);

    /**
     * @brief Aciona o freio ativo (Short Brake) em ambos os motores imediatamente.
     * 
     * Na TB6612FNG, Short Brake é obtido com IN1=HIGH e IN2=HIGH.
     * Essencial para parada imediata no Failsafe.
     */
    void applyShortBrake();

    /**
     * @brief Coloca a ponte H em estado de Standby (desabilita os MOSFETs).
     */
    void enableStandby(bool enable);

private:
    /**
     * @brief Define a potência individual de um motor.
     * @param pinIn1 Pino IN1.
     * @param pinIn2 Pino IN2.
     * @param pwmChannel Canal LEDC do PWM.
     * @param pwmPin Pino GPIO do PWM.
     * @param power Potência normalizada [-1.0f, 1.0f].
     */
    void setIndividualMotor(uint8_t pinIn1, uint8_t pinIn2, uint8_t pwmChannel, uint8_t pwmPin, float power);

    bool m_initialized{false};
};
