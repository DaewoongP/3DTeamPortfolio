#pragma once

#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)
class CMagicSlot;
END

BEGIN(Client)

class CEnergyBall final : public CGameObject
{
private:
	explicit CEnergyBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnergyBall(const CEnergyBall& rhs);
	virtual ~CEnergyBall() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

public:
	void Set_Protego_Collision(CTransform* pTransform, CEnemy::ATTACKTYPE eType) const;

private:
	CMagicSlot* m_pMagicSlot = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CModel* m_pModel = { nullptr };

private:
	HRESULT Make_Magics();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(const _uint& iLevel);
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

public:
	static CEnergyBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
