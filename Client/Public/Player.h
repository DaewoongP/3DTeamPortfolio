#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CCollider;
class CCustomModel;

END

BEGIN(Client)

class CDebug_Point;

class CPlayer final : public CGameObject
{
private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

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
	CShader*		m_pShader = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	CCustomModel*	m_pCustomModel = { nullptr };
	

#ifdef _DEBUG

	CCollider*		m_pTestCollider = { nullptr };

#endif // _DEBUG

private:

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Key_Input(_float fTimeDelta);

	HRESULT Ready_MeshParts();

#ifdef _DEBUG

	HRESULT Ready_Test_Collider();




	void Tick_ImGui();



#endif // _DEBUG

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END