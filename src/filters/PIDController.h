#pragma once

/**
 * @class PIDController
 * @brief Controlador PID discreto com Anti-Windup e Filtro Derivativo.
 * 
 * Módulo preparado para malha fechada de velocidade/posição dos motores N20 com encoders.
 */
class PIDController {
public:
    PIDController(float kp = 1.0f, float ki = 0.0f, float kd = 0.0f, 
                  float outMin = -1.0f, float outMax = 1.0f);
    
    ~PIDController() = default;

    /**
     * @brief Computa a saída do controlador PID.
     * @param setpoint Valor desejado.
     * @param measurement Valor medido pelo sensor.
     * @param dt Delta de tempo em segundos.
     * @return Sinal de controle calculado.
     */
    float compute(float setpoint, float measurement, float dt);

    void reset();

    void setGains(float kp, float ki, float kd);
    void setOutputLimits(float minVal, float maxVal);

private:
    float m_kp;
    float m_ki;
    float m_kd;

    float m_outMin;
    float m_outMax;

    float m_integral{0.0f};
    float m_prevError{0.0f};
    bool  m_firstRun{true};
};
