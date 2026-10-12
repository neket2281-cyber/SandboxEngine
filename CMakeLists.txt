cmake_minimum_required(VERSION 4.3)
project(Dev)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_executable(Dev src/main.cpp
        src/Headers/Runtime.h
        src/Runtime/Runtime.cpp
        src/Runtime/Time.cpp
        src/Runtime/Window.cpp
        src/Headers/Entity.h
        src/Runtime/Entities.cpp
        src/Runtime/Renderer.cpp

)

target_link_libraries(Dev PRIVATE Vulkan::Vulkan)