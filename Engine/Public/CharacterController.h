#pragma once
/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */
#include "Composite.h"

BEGIN(Engine)
class CShader;

class ENGINE_DLL CCharacterController final : public CComposite
{
public:
	_float3 Get_Position();
	void Set_Position(_float3 _vPosition);
#ifdef _DEBUG
	void Set_DebugColor(_float4 _vColor) { m_vColor = _vColor; }
#endif // _DEBUG
	
	void Move(_float3 _vVelocity, _float _fTimeDelta, _float _fMinDist = 0.f);

private:
	explicit CCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCharacterController(const CCharacterController& rhs);
	virtual ~CCharacterController() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif // _DEBUG

private:
	PxController*			m_pController = { nullptr };
	PxRigidDynamic*			m_pActor = { nullptr };

private: /* CallBack */
	class CBehaviorCallBack*	m_pBehaviorCallBack = { nullptr };
	class CReportCallBack*		m_pReportCallBack = { nullptr };

#ifdef _DEBUG
private:
	CShader*				m_pShader = { nullptr };
	CComponent*				m_pDebug_Render = { nullptr };
	_float4					m_vColor;
#endif // _DEBUG

#ifdef _DEBUG
private:
	HRESULT Add_Components(PxControllerDesc* pControllerDesc);
	HRESULT SetUp_ShaderResources();
#endif // _DEBUG

public:
	static CCharacterController* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END