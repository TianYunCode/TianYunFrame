#include "DemoHead.h"

#if T_ThreadSafeQueueDemo

#include "ThreadSafeQueue.hpp"
#include <thread>
#include <QDebug>

using namespace std;

int main()
{
    qDebug() << "=== ThreadSafeQueue 演示 ===";

    ThreadSafeQueue<int> queue(5); // 容量为5的有界队列

    // 生产者线程
    thread producer([&queue]()
                    {
                        for (int i = 0; i < 10; ++i)
                        {
                            if (queue.push(i))
                            {
                                qDebug() << "生产: " << i;
                            }
                            else
                            {
                                qDebug() << "队列已满，无法生产: " << i;
                            }

                            this_thread::sleep_for(chrono::milliseconds(100));
                        }
                    });

    // 消费者线程
    thread consumer([&queue]()
                    {
                        for (int i = 0; i < 10; ++i)
                        {
                            auto item = queue.wait_pop(chrono::seconds(1));
                            if (item)
                            {
                                qDebug() << "消费: " << *item;
                            }
                            else
                            {
                                qDebug() << "消费超时";
                            }

                            this_thread::sleep_for(chrono::milliseconds(200));
                        }
                    });

    producer.join();
    consumer.join();

    qDebug() << "=== 演示结束 ===";
    return 0;
}

#endif
