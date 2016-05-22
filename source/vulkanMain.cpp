//---------------------------------------------------------------------------
//!
//! @file	vulkanMain.cpp
//! @brief	Vulkan�̃��C������
//! @author	Yuki.Ishigaki
//!
//---------------------------------------------------------------------------
#include "stdafx.h"





//---------------------------------------------------------------------------
// �C���[�W���C�A�E�g��ݒ肵�܂�.
//---------------------------------------------------------------------------
void SetImageLayout
(
	VkDevice            device,
	VkCommandBuffer     commandBuffer,
	VkImage             image,
	VkImageAspectFlags  aspectFlags,
	VkImageLayout       oldLayout,
	VkImageLayout       newLayout
)
{
	assert(device != nullptr);
	assert(commandBuffer != nullptr);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = nullptr;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.image = image;
	barrier.subresourceRange = { aspectFlags, 0, 1, 0, 1 };

	if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier);
}


//---------------------------------------------------------------------------
//! ������
//---------------------------------------------------------------------------
bool vkMain::initialize(HINSTANCE& hInstance, HWND& hWnd)
{
	
	if (!IDeviceManager()->initialize()) {
		OutputDebugStringA("Error : DeviceManager::Init() is Fail");
		return false;
	}

	auto& vkDevice = IDeviceManager()->getVkDevice();
	auto& vkInstance = IDeviceManager()->getVkInstance();
	auto* pPhysicalDevice = IDeviceManager()->getPhysicalDevice(0);

	// �f�o�C�X�ƃL���[�̐���.
	{
		
		// �L���[�̐����擾
		uint32_t propCount;
		vkGetPhysicalDeviceQueueFamilyProperties(*pPhysicalDevice, &propCount, nullptr);

		// �����f�o�C�X��ŗ��p�\�ȃL���[�̃v���p�e�B���擾
		std::vector<VkQueueFamilyProperties> props;
		props.resize(propCount);
		vkGetPhysicalDeviceQueueFamilyProperties(*pPhysicalDevice, &propCount, props.data());

		// �L���[����t�@�~���[�C���f�b�N�X���擾
		for (auto i = 0u; i<propCount; ++i)
		{
			// �`����s�������̂ŕ`��̃C���f�b�N�X���擾
			if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				_graphicsFamilyIndex = i;
			}
		}

		// �L���[�쐬���̐ݒ�
		VkDeviceQueueCreateInfo queueInfo;
		float queuePriorities[] = { 0.0f };
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.pNext = nullptr;
		queueInfo.queueCount = 1;
		queueInfo.queueFamilyIndex = _graphicsFamilyIndex;
		queueInfo.pQueuePriorities = queuePriorities;

		// �f�o�C�X�쐬���̐ݒ�
		VkDeviceCreateInfo deviceInfo = {};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pNext = nullptr;
		deviceInfo.queueCreateInfoCount = 1;
		deviceInfo.pQueueCreateInfos = &queueInfo;
		deviceInfo.enabledLayerCount = 0;
		deviceInfo.ppEnabledLayerNames = nullptr;
		deviceInfo.enabledExtensionCount = 0;
		deviceInfo.ppEnabledExtensionNames = nullptr;
		deviceInfo.pEnabledFeatures = nullptr;

		// �f�o�C�X�̍쐬
		auto result = vkCreateDevice(*pPhysicalDevice, &deviceInfo, nullptr, &vkDevice);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkCreateDevice() Failed.");
			return false;
		}

		vkGetDeviceQueue(vkDevice, _graphicsFamilyIndex, 0, &_graphicsQueue);

		props.clear();
	}

	// �t�F���X(CPU�̃������o���A)�̐���.
	{
		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		auto result = vkCreateFence(vkDevice, &info, nullptr, &_graphicsFence);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkCreateFence() Failed.");
			return false;
		}
	}

	// �Z�}�t�H(GPU�̃������o���A)�̐���.
	{
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;

		auto result = vkCreateSemaphore(vkDevice, &info, nullptr, &_graphicsSemaphore);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkCreatSemaphore() Failed.");
			return false;
		}
	}

	// �R�}���h�v�[���̐���.
	{
		VkCommandPoolCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.pNext = nullptr;
		info.queueFamilyIndex = _graphicsFamilyIndex;
		info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		auto result = vkCreateCommandPool(vkDevice, &info, nullptr, &_commandPool);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkCreateCommandPool() Failed.");
			return false;
		}
	}

	

	// �T�[�t�F�C�X�̐���.
	{
		// �T�[�t�F�C�X���̐ݒ�
		VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
		surfaceInfo.sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.pNext		= nullptr;
		surfaceInfo.flags		= 0;
		surfaceInfo.hinstance	= hInstance;
		surfaceInfo.hwnd		= hWnd;

		// �T�[�t�F�C�X�̍쐬
		auto result = vkCreateWin32SurfaceKHR(vkInstance, &surfaceInfo, nullptr, &_surface);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkCreateWin32SurfaceKHR() Failed.");
			return false;
		}
	}

	// �C���[�W�̃t�H�[�}�b�g�ݒ�
	VkFormat        imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	// �F��Ԃ̐ݒ�
	VkColorSpaceKHR imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

	// �X���b�v�`�F�C���̐���.
	{
		// �����f�o�C�X�̃T�[�t�F�C�X�t�H�[�}�b�g�̐����擾
		uint32_t count = 0;
		auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(*pPhysicalDevice, _surface, &count, nullptr);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkGetPhysicalDeviceSurfaceFormatKHR() Failed.");
			return false;
		}

		// �t�H�[�}�b�g�������������m�ۂ��A�T�[�t�F�C�X�t�H�[�}�b�g�����擾
		std::vector<VkSurfaceFormatKHR> formats;
		formats.resize(count);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(*pPhysicalDevice, _surface, &count, formats.data());
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkGetPhysicalDeviceSUrfaceFormatsKHR() Failed.");
			return false;
		}

		// �t�H�[�}�b�g�ꗗ�̒�������
		bool isFind = false;
		for (size_t i = 0; i<formats.size(); ++i)
		{
			// �ݒ肵�����F�t�H�[�}�b�g����ѐF��Ԃ����邩�ǂ���
			if (imageFormat == formats[i].format &&
				imageColorSpace == formats[i].colorSpace)
			{
				isFind = true;
				break;
			}
		}

		// �Ȃ��ꍇ�̓f�t�H���g�̂��̂��w�肷��
		if (!isFind)
		{
			imageFormat = formats[0].format;
			imageColorSpace = formats[0].colorSpace;
		}

		// �T�[�t�F�C�X�̋@�\����шړ���]�̃t���O�ݒ�
		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceTransformFlagBitsKHR preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		{
			// �T�[�t�F�C�X�̋@�\���擾
			auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
				*pPhysicalDevice,
				_surface,
				&capabilities);
			if (result != VK_SUCCESS)
			{
				OutputDebugStringA("Error : vkGetPhysicalDeviceSurfaceCapabilitiesKHR() Failed.");
				return false;
			}

			// �T�[�t�F�C�X���T�|�[�g���Ă���ړ���]�t���O��ݒ肷��
			if (!(capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR))
			{
				preTransform = capabilities.currentTransform;
			}
		}

		// �v���[���g���[�h�̐ݒ�
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
		{
			// �v���[���g���[�h�̌����擾
			uint32_t presentModeCount;
			result = vkGetPhysicalDeviceSurfacePresentModesKHR(
				*pPhysicalDevice,
				_surface,
				&presentModeCount,
				nullptr);
			if (result != VK_SUCCESS)
			{
				OutputDebugStringA("Error : vkGetPhysicalDeviceSurfacePresentModesKHR() Failed.");
				return false;
			}

			// �v���[���g���[�h�̌����������m��
			std::vector<VkPresentModeKHR> presentModes;
			presentModes.resize(presentModeCount);
			// �����f�o�C�X����v���[���g���[�h�̏����擾
			result = vkGetPhysicalDeviceSurfacePresentModesKHR(
				*pPhysicalDevice,
				_surface,
				&presentModeCount,
				presentModes.data());
			if (result != VK_SUCCESS)
			{
				OutputDebugStringA("Error : vkGetPhysicalDeviceSurfacePresentModesKHR() Failed.");
				return false;
			}

			// �v���[���g���[�h�ꗗ����ݒ肵�������̂����邩����
			for (size_t i = 0; i<presentModes.size(); ++i)
			{
				if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}
				if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
				{
					presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}

			presentModes.clear();
		}

		// �T�[�t�F�C�X���T�|�[�g���Ă���X���b�v�`�F�C���̐���ݒ�
		uint32_t desiredSwapChainImageCount = capabilities.minImageCount + 1;
		if ((capabilities.maxImageCount > 0) && (desiredSwapChainImageCount > capabilities.maxImageCount))
		{
			desiredSwapChainImageCount = capabilities.maxImageCount;
		}

		// �X���b�v�`�F�C���̍쐬
		{
			// �X���b�v�`�F�C���쐬���̐ݒ�
			VkSwapchainCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.surface = _surface;
			createInfo.minImageCount = desiredSwapChainImageCount;
			createInfo.imageFormat = imageFormat;
			createInfo.imageColorSpace = imageColorSpace;
			createInfo.imageExtent = { 1920, 1080 };
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
			createInfo.preTransform = preTransform;
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = presentMode;
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = VK_NULL_HANDLE;

			// �X���b�v�`�F�C���̍쐬
			auto result = vkCreateSwapchainKHR(vkDevice, &createInfo, nullptr, &_swapChain);
			if (result != VK_SUCCESS)
			{
				OutputDebugStringA("Error : vkCreateSwapChainKHR() Failed.");
				return false;
			}
		}

		// �X���b�v�`�F�C���C���[�W�̌����擾
		_swapChainImageCount = 0;
		result = vkGetSwapchainImagesKHR(vkDevice, _swapChain, &_swapChainImageCount, nullptr);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkGetSwapChainImagesKHR() Failed.");
			return false;
		}
	}

	

	// �R�}���h�o�b�t�@�̐���.
	{
		// ���������m��.
		_commandBuffers.resize(_swapChainImageCount);

		// �R�}���h�o�b�t�@�A���P�[�g���̐ݒ�
		VkCommandBufferAllocateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.pNext = nullptr;
		info.commandPool = _commandPool;
		info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		info.commandBufferCount = _swapChainImageCount;

		// �R�}���h�o�b�t�@�p�������̊��蓖��
		auto result = vkAllocateCommandBuffers(vkDevice, &info, _commandBuffers.data());
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkAllocateCommandBuffers() Failed.");
			return false;
		}

		// �R�}���h�o�b�t�@�p�����(?)�̐ݒ�
		VkCommandBufferInheritanceInfo inheritanceInfo = {};
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.pNext = nullptr;
		inheritanceInfo.renderPass = VK_NULL_HANDLE;
		inheritanceInfo.subpass = 0;
		inheritanceInfo.framebuffer = VK_NULL_HANDLE;
		inheritanceInfo.occlusionQueryEnable = VK_FALSE;
		inheritanceInfo.queryFlags = 0;
		inheritanceInfo.pipelineStatistics = 0;

		// �R�}���h�o�b�t�@�L�^�J�n�p�̏��ݒ�
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pNext = nullptr;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = &inheritanceInfo;

		// �R�}���h�̋L�^�J�n
		result = vkBeginCommandBuffer(_commandBuffers[_bufferIndex], &beginInfo);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkBeginCommandBuffer() Failed.");
			return false;
		}
	}

	// �C���[�W�̍쐬
	{
		
		// �o�b�N�o�b�t�@�̃������m��
		_backBuffers.resize(_swapChainImageCount);

		// �X���b�v�`�F�C���̐����C���[�W���쐬
		std::vector<VkImage> images;
		images.resize(_swapChainImageCount);
		auto result = vkGetSwapchainImagesKHR(vkDevice, _swapChain, &_swapChainImageCount, images.data());
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkGetSwapChainImagesKHR() Failed.");
			return false;
		}

		// �C���[�W�����i�[
		for (auto i = 0u; i<_swapChainImageCount; ++i)
		{
			_backBuffers[i]._image = images[i];
		}

		images.clear();
	}

	// �C���[�W�r���[�̐���.
	{
		// �o�b�N�o�b�t�@�̐����C���[�W�r���[���쐬
		for (size_t i = 0; i<_backBuffers.size(); ++i)
		{
			// �r���[���̐ݒ�
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.pNext = nullptr;
			viewInfo.flags = 0;
			viewInfo.image = _backBuffers[i]._image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = imageFormat;
			viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
			viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
			viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
			viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
			viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

			// �C���[�W�r���[�̍쐬
			auto result = vkCreateImageView(vkDevice, &viewInfo, nullptr, &_backBuffers[i]._view);
			if (result != VK_SUCCESS)
			{
				OutputDebugStringA("Error : vkCreateImageView() Failed.");
				return false;
			}

			// �C���[�W�̃��C�A�E�g��ݒ�
			SetImageLayout(
				vkDevice,
				_commandBuffers[_bufferIndex],
				_backBuffers[i]._image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		}
	}

	// �[�x�X�e���V���o�b�t�@�̐���.
	{
		VkFormat depthFormat = VK_FORMAT_D24_UNORM_S8_UINT;

		// �����f�o�C�X����t�H�[�}�b�g�v���p�e�B���擾
		VkImageTiling tiling;
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(*pPhysicalDevice, depthFormat, &props);

		// �t�H�[�}�b�g��ݒ�
		if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			tiling = VK_IMAGE_TILING_LINEAR;
		}
		else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			tiling = VK_IMAGE_TILING_OPTIMAL;
		}
		else
		{
			OutputDebugStringA("Error : Unsupported Format.");
			return false;
		}

		// �C���[�W�쐬���ݒ�
		VkImageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.imageType = VK_IMAGE_TYPE_2D;
		info.format = depthFormat;
		info.extent.width = 1900;
		info.extent.height = 1080;
		info.extent.depth = 1;
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = tiling;
		info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.queueFamilyIndexCount = 0;
		info.pQueueFamilyIndices = nullptr;
		info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		// �C���[�W�̍쐬
		auto result = vkCreateImage(vkDevice, &info, nullptr, &_depth._image);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkCreateImage() Failed.");
			return false;
		}

		// �������A���P�[�g�K�v�����̎擾
		VkMemoryRequirements requirements;
		vkGetImageMemoryRequirements(vkDevice, _depth._image, &requirements);

		VkFlags requirementsMask = 0;
		uint32_t typeBits = requirements.memoryTypeBits;
		uint32_t typeIndex = 0;
		auto*	 pPhysicalDeviceMemProp = IDeviceManager()->getPhysicalDeviceMemProps(0);
		for (auto i = 0u; i<32; ++i)
		{
			if ((typeBits & 0x1) == 1)
			{
				if ((pPhysicalDeviceMemProp->memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
				{
					typeIndex = i;
					break;
				}
			}
			typeBits >>= 1;
		}

		// �������A���P�[�g���̐ݒ�
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.allocationSize = requirements.size;
		allocInfo.memoryTypeIndex = typeIndex;

		// �������m��
		result = vkAllocateMemory(vkDevice, &allocInfo, nullptr, &_depth._memory);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkAllocateMemory() Failed.");
			return false;
		}

		// �C���[�W�Ɋ��蓖��
		result = vkBindImageMemory(vkDevice, _depth._image, _depth._memory, 0);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkBindImageMemory() Failed.");
			return false;
		}

		// �C���[�W�r���[�쐬���̐ݒ�
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.pNext = nullptr;
		viewInfo.image = _depth._image;
		viewInfo.format = depthFormat;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.flags = 0;

		// �C���[�W�r���[�̍쐬
		result = vkCreateImageView(vkDevice, &viewInfo, nullptr, &_depth._view);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkCreateImageView() Failed.");
			return false;
		}

		// �C���[�W���C�A�E�g�̐ݒ�
		SetImageLayout(
			vkDevice,
			_commandBuffers[_bufferIndex],
			_depth._image,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	// �t���[���o�b�t�@�̐���.
	{
		VkImageView attachments[2];

		// �t���[���o�b�t�@���̐ݒ�
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.renderPass = VK_NULL_HANDLE;
		info.attachmentCount = 2;
		info.pAttachments = attachments;
		info.width = 1900;
		info.height = 1080;
		info.layers = 1;

		// �X���b�v�`�F�C���̐��Ń������m��
		_frameBuffers.resize(_swapChainImageCount);
		// �X���b�v�`�F�C���̐����t���[���o�b�t�@���쐬
		for (auto i = 0u; i<_swapChainImageCount; ++i)
		{
			attachments[0] = _backBuffers[i]._view;
			attachments[1] = _depth._view;
			auto result = vkCreateFramebuffer(vkDevice, &info, nullptr, &_frameBuffers[i]);
			if (result != VK_SUCCESS)
			{
				OutputDebugStringA("Error : vkCreateFramebuffer() Failed.");
				return false;
			}
		}
	}

	// �R�}���h�����s���Ă���.
	{
		// �R�}���h�̋L�^�I��
		auto result = vkEndCommandBuffer(_commandBuffers[_bufferIndex]);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkEndCommandBuffer() Failed.");
			return false;
		}

		VkPipelineStageFlags pipeStageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

		// �T�u�~�b�g���̐ݒ�
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext = nullptr;
		info.waitSemaphoreCount = 0;
		info.pWaitSemaphores = nullptr;
		info.pWaitDstStageMask = &pipeStageFlags;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &_commandBuffers[_bufferIndex];
		info.signalSemaphoreCount = 0;
		info.pSignalSemaphores = nullptr;

		// �R�}���h�̎��s
		result = vkQueueSubmit(_graphicsQueue, 1, &info, VK_NULL_HANDLE);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkQueueSubmit() Failed.");
			return false;
		}

		// �R�}���h�����ҋ@
		result = vkQueueWaitIdle(_graphicsQueue);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkQueueWaitIdle() Failed.");
			return false;
		}
	}

	// �t���[����p��.
	{
		// �\���C���[�W�̍쐬
		auto result = vkAcquireNextImageKHR(
			vkDevice,
			_swapChain,
			UINT64_MAX,
			_graphicsSemaphore,
			VK_NULL_HANDLE,
			&_bufferIndex);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkAcquireNextImageKHR() Failed.");
		}
	}

	return true;


	return true;
}

