//---------------------------------------------------------------------------
//
// @file	DeviceManager.cpp
// @brief	�f�o�C�X�Ǘ�
// @author	Yuki.Ishigaki
//
//---------------------------------------------------------------------------
#include "stdafx.h"

//---------------------------------------------------------------------------
//	�������m�ۏ���.
//---------------------------------------------------------------------------
VKAPI_ATTR
void* VKAPI_CALL Alloc
(
	void* pUserData,
	size_t size,
	size_t alignment,
	VkSystemAllocationScope scope
)
{
	ASVK_UNUSED(pUserData);
	ASVK_UNUSED(scope);
	return _aligned_malloc(size, alignment);
}

//---------------------------------------------------------------------------
//	�������Ċm�ۏ���.
//---------------------------------------------------------------------------
VKAPI_ATTR
void* VKAPI_CALL Realloc
(
	void* pUserData,
	void* pOriginal,
	size_t size,
	size_t alignment,
	VkSystemAllocationScope scope
)
{
	ASVK_UNUSED(pUserData);
	ASVK_UNUSED(scope);
	return _aligned_realloc(pOriginal, size, alignment);
}

//---------------------------------------------------------------------------
// �������������.
//---------------------------------------------------------------------------
VKAPI_ATTR
void VKAPI_CALL Free(void* pUserData, void* pMemory)
{
	ASVK_UNUSED(pUserData);
	_aligned_free(pMemory);
}



//---------------------------------------------------------------------------
// ������
//---------------------------------------------------------------------------
bool DeviceManager::initialize()
{
	// �C���X�^���X�̐���.
	{
		std::array<const char*, 2> extensions;
		extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
		extensions[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;

		// �A�v���P�[�V�������̐ݒ�
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "SampleApp";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "asvk";
		appInfo.engineVersion = 1;
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// �C���X�^���X�������̏��̐ݒ�
		VkInstanceCreateInfo instanceInfo = {};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pNext = nullptr;
		instanceInfo.flags = 0;
		instanceInfo.pApplicationInfo = &appInfo;
		instanceInfo.enabledLayerCount = 0;
		instanceInfo.ppEnabledLayerNames = nullptr;
		instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceInfo.ppEnabledExtensionNames = extensions.data();

		// �������m�ێ��̏����̃R�[���o�b�N�֐���ݒ�
		_allocatorCallbacks.pfnAllocation = Alloc;
		_allocatorCallbacks.pfnFree = Free;
		_allocatorCallbacks.pfnReallocation = Realloc;
		_allocatorCallbacks.pfnInternalAllocation = nullptr;
		_allocatorCallbacks.pfnInternalFree = nullptr;

		// �C���X�^���X�̍쐬
		auto result = vkCreateInstance(&instanceInfo, &_allocatorCallbacks, &_vkInstance);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkCreatInstance() Failed.");
			return false;
		}
	}

	// �����f�o�C�X�̎擾.
	{
		// �܂��͕����f�o�C�X�̐����擾����
		uint32_t count = 0;
		auto result = vkEnumeratePhysicalDevices(_vkInstance, &count, nullptr);
		if (result != VK_SUCCESS || count < 1)
		{
			OutputDebugStringA("Error : vkEnumeratePhysicalDevices() Failed.");
			return false;
		}

		// �����f�o�C�X�̐��ɉ����ăf�o�C�X�����i�[����z���p��
		std::vector<VkPhysicalDevice> physicalDevices;
		physicalDevices.resize(count);

		// �f�o�C�X���̎擾
		result = vkEnumeratePhysicalDevices(_vkInstance, &count, physicalDevices.data());
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkEnumeratePhysicalDevices() Failed.");
			return false;
		}

		// Gpu���i�[�p�z������T�C�Y
		_Gpus.resize(count);

		// �J�E���g���f�o�C�X�̃����������擾
		for (auto i = 0u; i<count; ++i)
		{
			_Gpus[i]._device = physicalDevices[i];
			vkGetPhysicalDeviceMemoryProperties(_Gpus[i]._device, &_Gpus[i]._memoryProps);
		}

		physicalDevices.clear();
	}

	return true;
}

//---------------------------------------------------------------------------
// ���
//---------------------------------------------------------------------------
void DeviceManager::cleanup()
{
	if (_vkDevice != VK_NULL_HANDLE)
	{
		vkDestroyDevice(_vkDevice, nullptr);
	}

	if (_vkInstance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(_vkInstance, &_allocatorCallbacks);
	}

	_Gpus.clear();

	_vkDevice	= VK_NULL_HANDLE;
	_vkInstance = VK_NULL_HANDLE;
}


//---------------------------------------------------------------------------
// END OF FILE
//---------------------------------------------------------------------------