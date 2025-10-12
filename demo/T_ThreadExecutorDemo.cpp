#include "DemoHead.h"

#if T_ThreadExecutorDemo

#include "ThreadExecutor.hpp"
#include <future>
#include <QDebug>

using namespace std;

int main()
{
    qDebug() << "=== ThreadExecutor 演示 ===";

    ThreadExecutor executor("DemoThread");
    executor.start();

    // 提交异步任务
    executor.post([]() { qDebug() << "异步任务1执行"; });

    // 提交同步任务
    executor.postAndWait([]() { qDebug() << "同步任务执行"; });

    // 提交带返回值的任务 - 使用 std::async 和 promise
    auto future = std::async(std::launch::async, [&executor]()
                             {
                                 int result = 0;
                                 executor.postAndWait([&result]()
                                                      {
                                                          qDebug() << "带返回值的任务执行";
                                                          result = 42;
                                                      });

                                 return result;
                             });

    qDebug() << "任务结果: " << future.get();

    executor.stop();
    qDebug() << "=== 演示结束 ===";
    return 0;
}

#endif
