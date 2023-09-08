#pragma once
struct ENGINE_DLL SHADER_NODE
{
	_bool isActivate = { false };
};

struct ENGINE_DLL TIME_NODE : public SHADER_NODE
{
	TIME_NODE() : SHADER_NODE() {};

	void Tick(_float fTimeDelta);
	_float fTimeAcc = { 0.f };
	_float fSineTime = { 0.f };
	_float fCosTime = { 0.f };
	_float fDeltaTime = { 0.f };
};

struct ENGINE_DLL TILING_AND_OFFSET : public SHADER_NODE
{

	_float2 vOffset = { 0.f, 0.f };
};

struct ENGINE_DLL MESHEFFECT
{
	TIME_NODE timeNodeDesc;
	
};