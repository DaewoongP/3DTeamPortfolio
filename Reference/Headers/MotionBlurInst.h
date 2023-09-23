
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
class CVIBuffer_Rect;

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

	virtual HRESULT Render() override;

	HRESULT Begin_MRT();
	HRESULT End_MRT();
private: /* For. Component */
	CShader* m_pShader = { nullptr };
	CVIBuffer_Rect* m_pBuffer = { nullptr };

private:
	_uint m_iLevel = { 0 };
	_uint m_iNumInstance = { MAX_PARTICLE_NUM };
	_float4x4 m_PrevWorldMatrix;

	_float4x4 m_WorldMatrix;
	_float4x4 m_ViewMatrix;
	_float4x4 m_ProjMatrix;

public:
	static CMotionBlurInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

