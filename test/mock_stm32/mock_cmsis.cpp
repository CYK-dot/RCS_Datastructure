#include "mock_cmsis.hpp"
#include <iostream>
#include <map>
#include <cstdint>
#include <functional>

namespace {
    uint32_t current_irq = 0;
    std::map<uint32_t, void (*)(void)> isr_table;
}

// 用于测试的全局变量
int mock__get_IPSR_return = 0;

// --- 模拟 CMSIS ---

extern "C" {

uint32_t mock__get_IPSR(void) {
    return mock__get_IPSR_return != 0 ? mock__get_IPSR_return : current_irq;
}

void mock_trigger_interrupt(uint32_t irq_number) {
    auto it = isr_table.find(irq_number);
    if (it != isr_table.end() && it->second) {
        std::cout << "[mock] Trigger IRQ " << irq_number << std::endl;
        current_irq = irq_number;
        it->second();
        current_irq = 0;
    } else {
        std::cerr << "[mock] IRQ " << irq_number << " not registered\n";
    }
}

void mock_register_interrupt(uint32_t irq_number, void (*handler)(void)) {
    isr_table[irq_number] = handler;
}

}

// --- mock_interrupt 控制接口 ---

namespace mock_interrupt {
    void reset() {
        current_irq = 0;
        mock__get_IPSR_return = 0;
        isr_table.clear();
    }

    void set_current_exception(uint32_t irq) {
        current_irq = irq;
    }
}
