#pragma once

// 피직스 렌더링용 객체입니다

#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Line;
class CVIBuffer_Triangle;
END

BEGIN(Tool)

class CPhysXRender final : public CGameObject
{
private:
	explicit CPhysXRender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPhysXRender(const CPhysXRender& rhs);
	virtual ~CPhysXRender() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShader = { nullptr };
	CRenderer*					m_pRenderer = { nullptr };
	CVIBuffer_Line*				m_pLine = { nullptr };
	CVIBuffer_Triangle*			m_pTriangle = { nullptr };

private:
	PxScene*					m_pScene = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT Make_Buffers();

public:
	static CPhysXRender* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END