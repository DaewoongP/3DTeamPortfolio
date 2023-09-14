#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//	�� : 
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
	// 1�� ���� : ����ġ�� ���Ϳ� ���� (��ü ������� �۾ƾ���)
	// 2�� ���� : �������� ���� ������ ����
	// ��ȯ : ������� �������� �ϳ��� ��ȯ
	// ���ͻ������ ���� �ٸ���� ���Ƿ� ���Ϳ� ���� �ۼ�Ʈ�� �й��ؼ� �־���.
	// ������ ���ƾ��ϹǷ� ȿ������ ���̷��� ����� �����ָ� ����.
	_int RandomChoose(vector<_float> Weights, _uint iChooseSize);
	_float4 Get_RandomVectorInSphere(_float fRadius);
	HRESULT ReadFileInDirectory(_Inout_ vector<wstring>& OutVector, const _tchar* pFilePath, const _tchar* pExt);

	_float3 PolarToCartesian(_float _fLength, _float _fTheta, _float fPhi); // 3����
	_float2 PolarToCartesian(_float _fLength, _float _fTheta); // 2����
	_float3 GetVectorSlerp(_float3 v1, _float3 v2, _float3 vUp, _float k, _float f);

	// �� ������ �����ϴ� right, up, look���͸� ä���� ��ȯ����.
	// ex) _float4x4 ResultMatrix = RightUpLook_In_Vectors(vPos, vCamPos);
	_float4x4 RightUpLook_In_Vectors(_float3 vSourPos, _float3 vDestPos);
public:
	virtual void Free() override;
};

END