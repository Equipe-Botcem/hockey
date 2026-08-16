#include "drivers/MotorDriverTB6612.h"
#include <algorithm>

MotorDriverTB6612::MotorDriverTB6612() : m_initialized(false) {}

void MotorDriverTB6612::begin() {
    // Configura os pinos de direção e standby como saída
    pinMode(Config::Pins::MOTOR_A_IN1, OUTPUT);
    pinMode(Config::Pins::MOTOR_A_IN2, OUTPUT);
    pinMode(Config::Pins::MOTOR_B_IN1, OUTPUT);
    pinMode(Config::Pins::MOTOR_B_IN2, OUTPUT);
    pinMode(Config::Pins::MOTOR_STBY, OUTPUT);

    // Garante modo de freio inicial
    digitalWrite(Config::Pins::MOTOR_A_IN1, HIGH);
    digitalWrite(Config::Pins::MOTOR_A_IN2, HIGH);
    digitalWrite(Config::Pins::MOTOR_B_IN1, HIGH);
    digitalWrite(Config::Pins::MOTOR_B_IN2, HIGH);

    // Ativa a ponte H ativando o pino STBY
    digitalWrite(Config::Pins::MOTOR_STBY, HIGH);

    // Configuração dos canais PWM via API LEDC do ESP32
#if defined(ESP_ARDUINO_VERSION_VAL) && (ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0))
    ledcAttach(Config::Pins::MOTOR_A_PWM, Config::PWM::FREQUENCY_HZ, Config::PWM::RESOLUTION_BITS);
    ledcAttach(Config::Pins::MOTOR_B_PWM, Config::PWM::FREQUENCY_HZ, Config::PWM::RESOLUTION_BITS);
    ledcWrite(Config::Pins::MOTOR_A_PWM, 0);
    ledcWrite(Config::Pins::MOTOR_B_PWM, 0);
#else
    ledcSetup(Config::PWM::CHANNEL_LEFT, Config::PWM::FREQUENCY_HZ, Config::PWM::RESOLUTION_BITS);
    ledcAttachPin(Config::Pins::MOTOR_A_PWM, Config::PWM::CHANNEL_LEFT);

    ledcSetup(Config::PWM::CHANNEL_RIGHT, Config::PWM::FREQUENCY_HZ, Config::PWM::RESOLUTION_BITS);
    ledcAttachPin(Config::Pins::MOTOR_B_PWM, Config::PWM::CHANNEL_RIGHT);

    ledcWrite(Config::PWM::CHANNEL_LEFT, 0);
    ledcWrite(Config::PWM::CHANNEL_RIGHT, 0);
#endif

    m_initialized = true;
}

void MotorDriverTB6612::enableStandby(bool enable) {
    digitalWrite(Config::Pins::MOTOR_STBY, enable ? LOW : HIGH);
}

void MotorDriverTB6612::applyShortBrake() {
    // Na TB6612FNG, IN1=HIGH e IN2=HIGH acionam o freio dinâmico (Short Brake)
    digitalWrite(Config::Pins::MOTOR_A_IN1, HIGH);
    digitalWrite(Config::Pins::MOTOR_A_IN2, HIGH);
    digitalWrite(Config::Pins::MOTOR_B_IN1, HIGH);
    digitalWrite(Config::Pins::MOTOR_B_IN2, HIGH);

#if defined(ESP_ARDUINO_VERSION_VAL) && (ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0))
    ledcWrite(Config::Pins::MOTOR_A_PWM, 0);
    ledcWrite(Config::Pins::MOTOR_B_PWM, 0);
#else
    ledcWrite(Config::PWM::CHANNEL_LEFT, 0);
    ledcWrite(Config::PWM::CHANNEL_RIGHT, 0);
#endif
}

void MotorDriverTB6612::setSpeeds(float leftPower, float rightPower) {
    if (!m_initialized) return;

    setIndividualMotor(
        Config::Pins::MOTOR_A_IN1,
        Config::Pins::MOTOR_A_IN2,
        Config::PWM::CHANNEL_LEFT,
        Config::Pins::MOTOR_A_PWM,
        leftPower
    );

    setIndividualMotor(
        Config::Pins::MOTOR_B_IN1,
        Config::Pins::MOTOR_B_IN2,
        Config::PWM::CHANNEL_RIGHT,
        Config::Pins::MOTOR_B_PWM,
        rightPower
    );
}

void MotorDriverTB6612::setIndividualMotor(uint8_t pinIn1, uint8_t pinIn2, uint8_t pwmChannel, uint8_t pwmPin, float power) {
    // Clamping do valor de potência em [-1.0, 1.0]
    power = std::max(-1.0f, std::min(1.0f, power));

    // Conversão da potência para valor de Duty Cycle (0 a 255)
    uint32_t duty = static_cast<uint32_t>(std::abs(power) * 255.0f);

    if (power > 0.001f) {
        // Frente
        digitalWrite(pinIn1, HIGH);
        digitalWrite(pinIn2, LOW);
    } else if (power < -0.001f) {
        // Ré
        digitalWrite(pinIn1, LOW);
        digitalWrite(pinIn2, HIGH);
    } else {
        // Neutro / Freio Ativo quando pot zero
        digitalWrite(pinIn1, HIGH);
        digitalWrite(pinIn2, HIGH);
        duty = 0;
    }

#if defined(ESP_ARDUINO_VERSION_VAL) && (ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0))
    ledcWrite(pwmPin, duty);
#else
    (void)pwmPin;
    ledcWrite(pwmChannel, duty);
#endif
}
