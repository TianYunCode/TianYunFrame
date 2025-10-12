#include "DemoHead.h"

#if T_UniversalRedirectorDemo

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "UniversalRedirector.hpp"

// 测试各种输出方式
void testAllOutputMethods()
{
    std::cout << "1. C++ cout 输出" << std::endl;
    std::cerr << "2. C++ cerr 输出（错误流）" << std::endl;
    std::clog << "3. C++ clog 输出（日志流）" << std::endl;

    printf("4. C printf 输出\n");
    fprintf(stdout, "5. C fprintf stdout 输出\n");
    fprintf(stderr, "6. C fprintf stderr 输出（错误）\n");

    // 模拟一些延迟输出
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "进度: " << (i + 1) << "/3" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // 系统调用
#ifdef _WIN32
    system("echo 7. Windows 系统命令输出");
#else
    system("echo '7. Linux/macOS 系统命令输出'");
#endif
}

int main()
{
    std::cout << "=== 跨平台输出重定向测试 ===" << std::endl;

    // 测试1: 双输出模式（同时输出到控制台和文件）
    std::cout << "\n1. 测试双输出模式..." << std::endl;
    UniversalRedirector redirector1;
    if (redirector1.startRedirect("redirect/dual_output.log", true))
    {
        testAllOutputMethods();
        redirector1.stopRedirect();
        std::cout << "双输出模式测试完成！" << std::endl;
    }

    // 测试2: 单输出模式（仅输出到文件）
    std::cout << "\n2. 测试单输出模式..." << std::endl;
    UniversalRedirector redirector2;
    if (redirector2.startRedirect("redirect/single_output.log", false))
    {
        testAllOutputMethods();
        std::cout << "这段输出不会显示在控制台上，只会写入文件" << std::endl;
        redirector2.stopRedirect();
        std::cout << "单输出模式测试完成！请查看 single_output.log 文件" << std::endl;
    }

    // 测试3: 使用全局快速重定向
    std::cout << "\n3. 测试全局快速重定向..." << std::endl;
    if (QuickRedirect::start("redirect/quick_redirect.log", true))
    {
        std::cout << "使用全局快速重定向..." << std::endl;
        testAllOutputMethods();
        QuickRedirect::stop();
        std::cout << "全局快速重定向测试完成！" << std::endl;
    }

    // 测试4: 错误和异常情况
    std::cout << "\n4. 测试错误处理..." << std::endl;
    UniversalRedirector redirector3;

    // 测试无效文件名
    if (!redirector3.startRedirect("/invalid/path/log.txt"))
    {
        std::cout << "正确处理了无效路径错误" << std::endl;
    }

    // 测试重复启动重定向
    if (redirector3.startRedirect("redirect/test.log"))
    {
        if (!redirector3.startRedirect("redirect/another.log"))
        {
            std::cout << "正确处理了重复启动错误" << std::endl;
        }
        redirector3.stopRedirect();
    }

    std::cout << "\n=== 所有测试完成 ===" << std::endl;
    return 0;
}

#endif