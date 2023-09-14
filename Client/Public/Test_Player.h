#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CRigidBody;
class CCustomModel;
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
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };
	CRigidBody*				m_pRigidBody = { nullptr };
	CCustomModel*			m_pModelCom = { nullptr };
	
	CCharacterController*	m_pController = { nullptr };

private:
	_float					m_fWindPower = { 10.f };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Key_Input(_float fTimeDelta);

#ifdef _DEBUG
	_int m_iIndex = { 0 };
	void Tick_ImGui();
#endif // _DEBUG

public:
	static CTest_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END