#pragma once
#include "Composite.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Line;
class CVIBuffer_Triangle;

class ENGINE_DLL CCharacterController final : public CComposite
{
public:
	_float3 Get_Position();
	void Set_Position(_float3 _vPosition);
	void Move(_float3 _vVelocity, _float _fTimeDelta, _float _fMinDist = 0.f);

private:
	explicit CCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCharacterController(const CCharacterController& rhs);
	virtual ~CCharacterController() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	
#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif // _DEBUG

private:
	PxController*			m_pController = { nullptr };

#ifdef _DEBUG
private:
	CShader*				m_pShader = { nullptr };
	CVIBuffer_Line*			m_pLine = { nullptr };
	CVIBuffer_Triangle*		m_pTriangle = { nullptr };

private:
	_uint					m_iNumLineBuffer = { 0 };
	_uint					m_iNumTriangleBuffer = { 0 };
#endif // _DEBUG

#ifdef _DEBUG
private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Make_Buffers();
#endif // _DEBUG

public:
	static CCharacterController* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END