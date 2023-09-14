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
class CPlayer_Camera;

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
	
private:
	CPlayer_Camera* m_pPlayer_Camera = { nullptr };

private:
	//ī�޶��� �÷��̾���� ���� ���� ������� ����(������ ��� ������, ����� ��� ����)
	_float m_fLookAngle{};

#ifdef _DEBUG

	CCollider*		m_pTestCollider = { nullptr };

#endif // _DEBUG

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	void Key_Input(_float fTimeDelta);

private:
	HRESULT Ready_MeshParts();
	HRESULT Ready_Caemra();

#ifdef _DEBUG
private:
	HRESULT Ready_Test_Collider();
	void Tick_ImGui();
#endif // _DEBUG

private:
	//ī�޶�� �÷��̾��� ���� �˻��ؼ� �����Ѵ�.
	void UpdateLookAngle();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END