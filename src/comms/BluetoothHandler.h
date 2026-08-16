#pragma once

#include <Arduino.h>
#include <BluetoothSerial.h>
#include "config/RobotConfig.h"

/**
 * @struct RawCommandData
 * @brief Pacote descompactado com os valores de Throttle e Turn extraídos.
 */
struct RawCommandData {
    float throttle; ///< [-1.0f, 1.0f]
    float turn;     ///< [-1.0f, 1.0f]
    bool valid;     ///< Indica se o pacote foi recebido e validado com sucesso
};

/**
 * @class BluetoothHandler
 * @brief Gerenciador de conexão Bluetooth Serial Classic do ESP32 com parser de pacotes binários e ASCII.
 */
class BluetoothHandler {
public:
    BluetoothHandler();
    ~BluetoothHandler() = default;

    /**
     * @brief Inicializa o dispositivo Bluetooth Serial com o nome configurado.
     * @return true se inicializado com sucesso.
     */
    bool begin(const char* deviceName = Config::Bluetooth::DEVICE_NAME);

    /**
     * @brief Processa o buffer da Serial de forma não-bloqueante e retorna novo comando se disponível.
     * @return RawCommandData contendo o novo sinal de comando e flag de validade.
     */
    RawCommandData update();

    /**
     * @brief Verifica se há cliente Bluetooth conectado.
     */
    bool isConnected();

    /**
     * @brief Retorna o timestamp (millis) da última mensagem válida recebida.
     */
    uint32_t getLastRxTimeMs() const { return m_lastRxTimestamp; }

private:
    /**
     * @brief Parser para protocolo binário compacto de 5 bytes: [0xAA, 0x55, throttle_int8, turn_int8, checksum]
     */
    bool parseBinaryFrame(uint8_t* buffer, size_t len, float& outThrottle, float& outTurn);

    /**
     * @brief Parser fallback para strings ASCII ex: "0.5,-0.3\n" ou "T:50,R:-20\n"
     */
    bool parseAsciiFrame(const String& line, float& outThrottle, float& outTurn);

    BluetoothSerial m_serialBT;
    uint32_t m_lastRxTimestamp{0};

    // Buffer circular interno para recepção de pacotes
    static constexpr size_t BUFFER_SIZE = 64;
    uint8_t m_rxBuffer[BUFFER_SIZE];
    size_t m_rxIndex{0};
};
