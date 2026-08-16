#include "filters/InputFilter.h"
#include <cmath>
#include <algorithm>

InputFilter::InputFilter(float deadband, float alpha, float gamma)
    : m_deadband(deadband), m_alpha(alpha), m_gamma(gamma) {}

float InputFilter::process(float rawInput) const {
    // 0. Clamping de entrada no intervalo [-1.0, 1.0]
    float input = std::max(-1.0f, std::min(1.0f, rawInput));
    float absInput = std::abs(input);

    // 1. Aplicação da Deadband
    if (absInput <= m_deadband) {
        return 0.0f;
    }

    // Normalização no intervalo [0.0, 1.0] após a remoção da zona morta
    float normInput = (absInput - m_deadband) / (1.0f - m_deadband);
    normInput = std::max(0.0f, std::min(1.0f, normInput));

    // 2. Mistura Linear + Exponencial (Expo Filter):
    // Output = sign(x) * [ alpha * (normInput ^ gamma) + (1 - alpha) * normInput ]
    float expoPart = std::pow(normInput, m_gamma);
    float linearPart = normInput;

    float magnitude = (m_alpha * expoPart) + ((1.0f - m_alpha) * linearPart);

    // Preserva o sinal original da entrada
    return (input < 0.0f) ? -magnitude : magnitude;
}
