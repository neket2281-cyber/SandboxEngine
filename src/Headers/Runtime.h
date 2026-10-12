#include <chrono>
#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>
#include <Windows.h>
#include <vulkan/vulkan_win32.h>


#pragma once
#ifndef DEV_RUNTIME_H
#define DEV_RUNTIME_H
namespace Engine {
    class Runtime;

    namespace EngineObjects {
        class Entity;
    }

    namespace Renderer {
        class Renderer;
    }


    // ---------------
    namespace Platform {
        class Window;

        class InputSystem {
            Window* window;
        public:
            InputSystem(Window* window) {this->window = window;}

            void ProcessMessages(WPARAM wParam);
            void End();
        };

        // ---------------

        class Window {
        public:
            HWND handle{nullptr};

            //constructor
            WNDCLASSW ws{};

            //helpers
            InputSystem inputSystem{this};

            Engine::Runtime* runtime;

            void SetHandle(HWND handle) {this->handle = handle;}
            HWND GetHandle() {return handle;}
            void StartWindowCreation(Runtime* runtime);
        };
        // ---------------
        LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);
        void ProcessMessages();
    }

    // ---------------

    namespace Renderer {
        class Renderer {

            VkApplicationInfo vkApplicationInfo
            {
                VK_STRUCTURE_TYPE_APPLICATION_INFO,
                nullptr,
                "SandboxEngine",
                VK_MAKE_VERSION(0, 1, 0),
                "SandboxEngine",
                VK_MAKE_VERSION(0, 1, 0),
                VK_API_VERSION_1_4
            };

            VkInstanceCreateInfo vkInstanceInfo
            {
                VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                nullptr,
                0,
                nullptr, //will configure later
                0,
                nullptr,
                2,
                surfaceExtensions,
            };

            // ---- arrays ----
            std::vector<VkLayerProperties> layerProperties;
            std::vector<VkPhysicalDevice> physicalDevices;
            std::vector<VkPhysicalDeviceProperties> physDevProperties;
            std::vector<std::vector<VkQueueFamilyProperties>> queueFamilyPropertiesArray;
            std::vector<VkQueueFamilyProperties> queueFamilyProperties;

            //extensions

            const char* surfaceExtensions[2] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
            const char* deviceExtensions[1]{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

            uint32_t layerCount{0};
            uint32_t physicalDeviceCount{0};
            uint32_t queueFamilyPropertyCount{0};


            VkInstance instance{};

            uint32_t queueFamilyIndex{0};
            VkBool32 supportsSurfaceKHR{};

            VkDisplaySurfaceCreateFlagsKHR surfaceCreateFlags{};
            VkWin32SurfaceCreateInfoKHR surfaceInfo{};
            VkSurfaceKHR surface{};

            VkPhysicalDeviceProperties* deviceProperties{};
            VkPhysicalDevice* vkPhysicalDevice{};
            VkQueueFamilyProperties* vkSuitableQueueFamilyProperties{};


            VkPhysicalDeviceFeatures vkSuitableDeviceFeatures{};
            VkDeviceCreateInfo vkDeviceCreateInfo{};
            VkDeviceQueueCreateInfo queueCreateInfo{};
            float queuePriorities{1.0f};

            VkDevice device{};
            VkQueue queue{};
            VkSurfaceCapabilitiesKHR capabilities{};

            uint_fast32_t surfaceFormatCount;
            std::vector<VkSurfaceFormatKHR> surfaceFormats;

            VkSwapchainCreateInfoKHR swapchainInfo{
                VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                nullptr,
                0,
            };

            uint_fast32_t presentModeCount{};
            std::vector<VkPresentModeKHR> presentModes{};

            VkSwapchainKHR swapchain{};

            uint32_t swapchainImageCount;
            std::vector<VkImage> swapchainImages;


            std::vector<VkImageViewCreateInfo> imageViewInfo;
            std::vector<VkImageView> imageView;

            VkAttachmentDescription attachmentDescription{
                0,
                VK_FORMAT_B8G8R8A8_SRGB,
                VK_SAMPLE_COUNT_1_BIT,
                VK_ATTACHMENT_LOAD_OP_CLEAR,
                VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            };

            VkAttachmentReference attachmentReference{
                0,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            };

            VkSubpassDescription subPassDescription{
                0,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                0,
                0,
                1,
                &attachmentReference,
                nullptr,
                nullptr,
                0,
                0
            };

            VkSubpassDependency subpassDependency{
                VK_SUBPASS_EXTERNAL,
                0,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                0
            };

            VkRenderPassCreateInfo renderPassCreateInfo
            {
                VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                0,
                0,
                1,
                &attachmentDescription,
                1,
                &subPassDescription,
                1,
                &subpassDependency
            };

            VkRenderPass renderPass{};

            std::vector<VkFramebufferCreateInfo> framebufferCreateInfos{};

            std::vector<VkFramebuffer> framebuffers;

            VkCommandPoolCreateInfo commandPoolCreateInfo{
                VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                nullptr,
                0,
            };

            VkCommandPool commandPool;

            VkCommandBufferAllocateInfo commandBufferAllocateInfo{};

            std::vector<VkCommandBuffer> commandBuffers;

            std::ifstream triangleVert;
            std::ifstream triangleFrag;

            std::vector<uint32_t> triangleVertCode;
            std::vector<uint32_t> triangleFragCode;


            VkShaderModuleCreateInfo shaderVertModuleCreateInfo{
                VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                nullptr,
                0,
            };

            VkShaderModuleCreateInfo shaderFragModuleCreateInfo{
                VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                nullptr,
                0,
            };

            VkShaderModule shaderVertModule{};
            VkShaderModule shaderFragModule{};

            VkPipelineShaderStageCreateInfo piplineShaderInfoVert{
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                nullptr,
                0,
                VK_SHADER_STAGE_VERTEX_BIT,
            };

            VkPipelineShaderStageCreateInfo piplineShaderInfoFrag{
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                nullptr,
                0,
                VK_SHADER_STAGE_FRAGMENT_BIT,
            };

            VkPipelineShaderStageCreateInfo piplineShaderStageInfos[2];

            VkPipelineVertexInputStateCreateInfo vertexInputState {
                VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                nullptr,
                0,
                0,
                nullptr,
                0,
                nullptr,
            };

            VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
VK_FALSE
            };

            VkPipelineTessellationStateCreateInfo pTessellationState{
                VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
                nullptr,
                0,

            };

            VkViewport viewport{};
            VkRect2D scissor{};

            VkPipelineViewportStateCreateInfo viewportStateInfo{
                VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                nullptr,
                0,
                1,
            };

            VkPipelineRasterizationStateCreateInfo rasterizationInfo{
                VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_FALSE,
                VK_FALSE,
                VK_POLYGON_MODE_FILL,
                VK_CULL_MODE_NONE,
                VK_FRONT_FACE_COUNTER_CLOCKWISE,
                VK_FALSE,
                0.0f,
                0.0f,
                0.0f,
                1.0f
            };

            VkPipelineMultisampleStateCreateInfo multisampleState{
                VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_SAMPLE_COUNT_1_BIT,
                VK_FALSE,
                0.0f,
                nullptr,
                VK_FALSE,
                VK_FALSE
            };

            VkPipelineColorBlendAttachmentState colorBlendAttachmentState{
                VK_FALSE,
                VK_BLEND_FACTOR_ONE,
                VK_BLEND_FACTOR_ZERO,
                VK_BLEND_OP_ADD,
                VK_BLEND_FACTOR_ONE,
                VK_BLEND_FACTOR_ZERO,
                VK_BLEND_OP_ADD,
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
            };

            VkPipelineColorBlendStateCreateInfo colorBlendState{
                VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_FALSE,
                VK_LOGIC_OP_COPY,
                1,
                &colorBlendAttachmentState,
                0.0f
            };

            VkPipelineLayout pipelineLayout{};

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
                VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                nullptr,
                0,
                0,
                nullptr,
                0,
                nullptr
            };

            VkPipeline graphicsPipeline {VK_NULL_HANDLE};


            VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo {
                VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                nullptr,
                0,
                2,
            };

            VkCommandBufferBeginInfo commandBufferBeginInfo{
                VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                nullptr,
                0,
                nullptr
            };

            VkClearValue clearColorValue{};

            VkRenderPassBeginInfo renderPassBeginInfo{
                VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                nullptr
            };

            VkSemaphoreCreateInfo semaphoreCreateInfoWait{
                VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                nullptr,
                0,
            };

            VkSemaphoreCreateInfo semaphoreCreateInfoSignal{
                VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                nullptr,
                0,
            };

            VkSemaphore semaphoreWait{};
            VkSemaphore semaphoreSignal{};

            VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

            VkSubmitInfo submitInfo{
                VK_STRUCTURE_TYPE_SUBMIT_INFO,
                nullptr
            };

            VkFence fence{};
            VkFenceCreateInfo fenceCreateInfo{
                VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                nullptr,
                0
            };

            uint32_t imageIndex;

            VkPresentInfoKHR presentInfo{
                VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                nullptr,
                1,
            };

        public:
            void Start(HINSTANCE* hinstance, HWND* hwnd);
        };
    }

    // ---------------

    class TimeCalculation{
    private:
        std::chrono::steady_clock::time_point t0{std::chrono::steady_clock::now()};
        std::chrono::steady_clock::time_point t1{};

        std::chrono::steady_clock::time_point t0t{std::chrono::steady_clock::now()};
        std::chrono::steady_clock::time_point t1t{std::chrono::steady_clock::now()};

        float time;
        float delta;
    public:
        float CalculateDeltaTime();
        float CalculateTime();
    };

    // ---------------

    template<typename T>
    concept isAnEntity = std::is_base_of_v<EngineObjects::Entity, T>;

    class Runtime {
    private:
        HINSTANCE hinstance{nullptr};

        TimeCalculation timeCalculation;
        bool isRunning{true};
        Platform::Window window;
        Renderer::Renderer renderer;

        float updateDelay{0.016};
        float delta{};
        float deltaTime{};

        std::vector<std::unique_ptr<EngineObjects::Entity>> entities;

        //Main cycle
    public:


        void Run();

        void Start();

        void Update();

        void Terminate();

        void End();
        // Helpers

        template<isAnEntity T>
        void CreateEntity() {
            entities.push_back(std::make_unique<T>());
        }
    };

    // ---------------

    namespace EngineObjects {
        class Entity {
        private:
            struct Position {
                float x{0};
                float y{0};
                float z{0};
            };
            struct Rotation {
                float x{0};
                float y{0};
                float z{0};
            };

            class Transform {
            public:
                Position position;
                Rotation rotation;

            };
        public:
            //lifetime
            bool isMarkedForDestruction{false};

            //classes
            Transform transform;

            //virtual methods;
            virtual void Update(float){};
            virtual ~Entity(){};
            void Destroy() { isMarkedForDestruction = true; };
        };
    }
}
#endif