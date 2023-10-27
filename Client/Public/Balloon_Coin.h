#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CBalloon_Coin final : public CGameObject
{
private:
	explicit CBalloon_Coin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBalloon_Coin(const CBalloon_Coin& rhs);
	virtual ~CBalloon_Coin() = default;

public:
	HRESULT Initialize(void* pArg) override;
	void Tick(_float fTimeDelta) override;
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	CShader*						m_pShader = { nullptr };
	vector<CTexture*>				m_Textures = { nullptr };
	CRenderer*						m_pRenderer = { nullptr };
	CVIBuffer_Point_Instance*		m_pBuffer = { nullptr };

private:
	HRESULT Add_Component();
	HRESULT SetUp_ShaderResources();

public:
	static CBalloon_Coin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END