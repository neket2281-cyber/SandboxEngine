#include <filesystem>
#include <fstream>
#include <iostream>
#include "../Headers/Runtime.h"
#include <vector>

//          ==============================
//          ======RENDERER CREATION=======
//          ==============================

void Engine::Renderer::Renderer::createInstance(HINSTANCE *hinstance, HWND *hwnd) {
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
}

void Engine::Renderer::Renderer::getDeviceInformation() {
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
}

void Engine::Renderer::Renderer::setSwapchain() {
    //Swaphain specific info and configuration
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

    // Viewport and Scissor configuration
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


    //Commandpool, RenderPass, and command buffer creation and allocation
    vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);

    framebufferCreateInfos.resize(imageView.size());
    framebuffersTriangle.resize(imageView.size());
    framebuffersSquare.resize(imageView.size());
    commandBuffersTriangle.resize(imageView.size());
    commandBuffersSquare.resize(imageView.size());

    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

    vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);

    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = imageView.size();

    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffersTriangle.data());
    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffersSquare.data());

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

        vkCreateFramebuffer(device, &framebufferCreateInfos[i], nullptr, &framebuffersTriangle[i]);
        vkCreateFramebuffer(device, &framebufferCreateInfos[i], nullptr, &framebuffersSquare[i]);
    }
}

void Engine::Renderer::Renderer::shaderManagement() {
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
}

void Engine::Renderer::Renderer::createPipeline() {
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
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateInfo;
    graphicsPipelineCreateInfo.layout = pipelineLayout;
    graphicsPipelineCreateInfo.renderPass = renderPass;
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline);

    //renderPass infos
    renderPassBeginInfosTriangle.resize(imageView.size());
    renderPassBeginInfosSquare.resize(imageView.size());
    for (int i = 0; i < imageView.size(); i++) {
        //triangle
        renderPassBeginInfosTriangle[i].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        renderPassBeginInfosTriangle[i].pNext = nullptr;
        renderPassBeginInfosTriangle[i].renderPass = renderPass;
        renderPassBeginInfosTriangle[i].framebuffer = framebuffersTriangle[i];
        renderPassBeginInfosTriangle[i].renderArea = scissor;
        renderPassBeginInfosTriangle[i].clearValueCount = 1;
        renderPassBeginInfosTriangle[i].pClearValues = &clearColorValue;

        //square
        renderPassBeginInfosSquare[i].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        renderPassBeginInfosSquare[i].pNext = nullptr;
        renderPassBeginInfosSquare[i].renderPass = renderPass;
        renderPassBeginInfosSquare[i].framebuffer = framebuffersSquare[i];
        renderPassBeginInfosSquare[i].renderArea = scissor;
        renderPassBeginInfosSquare[i].clearValueCount = 1;
        renderPassBeginInfosSquare[i].pClearValues = &clearColorValue;
    }

    vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);
}

