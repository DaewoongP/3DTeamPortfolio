
#pragma once

#include "GameObject.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CCollider;
class CNavigation;
class CModel;
class CVIBuffer_Velocity_Instance;
class CVIBuffer;

END

BEGIN(Engine)
class CMotionBlurInstance final : public CGameObject
{
private:
	CMotionBlurInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMotionBlurInstance(const CMotionBlurInstance& rhs);
	virtual ~CMotionBlurInstance() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private: /* For. Component */
	CShader* m_pVelocityMapInst = { nullptr };
	CVIBuffer_Velocity_Instance* m_pVIBufferVelocity = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

private:
	_uint m_iLevel = { 0 };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMotionBlurInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

