// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
// TODO: reference additional headers your program requires here

#include <string>
#include <sstream>
#include <fstream>

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <mmsyscom.h>

#include <vector>
#include <array>
#include <math.h>
#include <algorithm>
#include <functional>
#include <memory>

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
//#include "lua-5.3.2/src/lua.hpp"