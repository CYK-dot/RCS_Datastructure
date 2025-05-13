#pragma once

#include <cstdint>
#include <functional>

// 可用于模拟的句柄类型
using MockTaskHandle_t = void*;
using MockSemaphoreHandle_t = void*;

#ifdef __cplusplus
extern "C" {
#endif

// 模拟任务创建
int xTaskCreate(void (*taskFunc)(void*), const char* name, uint16_t stackSize, void* param, int priority, MockTaskHandle_t* outHandle);

// 模拟信号量等待
int xSemaphoreTake(MockSemaphoreHandle_t sem, uint32_t timeoutTicks);

// 模拟信号量释放
int xSemaphoreGive(MockSemaphoreHandle_t sem);

// 模拟进入临界区
void vPortEnterCritical(void);

// 模拟退出临界区
void vPortExitCritical(void);

#ifdef __cplusplus
}
#endif

// 模拟环境注入点
namespace freertos_mock {
    void reset();

    // 可自定义任务创建行为（如失败、记录调用等）
    extern std::function<int(void (*)(void*), const char*, uint16_t, void*, int, MockTaskHandle_t*)> onTaskCreate;

    // 可模拟信号量行为
    extern std::function<int(MockSemaphoreHandle_t, uint32_t)> onSemaphoreTake;
    extern std::function<int(MockSemaphoreHandle_t)> onSemaphoreGive;

    // 模拟临界区嵌套计数器（可选）
    extern int criticalNesting;
}
