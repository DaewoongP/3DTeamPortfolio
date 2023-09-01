#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CCustomModel;
class CShader;
class CRenderer;
class CRigidBody;
class CVIBuffer_Line;
class CVIBuffer_Triangle;
class CCharacterController;
END

BEGIN(Client)

class CTest_Player final : public CGameObject
{
private:
	explicit CTest_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTest_Player(const CTest_Player& rhs);
	virtual ~CTest_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CCustomModel*	m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	CRigidBody*		m_pRigidBody = { nullptr };
	CVIBuffer_Line* m_pLine = { nullptr };
	CShader*		m_pDebugShader = { nullptr };
	CVIBuffer_Triangle*		m_pTriangle = { nullptr };

	CCharacterController* m_pController = { nullptr };


	_float4x4		m_ConvMatrix;

private:
	PxScene*		m_pScene = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Key_Input(_float fTimeDelta);

public:
	static CTest_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END