#include "comms/BluetoothHandler.h"
#include <cstdlib>

BluetoothHandler::BluetoothHandler() : m_lastRxTimestamp(0), m_rxIndex(0) {}

bool BluetoothHandler::begin(const char* deviceName) {
    m_rxIndex = 0;
    m_lastRxTimestamp = millis();
    return m_serialBT.begin(deviceName);
}

bool BluetoothHandler::isConnected() {
    return m_serialBT.hasClient();
}

RawCommandData BluetoothHandler::update() {
    RawCommandData result = {0.0f, 0.0f, false};

    while (m_serialBT.available()) {
        uint8_t b = static_cast<uint8_t>(m_serialBT.read());

        // Adiciona ao buffer circular/linear de recepção
        if (m_rxIndex < BUFFER_SIZE) {
            m_rxBuffer[m_rxIndex++] = b;
        } else {
            // Buffer overflow - desloca buffer para não travar
            memmove(m_rxBuffer, m_rxBuffer + 1, BUFFER_SIZE - 1);
            m_rxBuffer[BUFFER_SIZE - 1] = b;
        }

        // 1. Tenta encontrar e processar Pacote Binário de 5 Bytes: [0xAA, 0x55, throttle, turn, checksum]
        if (m_rxIndex >= 5) {
            for (size_t i = 0; i <= m_rxIndex - 5; ++i) {
                if (m_rxBuffer[i] == 0xAA && m_rxBuffer[i + 1] == 0x55) {
                    float th = 0.0f, tr = 0.0f;
                    if (parseBinaryFrame(&m_rxBuffer[i], 5, th, tr)) {
                        result.throttle = th;
                        result.turn = tr;
                        result.valid = true;
                        m_lastRxTimestamp = millis();

                        // Limpa os bytes consumidos do buffer
                        size_t remaining = m_rxIndex - (i + 5);
                        if (remaining > 0) {
                            memmove(m_rxBuffer, m_rxBuffer + i + 5, remaining);
                        }
                        m_rxIndex = remaining;
                        return result;
                    }
                }
            }
        }

        // 2. Tenta processar ASCII se o caractere recebido for terminação de linha '\n'
        if (b == '\n' || b == '\r') {
            if (m_rxIndex > 1) {
                m_rxBuffer[m_rxIndex - 1] = '\0'; // NUL terminate
                String strLine = String(reinterpret_cast<char*>(m_rxBuffer));
                strLine.trim();

                float th = 0.0f, tr = 0.0f;
                if (parseAsciiFrame(strLine, th, tr)) {
                    result.throttle = th;
                    result.turn = tr;
                    result.valid = true;
                    m_lastRxTimestamp = millis();
                }
            }
            m_rxIndex = 0; // Reseta buffer após nova linha
            if (result.valid) {
                return result;
            }
        }
    }

    return result;
}

bool BluetoothHandler::parseBinaryFrame(uint8_t* buffer, size_t len, float& outThrottle, float& outTurn) {
    if (len < 5) return false;

    uint8_t header1  = buffer[0];
    uint8_t header2  = buffer[1];
    int8_t rawTh     = static_cast<int8_t>(buffer[2]);
    int8_t rawTr     = static_cast<int8_t>(buffer[3]);
    uint8_t checksum = buffer[4];

    // Validação de Integridade por Checksum XOR
    uint8_t expectedCheck = header1 ^ header2 ^ static_cast<uint8_t>(rawTh) ^ static_cast<uint8_t>(rawTr);
    if (checksum != expectedCheck) {
        return false;
    }

    // Conversão de int8 [-100, 100] para float [-1.0f, 1.0f]
    outThrottle = std::max(-1.0f, std::min(1.0f, static_cast<float>(rawTh) / 100.0f));
    outTurn     = std::max(-1.0f, std::min(1.0f, static_cast<float>(rawTr) / 100.0f));

    return true;
}

bool BluetoothHandler::parseAsciiFrame(const String& line, float& outThrottle, float& outTurn) {
    if (line.length() < 3) return false;

    // Ex: "0.5,-0.2"
    int commaIndex = line.indexOf(',');
    if (commaIndex != -1) {
        String part1 = line.substring(0, commaIndex);
        String part2 = line.substring(commaIndex + 1);

        outThrottle = std::max(-1.0f, std::min(1.0f, part1.toFloat()));
        outTurn     = std::max(-1.0f, std::min(1.0f, part2.toFloat()));
        return true;
    }

    return false;
}
