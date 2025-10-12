#include "DemoHead.h"

#if T_ExceptionHandlerDemo

#include "ExceptionHandler.h"

int main()
{
	ExceptionHandler::setupExceptionHandler();

	// 触发异常以测试异常处理程序
	int* p = nullptr;
	*p = 42; // 这将引发访问冲突异常

	return 0;
}

#endif