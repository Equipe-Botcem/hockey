#pragma once

#include "config/RobotConfig.h"

/**
 * @class InputFilter
 * @brief Processamento de sinal de entrada para joysticks analógicos.
 * 
 * Aplica Deadband (zona morta central) e filtro misto Linear-Exponencial (Expo)
 * para otimizar o controle fino em baixa velocidade mantendo a aceleração máxima no limite.
 */
class InputFilter {
public:
    InputFilter(float deadband = Config::Filter::JOYSTICK_DEADBAND,
                float alpha = Config::Filter::EXPO_ALPHA,
                float gamma = Config::Filter::EXPO_GAMMA);

    ~InputFilter() = default;

    /**
     * @brief Processa uma entrada analógica bruta no intervalo [-1.0, 1.0].
     * @param rawInput Valor bruto do joystick.
     * @return Valor filtrado no intervalo [-1.0, 1.0].
     */
    float process(float rawInput) const;

    // Setters para reconfiguração em tempo de execução
    void setDeadband(float deadband) { m_deadband = deadband; }
    void setAlpha(float alpha) { m_alpha = alpha; }
    void setGamma(float gamma) { m_gamma = gamma; }

private:
    float m_deadband;
    float m_alpha;
    float m_gamma;
};
