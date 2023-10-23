#pragma once

/* =============================================== */
//	[CParts]
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CParts abstract : public CGameObject
{
public:
	 typedef struct tagParentMatrixDesc
	 {
		_float4x4        OffsetMatrix;
		_float4x4        PivotMatrix;
		const _float4x4* pCombindTransformationMatrix;
		const _float4x4* pParentWorldMatrix;
	 }PARENTMATRIXDESC;

protected:
	explicit CParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CParts(const CGameObject& rhs);
	virtual ~CParts() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	/* Parts���� Transform�� ����� ���� ��������� �ϱ� ������ 
	Parts�� Late_Tick()�� ���� ȣ�� �ϰ� �ڽ��� Late_Tick()�� �����ؾ� �Ѵ�. */
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	PARENTMATRIXDESC m_ParentMatrixDesc;

public:
	virtual CParts* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END