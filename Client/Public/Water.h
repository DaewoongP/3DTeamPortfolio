#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Enigne)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CWater final : public CGameObject
{
private:
	explicit CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWater(const CWater& rhs);
	virtual ~CWater() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShader = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };
	CVIBuffer_Rect*		m_pBuffer = { nullptr };

private:
	_float				m_fTimeAcc = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CWater* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END