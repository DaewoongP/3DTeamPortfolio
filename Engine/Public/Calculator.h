#pragma once
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
	// Ư�� �ð����� true ��ȯ
	_bool Timer(_float fAlarmTime, _float fTimeDelta);
	_float4 Get_RandomVectorInSphere(_float fRadius);
	template<typename T>
	inline void Clamp(T& _value, T _min, T _max);

private:
	_float	m_fAlarmTimeAcc = { 0.f };

public:
	virtual void Free() override;
};

END