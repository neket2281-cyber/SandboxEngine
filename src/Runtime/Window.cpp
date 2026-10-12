#include <iostream>
#include "../Headers/Runtime.h"

//procedure
LRESULT CALLBACK Engine::Platform::WindowProc(HWND hwnd,UINT uint,WPARAM wparam,LPARAM lParam) {
    Window* window;
    if (uint == WM_CREATE) {
        Window* window{};
        LPCREATESTRUCT createStruct = (LPCREATESTRUCT)lParam;
        window = (Window*)createStruct->lpCreateParams ;
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,(LONG_PTR)window);
        std::cout << (Window*)createStruct->lpCreateParams << std::endl;
    }
    else if (uint == WM_KEYDOWN) {
         window = (Window*)GetWindowLongPtrW(hwnd,GWLP_USERDATA);
         window->inputSystem.ProcessMessages(wparam);
    }
     return DefWindowProcW(hwnd,uint,wparam,lParam);
}
//create window
void Engine::Platform::Window::StartWindowCreation(Runtime* runtime) {
    LPCWSTR className = L"Engine";

    this->runtime = runtime;
    ws.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    ws.lpfnWndProc = WindowProc;
    ws.hInstance = GetModuleHandle(nullptr);
    ws.hCursor = LoadCursor(nullptr, IDC_ARROW);
    ws.lpszClassName = className;

    RegisterClassW(&ws);
    handle = CreateWindowExW(0, ws.lpszClassName,nullptr,WS_OVERLAPPEDWINDOW,960,540,480,290,nullptr,nullptr, ws.hInstance,this);
    ShowWindow(handle,SW_SHOW);
    std::cout << this << std::endl;
}
//process messages
void Engine::Platform::ProcessMessages() {
    MSG msg{};
    if (PeekMessage(&msg, nullptr,0,0,PM_REMOVE) != 1) return ;
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
}


// ---- INPUT ----
void Engine::Platform::InputSystem::ProcessMessages(WPARAM wParam) {
switch ((WPARAM)wParam)
    case VK_ESCAPE:End();

}

void Engine::Platform::InputSystem::End() {
    window->runtime->Terminate();
}