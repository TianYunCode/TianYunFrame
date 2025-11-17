# TianYunFrame/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

#TianYunFrame模块项目
project(TianYunFrame LANGUAGES C CXX)

# 添加rpc子模块
add_subdirectory(rpc)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动包含当前目录的MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(WIN32)
    # Windows 特定设置
    add_definitions(-D_WIN32_WINNT=0x0601)
else()
    # Linux/macOS 特定设置
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -g")
endif()

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# 设置包含目录
set(TianYunFrame_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/rpc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/rpc/thirdparty/asio
    ${CMAKE_CURRENT_SOURCE_DIR}/rpc/thirdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/demo
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/config/cJSON
    ${CMAKE_CURRENT_SOURCE_DIR}/csv
    ${CMAKE_CURRENT_SOURCE_DIR}/help
    ${CMAKE_CURRENT_SOURCE_DIR}/logger
    ${CMAKE_CURRENT_SOURCE_DIR}/memory
    ${CMAKE_CURRENT_SOURCE_DIR}/signal
    ${CMAKE_CURRENT_SOURCE_DIR}/thread
    ${CMAKE_CURRENT_SOURCE_DIR}/event
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/common
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/JsonStructConvert
)

# 头文件
set(TianYunFrame_HEADERS
    config/ConfigManager.hpp
    config/cJSON/cJSON.h

    core/Singleton.hpp
    core/TimeCounter.h
    core/Random.h
    core/FileSystem.h 
    core/MD5.h
    core/UniversalRedirector.h
    core/ExceptionHandler.h
    core/Utility.h
    core/GenericSmartPointer.h
    core/RingBufferManager.h

    csv/CSVHandler.h

    demo/DemoHead.h
    demo/T_BorderlessStretchWindowTest.h
    demo/T_FlatUITest.h
    demo/T_LedIndicatorTest.h
    demo/T_ListWidgetTest.h
    demo/T_MacAddressEditTest.h
    demo/T_MaskWidgetTest.h

    event/EventBus.hpp

    help/QtHelp.h
    help/ScreenUtils.h
    help/StaticTool.h

    logger/Logger.h
    logger/LoggerBase.h
    logger/EasyLogger.h

    memory/MemoryPool.hpp
    memory/ObjectPool.hpp
    memory/PooledSharedPtr.hpp

    signal/Connection.hpp
    signal/Object.h
    signal/Signal.hpp

    thread/ThreadExecutor.hpp
    thread/ThreadPool.hpp
    thread/ThreadSafeQueue.hpp

    ui/AnimatedCheckBox.h
    ui/BorderlessStretchWindow.h
    ui/ImageSwitch.h
    ui/LedIndicator.h
    ui/ListWidget.h
    ui/LoadLabel.h
    ui/LoginRegisterWindowOne.h
    ui/MacAddressEdit.h
    ui/PushButton.h
    ui/MaskWidget.h 
    ui/RefreshButton.h

    ui/common/EventFilterObject.h
    ui/common/FlatUI.h
)

# 源文件
set(TianYunFrame_SOURCES
    config/cJSON/cJSON.c
    
    demo/T_AnimatedCheckBoxDemo.cpp
    demo/T_BorderlessStretchWindowDemo.cpp
    demo/T_CSVHandlerDemo.cpp
    demo/T_ConfigManagerDemo.cpp
    demo/T_EventBusDemo.cpp
    demo/T_FlatUIDemo.cpp
    demo/T_ImageSwitchDemo.cpp
    demo/T_JsonStructConvertDemo.cpp
    demo/T_LedIndicatorDemo.cpp
    demo/T_ListWidgetDemo.cpp
    demo/T_LoadLabelDemo.cpp
    demo/T_LoggerDemo.cpp
    demo/T_LoginRegisterWindowOneDemo.cpp
    demo/T_MacAddressEditDemo.cpp
    demo/T_MemoryDemo.cpp
    demo/T_PushButtonDemo.cpp
    demo/T_SignalDemo.cpp
    demo/T_ThreadExecutorDemo.cpp
    demo/T_ThreadPoolDemo.cpp
    demo/T_ThreadSafeQueueDemo.cpp
    demo/T_MaskWidgetDemo.cpp
    demo/T_RandomDemo.cpp
    demo/T_FileSystemDemo.cpp
    demo/T_CJSONDemo.cpp
    demo/T_RefreshButtonDemo.cpp
    demo/T_MD5Demo.cpp
    demo/T_UniversalRedirectorDemo.cpp
    demo/T_ExceptionHandlerDemo.cpp

    core/FileSystem.cpp
    core/MD5.cpp
    core/Utility.cpp

    logger/EasyLogger.cpp

    help/QtHelp.cpp
    help/StaticTool.cpp

    ui/AnimatedCheckBox.cpp
    ui/BorderlessStretchWindow.cpp
    ui/ImageSwitch.cpp
    ui/LedIndicator.cpp
    ui/ListWidget.cpp
    ui/LoadLabel.cpp
    ui/LoginRegisterWindowOne.cpp
    ui/MacAddressEdit.cpp
    ui/MaskWidget.cpp
    ui/RefreshButton.cpp

    ui/common/EventFilterObject.cpp
    ui/common/FlatUI.cpp
)

# 资源文件
set(TianYunFrame_RESOURCES
    ui/res/res.qrc
)

# 创建TianYunFrame静态库
add_library(TianYunFrame STATIC
    ${TianYunFrame_HEADERS}
    ${TianYunFrame_SOURCES}
    ${TianYunFrame_RESOURCES}
)

# 设置包含目录
target_include_directories(TianYunFrame PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TianYunFrame_INCLUDE_DIRS}
)

# 链接Qt库
target_link_libraries(TianYunFrame PUBLIC 
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

########################################################################################
# 异常处理库路径
if(WIN32)
    target_link_libraries(TianYunFrame PUBLIC DbgHelp.lib)
endif()
########################################################################################

# 设置输出目录
set_target_properties(TianYunFrame PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
