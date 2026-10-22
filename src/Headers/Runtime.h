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
            /*
            ========================================================
              1. INSTANCE AND SURFACE
            ========================================================
            */


            const char* surfaceExtensions[2] = {
                VK_KHR_SURFACE_EXTENSION_NAME,
                VK_KHR_WIN32_SURFACE_EXTENSION_NAME
            };

            VkApplicationInfo vkApplicationInfo{
                VK_STRUCTURE_TYPE_APPLICATION_INFO,
                nullptr,
                "SandboxEngine",
                VK_MAKE_VERSION(0, 1, 0),
                "SandboxEngine",
                VK_MAKE_VERSION(0, 1, 0),
                VK_API_VERSION_1_4
            };

            VkInstanceCreateInfo vkInstanceInfo{
                VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                nullptr,
                0,
                nullptr,
                0,
                nullptr,
                2,
                surfaceExtensions
            };

            VkInstance instance{};

            VkDisplaySurfaceCreateFlagsKHR surfaceCreateFlags{};
            VkWin32SurfaceCreateInfoKHR surfaceInfo{};
            VkSurfaceKHR surface{};


            /*
            ========================================================
              2. PHYSICAL DEVICE AND LOGICAL DEVICE
            ========================================================
            */

            // Discovery data
            std::vector<VkLayerProperties> layerProperties;
            std::vector<VkPhysicalDevice> physicalDevices;
            std::vector<VkPhysicalDeviceProperties2> physDevProperties;
            std::vector<VkPhysicalDeviceDriverProperties> driverProperties;

            std::vector<std::vector<VkQueueFamilyProperties>>
                queueFamilyPropertiesArray;
            std::vector<std::string> deviceNames;

            std::vector<VkQueueFamilyProperties>
                queueFamilyProperties;

            // Device selection
            uint32_t layerCount{};
            uint32_t physicalDeviceCount{};
            uint32_t queueFamilyPropertyCount{};
            uint32_t queueFamilyIndex{};
            uint32_t queueIndex{};

            VkBool32 supportsSurfaceKHR{};

            VkPhysicalDeviceProperties2* deviceProperties{};
            VkPhysicalDevice* vkPhysicalDevice{};
            VkQueueFamilyProperties* vkSuitableQueueFamilyProperties{};
            VkPhysicalDeviceDriverProperties* selectedDeviceDriverProperties{};

            VkPhysicalDeviceFeatures vkSuitableDeviceFeatures{};

            // Logical device and queue
            const char* deviceExtensions[1]{
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };

            VkDeviceCreateInfo vkDeviceCreateInfo{};
            VkDeviceQueueCreateInfo queueCreateInfo{};
            float queuePriorities{1.0f};

            VkDevice device{};
            VkQueue queue{};


            /*
            ========================================================
              3. SWAPCHAIN
            ========================================================
            */

            VkSurfaceCapabilitiesKHR capabilities{};

            uint_fast32_t surfaceFormatCount{};
            std::vector<VkSurfaceFormatKHR> surfaceFormats;

            uint_fast32_t presentModeCount{};
            std::vector<VkPresentModeKHR> presentModes;

            VkSwapchainCreateInfoKHR swapchainInfo{
                VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                nullptr,
                0
            };

            VkSwapchainKHR swapchain{};

            uint32_t swapchainImageCount{};
            std::vector<VkImage> swapchainImages;

            std::vector<VkImageViewCreateInfo> imageViewInfo;
            std::vector<VkImageView> imageView;


            /*
            ========================================================
              4. RENDER PASS AND FRAMEBUFFERS
            ========================================================
            */

            VkAttachmentDescription attachmentDescription{
                0,
                VK_FORMAT_B8G8R8A8_SRGB,
                VK_SAMPLE_COUNT_1_BIT,
                VK_ATTACHMENT_LOAD_OP_CLEAR,
                VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
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

            VkRenderPassCreateInfo renderPassCreateInfo{
                VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                nullptr,
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


            /*
            ========================================================
              5. SHADERS
            ========================================================
            */

            // Shader files and compiled SPIR-V
            std::ifstream triangleVert;
            std::ifstream triangleFrag;

            std::vector<uint32_t> triangleVertCode;
            std::vector<uint32_t> triangleFragCode;

            // Shader module creation
            VkShaderModuleCreateInfo shaderVertModuleCreateInfo{
                VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                nullptr,
                0
            };

            VkShaderModuleCreateInfo shaderFragModuleCreateInfo{
                VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                nullptr,
                0
            };

            VkShaderModule shaderVertModule{};
            VkShaderModule shaderFragModule{};

            VkPipelineShaderStageCreateInfo piplineShaderInfoVert{
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                nullptr,
                0,
                VK_SHADER_STAGE_VERTEX_BIT
            };

            VkPipelineShaderStageCreateInfo piplineShaderInfoFrag{
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                nullptr,
                0,
                VK_SHADER_STAGE_FRAGMENT_BIT
            };

            VkPipelineShaderStageCreateInfo piplineShaderStageInfos[2];


            /*
            ========================================================
              6. GRAPHICS PIPELINE
            ========================================================
            */

            // Vertex input and assembly

            VkVertexInputBindingDescription vertexInputBindingDescription{
                0,
                sizeof(float) * 2,
                VK_VERTEX_INPUT_RATE_VERTEX
            };

            VkVertexInputAttributeDescription vertexInputAttributeDescription{
                0,
                0,
                VK_FORMAT_R32G32_SFLOAT,
                0
            };

            VkPipelineVertexInputStateCreateInfo vertexInputState{
                VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                nullptr,
                0,
                1,
                &vertexInputBindingDescription,
                1,
                &vertexInputAttributeDescription
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
                0
            };

            // Viewport and scissor
            VkViewport viewport{};
            VkRect2D scissor{};

            VkPipelineViewportStateCreateInfo viewportStateInfo{
                VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                nullptr,
                0,
                1
            };

            // Rasterization
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

            // Multisampling
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

            // Color blending
            VkPipelineColorBlendAttachmentState colorBlendAttachmentState{
                VK_FALSE,
                VK_BLEND_FACTOR_ONE,
                VK_BLEND_FACTOR_ZERO,
                VK_BLEND_OP_ADD,
                VK_BLEND_FACTOR_ONE,
                VK_BLEND_FACTOR_ZERO,
                VK_BLEND_OP_ADD,
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT
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

            // Pipeline layout and pipeline
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

            VkPipeline graphicsPipeline{VK_NULL_HANDLE};

            VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{
                VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                nullptr,
                0,
                2
            };

            /*
            ========================================================
              7. COMMAND POOL AND COMMAND BUFFERS
            ========================================================
            */

            VkCommandPoolCreateInfo commandPoolCreateInfo{
                VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                nullptr,
                0
            };

            VkCommandPool commandPool{};

            VkCommandBufferAllocateInfo commandBufferAllocateInfo{};

            std::vector<VkCommandBuffer> commandBuffers;

            VkCommandBufferBeginInfo commandBufferBeginInfo{
                VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                nullptr,
                0,
                nullptr
            };

            VkClearValue clearColorValue{};

            std::vector<VkRenderPassBeginInfo> renderPassBeginInfos{};


            /*
            ========================================================
              8. SYNCHRONIZATION AND PRESENTATION
            ========================================================
            */

            // Semaphores
            VkSemaphoreCreateInfo semaphoreCreateInfoWait{
                VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                nullptr,
                0
            };

            VkSemaphoreCreateInfo semaphoreCreateInfoSignal{
                VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                nullptr,
                0
            };

            VkSemaphore waitSemaphore{};
            std::vector<VkSemaphore> signalSemaphores{};

            // Queue submission
            VkPipelineStageFlags waitDstStageMask =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

            std::vector<VkSubmitInfo> submitInfos{};

            VkFence fence{};

            VkFenceCreateInfo fenceCreateInfo{
                VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                nullptr,
                0
            };

            // Presentation
            uint32_t imageIndex{};

            std::vector<VkPresentInfoKHR> presentInfos{};

            // VertexBuffer

            VkBufferCreateInfo bufferCreateInfo{
                VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                nullptr,
                0,
                24,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_SHARING_MODE_EXCLUSIVE,
                1,
                0
            };

            float vertices[6] {
                0.0, 0.5,
                0.5,-0.5,
                -0.5,-0.5
            };

            VkBuffer vertexBuffer{};

            void* vertexBufferData{};

            // Memory
            VkMemoryPropertyFlags requiredFlags =
  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            VkMemoryRequirements memoryRequirements{};

            VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};

            VkMemoryAllocateInfo memoryAllocateInfo{
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                nullptr,
            };

            VkDeviceMemory deviceMemory{};

            VkDeviceSize deviceSize{0};


            // Render unrelated

            

        public:
            void start(HINSTANCE* hinstance, HWND* hwnd);
            void createSwapchain(HINSTANCE* hinstance, HWND* hwnd);
            void createAndConfigureRenderSpecificInformation();
            void renderDefaultTriangle();
            void recordCommandBuffer();
            void triangleSlideToRight();
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
        float calculateDeltaTime();
        float calculateTime();
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