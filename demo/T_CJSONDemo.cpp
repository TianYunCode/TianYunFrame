#include "DemoHead.h"

#if T_CJSONDemo

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <QDebug>

// 创建JSON示例
void create_json_example() 
{
    qDebug("=== 创建JSON示例 ===");

    // 创建根对象
    cJSON* root = cJSON_CreateObject();

    // 添加基本类型
    cJSON_AddStringToObject(root, "name", "张三");
    cJSON_AddNumberToObject(root, "age", 25);
    cJSON_AddBoolToObject(root, "is_student", 1);
    cJSON_AddNullToObject(root, "null_field");

    // 创建数组
    cJSON* hobbies = cJSON_CreateArray();
    cJSON_AddItemToArray(hobbies, cJSON_CreateString("游泳"));
    cJSON_AddItemToArray(hobbies, cJSON_CreateString("阅读"));
    cJSON_AddItemToArray(hobbies, cJSON_CreateString("编程"));
    cJSON_AddItemToObject(root, "hobbies", hobbies);

    // 创建嵌套对象
    cJSON* address = cJSON_CreateObject();
    cJSON_AddStringToObject(address, "city", "北京");
    cJSON_AddStringToObject(address, "street", "朝阳区");
    cJSON_AddNumberToObject(address, "zipcode", 100000);
    cJSON_AddItemToObject(root, "address", address);

    // 转换为字符串并打印
    char* json_str = cJSON_Print(root);
    qDebug("创建的JSON:\n%s", json_str);

    // 清理内存
    free(json_str);
    cJSON_Delete(root);
}

// 解析JSON示例
void parse_json_example() 
{
    qDebug("\n=== 解析JSON示例 ===");

    const char* json_string =
        "{"
        "\"name\": \"李四\","
        "\"age\": 30,"
        "\"scores\": [85, 92, 78],"
        "\"profile\": {"
        "    \"height\": 175.5,"
        "    \"weight\": 70.2"
        "},"
        "\"graduated\": true"
        "}";

    // 解析JSON字符串
    cJSON* root = cJSON_Parse(json_string);
    if (root == NULL)
    {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) 
        {
            fprintf(stderr, "解析错误位置: %s", error_ptr);
        }
        return;
    }

    // 提取值
    cJSON* name = cJSON_GetObjectItem(root, "name");
    cJSON* age = cJSON_GetObjectItem(root, "age");
    cJSON* scores = cJSON_GetObjectItem(root, "scores");
    cJSON* profile = cJSON_GetObjectItem(root, "profile");
    cJSON* graduated = cJSON_GetObjectItem(root, "graduated");

    qDebug("解析结果:");
    qDebug("姓名: %s", cJSON_GetStringValue(name));
    qDebug("年龄: %d", age->valueint);
    qDebug("是否毕业: %s", cJSON_IsTrue(graduated) ? "是" : "否");

    // 解析嵌套对象
    if (profile != NULL)
    {
        cJSON* height = cJSON_GetObjectItem(profile, "height");
        cJSON* weight = cJSON_GetObjectItem(profile, "weight");
        qDebug("身高: %.1f cm", height->valuedouble);
        qDebug("体重: %.1f kg", weight->valuedouble);
    }

    // 解析数组
    if (scores != NULL && cJSON_IsArray(scores)) 
    {
        qDebug("分数: ");
        cJSON* score_item = NULL;
        cJSON_ArrayForEach(score_item, scores)
        {
            qDebug("%d ", score_item->valueint);
        }
        qDebug("\n");
    }

    cJSON_Delete(root);
}

// 修改JSON示例
void modify_json_example()
{
    qDebug("\n=== 修改JSON示例 ===");

    const char* original_json =
        "{"
        "\"product\": \"手机\","
        "\"price\": 2999,"
        "\"in_stock\": true"
        "}";

    cJSON* root = cJSON_Parse(original_json);

    qDebug("原始JSON:\n%s", cJSON_Print(root));

    // 修改现有值
    cJSON* price = cJSON_GetObjectItem(root, "price");
    if (price != NULL) 
    {
        cJSON_SetNumberValue(price, 2599); // 降价
    }

    // 添加新字段
    cJSON_AddStringToObject(root, "brand", "华为");

    // 删除字段
    cJSON_DeleteItemFromObject(root, "in_stock");

    qDebug("修改后的JSON:\n%s", cJSON_Print(root));

    cJSON_Delete(root);
}

