#pragma once

/* =============================================== */
//	[CWeapon_DarkWizard_Wand]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Parts.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CWeapon_DarkWizard_Wand : public CParts
{
private:
	explicit CWeapon_DarkWizard_Wand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_DarkWizard_Wand(const CWeapon_DarkWizard_Wand& rhs);
	virtual ~CWeapon_DarkWizard_Wand() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Set_Shader_Resources();

public:
	static CWeapon_DarkWizard_Wand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_DarkWizard_Wand* Clone(void* pArg) override;
	virtual void Free() override;
};

END