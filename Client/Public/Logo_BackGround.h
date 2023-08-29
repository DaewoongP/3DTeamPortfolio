#pragma once
#include "UI.h"

BEGIN(Client)

class CLogo_BackGround final : public CUI
{
private:
	explicit CLogo_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLogo_BackGround(const CLogo_BackGround& rhs);
	virtual ~CLogo_BackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CLogo_BackGround* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END