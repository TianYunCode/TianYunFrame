#include "DemoHead.h"

#if T_SignalDemo

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "Signal.hpp"

using namespace std;

// 自定义数据类
class Data
{
public:
    int value;
    string name;

    Data(int v = 0, const string& n = "") : value(v), name(n) {}

    friend ostream& operator<<(ostream& os, const Data& data)
    {
        os << "Data{value=" << data.value << ", name='" << data.name << "'}";
        return os;
    }
};

// 测试类1 - 继承自Object以使用线程池功能
class Receiver : public Object
{
public:
    // 槽函数1 - 无参数
    void slotNoArgs()
    {
        cout << "Receiver::slotNoArgs() called in thread: " << this_thread::get_id() << endl;
    }

    // 槽函数2 - 带int参数
    void slotInt(int value)
    {
        cout << "Receiver::slotInt(" << value << ") called in thread: " << this_thread::get_id() << endl;
    }

    // 槽函数3 - 带多个参数
    void slotMultiArgs(int a, double b, const string& c)
    {
        cout << "Receiver::slotMultiArgs(" << a << ", " << b << ", '" << c << "') called in thread: " << this_thread::get_id() << endl;
    }

    // 槽函数4 - 带自定义类型参数
    void slotCustomType(const Data& data)
    {
        cout << "Receiver::slotCustomType(" << data << ") called in thread: " << this_thread::get_id() << endl;
    }
};

// 测试类2 - 另一个接收者
class AnotherReceiver : public Object
{
public:
    void slotString(const string& message)
    {
        cout << "AnotherReceiver::slotString('" << message << "') called in thread: " << this_thread::get_id() << endl;
    }
};

// 自由函数
void freeFunction(int x, double y, string z)
{
    cout << "freeFunction(" << x << ", " << std::to_string(y) << ", " << z <<") called in thread: " << this_thread::get_id() << endl;
}

int main(int argc, char** argv)
{
    cout << "Main thread: " << this_thread::get_id() << endl;

    // 创建信号
    Signal<> signalNoArgs;
    Signal<int> signalInt;
    Signal<int, double, string> signalMultiArgs;
    Signal<Data> signalCustomType;
    Signal<string> signalString;

    // 创建接收者对象
    Receiver receiver1, receiver2;
    AnotherReceiver anotherReceiver;

    // 演示各种连接方式

    // 1. 连接成员函数
    auto conn1 = signalNoArgs.connect(&receiver1, SLOT(Receiver::slotNoArgs));
    auto conn2 = signalInt.connect(&receiver1, SLOT(Receiver::slotInt));
    auto conn3 = signalMultiArgs.connect(&receiver1, SLOT(Receiver::slotMultiArgs));
    auto conn4 = signalCustomType.connect(&receiver1, SLOT(Receiver::slotCustomType));

    // 2. 连接另一个对象的成员函数
    auto conn5 = signalString.connect(&anotherReceiver, SLOT(AnotherReceiver::slotString));

    // 3. 连接自由函数
    auto conn6 = signalMultiArgs.connect(&freeFunction);

    // 4. 连接Lambda表达式
    auto conn7 = signalInt.connect([](int value)
                                   {
                                       cout << "Lambda slot(" << value << ") called in thread: " << this_thread::get_id() << endl;
                                   });

    // 5. 使用全局connect函数连接
    auto conn8 = connect(signalString, [](const string& msg)
                         {
                             cout << "Global connect lambda: '" << msg << "' in thread: " << this_thread::get_id() << endl;
                         });

    cout << "\n=== Simultaneous emission of signals ===\n";

    // 同步发射信号
    signalNoArgs.emit();
    signalInt.emit(42);
    signalMultiArgs.emit(10, 3.14, "Hello");

    Data data(100, "Test");
    signalCustomType.emit(data);
    signalString.emit("Sync message");

    // 等待一下，让异步调用有机会执行
    this_thread::sleep_for(chrono::milliseconds(100));

    cout << "\n=== asynchronous emission signals ===\n";

    // 异步发射信号
    signalNoArgs.emitAsync();
    signalInt.emitAsync(99);
    signalMultiArgs.emitAsync(20, 2.71, "World");

    Data data2(200, "Async");
    signalCustomType.emitAsync(data2);
    signalString.emitAsync("Async message");

    // 等待异步任务完成
    this_thread::sleep_for(chrono::milliseconds(500));

    cout << "\n=== Disconnect Demo ===\n";

    // 断开一个连接
    cout << "Connection 1 is valid: " << conn1.isConnected() << endl;
    conn1.disconnect();
    cout << "Disconnection connection 1 is valid: " << conn1.isConnected() << endl;

    // 尝试发射信号，已断开的连接不会被调用
    signalNoArgs.emit();

    cout << "\n=== Thread Pool Demo ===\n";

    // 创建专用线程池
    ThreadPool customPool(4);

    // 设置接收者使用自定义线程池
    receiver2.setThreadPool(&customPool);

    // 连接到使用自定义线程池的接收者
    Signal<int> signalThreadPool;
    auto conn9 = signalThreadPool.connect(&receiver2, SLOT(Receiver::slotInt));

    // 发射多个信号，它们将在自定义线程池中执行
    for (int i = 0; i < 10; ++i)
    {
        signalThreadPool.emitAsync(i);
    }

    // 等待所有任务完成
    this_thread::sleep_for(chrono::milliseconds(1000));

    cout << "\n=== Connection Counting Demo ===\n";

    cout << "signalInt connect number: " << signalInt.getConnectionCount() << endl;
    cout << "signalNoArgs connect number: " << signalNoArgs.getConnectionCount() << endl;

    // 断开所有连接
    conn2.disconnect();
    conn3.disconnect();
    conn4.disconnect();
    conn5.disconnect();
    conn6.disconnect();
    conn7.disconnect();
    conn8.disconnect();
    conn9.disconnect();

    cout << "disconnect signalInt connect number: " << signalInt.getConnectionCount() << endl;
    cout << "disconnect signalNoArgs connect number: " << signalNoArgs.getConnectionCount() << endl;

    cout << "\n=== End of demo ===\n";

    return 0;
}

#endif
