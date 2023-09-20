#pragma once

/* =============================================== */
//	[CWeapon_Fig_Wand]
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

class CWeapon_Fig_Wand : public CParts
{
private:
	explicit CWeapon_Fig_Wand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_Fig_Wand(const CWeapon_Fig_Wand& rhs);
	virtual ~CWeapon_Fig_Wand() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float4x4 Get_Wand_Point_OffsetMatrix() { 
		return m_WandPointOffsetMatrix; 
	}

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

private:
	_float4x4 m_WandPointOffsetMatrix = {};

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Set_Shader_Resources();

public:
	static CWeapon_Fig_Wand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_Fig_Wand* Clone(void* pArg) override;
	virtual void Free() override;
};

END