//---------------------------------------------------------------------------
//! �I��������
//---------------------------------------------------------------------------
void vkMain::cleanup()
{
	auto& vkDevice = IDeviceManager()->getVkDevice();
	auto& vkInstance = IDeviceManager()->getVkInstance();

	for (auto i = 0u; i<_swapChainImageCount; ++i)
	{
		vkDestroyFramebuffer(vkDevice, _frameBuffers[i], nullptr);
	}

	if (_depth._view != VK_NULL_HANDLE)
	{
		vkDestroyImageView(vkDevice, _depth._view, nullptr);
	}

	if (_depth._image != VK_NULL_HANDLE)
	{
		vkDestroyImage(vkDevice, _depth._image, nullptr);
	}

	if (_depth._memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(vkDevice, _depth._memory, nullptr);
	}

	if (!_commandBuffers.empty())
	{
		vkFreeCommandBuffers(vkDevice, _commandPool, _swapChainImageCount, _commandBuffers.data());
	}

	if (_commandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(vkDevice, _commandPool, nullptr);
	}

	if (_graphicsSemaphore != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(vkDevice, _graphicsSemaphore, nullptr);
	}

	if (_graphicsFence != VK_NULL_HANDLE)
	{
		vkDestroyFence(vkDevice, _graphicsFence, nullptr);
	}

	if (_swapChain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(vkDevice, _swapChain, nullptr);
	}

	if (_surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(vkInstance, _surface, nullptr);
	}

	
	_frameBuffers.clear();
	
	_commandBuffers.clear();

	_graphicsFamilyIndex = 0;

	_surface			= VK_NULL_HANDLE;
	_swapChain			= VK_NULL_HANDLE;
	_commandPool		= VK_NULL_HANDLE;
	_graphicsSemaphore	= VK_NULL_HANDLE;
	_graphicsFence		= VK_NULL_HANDLE;
	_graphicsQueue		= VK_NULL_HANDLE;
}

