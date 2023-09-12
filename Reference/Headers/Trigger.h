#pragma once
/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */
#include "Composite.h"

BEGIN(Engine)

#ifdef _DEBUG
class CShader;
#endif // _DEBUG

class ENGINE_DLL CTrigger final : public CComposite
{
private:
	explicit CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

#ifdef _DEBUG
	void Set_DebugColor(_float4 _vColor) { m_vColor = _vColor; }
#endif // _DEBUG

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif // _DEBUG

public:
	HRESULT Create_Actor();

private:
	PxRigidActor*			m_pActor = { nullptr };
	PxMaterial*				m_pMaterial = { nullptr };
	PxScene*				m_pScene = { nullptr };

#ifdef _DEBUG
private:
	CShader*				m_pShader = { nullptr };
	CComponent*				m_pDebug_Render = { nullptr };
	_float4					m_vColor;
#endif // _DEBUG

#ifdef _DEBUG
private:
	HRESULT Add_Components(PxGeometry* pPxValues);
	HRESULT SetUp_ShaderResources();
#endif // _DEBUG

public:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END