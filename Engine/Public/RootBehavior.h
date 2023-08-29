#pragma once

/*
*	CRootBehavior
*	정 : 주성환
*	부 : 
*/

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
	virtual HRESULT Tick(const _float & fTimeDelta) override;

public:
	HRESULT Add_Type(const string& strTypename, any Type);

public:
	static CRootBehavior* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CRootBehavior* Clone(void* pArg);
	virtual void Free() override;

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;

	HRESULT Add_Render_Debug_Group(const _float2& vFontPosition, class CRenderer* pRenderer);

private:
	_float2 m_vFontPosition;
	stack<wstring> m_DebugBehaviorTags;
#endif // _DEBUG
};

END