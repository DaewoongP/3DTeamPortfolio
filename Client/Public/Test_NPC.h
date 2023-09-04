#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CCustomModel;
class CShader;
class CRenderer;
class CRigidBody;
class CCharacterController;
END

BEGIN(Client)

class CTest_NPC final : public CGameObject
{
private:
	explicit CTest_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTest_NPC(const CTest_NPC& rhs);
	virtual ~CTest_NPC() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

	CCharacterController* m_pController = { nullptr };

private:
	PxScene* m_pScene = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Key_Input(_float fTimeDelta);

#ifdef _DEBUG
	void Tick_ImGui();
#endif // _DEBUG

public:
	static CTest_NPC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END