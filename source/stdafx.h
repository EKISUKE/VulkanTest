// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>

// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <array>
#include <cassert>


// Vulkan関連
#include <vulkan/vulkan.h>


#ifndef ASVK_UNUSED
#define ASVK_UNUSED(x)              ((void)x)
#endif//ASVK_UNUSED

// 自作関連
#include "DefineStruct.h"
#include "DeviceManager.h"
#include "vulkanMain.h"
#include "main.h"

