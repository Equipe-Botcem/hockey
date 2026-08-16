#include "filters/KalmanFilter.h"

KalmanFilter::KalmanFilter(float q, float r, float p, float initialValue)
    : m_q(q), m_r(r), m_p(p), m_k(0.0f), m_x(initialValue) {}

float KalmanFilter::update(float measurement) {
    // Etapa de Predição
    m_p = m_p + m_q;

    // Etapa de Atualização (Ganho de Kalman)
    m_k = m_p / (m_p + m_r);
    m_x = m_x + m_k * (measurement - m_x);
    m_p = (1.0f - m_k) * m_p;

    return m_x;
}

void KalmanFilter::reset(float initialValue) {
    m_x = initialValue;
    m_p = 1.0f;
    m_k = 0.0f;
}

void KalmanFilter::setParameters(float processNoiseQ, float measurementNoiseR) {
    m_q = processNoiseQ;
    m_r = measurementNoiseR;
}