//---------------------------------------------------------------------------
//! �`�揈��
//---------------------------------------------------------------------------
void vkMain::render()
{
	//ASVK_UNUSED(args);
	auto cmd = _commandBuffers[_bufferIndex];

	// �f�o�C�X�擾
	auto& vkDevice = IDeviceManager()->getVkDevice();

	// �R�}���h�L�^�J�n.
	{
		VkCommandBufferInheritanceInfo inheritanceInfo = {};
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.pNext = nullptr;
		inheritanceInfo.renderPass = VK_NULL_HANDLE;
		inheritanceInfo.subpass = 0;
		inheritanceInfo.framebuffer = _frameBuffers[_bufferIndex];
		inheritanceInfo.occlusionQueryEnable = VK_FALSE;
		inheritanceInfo.queryFlags = 0;
		inheritanceInfo.pipelineStatistics = 0;

		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.pNext = nullptr;
		cmdBeginInfo.flags = 0;
		cmdBeginInfo.pInheritanceInfo = &inheritanceInfo;

		vkBeginCommandBuffer(cmd, &cmdBeginInfo);
	}

	// �J���[�o�b�t�@���N���A.
	{
		VkClearColorValue clearColor;
		clearColor.float32[0] = 0.392156899f;
		clearColor.float32[1] = 0.584313750f;
		clearColor.float32[2] = 0.929411829f;
		clearColor.float32[3] = 1.0f;

		VkImageSubresourceRange range;
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		vkCmdClearColorImage(
			cmd,
			_backBuffers[_bufferIndex]._image,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			&clearColor,
			1,
			&range);
	}

	// �[�x�o�b�t�@���N���A.
	{
		VkClearDepthStencilValue clearDepthStencil;
		clearDepthStencil.depth = 1.0f;
		clearDepthStencil.stencil = 0;

		VkImageSubresourceRange range;
		range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		vkCmdClearDepthStencilImage(
			cmd,
			_depth._image,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			&clearDepthStencil,
			1,
			&range);
	}

	// ���\�[�X�o���A�̐ݒ�.
	{
		VkImageMemoryBarrier barrier;
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.image = _backBuffers[_bufferIndex]._image;

		vkCmdPipelineBarrier(
			cmd,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier);
	}

	// �R�}���h�̋L�^���I��.
	vkEndCommandBuffer(cmd);

	// �R�}���h�����s���C�\������.
	{
		VkPipelineStageFlags pipeStageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext = nullptr;
		info.waitSemaphoreCount = 0;
		info.pWaitSemaphores = nullptr;
		info.pWaitDstStageMask = &pipeStageFlags;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &_commandBuffers[_bufferIndex];
		info.signalSemaphoreCount = 0;
		info.pSignalSemaphores = nullptr;

		// �R�}���h�����s.
		auto result = vkQueueSubmit(_graphicsQueue, 1, &info, _graphicsFence);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkQueueSubmit() Failed.");
		}

		// ������ҋ@.
		result = vkWaitForFences(vkDevice, 1, &_graphicsFence, VK_TRUE, TimeOutNanoSec);

		// ����������\��.
		if (result == VK_SUCCESS)
		{
			VkPresentInfoKHR present = {};
			present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			present.pNext = nullptr;
			present.swapchainCount = 1;
			present.pSwapchains = &_swapChain;
			present.pImageIndices = &_bufferIndex;
			present.pWaitSemaphores = nullptr;
			present.waitSemaphoreCount = 0;
			present.pResults = nullptr;

			result = vkQueuePresentKHR(_graphicsQueue, &present);
			if (result != VK_SUCCESS)
			{
				OutputDebugStringA("Error : vkQueuePresentKHR() Failed.");
			}
		}
		else if (result == VK_TIMEOUT)
		{
			OutputDebugStringA("Error : vkWaitForFences() Timeout.");
		}

		// �t�F���X�����Z�b�g.
		result = vkResetFences(vkDevice, 1, &_graphicsFence);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkResetFences() Failed.");
		}

		// ���̉摜���擾.
		result = vkAcquireNextImageKHR(
			vkDevice,
			_swapChain,
			TimeOutNanoSec,
			_graphicsSemaphore,
			VK_NULL_HANDLE,
			&_bufferIndex);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkAcquireNextImageKHR() Failed.");
		}
	}
}

//---------------------------------------------------------------------------
// END OF FILE
//---------------------------------------------------------------------------
