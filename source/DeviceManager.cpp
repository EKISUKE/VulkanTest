//---------------------------------------------------------------------------
//
// @file	DeviceManager.cpp
// @brief	デバイス管理
// @author	Yuki.Ishigaki
//
//---------------------------------------------------------------------------
#include "stdafx.h"

//---------------------------------------------------------------------------
//	メモリ確保処理.
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
//	メモリ再確保処理.
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
// メモリ解放処理.
//---------------------------------------------------------------------------
VKAPI_ATTR
void VKAPI_CALL Free(void* pUserData, void* pMemory)
{
	ASVK_UNUSED(pUserData);
	_aligned_free(pMemory);
}



//---------------------------------------------------------------------------
// 初期化
//---------------------------------------------------------------------------
bool DeviceManager::initialize()
{
	// インスタンスの生成.
	{
		std::array<const char*, 2> extensions;
		extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
		extensions[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;

		// アプリケーション情報の設定
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "SampleApp";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "asvk";
		appInfo.engineVersion = 1;
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// インスタンス生成時の情報の設定
		VkInstanceCreateInfo instanceInfo = {};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pNext = nullptr;
		instanceInfo.flags = 0;
		instanceInfo.pApplicationInfo = &appInfo;
		instanceInfo.enabledLayerCount = 0;
		instanceInfo.ppEnabledLayerNames = nullptr;
		instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceInfo.ppEnabledExtensionNames = extensions.data();

		// メモリ確保時の処理のコールバック関数を設定
		_allocatorCallbacks.pfnAllocation = Alloc;
		_allocatorCallbacks.pfnFree = Free;
		_allocatorCallbacks.pfnReallocation = Realloc;
		_allocatorCallbacks.pfnInternalAllocation = nullptr;
		_allocatorCallbacks.pfnInternalFree = nullptr;

		// インスタンスの作成
		auto result = vkCreateInstance(&instanceInfo, &_allocatorCallbacks, &_vkInstance);
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkCreatInstance() Failed.");
			return false;
		}
	}

	// 物理デバイスの取得.
	{
		// まずは物理デバイスの数を取得する
		uint32_t count = 0;
		auto result = vkEnumeratePhysicalDevices(_vkInstance, &count, nullptr);
		if (result != VK_SUCCESS || count < 1)
		{
			OutputDebugStringA("Error : vkEnumeratePhysicalDevices() Failed.");
			return false;
		}

		// 物理デバイスの数に応じてデバイス情報を格納する配列を用意
		std::vector<VkPhysicalDevice> physicalDevices;
		physicalDevices.resize(count);

		// デバイス情報の取得
		result = vkEnumeratePhysicalDevices(_vkInstance, &count, physicalDevices.data());
		if (result != VK_SUCCESS)
		{
			OutputDebugStringA("Error : vkEnumeratePhysicalDevices() Failed.");
			return false;
		}

		// Gpu情報格納用配列をリサイズ
		_Gpus.resize(count);

		// カウント分デバイスのメモリ情報を取得
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
// 解放
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