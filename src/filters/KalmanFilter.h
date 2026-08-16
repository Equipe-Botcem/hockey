#pragma once

/**
 * @class KalmanFilter
 * @brief Filtro de Kalman 1D (Unidimensional) para fusão/filtragem de sensores ou estimativa de estado.
 */
class KalmanFilter {
public:
    KalmanFilter(float q = 0.01f, float r = 0.1f, float p = 1.0f, float initialValue = 0.0f);
    ~KalmanFilter() = default;

    /**
     * @brief Atualiza a estimativa de estado com base na nova medição.
     * @param measurement Nova leitura do sensor/sinal.
     * @return Estado estimado filtrado.
     */
    float update(float measurement);

    void reset(float initialValue = 0.0f);

    void setParameters(float processNoiseQ, float measurementNoiseR);

private:
    float m_q; ///< Ruído do processo
    float m_r; ///< Ruído da medição
    float m_p; ///< Covariância da estimativa
    float m_k; ///< Ganho de Kalman
    float m_x; ///< Estado estimado
};
