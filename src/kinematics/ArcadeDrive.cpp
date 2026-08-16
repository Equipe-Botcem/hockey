#include "kinematics/ArcadeDrive.h"
#include <cmath>
#include <algorithm>

DriveOutput ArcadeDrive::calculate(float throttle, float turn) {
    // Clamping de segurança para garantir que throttle e turn estão em [-1.0, 1.0]
    throttle = std::max(-1.0f, std::min(1.0f, throttle));
    turn     = std::max(-1.0f, std::min(1.0f, turn));

    // Mistura de canais Arcade Drive:
    // Left_raw  = Throttle + Turn
    // Right_raw = Throttle - Turn
    float leftRaw  = throttle + turn;
    float rightRaw = throttle - turn;

    // Normalização Dinâmica:
    // Determina a maior magnitude absoluta entre o lado esquerdo e direito
    float maxMagnitude = std::max(std::abs(leftRaw), std::abs(rightRaw));

    DriveOutput output;

    // Se o valor absoluto ultrapassar 1.0, dividimos ambos pelo valor máximo.
    // Isso evita o 'clipping' em um motor e preserva a autoridade de curva em alta aceleração.
    if (maxMagnitude > 1.0f) {
        output.leftPower  = leftRaw / maxMagnitude;
        output.rightPower = rightRaw / maxMagnitude;
    } else {
        output.leftPower  = leftRaw;
        output.rightPower = rightRaw;
    }

    return output;
}
