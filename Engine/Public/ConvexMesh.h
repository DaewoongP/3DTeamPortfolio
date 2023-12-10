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

class ENGINE_DLL CConvexMesh final : public CComposite
{
private:
	explicit CConvexMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CConvexMesh(const CConvexMesh& rhs);
	virtual ~CConvexMesh() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	PxRigidStatic*			m_pActor = { nullptr };
	PxMaterial*				m_pMaterial = { nullptr };
	PxScene*				m_pScene = { nullptr };
	
private:
	HRESULT Create_ConvexMeshActor(void* pArg);

public:
	static CConvexMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END