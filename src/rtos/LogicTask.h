#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "rtos/SharedData.h"
#include "comms/BluetoothHandler.h"
#include "filters/InputFilter.h"
#include "kinematics/ArcadeDrive.h"
#include "config/RobotConfig.h"

/**
 * @file LogicTask.h
 * @brief Tarefas executadas no Core 1 para comunicação Bluetooth e processamento matemático de sinal.
 */

/**
 * @brief Tarefa de recepção de dados Bluetooth no Core 1.
 */
void vCommTask(void* pvParameters);

/**
 * @brief Tarefa de aplicação dos filtros e cálculo da cinemática no Core 1.
 */
void vLogicTask(void* pvParameters);
