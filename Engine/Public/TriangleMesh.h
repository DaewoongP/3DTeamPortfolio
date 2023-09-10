#pragma once
/* =============================================== */
// 
//	¡§ : æ»√∂πŒ
//	∫Œ :
//
/* =============================================== */
#include "Composite.h"

BEGIN(Engine)

class ENGINE_DLL CTriangleMesh final : public CComposite
{
private:
	explicit CTriangleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTriangleMesh(const CTriangleMesh& rhs);
	virtual ~CTriangleMesh() = default;

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
	HRESULT	Read_File_MeshCol(_tchar* pModelFilePath);
	HRESULT Create_ConvexMeshActor(COLMESH* PMesh);
	HRESULT Release_Mesh(COLMESH* Mesh);

public:
	static CTriangleMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END