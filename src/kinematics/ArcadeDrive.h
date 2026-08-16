#pragma once

/**
 * @struct DriveOutput
 * @brief Saídas de comando de potência normalizadas [-1.0, 1.0] para os motores.
 */
struct DriveOutput {
    float leftPower;  ///< Potência do motor esquerdo [-1.0, 1.0]
    float rightPower; ///< Potência do motor direito [-1.0, 1.0]
};

/**
 * @class ArcadeDrive
 * @brief Cinemática diferencial 2WD com saturação e normalização dinâmica.
 */
class ArcadeDrive {
public:
    ArcadeDrive() = default;
    ~ArcadeDrive() = default;

    /**
     * @brief Computa a saída diferencial com normalização dinâmica.
     * @param throttle Aceleração linear (frente/ré) no intervalo [-1.0, 1.0].
     * @param turn Curva/Rotação no intervalo [-1.0, 1.0].
     * @return Estrutura DriveOutput com as potências normalizadas dos motores.
     */
    static DriveOutput calculate(float throttle, float turn);
};
