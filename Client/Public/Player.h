#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CCollider;
class CCustomModel;
class CRigidBody;
END

BEGIN(Client)
class CPlayer_Camera;
class CWeapon_Player_Wand;
class CStateContext;
END

BEGIN(Client)
class CPlayer final : public CGameObject
{
private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;
	
public:
	void Set_TargetTransform(CTransform* _pTargetTransform = nullptr) { m_pTargetTransform = _pTargetTransform; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CShader*		m_pShader = { nullptr };
	CShader*		m_pShadowShader = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	CCustomModel*	m_pCustomModel = { nullptr };
	CRigidBody*		m_pRigidBody = { nullptr };

private:
	CPlayer_Camera* m_pPlayer_Camera = { nullptr };

private:
	//ī�޶��� �÷��̾���� ���� ���� ������� ����(������ ��� ������, ����� ��� ����)
	_float m_fLookAngle{};
	//����Ű �Է��� ���Դ��� Ȯ���ϴ� ����
	_bool m_isDirectionKeyPressed { false };
	//Ÿ���� ���ϱ����� �� ����
	_float m_fTargetAngle{};

	_bool		m_isFixMouse = { false };
	CStateContext* m_pStateContext = { nullptr };
	
	//��Ÿ, �ǵ尡 �̹� ž��� ���� ���� 
	class CMagicSlot*	m_pMagicSlot = { nullptr };
	CWeapon_Player_Wand*	m_pWeapon = { nullptr };

	CTransform* m_pTargetTransform = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
	HRESULT Add_Magic();

private:
	void Key_Input(_float fTimeDelta);
	void Fix_Mouse();

private:
	HRESULT Ready_MeshParts();
	HRESULT Ready_Camera();

public:
	// ������ �Լ��� �� ���� �׽�Ʈ�뵵�Դϴ�.
	// ���Ŀ� ���� ���� �����̰� ������ �ϴ� ��ƼŬ action�� ���⿡ �־��ָ� �ɰŰ���.
	void MagicTestTextOutput();

#ifdef _DEBUG
private:
	void Tick_ImGui();
	_bool	m_isGravity;
#endif // _DEBUG

private:
	//ī�޶�� �÷��̾��� ���� �˻��ؼ� �����Ѵ�.
	void UpdateLookAngle();
	//Ÿ�ٰ��� ���� ���ϱ� �����Լ�
	void Update_Target_Angle();

	void Shot_Basic_Spell();

	void Protego();

	HRESULT Bind_Notify();

	void Update_Cloth(_float fTimeDelta);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END