//          ======MASHES=======
//DEFAULT TRIANGLE
void Engine::Renderer::Renderer::shaderBufferConfigurationTriangle(){
    vkCreateBuffer(device, &bufferCreateInfoTriangle, nullptr, &vertexBufferTriangle);

    vkGetBufferMemoryRequirements(device, vertexBufferTriangle, &memoryRequirements);

    vkGetPhysicalDeviceMemoryProperties(*vkPhysicalDevice, &physicalDeviceMemoryProperties);

    for(int i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++) {

        bool compatible  = (memoryRequirements.memoryTypeBits & 1u << i)
        != 0;

        bool hasProperties = (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & requiredFlags)
        == requiredFlags;

        if (compatible && hasProperties) {
            //memoryTypeIndex = i;
            memoryAllocateInfo.memoryTypeIndex = i;
        }
    }

    memoryAllocateInfo.allocationSize = memoryRequirements.size;

    vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &deviceMemoryTriangle);

    vkBindBufferMemory(device, vertexBufferTriangle, deviceMemoryTriangle, 0);

    vkMapMemory(device, deviceMemoryTriangle, 0, 24,0, &vertexBufferDataTriangle);

    std::memcpy(vertexBufferDataTriangle, verticesTriangle, sizeof(verticesTriangle));

}
void Engine::Renderer::Renderer::recordCommandBufferTriangle() {
    for (int i = 0; i < imageView.size(); i++) {
        vkBeginCommandBuffer(commandBuffersTriangle[i], &commandBufferBeginInfo);
        vkCmdBeginRenderPass(commandBuffersTriangle[i], &renderPassBeginInfosTriangle[i], VK_SUBPASS_CONTENTS_INLINE);

        vkCmdSetViewport(commandBuffersTriangle[i], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffersTriangle[i], 0, 1, &scissor);
        vkCmdBindPipeline(commandBuffersTriangle[i],VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        vkCmdBindVertexBuffers(commandBuffersTriangle[i],0, 1, &vertexBufferTriangle, &deviceSizeTriangle);
        vkCmdDraw(commandBuffersTriangle[i],3,1,0,0);

        vkCmdEndRenderPass(commandBuffersTriangle[i]);
        vkEndCommandBuffer(commandBuffersTriangle[i]);
    }

    vkCreateSemaphore(device, &semaphoreCreateInfoWait, nullptr, &waitSemaphore);

    signalSemaphoresTriangle.resize(swapchainImageCount);
    signalSemaphoresSquare.resize(swapchainImageCount);
    submitInfosTriangle.resize(swapchainImageCount);
    submitInfosSquare.resize(swapchainImageCount);
    presentInfosTriangle.resize(swapchainImageCount);
    presentInfosSquare.resize(swapchainImageCount);

    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        //triangle
        vkCreateSemaphore(device, &semaphoreCreateInfoSignal, nullptr, &signalSemaphoresTriangle[i]);

        submitInfosTriangle[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfosTriangle[i].pNext = nullptr;
        submitInfosTriangle[i].waitSemaphoreCount = 1;
        submitInfosTriangle[i].pWaitSemaphores = &waitSemaphore;
        submitInfosTriangle[i].pWaitDstStageMask = &waitDstStageMask;
        submitInfosTriangle[i].commandBufferCount = 1;
        submitInfosTriangle[i].pCommandBuffers = &commandBuffersTriangle[i];
        submitInfosTriangle[i].signalSemaphoreCount = 1;
        submitInfosTriangle[i].pSignalSemaphores = &signalSemaphoresTriangle[i];

        presentInfosTriangle[i].sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfosTriangle[i].pNext = nullptr;
        presentInfosTriangle[i].waitSemaphoreCount = 1;
        presentInfosTriangle[i].pWaitSemaphores = &signalSemaphoresTriangle[i];
        presentInfosTriangle[i].swapchainCount = 1;
        presentInfosTriangle[i].pSwapchains = &swapchain;

        //square
        vkCreateSemaphore(device, &semaphoreCreateInfoSignal, nullptr, &signalSemaphoresSquare[i]);

        submitInfosSquare[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfosSquare[i].pNext = nullptr;
        submitInfosSquare[i].waitSemaphoreCount = 1;
        submitInfosSquare[i].pWaitSemaphores = &waitSemaphore;
        submitInfosSquare[i].pWaitDstStageMask = &waitDstStageMask;
        submitInfosSquare[i].commandBufferCount = 1;
        submitInfosSquare[i].pCommandBuffers = &commandBuffersSquare[i];
        submitInfosSquare[i].signalSemaphoreCount = 1;
        submitInfosSquare[i].pSignalSemaphores = &signalSemaphoresSquare[i];

        presentInfosSquare[i].sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfosSquare[i].pNext = nullptr;
        presentInfosSquare[i].waitSemaphoreCount = 1;
        presentInfosSquare[i].pWaitSemaphores = &signalSemaphoresSquare[i];
        presentInfosSquare[i].swapchainCount = 1;
        presentInfosSquare[i].pSwapchains = &swapchain;
    }
}

void Engine::Renderer::Renderer::renderDefaultTriangle() {
    //vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    std::cout << "IMAGE ACQUISITION: " << vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, waitSemaphore , VK_NULL_HANDLE, &imageIndex) << std::endl;
    //std::cout << "INDEX USED: " << imageIndex << std::endl;
    //std::cout << "COMMAND BUFFER USED: " << submitInfos[imageIndex].pCommandBuffers[0] << std::endl;
    //std::cout << "FRAME BUFFER USED: " << renderPassBeginInfos[imageIndex].framebuffer << std::endl;
    //vkResetFences(device, 1, &fence);

    vkQueueSubmit(queue, 1, &submitInfosTriangle[imageIndex], fence);

    presentInfosTriangle[imageIndex].pImageIndices = &imageIndex;

    vkQueuePresentKHR(queue, &presentInfosTriangle[imageIndex]);
}
void Engine::Renderer::Renderer::triangleSlideToRight()
{
    //vertices[0] += 0.01;
    verticesTriangle[2] += 0.01;
    verticesTriangle[4] += 0.01;

    std::memcpy(vertexBufferDataTriangle, verticesTriangle, sizeof(verticesTriangle));

    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);

    imageAcquisitionResult = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, waitSemaphore , VK_NULL_HANDLE, &imageIndex);

    if (imageAcquisitionResult == VK_SUCCESS) {
        vkResetFences(device, 1, &fence);

        vkQueueSubmit(queue, 1, &submitInfosTriangle[imageIndex], fence);

        presentInfosTriangle[imageIndex].pImageIndices = &imageIndex;

        vkQueuePresentKHR(queue, &presentInfosTriangle[imageIndex]);
    }
    else if (imageAcquisitionResult == VK_ERROR_OUT_OF_DATE_KHR) {
        oldSwapchain = swapchain;
        updateSwapchain();
    }
}
//DEFAULT SQUARE
void Engine::Renderer::Renderer::shaderBufferConfigurationSquare(){

    vkCreateBuffer(device, &bufferCreateInfoSquare, nullptr, &vertexBufferSquare);

    vkGetBufferMemoryRequirements(device, vertexBufferSquare, &memoryRequirements);

    vkGetPhysicalDeviceMemoryProperties(*vkPhysicalDevice, &physicalDeviceMemoryProperties);

    for(int i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++) {

        bool compatible  = (memoryRequirements.memoryTypeBits & 1u << i)
        != 0;

        bool hasProperties = (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & requiredFlags)
        == requiredFlags;

        if (compatible && hasProperties) {
            //memoryTypeIndex = i;
            memoryAllocateInfo.memoryTypeIndex = i;
        }
    }

    memoryAllocateInfo.allocationSize = memoryRequirements.size;

    vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &deviceMemorySquare);

    vkBindBufferMemory(device, vertexBufferSquare, deviceMemorySquare, 0);

    vkMapMemory(device, deviceMemorySquare, 0, 48,0, &vertexBufferDataSquare);

    std::memcpy(vertexBufferDataSquare, verticesSquare, sizeof(verticesSquare));

}
void Engine::Renderer::Renderer::recordCommandBufferSquare() {
    for (int i = 0; i < imageView.size(); i++) {
        vkBeginCommandBuffer(commandBuffersSquare[i], &commandBufferBeginInfo);
        vkCmdBeginRenderPass(commandBuffersSquare[i], &renderPassBeginInfosSquare[i], VK_SUBPASS_CONTENTS_INLINE);

        vkCmdSetViewport(commandBuffersSquare[i], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffersSquare[i], 0, 1, &scissor);
        vkCmdBindPipeline(commandBuffersSquare[i],VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        vkCmdBindVertexBuffers(commandBuffersSquare[i],0, 1, &vertexBufferSquare, &deviceSizeSquare);
        vkCmdDraw(commandBuffersSquare[i],6,1,0,0);

        vkCmdEndRenderPass(commandBuffersSquare[i]);
        vkEndCommandBuffer(commandBuffersSquare[i]);
    }

    vkCreateSemaphore(device, &semaphoreCreateInfoWait, nullptr, &waitSemaphore);

    signalSemaphoresSquare.resize(swapchainImageCount);
    submitInfosSquare.resize(swapchainImageCount);
    presentInfosSquare.resize(swapchainImageCount);

    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        vkCreateSemaphore(device, &semaphoreCreateInfoSignal, nullptr, &signalSemaphoresSquare[i]);

        submitInfosSquare[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfosSquare[i].pNext = nullptr;
        submitInfosSquare[i].waitSemaphoreCount = 1;
        submitInfosSquare[i].pWaitSemaphores = &waitSemaphore;
        submitInfosSquare[i].pWaitDstStageMask = &waitDstStageMask;
        submitInfosSquare[i].commandBufferCount = 1;
        submitInfosSquare[i].pCommandBuffers = &commandBuffersSquare[i];
        submitInfosSquare[i].signalSemaphoreCount = 1;
        submitInfosSquare[i].pSignalSemaphores = &signalSemaphoresSquare[i];

        presentInfosSquare[i].sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfosSquare[i].pNext = nullptr;
        presentInfosSquare[i].waitSemaphoreCount = 1;
        presentInfosSquare[i].pWaitSemaphores = &signalSemaphoresSquare[i];
        presentInfosSquare[i].swapchainCount = 1;
        presentInfosSquare[i].pSwapchains = &swapchain;
    }
}

void Engine::Renderer::Renderer::renderDefaultSquare() {
    std::cout << "IMAGE ACQUISITION: " << vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, waitSemaphore , VK_NULL_HANDLE, &imageIndex) << std::endl;

    vkQueueSubmit(queue, 1, &submitInfosSquare[imageIndex], fence);

    presentInfosSquare[imageIndex].pImageIndices = &imageIndex;

    vkQueuePresentKHR(queue, &presentInfosSquare[imageIndex]);
}
void Engine::Renderer::Renderer::squareSlideToRight()
{
    verticesSquare[0] += 0.01;
    verticesSquare[2] += 0.01;
    verticesTriangle[4] += 0.01;
    verticesSquare[6] += 0.01;
    verticesSquare[8] += 0.01;
    verticesTriangle[10] += 0.01;

    std::memcpy(vertexBufferDataSquare, verticesSquare, sizeof(verticesSquare));

    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);

    imageAcquisitionResult = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, waitSemaphore , VK_NULL_HANDLE, &imageIndex);

    if (imageAcquisitionResult == VK_SUCCESS) {
        vkResetFences(device, 1, &fence);

        vkQueueSubmit(queue, 1, &submitInfosSquare[imageIndex], fence);

        presentInfosSquare[imageIndex].pImageIndices = &imageIndex;

        vkQueuePresentKHR(queue, &presentInfosSquare[imageIndex]);
    }
    else if (imageAcquisitionResult == VK_ERROR_OUT_OF_DATE_KHR) {
        oldSwapchain = swapchain;
        updateSwapchain();
    }
}

