#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRadialBlur final : public CGameObject
{
private:
	explicit CRadialBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRadialBlur(const CRadialBlur& rhs);
	virtual ~CRadialBlur() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader*			m_pShader = { nullptr };
	class CRenderer*		m_pRenderer = { nullptr };
	class CVIBuffer_Rect*	m_pBuffer = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CRadialBlur* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END