#include "DemoHead.h"

#if T_ImGUIDemo

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL.h>
#include <stdio.h>

// 添加字体加载函数
bool LoadChineseFont(ImGuiIO& io, float font_size = 18.0f)
{
    // 尝试加载系统中常见的中文字体
    const char* font_paths[] = 
    {
        "C:/Windows/Fonts/simhei.ttf",      // Windows 黑体
        "C:/Windows/Fonts/msyh.ttc",        // Windows 微软雅黑
        "C:/Windows/Fonts/simkai.ttf",      // Windows 楷体
        "/System/Library/Fonts/PingFang.ttc", // macOS 苹方
        "/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf", // Linux
        "/usr/share/fonts/truetype/wqy/wqy-microhei.ttc", // Linux 文泉驿
        "simhei.ttf",                       // 当前目录下的字体
        nullptr
    };

    ImFont* font = nullptr;
    for (int i = 0; font_paths[i] != nullptr; i++)
    {
        font = io.Fonts->AddFontFromFileTTF(font_paths[i], font_size, nullptr,
            io.Fonts->GetGlyphRangesChineseFull());
        if (font != nullptr)
        {
            printf("成功加载字体: %s\n", font_paths[i]);
            return true;
        }
    }

    // 如果系统字体都找不到，使用默认字体并添加中文字符
    font = io.Fonts->AddFontDefault(nullptr);
    if (font != nullptr)
    {
        printf("使用默认字体并添加中文字符集\n");
        return true;
    }

    printf("警告: 无法加载中文字体，中文将显示为问号\n");
    return false;
}

int main(int, char**)
{
    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("错误: %s\n", SDL_GetError());
        return -1;
    }

    // 创建 SDL 窗口
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui 中文示例", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr)
    {
        printf("错误: 创建窗口失败 %s\n", SDL_GetError());
        return -1;
    }

    // 创建 SDL_Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        printf("错误: 创建渲染器失败 %s\n", SDL_GetError());
        return -1;
    }

    // 设置 ImGui 上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // 设置 ImGui 样式
    ImGui::StyleColorsDark();

    // 加载中文字体
    if (!LoadChineseFont(io, 18.0f))
    {
        // 如果加载失败，使用默认字体
        io.Fonts->AddFontDefault();
    }

    // 设置平台/渲染器后端
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // 应用程序状态变量
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    float slider_float = 0.0f;
    int counter = 0;
    char text_input[128] = "你好，ImGui!";

    // 自定义样式变量
    float custom_size = 1.0f;
    bool custom_toggle = false;
    int custom_combo = 0;
    float custom_color[3] = { 1.0f, 0.0f, 0.0f };

    // 主循环标志
    bool done = false;

    // 主循环
    while (!done)
    {
        // 处理事件
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
                done = true;
            }

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            {
                done = true;
            }
        }

        // 开始新帧
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. 显示 ImGui 演示窗口
        if (show_demo_window)
        {
            ImGui::Begin("ImGui 演示窗口", &show_demo_window);
            ImGui::Text("这是一个包含中文的演示窗口");
            ImGui::Separator();
            ImGui::ShowDemoWindow(&show_demo_window);
            ImGui::End();
        }

        // 2. 显示主控制窗口
        {
            ImGui::Begin("控制面板");                          // 创建窗口

            ImGui::Text("这是一个 Dear ImGui + SDL2 的中文示例程序");
            ImGui::Text("现在应该可以正常显示中文了！");
            ImGui::Separator();

            // 窗口控制
            ImGui::Checkbox("显示演示窗口", &show_demo_window);
            ImGui::Checkbox("显示另一个窗口", &show_another_window);

            ImGui::Separator();

            // 各种控件示例
            if (ImGui::Button("点击我!"))
            {
                counter++;
            }

            ImGui::SameLine();
            ImGui::Text("按钮点击次数: %d", counter);

            ImGui::SliderFloat("浮点滑块", &slider_float, 0.0f, 100.0f, "%.1f");
            ImGui::ColorEdit3("背景颜色", (float*)&clear_color);

            ImGui::InputText("文本输入", text_input, IM_ARRAYSIZE(text_input));

            ImGui::SliderFloat("自定义大小", &custom_size, 0.5f, 2.0f);
            ImGui::Checkbox("自定义开关", &custom_toggle);

            const char* combo_items[] = { "选项一", "选项二", "选项三" };
            ImGui::Combo("组合框", &custom_combo, combo_items, IM_ARRAYSIZE(combo_items));

            ImGui::ColorEdit3("自定义颜色", custom_color);

            ImGui::Separator();
            ImGui::Text("应用程序性能");
            ImGui::Text("平均 %.3f 毫秒/帧 (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }

        // 3. 显示自定义样式窗口
        if (show_another_window)
        {
            ImGui::Begin("自定义样式预览", &show_another_window);

            ImGui::Text("这里展示了一些自定义样式的控件");
            ImGui::Text("所有中文文本都应该正常显示");
            ImGui::Separator();

            // 使用自定义变量
            ImGui::Text("大小缩放: %.2f", custom_size);
            ImGui::Text("开关状态: %s", custom_toggle ? "开启" : "关闭");
            ImGui::Text("选择项: %d", custom_combo + 1);
            ImGui::Text("颜色值: (%.2f, %.2f, %.2f)", custom_color[0], custom_color[1], custom_color[2]);

            // 进度条示例
            static float progress = 0.0f;
            ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
            progress += 0.001f;
            if (progress >= 1.0f)
            {
                progress = 0.0f;
            }

            if (ImGui::Button("重置进度"))
            {
                progress = 0.0f;
            }

            ImGui::End();
        }

        // 4. 渲染
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // 清理
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#endif