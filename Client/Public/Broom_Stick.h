#pragma once

/* =============================================== */
//	[CBroom_Stick]
// 
//	¡§ : æ»√∂πŒ
//	∫Œ :
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

class CBroom_Stick : public CParts
{
public:
	typedef struct tagCWeapon_Player_BroomDesc
	{
		PARENTMATRIXDESC ParentMatrixDesc;
	}CWEAPON_PLAYER_BROOM_DESC;
private:
	explicit CBroom_Stick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBroom_Stick(const CBroom_Stick& rhs);
	virtual ~CBroom_Stick() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void HeadParticle(_float fTimeDelta);

private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CParticleSystem*	m_pParticle = { nullptr };
private:
	_float				AccTime = 0.f;
	_float				DelayTime = 0.0f;

private:
	void Tick_Debug();

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Set_Shader_Resources();

public:
	static CBroom_Stick* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBroom_Stick* Clone(void* pArg) override;
	virtual void Free() override;
};

END