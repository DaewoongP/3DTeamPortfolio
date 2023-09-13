#pragma once

/* =============================================== */
//	[CRandomChoose]
//	- 가중치에 따른 행동을 랜덤 선택
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CRandomChoose : public CBehavior
{
protected:
	explicit CRandomChoose(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRandomChoose(const CBehavior& rhs);
	virtual ~CRandomChoose() = default;

public:
	virtual HRESULT Initialize_Prototype() { return S_OK; }
	virtual HRESULT Initialize(void* pArg) { return S_OK; }
	virtual HRESULT Tick(const _float & fTimeDelta);

public:
	HRESULT Assemble_Behavior(const wstring& BehaviorTag, CBehavior* pBehavior, const _float& fWeight);
	void Set_Random_Behavior();

protected:
	vector<_float>		m_ChildWeights;

public:
	static CRandomChoose* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CRandomChoose* Clone(void* pArg);
	virtual void Free() override;
};

END