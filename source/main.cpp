//---------------------------------------------------------------------------
//
// @file	main.cpp
// @brief	�������C��
// @author	Yuki.Ishigaki
//
//---------------------------------------------------------------------------
#include "stdafx.h"


namespace mySystem
{
	//----------------------------------------------------------
	//! ������
	//----------------------------------------------------------
	bool main::initialize(HINSTANCE& hInstance, HWND& hWnd)
	{
		return vkMain()->initialize(hInstance, hWnd);
	}

	//----------------------------------------------------------
	//! ���C�����[�v
	//----------------------------------------------------------
	void main::mainLoop(MSG & msg, HACCEL hAccelTable)
	{
		// ���C�� ���b�Z�[�W ���[�v:
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