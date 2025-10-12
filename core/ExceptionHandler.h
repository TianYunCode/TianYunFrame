#pragma once

#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>

namespace ExceptionHandler
{
	static std::string g_filePath = "exception.log";

	// 辅助函数：获取当前时间字符串
	std::string getCurrentTimeString()
	{
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

		std::tm tm;

#ifdef _WIN32
		localtime_s(&tm, &time_t);
#else
		localtime_r(&time_t, &tm);
#endif

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
		oss << "." << std::setfill('0') << std::setw(3) << ms.count();

		return oss.str();
	}

	// 辅助函数：从完整路径中提取文件名
	std::string getFileNameFromPath(const std::string& path)
	{
		size_t pos = path.find_last_of("\\/");
		if (pos != std::string::npos)
		{
			return path.substr(pos + 1);
		}

		return path;
	}

#ifdef _WIN32

#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

	// Windows 异常处理器
	LONG WINAPI exceptionHandler(EXCEPTION_POINTERS* exceptionInfo)
	{
		std::ofstream logFile(g_filePath, std::ios::app);

		if (logFile.is_open())
		{
			logFile << "========================================\n";
			logFile << "Platform: Windows\n";
			logFile << getCurrentTimeString() << "\n";

			// 基本异常信息
			logFile << "Exception code: 0x" << std::hex << exceptionInfo->ExceptionRecord->ExceptionCode << std::dec << "\n";
			logFile << "Exception flags: 0x" << std::hex << exceptionInfo->ExceptionRecord->ExceptionFlags << std::dec << "\n";
			logFile << "Exception address: 0x" << std::hex << exceptionInfo->ExceptionRecord->ExceptionAddress << std::dec << "\n";

			// 异常参数
			logFile << "Number of parameters: " << exceptionInfo->ExceptionRecord->NumberParameters << "\n";
			for (DWORD i = 0; i < exceptionInfo->ExceptionRecord->NumberParameters; i++)
			{
				logFile << "Parameter[" << i << "]: 0x" << std::hex << exceptionInfo->ExceptionRecord->ExceptionInformation[i] << std::dec << "\n";
			}

			// 进程和线程信息
			logFile << "Process ID: " << GetCurrentProcessId() << "\n";
			logFile << "Thread ID: " << GetCurrentThreadId() << "\n";

			HANDLE process = GetCurrentProcess();
			HANDLE thread = GetCurrentThread();

			// 初始化符号处理
			SymInitialize(process, NULL, TRUE);
			SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

			// 配置堆栈帧
			STACKFRAME64 stackFrame;
			ZeroMemory(&stackFrame, sizeof(STACKFRAME64));

			// 根据平台设置机器类型和上下文
#ifdef _M_IX86
			DWORD machineType = IMAGE_FILE_MACHINE_I386;
			stackFrame.AddrPC.Offset = exceptionInfo->ContextRecord->Eip;
			stackFrame.AddrStack.Offset = exceptionInfo->ContextRecord->Esp;
			stackFrame.AddrFrame.Offset = exceptionInfo->ContextRecord->Ebp;
#elif _M_X64
			DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
			stackFrame.AddrPC.Offset = exceptionInfo->ContextRecord->Rip;
			stackFrame.AddrStack.Offset = exceptionInfo->ContextRecord->Rsp;
			stackFrame.AddrFrame.Offset = exceptionInfo->ContextRecord->Rbp;
#endif
			stackFrame.AddrPC.Mode = AddrModeFlat;
			stackFrame.AddrStack.Mode = AddrModeFlat;
			stackFrame.AddrFrame.Mode = AddrModeFlat;

			// 打印寄存器状态
			logFile << "\nRegister state:\n";
#ifdef _M_IX86
			logFile << "EAX: 0x" << std::hex << exceptionInfo->ContextRecord->Eax << " ";
			logFile << "EBX: 0x" << std::hex << exceptionInfo->ContextRecord->Ebx << " ";
			logFile << "ECX: 0x" << std::hex << exceptionInfo->ContextRecord->Ecx << " ";
			logFile << "EDX: 0x" << std::hex << exceptionInfo->ContextRecord->Edx << "\n";
			logFile << "ESI: 0x" << std::hex << exceptionInfo->ContextRecord->Esi << " ";
			logFile << "EDI: 0x" << std::hex << exceptionInfo->ContextRecord->Edi << " ";
			logFile << "EBP: 0x" << std::hex << exceptionInfo->ContextRecord->Ebp << " ";
			logFile << "ESP: 0x" << std::hex << exceptionInfo->ContextRecord->Esp << "\n";
#elif _M_X64
			logFile << "RAX: 0x" << std::hex << exceptionInfo->ContextRecord->Rax << " ";
			logFile << "RBX: 0x" << std::hex << exceptionInfo->ContextRecord->Rbx << " ";
			logFile << "RCX: 0x" << std::hex << exceptionInfo->ContextRecord->Rcx << " ";
			logFile << "RDX: 0x" << std::hex << exceptionInfo->ContextRecord->Rdx << "\n";
			logFile << "RSI: 0x" << std::hex << exceptionInfo->ContextRecord->Rsi << " ";
			logFile << "RDI: 0x" << std::hex << exceptionInfo->ContextRecord->Rdi << " ";
			logFile << "RBP: 0x" << std::hex << exceptionInfo->ContextRecord->Rbp << " ";
			logFile << "RSP: 0x" << std::hex << exceptionInfo->ContextRecord->Rsp << "\n";
			logFile << "R8: 0x" << std::hex << exceptionInfo->ContextRecord->R8 << " ";
			logFile << "R9: 0x" << std::hex << exceptionInfo->ContextRecord->R9 << " ";
			logFile << "R10: 0x" << std::hex << exceptionInfo->ContextRecord->R10 << " ";
			logFile << "R11: 0x" << std::hex << exceptionInfo->ContextRecord->R11 << "\n";
			logFile << "R12: 0x" << std::hex << exceptionInfo->ContextRecord->R12 << " ";
			logFile << "R13: 0x" << std::hex << exceptionInfo->ContextRecord->R13 << " ";
			logFile << "R14: 0x" << std::hex << exceptionInfo->ContextRecord->R14 << " ";
			logFile << "R15: 0x" << std::hex << exceptionInfo->ContextRecord->R15 << "\n";
#endif

			logFile << "\nStack trace:\n";
			DWORD frameNumber = 0;

			// 遍历堆栈帧
			while (StackWalk64(machineType, process, thread, &stackFrame, exceptionInfo->ContextRecord, NULL,
				SymFunctionTableAccess64, SymGetModuleBase64, NULL))
			{
				if (stackFrame.AddrPC.Offset == 0) break;

				logFile << "Frame " << frameNumber++ << ": ";
				logFile << "Address: 0x" << std::hex << stackFrame.AddrPC.Offset << std::dec;

				// 获取模块名称
				char moduleName[MAX_PATH];
				DWORD64 moduleBase = SymGetModuleBase64(process, stackFrame.AddrPC.Offset);
				if (moduleBase && GetModuleFileNameA((HINSTANCE)moduleBase, moduleName, MAX_PATH))
				{
					logFile << " Module: " << getFileNameFromPath(moduleName);
				}

				// 获取符号信息
				char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
				PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
				pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
				pSymbol->MaxNameLen = MAX_SYM_NAME;

				DWORD64 displacement = 0;
				if (SymFromAddr(process, stackFrame.AddrPC.Offset, &displacement, pSymbol))
				{
					logFile << " Function: " << pSymbol->Name;
					if (displacement) logFile << "+0x" << std::hex << displacement << std::dec;
				}

				// 获取行号信息
				IMAGEHLP_LINE64 line;
				line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
				DWORD displacement32 = 0;

				if (SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &displacement32, &line))
				{
					logFile << " File: " << line.FileName << " Line: " << line.LineNumber;
				}

				logFile << "\n";
			}

			SymCleanup(process);
			logFile << "========================================\n\n";
			logFile.close();
		}

