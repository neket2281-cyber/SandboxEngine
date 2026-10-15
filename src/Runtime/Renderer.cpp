#include <filesystem>
#include <fstream>
#include <iostream>
#include "../Headers/Runtime.h"
#include <vector>


void Engine::Renderer::Renderer::createSwapchain(HINSTANCE *hinstance, HWND *hwnd) {
    const char* layerName = "VK_LAYER_KHRONOS_validation";
    const char* layerNames[] {
        layerName
      };
    uint32_t layerCount = 0;

    // ---- layer properties ----
    vkEnumerateInstanceLayerProperties(&layerCount,nullptr);
    layerProperties.resize(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount,layerProperties.data());

    // ---- creating instance ----
    for (auto layer_property: layerProperties) {
        if (std::strcmp(layerName, layer_property.layerName) == 0)
        {
            vkInstanceInfo.ppEnabledLayerNames = layerNames;
            vkInstanceInfo.enabledLayerCount = std::size(layerNames);
            break;
        }
    }
    vkCreateInstance(&vkInstanceInfo,nullptr,&instance);

    // ---- finding physical devices ----
    vkEnumeratePhysicalDevices(instance,&physicalDeviceCount,nullptr);
    physicalDevices.resize(physicalDeviceCount);
    physDevProperties.resize(physicalDeviceCount);
    driverProperties.resize(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance,&physicalDeviceCount,physicalDevices.data());

    for (int i = 0; i < physicalDeviceCount; i++) {
        physDevProperties[i].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        driverProperties[i].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
        physDevProperties[i].pNext = &driverProperties[i];
        vkGetPhysicalDeviceProperties2(physicalDevices[i],&physDevProperties[i]);
    }

    for (int i = physDevProperties.size() - 1; i >= 0; i--) {
        if ((physDevProperties[i].properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
            physDevProperties[i].properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) &&
            !(driverProperties[i].driverID == VK_DRIVER_ID_MESA_DOZEN ||
            driverProperties[i].driverID == VK_DRIVER_ID_GOOGLE_SWIFTSHADER ||
            driverProperties[i].driverID == VK_DRIVER_ID_MESA_LLVMPIPE ||
            driverProperties[i].driverID == VK_DRIVER_ID_MESA_VENUS ||
            driverProperties[i].driverID == VK_DRIVER_ID_MOLTENVK)) continue;
        physDevProperties.erase(physDevProperties.begin() + i);
        physicalDevices.erase(physicalDevices.begin() + i);
    }

    queueFamilyPropertiesArray.resize(physicalDevices.size());

    deviceNames.resize(physicalDevices.size());

    for (int i = 0; i < physicalDevices.size();i++) {
        uint32_t deviceQueueCount{0};
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i],&deviceQueueCount,nullptr);
        queueFamilyPropertiesArray[i].resize(deviceQueueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i],&deviceQueueCount,queueFamilyPropertiesArray[i].data());
        deviceNames[i] = physDevProperties[i].properties.deviceName;
    }

    bool endSelection {false};

    for (int i = 0; i < queueFamilyPropertiesArray.size(); i++) {
        //if (endSelection == true) break;
        for (int t = 0; t < queueFamilyPropertiesArray[i].size(); t++) {
            if (queueFamilyPropertiesArray[i][t].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                vkSuitableQueueFamilyProperties = &queueFamilyPropertiesArray[i][t];
                queueFamilyIndex = t;
                vkPhysicalDevice = &physicalDevices[i];
                deviceProperties = &physDevProperties[i];
                selectedDeviceDriverProperties = &driverProperties[i];

                //endSelection = true;

                //std::cout << queueFamilyPropertiesArray[i][t].queueFlags << " i = "<< i << " t = "<< t << " Device Name : " << deviceNames[i] << " " << selectedDeviceDriverProperties->driverID << std::endl;
                //std::cout << "FIRES" << std::endl;
            }
        }
    }

    vkGetPhysicalDeviceFeatures(*vkPhysicalDevice,&vkSuitableDeviceFeatures);

    surfaceCreateFlags = vkSuitableQueueFamilyProperties->queueFlags;

    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.flags = 0;
    surfaceInfo.hinstance = *hinstance;
    surfaceInfo.hwnd = *hwnd;

    vkCreateWin32SurfaceKHR(instance,&surfaceInfo,nullptr,&surface);

    vkGetPhysicalDeviceSurfaceSupportKHR(*vkPhysicalDevice,queueFamilyIndex,surface,&supportsSurfaceKHR);

    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.flags = 0;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriorities;

    vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    vkDeviceCreateInfo.pNext = nullptr;
    vkDeviceCreateInfo.flags = 0;
    vkDeviceCreateInfo.queueCreateInfoCount = 1;
    vkDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    vkDeviceCreateInfo.enabledExtensionCount = 1;
    vkDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;

    vkCreateDevice(*vkPhysicalDevice,&vkDeviceCreateInfo,nullptr,&device);
    vkGetDeviceQueue(device,queueFamilyIndex,0,&queue);

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*vkPhysicalDevice,surface,&capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(*vkPhysicalDevice,surface,&surfaceFormatCount,nullptr);
    surfaceFormats.resize(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(*vkPhysicalDevice,surface,&surfaceFormatCount,surfaceFormats.data());
    vkGetPhysicalDeviceSurfacePresentModesKHR(*vkPhysicalDevice,surface,&presentModeCount,nullptr);
    presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(*vkPhysicalDevice,surface,&presentModeCount, presentModes.data());

    swapchainInfo.surface = surface;
    swapchainInfo.minImageCount = capabilities.minImageCount;
    swapchainInfo.imageExtent = capabilities.currentExtent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.queueFamilyIndexCount = 0;
    swapchainInfo.pQueueFamilyIndices = nullptr;
    swapchainInfo.preTransform = capabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    for (auto pM: presentModes) {
        if (pM == VK_PRESENT_MODE_FIFO_KHR) {
            swapchainInfo.presentMode = pM;
            break;
        }
        swapchainInfo.presentMode = presentModes[0];
    }
    for (auto sF: surfaceFormats) {
        if (sF.format == VK_FORMAT_B8G8R8A8_SRGB) {
                swapchainInfo.imageFormat = sF.format;
                break;
            }
            else swapchainInfo.imageFormat = surfaceFormats[0].format;
        }
    for (auto sF: surfaceFormats) {
        if (sF.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            swapchainInfo.imageColorSpace = sF.colorSpace;
            break;
        }
        else swapchainInfo.imageColorSpace = surfaceFormats[0].colorSpace;
    }

    vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapchain);

    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
    swapchainImages.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data());

    imageViewInfo.resize(swapchainImages.size());

    imageView.resize(imageViewInfo.size());

    for (int i = 0; i < imageViewInfo.size(); i++) {
        imageViewInfo[i].sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo[i].pNext = nullptr;
        imageViewInfo[i].flags = 0;
        imageViewInfo[i].image = swapchainImages[i];
        imageViewInfo[i].viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo[i].format = VK_FORMAT_B8G8R8A8_SRGB;
        imageViewInfo[i].components.r = VK_COMPONENT_SWIZZLE_R;
        imageViewInfo[i].components.g = VK_COMPONENT_SWIZZLE_G;
        imageViewInfo[i].components.b = VK_COMPONENT_SWIZZLE_B;
        imageViewInfo[i].components.a = VK_COMPONENT_SWIZZLE_A;
        imageViewInfo[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo[i].subresourceRange.baseMipLevel = 0;
        imageViewInfo[i].subresourceRange.levelCount = 1;
        imageViewInfo[i].subresourceRange.baseArrayLayer = 0;
        imageViewInfo[i].subresourceRange.layerCount = 1;

        std::cout << vkCreateImageView(device,&imageViewInfo[i],nullptr,&imageView[i]) << std::endl;
    }

    std::cout << "CREATE PASS: " << vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass) << std::endl;

    framebufferCreateInfos.resize(imageView.size());
    framebuffers.resize(imageView.size());
    framebuffers.resize(imageView.size());
    commandBuffers.resize(imageView.size());

    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

    vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);

    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = imageView.size();

    std::cout << "CREATE COMMAND BUFFERS: " <<  vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data()) << std::endl;
}

