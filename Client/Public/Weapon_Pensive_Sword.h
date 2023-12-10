#pragma once

/* =============================================== */
//	[CWeapon_Pensive_Sword]
// 
//	¡§ : æ»√∂πŒ
//	∫Œ :
//
/* =============================================== */

#include "Parts.h"
#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CWeapon_Pensive_Sword : public CParts
{
private:
	explicit CWeapon_Pensive_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_Pensive_Sword(const CWeapon_Pensive_Sword& rhs);
	virtual ~CWeapon_Pensive_Sword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
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
	static CWeapon_Pensive_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_Pensive_Sword* Clone(void* pArg) override;
	virtual void Free() override;
};

END