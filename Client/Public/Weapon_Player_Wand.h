#pragma once

/* =============================================== */
//	[CWeapon_Player_Wand]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Parts.h"
#include "Light.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CWeapon_Player_Wand : public CParts
{
public:
	typedef struct tagCWeapon_Player_WandDesc
	{
		PARENTMATRIXDESC ParentMatrixDesc;
		_bool* pisLightOn;
	}CWEAPON_PLAYER_WAND_DESC;
private:
	explicit CWeapon_Player_Wand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_Player_Wand(const CWeapon_Player_Wand& rhs);
	virtual ~CWeapon_Player_Wand() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Do_Lumos(_float fTimeDelta);
	void HeadParticle(_float fTimeDelta);
private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CParticleSystem*	m_pHeadEffectCom = { nullptr };

private:
	_bool*				m_pisLightOn = { nullptr };
	_float4				m_LightIntensity = { 0.f,0.f,0.f,0.f };
	_float				AccTime = 0.f;
	_float				DelayTime = 0.0f;
	_bool				m_isPreLight = { false };

private:
	class CLight*		m_pLight = { nullptr };
	class CLight*		m_pVaultLight = { nullptr };
	CLight::LIGHTDESC	m_PrevDesc;

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Set_Shader_Resources();

public:
	static CWeapon_Player_Wand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_Player_Wand* Clone(void* pArg) override;
	virtual void Free() override;
};

END