//          ======HELPING AND RENDERING METHODS======


void Engine::Renderer::Renderer::updateSwapchain() {

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
    swapchainInfo.oldSwapchain = oldSwapchain;

    std::cout << "CREATE SWAPCHAINKHR: " << vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapchain) << std::endl;

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

    // Viewport and Scissor configuration
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

    framebufferCreateInfos.resize(imageView.size());
    framebuffersTriangle.resize(imageView.size());
    framebuffersTriangle.resize(imageView.size());
    commandBuffersTriangle.resize(imageView.size());

    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = imageView.size();

    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffersTriangle.data());

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

        vkCreateFramebuffer(device, &framebufferCreateInfos[i], nullptr, &framebuffersTriangle[i]);
    }

    renderPassBeginInfosTriangle.resize(imageView.size());
    for (int i = 0; i < imageView.size(); i++) {
        renderPassBeginInfosTriangle[i].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        renderPassBeginInfosTriangle[i].pNext = nullptr;
        renderPassBeginInfosTriangle[i].renderPass = renderPass;
        renderPassBeginInfosTriangle[i].framebuffer = framebuffersTriangle[i];
        renderPassBeginInfosTriangle[i].renderArea = scissor;
        renderPassBeginInfosTriangle[i].clearValueCount = 1;
        renderPassBeginInfosTriangle[i].pClearValues = &clearColorValue;
    }

    for (int i = 0; i < imageView.size(); i++) {
        vkBeginCommandBuffer(commandBuffersTriangle[i], &commandBufferBeginInfo);
        vkCmdBeginRenderPass(commandBuffersTriangle[i], &renderPassBeginInfosTriangle[i], VK_SUBPASS_CONTENTS_INLINE);

        vkCmdSetViewport(commandBuffersTriangle[i], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffersTriangle[i], 0, 1, &scissor);
        vkCmdBindPipeline(commandBuffersTriangle[i],VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        vkCmdBindVertexBuffers(commandBuffersTriangle[i],0, 1, &vertexBufferTriangle, &deviceSizeTriangle);
        vkCmdDraw(commandBuffersTriangle[i],3,1,0,0);

        vkCmdEndRenderPass(commandBuffersTriangle[i]);
        vkEndCommandBuffer(commandBuffersTriangle[i]);
    }

    vkCreateSemaphore(device, &semaphoreCreateInfoWait, nullptr, &waitSemaphore);

    signalSemaphoresTriangle.resize(swapchainImageCount);
    submitInfosTriangle.resize(swapchainImageCount);
    presentInfosTriangle.resize(swapchainImageCount);

    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        vkCreateSemaphore(device, &semaphoreCreateInfoSignal, nullptr, &signalSemaphoresTriangle[i]);

        submitInfosTriangle[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfosTriangle[i].pNext = nullptr;
        submitInfosTriangle[i].waitSemaphoreCount = 1;
        submitInfosTriangle[i].pWaitSemaphores = &waitSemaphore;
        submitInfosTriangle[i].pWaitDstStageMask = &waitDstStageMask;
        submitInfosTriangle[i].commandBufferCount = 1;
        submitInfosTriangle[i].pCommandBuffers = &commandBuffersTriangle[i];
        submitInfosTriangle[i].signalSemaphoreCount = 1;
        submitInfosTriangle[i].pSignalSemaphores = &signalSemaphoresTriangle[i];

        presentInfosTriangle[i].sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfosTriangle[i].pNext = nullptr;
        presentInfosTriangle[i].waitSemaphoreCount = 1;
        presentInfosTriangle[i].pWaitSemaphores = &signalSemaphoresTriangle[i];
        presentInfosTriangle[i].swapchainCount = 1;
        presentInfosTriangle[i].pSwapchains = &swapchain;
    }
}

//          ======START======

void Engine::Renderer::Renderer::start(HINSTANCE* hinstance, HWND* hwnd) {

    createInstance(hinstance, hwnd);

    getDeviceInformation();

    setSwapchain();

    shaderManagement();

    createPipeline();

    shaderBufferConfigurationTriangle();
    recordCommandBufferTriangle();

    shaderBufferConfigurationSquare();
    recordCommandBufferSquare();

    renderDefaultSquare();
    //renderDefaultTriangle();
}