		return EXCEPTION_EXECUTE_HANDLER;
	}

	// 设置Windows异常处理器的辅助函数
	void setupExceptionHandler(const std::string& filePath = "")
	{
		if (!filePath.empty())
		{
			g_filePath = filePath;
		}

		SetUnhandledExceptionFilter(exceptionHandler);
	}

#else // Linux 版本

#include <csignal>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>
#include <ucontext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

	// Linux 信号处理器
	void signalHandler(int sig, siginfo_t* info, void* context)
	{
		std::ofstream logFile(g_filePath, std::ios::app);

		if (logFile.is_open())
		{
			logFile << "========================================\n";
			logFile << "Platform: Linux\n";
			logFile << getCurrentTimeString() << "\n";

			// 信号信息
			logFile << "Signal: " << sig << " (" << strsignal(sig) << ")\n";
			logFile << "Signal code: " << info->si_code << "\n";
			logFile << "Fault address: 0x" << std::hex << info->si_addr << std::dec << "\n";

			// 进程和线程信息
			logFile << "Process ID: " << getpid() << "\n";
			logFile << "Parent Process ID: " << getppid() << "\n";
			logFile << "User ID: " << getuid() << "\n";

			ucontext_t* ucontext = (ucontext_t*)context;

			// 寄存器状态 (x86_64)
#ifdef __x86_64__
			logFile << "\nRegister state:\n";
			logFile << "RAX: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_RAX] << " ";
			logFile << "RBX: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_RBX] << " ";
			logFile << "RCX: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_RCX] << " ";
			logFile << "RDX: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_RDX] << "\n";
			logFile << "RSI: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_RSI] << " ";
			logFile << "RDI: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_RDI] << " ";
			logFile << "RBP: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_RBP] << " ";
			logFile << "RSP: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_RSP] << "\n";
			logFile << "R8: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_R8] << " ";
			logFile << "R9: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_R9] << " ";
			logFile << "R10: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_R10] << " ";
			logFile << "R11: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_R11] << "\n";
			logFile << "R12: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_R12] << " ";
			logFile << "R13: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_R13] << " ";
			logFile << "R14: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_R14] << " ";
			logFile << "R15: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_R15] << "\n";
			logFile << "RIP: 0x" << std::hex << ucontext->uc_mcontext.gregs[REG_RIP] << std::dec << "\n";
#endif

			logFile << "\nStack trace:\n";

			// 获取堆栈跟踪
			void* callstack[128];
			int frames = backtrace(callstack, 128);
			char** symbols = backtrace_symbols(callstack, frames);

			if (symbols)
			{
				for (int i = 0; i < frames; i++)
				{
					logFile << "Frame " << i << ": " << symbols[i];

					// 尝试解析符号名称
					Dl_info info;
					if (dladdr(callstack[i], &info) && info.dli_sname)
					{
						int status;
						char* demangled = abi::__cxa_demangle(info.dli_sname, NULL, 0, &status);
						if (demangled)
						{
							logFile << " [" << demangled << "]";
							free(demangled);
						}
						else
						{
							logFile << " [" << info.dli_sname << "]";
						}
					}
					logFile << "\n";
				}
				free(symbols);
			}

			logFile << "========================================\n\n";
			logFile.close();
		}

		// 重新抛出信号以终止程序
		signal(sig, SIG_DFL);
		raise(sig);
	}

	// 设置Linux信号处理器的辅助函数
	void setupExceptionHandler(const std::string& filePath = "")
	{
		if (!filePath.empty())
		{
			g_filePath = filePath;
		}

		struct sigaction sa;
		sa.sa_sigaction = signalHandler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_SIGINFO | SA_RESTART;

		// 设置常见崩溃信号的处理器
		sigaction(SIGSEGV, &sa, NULL);  // 段错误
		sigaction(SIGABRT, &sa, NULL);  // 中止
		sigaction(SIGFPE, &sa, NULL);   // 浮点异常
		sigaction(SIGILL, &sa, NULL);   // 非法指令
		sigaction(SIGBUS, &sa, NULL);   // 总线错误
	}

#endif

} // namespace ExceptionHandler