void Engine::Renderer::Renderer::createAndConfigureRenderSpecificInformation() {
    for (int i = 0; i < imageView.size();i++) {
        framebufferCreateInfos[i].sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfos[i].pNext = nullptr;
        framebufferCreateInfos[i].flags = 0;
        framebufferCreateInfos[i].renderPass = renderPass;
        framebufferCreateInfos[i].attachmentCount = 1;
        framebufferCreateInfos[i].pAttachments = &imageView[i];
        framebufferCreateInfos[i].width = swapchainInfo.imageExtent.width;
        framebufferCreateInfos[i].height = swapchainInfo.imageExtent.height;
        framebufferCreateInfos[i].layers = 1;

      std::cout << "CREATE FRAME BUFFERS: " <<  vkCreateFramebuffer(device, &framebufferCreateInfos[i], nullptr, &framebuffers[i]) << std::endl;
    }
    triangleVert.open("../src/CompiledShaders/triangle.vert.spv", std::ios::binary);
    triangleFrag.open("../src/CompiledShaders/triangle.frag.spv",std::ios::binary);

    triangleVert.seekg(0, std::ios::end);
    triangleFrag.seekg(0, std::ios::end);

    triangleVertCode.resize(static_cast<size_t>(triangleVert.tellg())/4);
    triangleFragCode.resize(static_cast<size_t>(triangleFrag.tellg())/4);

    triangleVert.seekg(0, std::ios::beg);
    triangleFrag.seekg(0, std::ios::beg);

    triangleVert.read(reinterpret_cast<char*>(triangleVertCode.data()), triangleVertCode.size() * sizeof(uint32_t));
    triangleFrag.read(reinterpret_cast<char*>(triangleFragCode.data()), triangleFragCode.size() * sizeof(uint32_t));

    shaderVertModuleCreateInfo.codeSize = triangleVertCode.size()* sizeof(uint32_t);
    shaderFragModuleCreateInfo.codeSize = triangleFragCode.size()* sizeof(uint32_t);

    shaderVertModuleCreateInfo.pCode = triangleVertCode.data();
    shaderFragModuleCreateInfo.pCode = triangleFragCode.data();


    vkCreateShaderModule(device, &shaderVertModuleCreateInfo, nullptr, &shaderVertModule);
    vkCreateShaderModule(device, &shaderFragModuleCreateInfo, nullptr, &shaderFragModule);

    piplineShaderInfoVert.module = shaderVertModule;
    piplineShaderInfoFrag.module = shaderFragModule;

    piplineShaderInfoVert.pName = "main";
    piplineShaderInfoFrag.pName = "main";

    piplineShaderInfoVert.pSpecializationInfo = nullptr;
    piplineShaderInfoFrag.pSpecializationInfo = nullptr;

    piplineShaderStageInfos[0] = piplineShaderInfoVert;
    piplineShaderStageInfos[1] = piplineShaderInfoFrag;

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.height = swapchainInfo.imageExtent.height;
    viewport.width = swapchainInfo.imageExtent.width;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    scissor.offset.x = 0.0f;
    scissor.offset.y = 0.0f;
    scissor.extent = swapchainInfo.imageExtent;

    viewportStateInfo.pViewports = &viewport;
    viewportStateInfo.scissorCount = 1;
    viewportStateInfo.pScissors = &scissor;

    vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);

    graphicsPipelineCreateInfo.pStages = piplineShaderStageInfos;
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputState;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
    graphicsPipelineCreateInfo.pTessellationState = nullptr;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleState;
    graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendState;
    graphicsPipelineCreateInfo.pDynamicState = nullptr;
    graphicsPipelineCreateInfo.layout = pipelineLayout;
    graphicsPipelineCreateInfo.renderPass = renderPass;
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    std::cout << vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline);

    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = framebuffers[0];
    renderPassBeginInfo.renderArea = scissor;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColorValue;

    vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);

    //vkCreateBuffer(device, &bufferCreateInfo, nullptr, &vertexBuffer);
}

