//---------------------------------------------------------------------------
//!
//! @file	vulkanMain.h
//! @brief	Vulkanのメイン処理
//! @author	Yuki.Ishigaki
//!
//---------------------------------------------------------------------------
#pragma once


//---------------------------------------------------------------------------
//! @class Vulkanメイン
//---------------------------------------------------------------------------
class vkMain
{
public:
	//! 初期化
	bool initialize(HINSTANCE& hInstance, HWND& hWnd);

	//! 終了時処理
	void cleanup();

	//! 描画処理
	void render();

	//! インスタンス取得
	static vkMain* getInstance() {
		static vkMain _myIsntance;
		return &_myIsntance;
	}

private:
	//---- 外部でのインスタンス作成などを禁止し、シングルトン化
	//! コンストラクタ
	vkMain() {}
	//! コピーコンストラクタ
	vkMain(vkMain& main) {}
	//! デストラクタ
	virtual ~vkMain() {}

private:
	//---- メンバ変数
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
//! アクセスインターフェイス
//---------------------------------------------------------------------------
FORCEINLINE
vkMain* vkMain()
{
	return vkMain::getInstance();
}

//---------------------------------------------------------------------------
// END OF FILE
//---------------------------------------------------------------------------
