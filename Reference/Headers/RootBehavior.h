#pragma once

/* =============================================== */
//	[CRootBehavior]
//	- CBehavior 클래스 헤더의 설명 참고.
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CRootBehavior final : public CBehavior
{
private:
	explicit CRootBehavior(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CRootBehavior(const CRootBehavior& rhs);
	virtual ~CRootBehavior() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Tick_Behavior(const _float & fTimeDelta) override;

public:
	HRESULT Add_Type(const string& strTypename, any Type);

public:
	static CRootBehavior* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CRootBehavior* Clone(void* pArg);
	virtual void Free() override;

#ifdef _DEBUG
public:
	vector<wstring> Get_DebugBahaviorTags() const {
		return m_DebugBehaviorTags;
	}

private:
	vector<wstring> m_DebugBehaviorTags;
#endif // _DEBUG
};

END