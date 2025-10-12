#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <type_traits>

class CSVHandler
{
private:
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> data;
    char delimiter;
    bool hasHeaders;

    // 将任何类型转换为字符串
    template<typename T>
    std::string toString(const T& value) const
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            return value;
        }
        else if constexpr (std::is_same_v<T, const char*>)
        {
            return std::string(value);
        }
        else
        {
            return std::to_string(value);
        }
    }

    // 处理 CSV 字段中的特殊字符（引号、分隔符等）
    std::string processField(const std::string& field) const
    {
        bool needsQuotes = false;

        // 检查字段中是否包含特殊字符
        if (field.find(delimiter) != std::string::npos || field.find('\"') != std::string::npos || field.find('\n') != std::string::npos || field.find('\r') != std::string::npos)
        {
            needsQuotes = true;
        }

        // 如果字段需要引号，则转义内部的引号
        if (needsQuotes)
        {
            std::string processed;
            processed.reserve(field.length() + 2);
            processed += '\"';

            for (char c : field)
            {
                if (c == '\"')
                {
                    processed += "\"\"";
                }
                else
                {
                    processed += c;
                }
            }

            processed += '\"';
            return processed;
        }

        return field;
    }

    // 解析一行 CSV 数据
    std::vector<std::string> parseLine(const std::string& line) const
    {
        std::vector<std::string> result;
        std::string field;
        bool inQuotes = false;
        bool quoteInQuotes = false;

        for (char c : line)
        {
            if (c == '\"')
            {
                if (inQuotes)
                {
                    if (quoteInQuotes)
                    {
                        field += '\"';
                        quoteInQuotes = false;
                    }
                    else
                    {
                        quoteInQuotes = true;
                    }
                }
                else
                {
                    inQuotes = true;
                }
            }
            else if (c == delimiter && !inQuotes)
            {
                result.push_back(field);
                field.clear();
                quoteInQuotes = false;
            }
            else
            {
                if (quoteInQuotes)
                {
                    field += '\"';
                    quoteInQuotes = false;
                }
                field += c;
            }
        }

        if (quoteInQuotes)
        {
            field += '\"';
        }

        result.push_back(field);
        return result;
    }

