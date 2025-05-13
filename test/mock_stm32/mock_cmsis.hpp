#pragma once

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

// 模拟 CMSIS 的函数：获取当前中断号（0 = Thread mode）
uint32_t mock__get_IPSR(void);

// 模拟中断触发函数（通过中断号）
void mock_trigger_interrupt(uint32_t irq_number);

// 注册中断服务程序
void mock_register_interrupt(uint32_t irq_number, void (*handler)(void));

#ifdef __cplusplus
}
#endif

// 提供清除/设置函数（在测试中使用）
namespace mock_interrupt {
    void reset();
    void set_current_exception(uint32_t irq);
}