void Engine::Renderer::Renderer::renderDefaultTriangle() {
    vkBeginCommandBuffer(commandBuffers[0], &commandBufferBeginInfo);
    vkCmdBeginRenderPass(commandBuffers[0], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[0],VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    vkCmdDraw(commandBuffers[0],3,1,0,0);

    vkCmdEndRenderPass(commandBuffers[0]);
    vkEndCommandBuffer(commandBuffers[0]);

    vkCreateSemaphore(device, &semaphoreCreateInfoWait, nullptr, &semaphoreWait);
    vkCreateSemaphore(device, &semaphoreCreateInfoSignal, nullptr, &semaphoreSignal);

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &semaphoreWait;
    submitInfo.pWaitDstStageMask = &waitDstStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[0];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &semaphoreSignal;

    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, semaphoreWait , VK_NULL_HANDLE, &imageIndex);

    std::cout << vkQueueSubmit(queue, 1, &submitInfo, fence);

    presentInfo.pWaitSemaphores = &semaphoreWait;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;


    vkQueuePresentKHR(queue, &presentInfo);
}

void Engine::Renderer::Renderer::start(HINSTANCE* hinstance, HWND* hwnd) {

    createSwapchain(hinstance, hwnd);

    createAndConfigureRenderSpecificInformation();

    renderDefaultTriangle();
}