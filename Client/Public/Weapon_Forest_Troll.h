#pragma once

/* =============================================== */
//	[CWeapon_Forest_Troll]
// 
//	�� : �ּ�ȯ
//	�� :
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

class CWeapon_Forest_Troll : public CParts
{
private:
	explicit CWeapon_Forest_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_Forest_Troll(const CWeapon_Forest_Troll& rhs);
	virtual ~CWeapon_Forest_Troll() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Set_Shader_Resources();

public:
	static CWeapon_Forest_Troll* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_Forest_Troll* Clone(void* pArg) override;
	virtual void Free() override;
};

END