#include "DemoHead.h"

#if T_MD5Demo

#include <iostream>
#include <string>
#include "MD5.h"

using namespace std;

int main() 
{
    // 示例1: 计算字符串的MD5
    string str1 = "Hello, World!";
    string str2 = "The quick brown fox jumps over the lazy dog";

    MD5 md5_1(str1);
    MD5 md5_2(str2);

    cout << "MD5 demo:" << endl;
    cout << "==========" << endl;
    cout << "String: \"" << str1 << "\"" << endl;
    cout << "MD5: " << md5_1.toString() << endl;
    cout << endl;

    cout << "String: \"" << str2 << "\"" << endl;
    cout << "MD5: " << md5_2.toString() << endl;
    cout << endl;

    // 示例2: 分步更新计算MD5
    MD5 md5_3;
    md5_3.update("Hello, ");
    md5_3.update("World!");
    cout << "Incremental update:" << endl;
    cout << "MD5: " << md5_3.toString() << endl;
    cout << endl;

    // 示例3: 验证已知的MD5值
    string test_str = "";
    MD5 md5_empty(test_str);
    cout << "Empty string MD5: " << md5_empty.toString() << endl;
    cout << "Expected: d41d8cd98f00b204e9800998ecf8427e" << endl;
    cout << (md5_empty.toString() == "d41d8cd98f00b204e9800998ecf8427e" ? "✓ Correct" : "✗ Wrong") << endl;
    cout << endl;

    // 示例4: 重置MD5计算器
    MD5 md5_reset;
    md5_reset.update("First data");
    cout << "After first update: " << md5_reset.toString() << endl;

    md5_reset.reset();
    md5_reset.update("Second data");
    cout << "After reset and second update: " << md5_reset.toString() << endl;

    return 0;
}

#endif