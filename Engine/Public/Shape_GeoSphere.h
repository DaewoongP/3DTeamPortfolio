
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
class CVIBuffer_GeoSphere;

END

BEGIN(Engine)
class ENGINE_DLL CShape_GeoSphere final : public CGameObject
{
private:
	CShape_GeoSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShape_GeoSphere(const CShape_GeoSphere& rhs);
	virtual ~CShape_GeoSphere() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

private: /* For. Component */
	CVIBuffer_GeoSphere* m_pBuffer = { nullptr };
	CShader* m_pShader = { nullptr };

private:
	_float m_fRadius;
	_float m_fRadiusThickness = { 0.f };
	_float m_fArc = { 360.f };
	string m_strArcMode = { "" };
	_float m_fSpread = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CShape_GeoSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

