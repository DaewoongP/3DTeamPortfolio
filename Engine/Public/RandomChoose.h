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

class ENGINE_DLL CRandomChoose abstract : public CBehavior
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
	virtual HRESULT Assemble_Behavior(const wstring& BehaviorTag, CBehavior* pBehavior, const _float& fWeight);
	virtual void Set_Random_Behavior();
	virtual void Reset_Behavior(HRESULT result) {
		(*m_iterCurBehavior)->Reset_Behavior(result);
	}

protected:
	vector<_float>		m_ChildWeights;

protected:
	virtual HRESULT Assemble_Childs() override { return S_OK; }

public:
	virtual void Free() override;
};

END