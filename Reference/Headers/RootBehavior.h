#pragma once

#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CRoot final : public CBehavior
{
private:
	explicit CRoot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CRoot(const CRoot& rhs);
	virtual ~CRoot() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float & fTimeDelta) override;

public:
	HRESULT Add_Type(const string& strTypename, any Type);

public:
	static CRoot* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CRoot* Clone(void* pArg);
	virtual void Free() override;
};

END