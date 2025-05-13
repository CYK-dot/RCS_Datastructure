#include "mock_freertos.hpp"
#include <iostream>

// 默认行为（可在测试中用 gmock 重载）
std::function<int(void (*)(void*), const char*, uint16_t, void*, int, MockTaskHandle_t*)>
    freertos_mock::onTaskCreate = [](auto, auto, auto, auto, auto, auto) {
        std::cout << "[mock] xTaskCreate called\n";
        return 1; // pdPASS
    };

std::function<int(MockSemaphoreHandle_t, uint32_t)>
    freertos_mock::onSemaphoreTake = [](auto, auto) {
        std::cout << "[mock] xSemaphoreTake called\n";
        return 1; // pdTRUE
    };

std::function<int(MockSemaphoreHandle_t)>
    freertos_mock::onSemaphoreGive = [](auto) {
        std::cout << "[mock] xSemaphoreGive called\n";
        return 1; // pdTRUE
    };

int freertos_mock::criticalNesting = 0;
int mock_exit_critical_count = 0;

void freertos_mock::reset() {
    criticalNesting = 0;
    mock_exit_critical_count = 0;
}

// --- FreeRTOS API 模拟实现 ---

extern "C" {

int xTaskCreate(void (*taskFunc)(void*), const char* name, uint16_t stackSize, void* param, int priority, MockTaskHandle_t* outHandle) {
    return freertos_mock::onTaskCreate(taskFunc, name, stackSize, param, priority, outHandle);
}

int xSemaphoreTake(MockSemaphoreHandle_t sem, uint32_t timeoutTicks) {
    return freertos_mock::onSemaphoreTake(sem, timeoutTicks);
}

int xSemaphoreGive(MockSemaphoreHandle_t sem) {
    return freertos_mock::onSemaphoreGive(sem);
}

void vPortEnterCritical(void) {
    freertos_mock::criticalNesting++;
    std::cout << "[mock] Enter critical (nesting = " << freertos_mock::criticalNesting << ")\n";
}

void vPortExitCritical(void) {
    freertos_mock::criticalNesting--;
    mock_exit_critical_count++;
    std::cout << "[mock] Exit critical (nesting = " << freertos_mock::criticalNesting << ")\n";
}

}
