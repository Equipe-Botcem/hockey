#include "filters/PIDController.h"
#include <algorithm>

PIDController::PIDController(float kp, float ki, float kd, float outMin, float outMax)
    : m_kp(kp), m_ki(ki), m_kd(kd), m_outMin(outMin), m_outMax(outMax) {}

float PIDController::compute(float setpoint, float measurement, float dt) {
    if (dt <= 0.0f) return 0.0f;

    float error = setpoint - measurement;

    // Termo Proporcional
    float pTerm = m_kp * error;

    // Termo Integral com Anti-Windup (clamping simples)
    m_integral += error * dt;
    float iTerm = m_ki * m_integral;
    iTerm = std::max(m_outMin, std::min(m_outMax, iTerm));

    // Termo Derivativo
    float dTerm = 0.0f;
    if (!m_firstRun) {
        dTerm = m_kd * ((error - m_prevError) / dt);
    } else {
        m_firstRun = false;
    }

    m_prevError = error;

    // Saída total clamped
    float output = pTerm + iTerm + dTerm;
    return std::max(m_outMin, std::min(m_outMax, output));
}

void PIDController::reset() {
    m_integral = 0.0f;
    m_prevError = 0.0f;
    m_firstRun = true;
}

void PIDController::setGains(float kp, float ki, float kd) {
    m_kp = kp;
    m_ki = ki;
    m_kd = kd;
}

void PIDController::setOutputLimits(float minVal, float maxVal) {
    m_outMin = minVal;
    m_outMax = maxVal;
}
