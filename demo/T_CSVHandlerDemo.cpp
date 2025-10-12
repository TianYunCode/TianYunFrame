#include "DemoHead.h"

#if T_CsvDemo

#include <iostream>
#include "CSVHandler.h"

int main(int argc, char** argv)
{
    // 创建 CSV 处理器实例
    CSVHandler csv;

    // 设置表头
    csv.setHeaders({"Name", "Age", "City", "Salary"});

    // 添加单行数据（支持不同类型）
    csv.addRow("John Doe", 30, "New York", 50000.50);
    csv.addRow("Jane Smith", 28, "Los Angeles", 60000.75);
    csv.addRow("Bob Johnson", "35", "Chicago", 55000.25);  // 年龄作为字符串

    // 添加多行数据
    std::vector<std::vector<std::string>> additionalRows =
        {
            {"Alice Brown", "32", "Houston", "48000.80"},
            {"Charlie Wilson", "45", "Phoenix", "70000.00"}
        };
    csv.addRows(additionalRows);

    // 输出到控制台
    std::cout << "CSV content:" << std::endl;
    std::cout << csv << std::endl;

    // 写入文件
    csv.writeToFile("example.csv");
    std::cout << "CSV written to file: example.csv" << std::endl;

    // 读取文件
    CSVHandler csvReader(',', true);  // 指定有表头
    if (csvReader.readFromFile("example.csv"))
    {
        std::cout << "CSV read from file:" << std::endl;
        std::cout << csvReader << std::endl;

        // 获取特定列
        try
        {
            auto ageColumn = csvReader.getColumn("Age");
            std::cout << "Age column: ";
            for (const auto& age : ageColumn)
            {
                std::cout << age << " ";
            }
            std::cout << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        // 排序示例
        csvReader.sortByColumn("Age", false);  // 按年龄降序排序
        std::cout << "Sorted by Age (descending):" << std::endl;
        std::cout << csvReader << std::endl;
    }
    else
    {
        std::cerr << "Failed to read file." << std::endl;
    }

    // 修改数据示例
    csv.setCell(0, 1, "31");  // 将第一行第二列（Age）改为31
    std::cout << "After modifying cell (0,1):" << std::endl;
    std::cout << csv << std::endl;

    // 删除列示例
    csv.removeColumn("Salary");
    std::cout << "After removing Salary column:" << std::endl;
    std::cout << csv << std::endl;

    return 0;
}

#endif
