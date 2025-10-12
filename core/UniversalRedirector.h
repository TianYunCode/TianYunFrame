#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <vector>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

class UniversalRedirector 
{
private:
    std::ofstream m_logFile;
    std::streambuf* m_originalCout;
    std::streambuf* m_originalCerr;
    std::streambuf* m_originalClog;

    // 自定义流缓冲区，用于同时输出到控制台和文件
    class DualBuffer : public std::streambuf 
    {
    private:
        std::streambuf* m_consoleBuf;
        std::ofstream* m_fileStream;
        std::vector<char> m_buffer;

    public:
        DualBuffer(std::streambuf* console, std::ofstream* file) : m_consoleBuf(console), m_fileStream(file), m_buffer(1024)
        {
            setp(m_buffer.data(), m_buffer.data() + m_buffer.size() - 1);
        }

        ~DualBuffer() 
        {
            sync();
        }

    protected:
        int overflow(int c) override 
        {
            if (c != EOF) 
            {
                *pptr() = c;
                pbump(1);
                if (sync() != 0)
                {
                    return EOF;
                }
            }

            return c;
        }

        int sync() override 
        {
            if (pbase() != pptr()) 
            {
                // 写入控制台
                if (m_consoleBuf)
                {
                    m_consoleBuf->sputn(pbase(), pptr() - pbase());
                    m_consoleBuf->pubsync();
                }

                // 写入文件
                if (m_fileStream && m_fileStream->is_open())
                {
                    m_fileStream->write(pbase(), pptr() - pbase());
                    m_fileStream->flush();
                }

                setp(pbase(), epptr());
            }

            return 0;
        }

        std::streamsize xsputn(const char* s, std::streamsize n) override 
        {
            // 写入控制台
            if (m_consoleBuf)
            {
                m_consoleBuf->sputn(s, n);
            }

            // 写入文件
            if (m_fileStream && m_fileStream->is_open())
            {
                m_fileStream->write(s, n);
                m_fileStream->flush();
            }

            return n;
        }
    };

    std::unique_ptr<DualBuffer> m_dualCout;
    std::unique_ptr<DualBuffer> m_dualCerr;
    std::unique_ptr<DualBuffer> m_dualClog;

    // 用于C标准流重定向
#ifdef _WIN32
    int m_originalStdoutFd;
    int m_originalStderrFd;
    FILE* m_stdoutFile;
    FILE* m_stderrFile;
#else
    int m_originalStdoutFd;
    int m_originalStderrFd;
    int m_fileFd;
#endif

    bool m_isRedirected;
    bool m_dualMode; // 是否同时输出到控制台和文件

public:
    UniversalRedirector() : m_isRedirected(false), m_dualMode(false)
    {
#ifdef _WIN32
        m_originalStdoutFd = -1;
        m_originalStderrFd = -1;
        m_stdoutFile = nullptr;
        m_stderrFile = nullptr;
#else
        m_originalStdoutFd = -1;
        m_originalStderrFd = -1;
        m_fileFd = -1;
#endif
    }

    ~UniversalRedirector() 
    {
        stopRedirect();
    }

    // 开始重定向（可选择是否同时输出到控制台）
    bool startRedirect(const std::string& filename, bool dualOutput = true) 
    {
        if (m_isRedirected)
        {
            std::cerr << "重定向已经启动" << std::endl;
            return false;
        }

		// 判断路径是否存在，如果不存在则创建
		size_t lastSlash = filename.find_last_of("/\\");
		if (lastSlash != std::string::npos)
		{
			std::string dirPath = filename.substr(0, lastSlash);
#ifdef _WIN32
			_mkdir(dirPath.c_str());
#else
			mkdir(dirPath.c_str(), 0755);
#endif
		}

        m_dualMode = dualOutput;

        // 打开日志文件
        m_logFile.open(filename, std::ios::app);
        if (!m_logFile.is_open())
        {
            std::cerr << "无法打开文件: " << filename << std::endl;
            return false;
        }

        // 保存原始缓冲区
        m_originalCout = std::cout.rdbuf();
        m_originalCerr = std::cerr.rdbuf();
        m_originalClog = std::clog.rdbuf();

        if (m_dualMode)
        {
            // 双输出模式：同时输出到控制台和文件
            m_dualCout = std::make_unique<DualBuffer>(m_originalCout, &m_logFile);
            m_dualCerr = std::make_unique<DualBuffer>(m_originalCerr, &m_logFile);
            m_dualClog = std::make_unique<DualBuffer>(m_originalClog, &m_logFile);

            std::cout.rdbuf(m_dualCout.get());
            std::cerr.rdbuf(m_dualCerr.get());
            std::clog.rdbuf(m_dualClog.get());
        }
        else 
        {
            // 单输出模式：只输出到文件
            std::cout.rdbuf(m_logFile.rdbuf());
            std::cerr.rdbuf(m_logFile.rdbuf());
            std::clog.rdbuf(m_logFile.rdbuf());
        }

        // 重定向C标准流
        if (!redirectCStreams(filename)) 
        {
            restoreCppStreams();
            return false;
        }

        // 重定向系统级文件描述符
        if (!redirectFileDescriptors(filename)) 
        {
            restoreCStreams();
            restoreCppStreams();
            return false;
        }

        m_isRedirected = true;

        if (m_dualMode)
        {
            std::cout << "=== 双输出重定向已启动 (控制台 + 文件: " << filename << ") ===" << std::endl;
        }
        else
        {
            std::cout << "=== 单输出重定向已启动 (仅文件: " << filename << ") ===" << std::endl;
        }

        return true;
    }

