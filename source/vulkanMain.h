//---------------------------------------------------------------------------
//!
//! @file	vulkanMain.h
//! @brief	Vulkan�̃��C������
//! @author	Yuki.Ishigaki
//!
//---------------------------------------------------------------------------
#pragma once


//---------------------------------------------------------------------------
//! @class Vulkan���C��
//---------------------------------------------------------------------------
class vkMain
{
public:
	//! ������
	bool initialize(HINSTANCE& hInstance, HWND& hWnd);

	//! �I��������
	void cleanup();

	//! �`�揈��
	void render();

	//! �C���X�^���X�擾
	static vkMain* getInstance() {
		static vkMain _myIsntance;
		return &_myIsntance;
	}

private:
	//---- �O���ł̃C���X�^���X�쐬�Ȃǂ��֎~���A�V���O���g����
	//! �R���X�g���N�^
	vkMain() {}
	//! �R�s�[�R���X�g���N�^
	vkMain(vkMain& main) {}
	//! �f�X�g���N�^
	virtual ~vkMain() {}

private:
	//---- �����o�ϐ�
	uint32_t						_swapChainImageCount;
	static constexpr uint64_t		TimeOutNanoSec = 100000000;
	VkQueue                         _graphicsQueue;
	VkFence                         _graphicsFence;
	VkSemaphore                     _graphicsSemaphore;
	uint32_t                        _graphicsFamilyIndex;
	VkSurfaceKHR                    _surface;
	VkSwapchainKHR                  _swapChain;
	std::vector<Texture>            _backBuffers;
	uint32_t                        _bufferIndex;
	VkCommandPool                   _commandPool;
	std::vector<VkCommandBuffer>    _commandBuffers;
	DepthStencilBuffer              _depth;
	std::vector<VkFramebuffer>      _frameBuffers;

};

//---------------------------------------------------------------------------
//! �A�N�Z�X�C���^�[�t�F�C�X
//---------------------------------------------------------------------------
FORCEINLINE
vkMain* vkMain()
{
	return vkMain::getInstance();
}

//---------------------------------------------------------------------------
// END OF FILE
//---------------------------------------------------------------------------
