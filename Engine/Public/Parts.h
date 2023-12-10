#pragma once

/* =============================================== */
//	[CParts]
// 
//	정 : 주성환
//	부 :
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
	/* Parts에서 Transform의 행렬을 먼저 갱신해줘야 하기 때문에 
	Parts의 Late_Tick()을 먼저 호출 하고 자식의 Late_Tick()을 진행해야 한다. */
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	PARENTMATRIXDESC m_ParentMatrixDesc;

public:
	virtual CParts* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END