// 错误处理示例
void error_handling_example() 
{
    qDebug("\n=== 错误处理示例 ===");

    // 无效的JSON字符串
    const char* invalid_json = "{ \"name\": \"王五\", \"age\": }";

    cJSON* root = cJSON_Parse(invalid_json);
    if (root == NULL) 
    {
        const char* error_ptr = cJSON_GetErrorPtr();
        qDebug("解析失败! 错误位置: %s", error_ptr);

        // 获取更详细的错误信息
        qDebug("错误上下文: ");
        if (error_ptr != NULL)
        {
            // 打印错误位置附近的字符
            const char* start = error_ptr - 10;
            if (start < invalid_json) start = invalid_json;
            qDebug("...%.20s...", start);
        }
    }
    else
    {
        qDebug("解析成功!");
        cJSON_Delete(root);
    }
}

// 文件操作示例
void file_operations_example()
{
    qDebug("\n=== 文件操作示例 ===");

    // 创建要保存的JSON数据
    cJSON* config = cJSON_CreateObject();
    cJSON_AddStringToObject(config, "version", "1.0.0");
    cJSON_AddNumberToObject(config, "timeout", 30);

    cJSON* database = cJSON_CreateObject();
    cJSON_AddStringToObject(database, "host", "localhost");
    cJSON_AddNumberToObject(database, "port", 3306);
    cJSON_AddItemToObject(config, "database", database);

    // 保存到文件
    char* json_str = cJSON_Print(config);
    FILE* file = fopen("config.json", "w");
    if (file)
    {
        fputs(json_str, file);
        fclose(file);
        qDebug("配置已保存到 config.json");
    }

    free(json_str);
    cJSON_Delete(config);

    // 从文件读取
    file = fopen("config.json", "r");
    if (file) 
    {
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* file_content = (char*)malloc(file_size + 1);
        fread(file_content, 1, file_size, file);
        file_content[file_size] = '\0';
        fclose(file);

        cJSON* loaded_config = cJSON_Parse(file_content);
        if (loaded_config) 
        {
            qDebug("从文件加载的配置:\n%s", cJSON_Print(loaded_config));
            cJSON_Delete(loaded_config);
        }
        free(file_content);

        // 删除测试文件
        remove("config.json");
    }
}

// 实用工具函数示例
void utility_functions_example() 
{
    qDebug("\n=== 实用工具函数示例 ===");

    // 创建测试数据
    cJSON* data1 = cJSON_Parse("{\"a\": 1, \"b\": \"hello\", \"c\": [1,2,3]}");
    cJSON* data2 = cJSON_Parse("{\"a\": 1, \"b\": \"hello\", \"c\": [1,2,3]}");
    cJSON* data3 = cJSON_Parse("{\"a\": 2, \"b\": \"world\", \"c\": [1,2,3]}");

    // 比较JSON对象
    qDebug("data1 和 data2 是否相等: %s", cJSON_Compare(data1, data2, 1) ? "是" : "否");
    qDebug("data1 和 data3 是否相等: %s", cJSON_Compare(data1, data3, 1) ? "是" : "否");

    // 复制JSON对象
    cJSON* data1_copy = cJSON_Duplicate(data1, 1);
    qDebug("复制的数据:\n%s", cJSON_Print(data1_copy));

    // 压缩JSON（移除空白字符）
    char* minified = cJSON_PrintUnformatted(data1);
    qDebug("压缩后的JSON: %s", minified);

    // 清理
    free(minified);
    cJSON_Delete(data1);
    cJSON_Delete(data2);
    cJSON_Delete(data3);
    cJSON_Delete(data1_copy);
}

int main()
{
    qDebug("cJSON 完整示例程序");
    qDebug("cJSON 版本: %s\n", cJSON_Version());

    create_json_example();
    parse_json_example();
    modify_json_example();
    error_handling_example();
    file_operations_example();
    utility_functions_example();

    qDebug("\n=== 程序结束 ===");
    return 0;
}

#endif