#pragma once

#pragma warning (disable : 4005) // macro redefinition
#pragma warning (disable : 4251) // __declspec(dllexport)
#pragma warning (disable : 26812) // enum class
#pragma warning (disable : 4100) // unused variable
#pragma warning (disable : 6031) // except return
#pragma warning (disable : 4828) // korean comment
#pragma warning (disable : 26495) // Value Initialize
#pragma warning (disable : 6387) // Inout Value NULL
#pragma warning (disable : 4267) // size_t to uint change

#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#include "fx11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\ScreenGrab.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\Effects.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"

#include "fmod\fmod.h"
#include "fmod\fmod.hpp"

#ifdef _DEBUG

#pragma comment(lib, "PhysXd/PhysX_64.lib")
#pragma comment(lib, "PhysXd/PhysXCommon_64.lib")
#pragma comment(lib, "PhysXd/PhysXCooking_64.lib")
#pragma comment(lib, "PhysXd/PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXd/PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXd/PhysXCharacterKinematic_static_64.lib")

#pragma comment(lib, "Clothd/NvClothDEBUG_x64.lib")

#else // Release

#pragma comment(lib, "PhysX/PhysX_64.lib")
#pragma comment(lib, "PhysX/PhysXCommon_64.lib")
#pragma comment(lib, "PhysX/PhysXFoundation_64.lib")
#pragma comment(lib, "PhysX/PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysX/PhysXCharacterKinematic_static_64.lib")

#pragma comment(lib, "Cloth/NvCloth_x64.lib")

#endif // _DEBUG

// physx
#include <PxPhysicsAPI.h>

// nvcloth
#include <Factory.h>
#include <PhaseConfig.h>
#include <DxContextManagerCallback.h>
#include <NvClothExt/ClothFabricCooker.h>
#include <NvClothExt/ClothMeshDesc.h>
#include <Fabric.h>
#include <Cloth.h>
#include <Range.h>
#include <Solver.h>

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <array>
#include <queue>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <functional>
#include <any>
#include <stack>
#include <random>
#include <tchar.h>

// file system - c++ 17
#include <filesystem>

namespace fs = std::filesystem;

using namespace std;
using namespace DirectX;
using namespace physx; // physx
using namespace nv; // nvCloth

#include "DirectXTK\matrix.h"

#include "Engine_Enums.h"
#include "Engine_Function.h"
#include "Engine_TypeDef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"

#include "Engine_Converter.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 

#define New DBG_NEW

#endif // DBG_NEW
#endif // _DEBUG

#ifndef _DEBUG

#define New new

#endif // _DEBUG

using namespace Engine;
