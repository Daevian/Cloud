// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
// TODO: reference additional headers your program requires here

#define USE_DIRECTX12

#include <string>
#include <sstream>
#include <fstream>

#ifdef USE_DIRECTX12
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include "DirectX-Graphics-Samples/Libraries/D3DX12/d3dx12.h"
#else
#include <d3d11_1.h>
#include <d3dcompiler.h>
#endif

#include <mmsyscom.h>

#include <vector>
#include <array>
#include <span>
#include <map>
#include <math.h>
#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>

#include "utilities/Windows.h"
#include "utilities/ClMemory.h"
#include "utilities/ClFile.h"

#include "utilities/DefinesAsserts.h"
#include "utilities/DefinesMacros.h"
#include "utilities/DefinesTypes.h"

#include "utilities/IntrinsicFunctions.h"
#include "utilities/StaticArray.h"
#include "utilities/DynamicArray.h"

#include "utilities/MathUtilities.h"
#include "utilities/Float4.h"
#include "utilities/Float3.h"
#include "utilities/Float2.h"
#include "utilities/Matrix4.h"

#include "utilities/ClString.h"
#include "utilities/ClStringStream.h"

#include "debugging/DebugLog.h"

#include "DirectxHelpers.h"

#include "jsoncpp-src-0.5.0/include/json/json.h"
#include "luacpp/state_ex.h"
#include "imgui.h"

#include "assimp-3.3.1/include/assimp/Importer.hpp"
#include "assimp-3.3.1/include/assimp/scene.h"
#include "assimp-3.3.1/include/assimp/mesh.h"
#include "assimp-3.3.1/include/assimp/vector3.h"