#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "rtos/SharedData.h"
#include "drivers/MotorDriverTB6612.h"
#include "config/RobotConfig.h"

/**
 * @file MotorTask.h
 * @brief Tarefa de alta prioridade executada no Core 0 exclusivamente para controle de atuadores e Failsafe.
 */

/**
 * @brief Função da tarefa FreeRTOS para o laço dos motores no Core 0.
 * @param pvParameters Ponteiro para parâmetros (instância do MotorDriverTB6612 ou nullptr).
 */
void vMotorTask(void* pvParameters);
