#include "DemoHead.h"

#if T_EventBusDemo

// EventBus 单独演示
#include "EventBus.hpp"
#include <QDebug>

using namespace std;

// 定义事件类型
struct TestEvent
{
    int value;
    string message;
};

int main()
{
    qDebug() << "=== EventBus 演示 ===";

    EventBus bus;

    // 订阅事件
    auto subscriber = bus.subscribe<TestEvent>([](const TestEvent& event) { qDebug() << "收到事件: value=" << event.value << ", message=" << event.message; });

    // 发布事件
    bus.publish(TestEvent{42, "Hello EventBus"});
    bus.publish(TestEvent{100, "Another event"});

    qDebug() << "=== 演示结束 ===";
    return 0;
}

#endif
