//---------------------------------------------------------------------------
//
// @file	main.cpp
// @brief	処理メイン
// @author	Yuki.Ishigaki
//
//---------------------------------------------------------------------------
#include "stdafx.h"


namespace mySystem
{
	//----------------------------------------------------------
	//! 初期化
	//----------------------------------------------------------
	bool main::initialize(HINSTANCE& hInstance, HWND& hWnd)
	{
		return vkMain()->initialize(hInstance, hWnd);
	}

	//----------------------------------------------------------
	//! メインループ
	//----------------------------------------------------------
	void main::mainLoop(MSG & msg, HACCEL hAccelTable)
	{
		// メイン メッセージ ループ:
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			vkMain()->render();
		}
	}

}



//---------------------------------------------------------------------------
// END OF FILE
//---------------------------------------------------------------------------