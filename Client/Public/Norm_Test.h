#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Point_Instance;
class CRadialBlur;
END

BEGIN(Client)

class CNorm_Test final : public CGameObject
{
private:
	explicit CNorm_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CNorm_Test(const CNorm_Test& rhs);
	virtual ~CNorm_Test() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader*					m_pShader = { nullptr };
	class CTexture*					m_pTexture = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CVIBuffer_Point_Instance*	m_pBuffer = { nullptr };
	CRadialBlur* m_pRadial;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CNorm_Test* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END