#pragma once
/* =============================================== */
// 
//	정 : 박대웅
//	부 : 
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class CCalculator final : public CBase
{
	DECLARE_SINGLETON(CCalculator)

public:
	explicit CCalculator();
	virtual ~CCalculator() = default;

public:
	HRESULT Get_MouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _float4x4 PickingWorldMatrix_Inverse, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir);
	HRESULT Get_WorldMouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir);
	_bool IsMouseInClient(ID3D11DeviceContext* pContext, HWND hWnd);
	// 1번 인자 : 가중치를 벡터에 대입 (전체 사이즈보다 작아야함)
	// 2번 인자 : 랜덤으로 뽑을 사이즈 대입
	// 반환 : 사이즈보다 작은값중 하나를 반환
	// 벡터사이즈와 값이 다를경우 임의로 벡터에 남은 퍼센트를 분배해서 넣어줌.
	// 루프를 돌아야하므로 효율성을 높이려면 사이즈를 맞춰주면 좋음.
	_int RandomChoose(vector<_float> Weights, _uint iChooseSize);
	_float4 Get_RandomVectorInSphere(_float fRadius);
	HRESULT ReadFileInDirectory(_Inout_ vector<wstring>& OutVector, const _tchar* pFilePath, const _tchar* pExt);

	_float3 PolarToCartesian(_float _fLength, _float _fTheta, _float fPhi); // 3차원
	_float2 PolarToCartesian(_float _fLength, _float _fTheta); // 2차원
	_float3 GetVectorSlerp(_float3 v1, _float3 v2, _float3 vUp, _float k, _float f);

	// 두 점으로 직교하는 right, up, look벡터를 채워서 반환해줌.
	// ex) _float4x4 ResultMatrix = RightUpLook_In_Vectors(vPos, vCamPos);
	_float4x4 RightUpLook_In_Vectors(_float3 vSourPos, _float3 vDestPos);
public:
	virtual void Free() override;
};

END