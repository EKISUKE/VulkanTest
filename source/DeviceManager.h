//---------------------------------------------------------------------------
//!
// @file	DeviceManager.h
// @brief	デバイス管理
// @author	Yuki.Ishigaki
//!
//---------------------------------------------------------------------------
#pragma once

//---------------------------------------------------------------------------
//! @class デバイス管理
//---------------------------------------------------------------------------
class DeviceManager
{
public:
	//! 初期化
	bool initialize();
	
	//! 解放
	void cleanup();

	//! インスタンス取得
	static DeviceManager* getInstance() {
		static DeviceManager _myIsntance;
		return &_myIsntance;
	}

	//! 物理デバイスの取得
	VkPhysicalDevice* getPhysicalDevice(int gpuIndex) {
		if (!_Gpus.empty()) {
			return &(_Gpus[gpuIndex]._device);
		}
		return nullptr;
	}

	//! 物理デバイスのプロパティ取得
	VkPhysicalDeviceMemoryProperties* getPhysicalDeviceMemProps(int gpuIndex) {
		if (!_Gpus.empty()) {
			return &(_Gpus[gpuIndex]._memoryProps);
		}
		return nullptr;
	}

	//! デバイスの取得
	VkDevice& getVkDevice() { return _vkDevice; }

	//! Vulkanインスタンスの取得
	VkInstance& getVkInstance() { return _vkInstance; }


private:
	//---- 外部でのインスタンス作成などを禁止し、シングルトン化
	//! コンストラクタ
	DeviceManager() {}

	//! コピーコンストラクタ
	DeviceManager(DeviceManager& deviceManager) {}
	//! デストラクタ
	virtual ~DeviceManager() {}


private:
	// メンバ変数
	VkInstance						_vkInstance;
	VkDevice						_vkDevice;
	std::vector<Gpu>				_Gpus;
	VkAllocationCallbacks           _allocatorCallbacks;
};

//---------------------------------------------------------------------------
//! アクセスインターフェイス
//---------------------------------------------------------------------------
FORCEINLINE
DeviceManager* IDeviceManager()
{
	return DeviceManager::getInstance();
}


//---------------------------------------------------------------------------
// END OF FILE
//---------------------------------------------------------------------------