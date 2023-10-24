#pragma once

/* =============================================== */
//	[CBroom_Stick_DarkWizard]
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
class CParticleSystem;
END

BEGIN(Client)

class CBroom_Stick_DarkWizard : public CParts
{
private:
	explicit CBroom_Stick_DarkWizard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBroom_Stick_DarkWizard(const CBroom_Stick_DarkWizard& rhs);
	virtual ~CBroom_Stick_DarkWizard() = default;

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
	//CParticleSystem*	m_pParticle = { nullptr };

private:
#ifdef _DEBUG
	_float3 m_vPosition;
	_float3 m_vAngle;
	void Tick_Debug();
#endif // _DEBUG

private:
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Set_Shader_Resources();

public:
	static CBroom_Stick_DarkWizard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBroom_Stick_DarkWizard* Clone(void* pArg) override;
	virtual void Free() override;
};

END