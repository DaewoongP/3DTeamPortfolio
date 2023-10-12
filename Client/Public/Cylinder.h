#pragma once

#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CCylinder final : public CGameObject
{
public:
	typedef struct tagCylinderInitDesc
	{
		_float3 vPosition;
		_float3 vRight;
		_float3 vSacle;
		_float fRadian = { 0.f };
	}CYLINDERINITDESC;

private:
	explicit CCylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCylinder(const CCylinder& rhs);
	virtual ~CCylinder() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Reset(const CYLINDERINITDESC& tagResetDesc);

private:
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShader = { nullptr };
	CModel* m_pModel = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CCylinder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
