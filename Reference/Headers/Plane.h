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
class CVIBuffer_Line;
class CVIBuffer_Triangle;
#endif // _DEBUG

class ENGINE_DLL CPlane final : public CComposite
{
private:
	explicit CPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlane(const CPlane& rhs);
	virtual ~CPlane() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* szHeightMapPath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif // _DEBUG

private:
	PxRigidStatic*			m_pActor = { nullptr };
	PxMaterial*				m_pMaterial = { nullptr };
	PxScene*				m_pScene = { nullptr };
	
#ifdef _DEBUG
private:
	CShader*				m_pShader = { nullptr };
	CVIBuffer_Line*			m_pLine = { nullptr };
	CVIBuffer_Triangle*		m_pTriangle = { nullptr };

private:
	_uint					m_iNumLineBuffer = { 0 };
	_uint					m_iStartLineBufferIndex = { 0 };

	_uint					m_iNumTriangleBuffer = { 0 };
	_uint					m_iStartTriangleBufferIndex = { 0 };
#endif // _DEBUG

private:
	HRESULT Create_HeightMap(const _tchar* szHeightMapPath);
	HRESULT Create_PlaneActor();

#ifdef _DEBUG
private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Make_Buffers();
#endif // _DEBUG

public:
	static CPlane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* szHeightMapPath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END