public:
    // 构造函数
    explicit CSVHandler(char delim = ',', bool hasHeaders = false) : delimiter(delim), hasHeaders(hasHeaders) {}

    // 设置表头
    void setHeaders(const std::vector<std::string>& newHeaders)
    {
        headers = newHeaders;
        hasHeaders = true;
    }

    // 获取表头
    std::vector<std::string> getHeaders() const
    {
        if (!hasHeaders)
        {
            throw std::runtime_error("CSV file does not have headers");
        }

        return headers;
    }

    // 添加单行数据
    template<typename... Args>
    void addRow(Args... args)
    {
        std::vector<std::string> row;
        (row.push_back(toString(args)), ...);
        data.push_back(row);
    }

    // 添加多行数据
    void addRows(const std::vector<std::vector<std::string>>& rows)
    {
        for (const auto& row : rows)
        {
            data.push_back(row);
        }
    }

    // 获取行数
    size_t getRowCount() const
    {
        return data.size();
    }

    // 获取列数
    size_t getColCount() const
    {
        if (hasHeaders && !headers.empty())
        {
            return headers.size();
        }

        if (!data.empty())
        {
            return data[0].size();
        }

        return 0;
    }

    // 获取指定单元格的数据
    std::string getCell(size_t row, size_t col) const
    {
        if (row >= data.size() || col >= data[row].size())
        {
            throw std::out_of_range("Cell index out of range");
        }

        return data[row][col];
    }

    // 设置指定单元格的数据
    void setCell(size_t row, size_t col, const std::string& value)
    {
        if (row >= data.size() || col >= data[row].size()) {
            throw std::out_of_range("Cell index out of range");
        }

        data[row][col] = value;
    }

    // 读取 CSV 文件
    bool readFromFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        data.clear();
        headers.clear();

        std::string line;
        bool firstLine = true;

        while (std::getline(file, line))
        {
            // 处理 Windows 行尾
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

            auto row = parseLine(line);

            if (firstLine && hasHeaders)
            {
                headers = row;
                firstLine = false;
            }
            else
            {
                data.push_back(row);
                firstLine = false;
            }
        }

        file.close();
        return true;
    }

    // 写入 CSV 文件
    bool writeToFile(const std::string& filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        // 写入表头
        if (hasHeaders && !headers.empty())
        {
            for (size_t i = 0; i < headers.size(); ++i)
            {
                file << processField(headers[i]);
                if (i < headers.size() - 1)
                {
                    file << delimiter;
                }
            }

            file << "\n";
        }

        // 写入数据
        for (const auto& row : data)
        {
            for (size_t i = 0; i < row.size(); ++i)
            {
                file << processField(row[i]);
                if (i < row.size() - 1)
                {
                    file << delimiter;
                }
            }

            file << "\n";
        }

        file.close();
        return true;
    }

    // 清空数据
    void clear()
    {
        headers.clear();
        data.clear();
        hasHeaders = false;
    }

    // 设置分隔符
    void setDelimiter(char delim)
    {
        delimiter = delim;
    }

    // 获取分隔符
    char getDelimiter() const
    {
        return delimiter;
    }

    // 检查是否有表头
    bool hasHeadersSet() const
    {
        return hasHeaders;
    }

    // 获取所有数据
    std::vector<std::vector<std::string>> getAllData() const
    {
        return data;
    }

    // 获取指定行的数据
    std::vector<std::string> getRow(size_t index) const
    {
        if (index >= data.size())
        {
            throw std::out_of_range("Row index out of range");
        }

        return data[index];
    }

    // 获取指定列的数据
    std::vector<std::string> getColumn(size_t index) const
    {
        std::vector<std::string> column;
        for (const auto& row : data)
        {
            if (index < row.size())
            {
                column.push_back(row[index]);
            }
            else
            {
                throw std::out_of_range("Column index out of range in one or more rows");
            }
        }

        return column;
    }

    // 获取指定列的数据（通过列名）
    std::vector<std::string> getColumn(const std::string& columnName) const
    {
        if (!hasHeaders)
        {
            throw std::runtime_error("CSV file does not have headers");
        }

        auto it = std::find(headers.begin(), headers.end(), columnName);
        if (it == headers.end())
        {
            throw std::runtime_error("Column name not found in headers");
        }

        size_t index = std::distance(headers.begin(), it);
        return getColumn(index);
    }

    // 删除指定行
    void removeRow(size_t index)
    {
        if (index >= data.size())
        {
            throw std::out_of_range("Row index out of range");
        }

        data.erase(data.begin() + index);
    }

    // 删除指定列
    void removeColumn(size_t index)
    {
        if (hasHeaders && index < headers.size())
        {
            headers.erase(headers.begin() + index);
        }

        for (auto& row : data)
        {
            if (index < row.size())
            {
                row.erase(row.begin() + index);
            }
            else
            {
                throw std::out_of_range("Column index out of range in one or more rows");
            }
        }
    }

    // 删除指定列（通过列名）
    void removeColumn(const std::string& columnName)
    {
        if (!hasHeaders)
        {
            throw std::runtime_error("CSV file does not have headers");
        }

        auto it = std::find(headers.begin(), headers.end(), columnName);
        if (it == headers.end())
        {
            throw std::runtime_error("Column name not found in headers");
        }

        size_t index = std::distance(headers.begin(), it);
        removeColumn(index);
    }

    // 重命名列
    void renameColumn(const std::string& oldName, const std::string& newName)
    {
        if (!hasHeaders)
        {
            throw std::runtime_error("CSV file does not have headers");
        }

        auto it = std::find(headers.begin(), headers.end(), oldName);
        if (it == headers.end())
        {
            throw std::runtime_error("Column name not found in headers");
        }

        *it = newName;
    }

    // 排序（按指定列）
    void sortByColumn(size_t index, bool ascending = true)
    {
        if (data.empty()) return;

        if (index >= data[0].size())
        {
            throw std::out_of_range("Column index out of range");
        }

        std::sort(data.begin(), data.end(),
                  [index, ascending](const std::vector<std::string>& a, const std::vector<std::string>& b)
                  {
                      return ascending ? a[index] < b[index] : a[index] > b[index];
                  });
    }

    // 排序（按指定列名）
    void sortByColumn(const std::string& columnName, bool ascending = true)
    {
        if (!hasHeaders)
        {
            throw std::runtime_error("CSV file does not have headers");
        }

        auto it = std::find(headers.begin(), headers.end(), columnName);
        if (it == headers.end())
        {
            throw std::runtime_error("Column name not found in headers");
        }

        size_t index = std::distance(headers.begin(), it);
        sortByColumn(index, ascending);
    }

    // 重载输出运算符，便于调试
    friend std::ostream& operator<<(std::ostream& os, const CSVHandler& csv)
    {
        // 输出表头
        if (csv.hasHeaders && !csv.headers.empty())
        {
            for (size_t i = 0; i < csv.headers.size(); ++i)
            {
                os << csv.headers[i];
                if (i < csv.headers.size() - 1)
                {
                    os << csv.delimiter;
                }
            }

            os << "\n";
        }

        // 输出数据
        for (const auto& row : csv.data)
        {
            for (size_t i = 0; i < row.size(); ++i)
            {
                os << row[i];
                if (i < row.size() - 1)
                {
                    os << csv.delimiter;
                }
            }

            os << "\n";
        }

        return os;
    }
};