    // 停止重定向
    void stopRedirect()
    {
        if (m_isRedirected)
        {
            restoreCStreams();
            restoreCppStreams();

            m_isRedirected = false;
            m_dualMode = false;

            std::cout << "=== 输出重定向已停止 ===" << std::endl;
        }
    }

    // 检查是否正在重定向
    bool isRedirecting() const 
    {
        return m_isRedirected;
    }

    // 检查是否为双输出模式
    bool isDualMode() const 
    {
        return m_dualMode;
    }

    // 切换输出模式（仅在重定向时有效）
    bool toggleOutputMode() 
    {
        if (!m_isRedirected)
        {
            return false;
        }

        stopRedirect();
        return startRedirect(m_logFile.is_open() ? "temp_log.txt" : "output.log", !m_dualMode);
    }

private:
    // 重定向C标准流
    bool redirectCStreams(const std::string& filename) 
    {
        // 保存原始stdout和stderr
#ifdef _WIN32
        m_originalStdoutFd = _dup(_fileno(stdout));
        m_originalStderrFd = _dup(_fileno(stderr));
#else
        originalStdoutFd = dup(STDOUT_FILENO);
        originalStderrFd = dup(STDERR_FILENO);
#endif

        // 重定向stdout
        if (freopen(filename.c_str(), "a", stdout) == NULL) 
        {
            return false;
        }

        // 重定向stderr
        if (freopen(filename.c_str(), "a", stderr) == NULL) 
        {
            fclose(stdout);
            return false;
        }

        // 设置无缓冲，确保及时写入
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);

        return true;
    }

    // 重定向文件描述符（系统级错误）
    bool redirectFileDescriptors(const std::string& filename) 
    {
#ifdef _WIN32
        return redirectFileDescriptorsWindows(filename);
#else
        return redirectFileDescriptorsUnix(filename);
#endif
    }

#ifdef _WIN32
    bool redirectFileDescriptorsWindows(const std::string& filename) 
    {
        // 使用freopen已经处理了Windows下的重定向
        // 这里我们只需要确保文件已经打开
        m_stdoutFile = freopen(filename.c_str(), "a", stdout);
        m_stderrFile = freopen(filename.c_str(), "a", stderr);

        return (m_stdoutFile != nullptr && m_stderrFile != nullptr);
    }
#else
    bool redirectFileDescriptorsUnix(const std::string& filename) 
    {
        // 打开文件
        m_fileFd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (m_fileFd == -1)
        {
            return false;
        }

        // 重定向
        if (dup2(m_fileFd, STDOUT_FILENO) == -1 || dup2(m_fileFd, STDERR_FILENO) == -1)
        {
            close(m_fileFd);
            return false;
        }

        return true;
    }
#endif

    // 恢复C++流
    void restoreCppStreams() 
    {
        if (m_originalCout)
        {
            std::cout.rdbuf(m_originalCout);
        }

        if (m_originalCerr)
        {
            std::cerr.rdbuf(m_originalCerr);
        }

        if (m_originalClog)
        {
            std::clog.rdbuf(m_originalClog);
        }

        m_dualCout.reset();
        m_dualCerr.reset();
        m_dualClog.reset();

        if (m_logFile.is_open())
        {
            m_logFile.close();
        }
    }

    // 恢复C标准流
    void restoreCStreams() 
    {
#ifdef _WIN32
        if (m_originalStdoutFd != -1)
        {
            _dup2(m_originalStdoutFd, _fileno(stdout));
            _close(m_originalStdoutFd);
            m_originalStdoutFd = -1;
        }

        if (m_originalStderrFd != -1)
        {
            _dup2(m_originalStderrFd, _fileno(stderr));
            _close(m_originalStderrFd);
            m_originalStderrFd = -1;
        }

        if (m_stdoutFile)
        {
            fclose(stdout);
            m_stdoutFile = nullptr;
        }

        if (m_stderrFile)
        {
            fclose(stderr);
            m_stderrFile = nullptr;
        }

        // 重新打开控制台
        freopen("CON", "w", stdout);
        freopen("CON", "w", stderr);
#else
        if (m_originalStdoutFd != -1) 
        {
            dup2(m_originalStdoutFd, STDOUT_FILENO);
            close(m_originalStdoutFd);
            m_originalStdoutFd = -1;
        }

        if (m_originalStderrFd != -1) 
        {
            dup2(m_originalStderrFd, STDERR_FILENO);
            close(m_originalStderrFd);
            m_originalStderrFd = -1;
        }

        if (m_fileFd != -1) 
        {
            close(m_fileFd);
            m_fileFd = -1;
        }
#endif

        // 恢复缓冲
        setvbuf(stdout, NULL, _IOLBF, 1024);
        setvbuf(stderr, NULL, _IONBF, 0);
    }
};

// 简单的全局函数，用于快速重定向
namespace QuickRedirect 
{
    static std::unique_ptr<UniversalRedirector> s_globalRedirector;

    bool start(const std::string& filename, bool dualOutput = true) 
    {
        if (!s_globalRedirector)
        {
            s_globalRedirector = std::make_unique<UniversalRedirector>();
        }

        return s_globalRedirector->startRedirect(filename, dualOutput);
    }

    void stop() 
    {
        if (s_globalRedirector) 
        {
            s_globalRedirector->stopRedirect();
        }
    }

    bool isActive() 
    {
        return s_globalRedirector && s_globalRedirector->isRedirecting();
    }
}