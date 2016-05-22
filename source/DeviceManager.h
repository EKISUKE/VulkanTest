//---------------------------------------------------------------------------
//!
// @file	DeviceManager.h
// @brief	�f�o�C�X�Ǘ�
// @author	Yuki.Ishigaki
//!
//---------------------------------------------------------------------------
#pragma once

//---------------------------------------------------------------------------
//! @class �f�o�C�X�Ǘ�
//---------------------------------------------------------------------------
class DeviceManager
{
public:
	//! ������
	bool initialize();
	
	//! ���
	void cleanup();

	//! �C���X�^���X�擾
	static DeviceManager* getInstance() {
		static DeviceManager _myIsntance;
		return &_myIsntance;
	}

	//! �����f�o�C�X�̎擾
	VkPhysicalDevice* getPhysicalDevice(int gpuIndex) {
		if (!_Gpus.empty()) {
			return &(_Gpus[gpuIndex]._device);
		}
		return nullptr;
	}

	//! �����f�o�C�X�̃v���p�e�B�擾
	VkPhysicalDeviceMemoryProperties* getPhysicalDeviceMemProps(int gpuIndex) {
		if (!_Gpus.empty()) {
			return &(_Gpus[gpuIndex]._memoryProps);
		}
		return nullptr;
	}

	//! �f�o�C�X�̎擾
	VkDevice& getVkDevice() { return _vkDevice; }

	//! Vulkan�C���X�^���X�̎擾
	VkInstance& getVkInstance() { return _vkInstance; }


private:
	//---- �O���ł̃C���X�^���X�쐬�Ȃǂ��֎~���A�V���O���g����
	//! �R���X�g���N�^
	DeviceManager() {}

	//! �R�s�[�R���X�g���N�^
	DeviceManager(DeviceManager& deviceManager) {}
	//! �f�X�g���N�^
	virtual ~DeviceManager() {}


private:
	// �����o�ϐ�
	VkInstance						_vkInstance;
	VkDevice						_vkDevice;
	std::vector<Gpu>				_Gpus;
	VkAllocationCallbacks           _allocatorCallbacks;
};

//---------------------------------------------------------------------------
//! �A�N�Z�X�C���^�[�t�F�C�X
//---------------------------------------------------------------------------
FORCEINLINE
DeviceManager* IDeviceManager()
{
	return DeviceManager::getInstance();
}


//---------------------------------------------------------------------------
// END OF FILE
//---------------------------------------------------------------------------