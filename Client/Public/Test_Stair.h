#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CConvexMesh;
class CTriangleMesh;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CTest_Stair final : public CGameObject
{
private:
	explicit CTest_Stair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTest_Stair(const CTest_Stair& rhs);
	virtual ~CTest_Stair() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };
	CConvexMesh*			m_pConvexMesh = { nullptr };
	CTriangleMesh*			m_pTriangleMesh = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTest_Stair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END