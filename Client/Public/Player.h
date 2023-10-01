#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "Magic.h"
#include "HitState.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CCollider;
class CCustomModel;
class CRigidBody;
class CCoolTime;
END

BEGIN(Client)
class CPlayer_Camera;
class CWeapon_Player_Wand;
class CStateContext;
class CPlayer_Information;
class CUI_Group_Skill;
class CMagicBall;
class CAccPotion;
class CWiggenweldPotion;
END

BEGIN(Client)
class CPlayer final : public CGameObject
{
public:
	typedef struct tagPlayerDesc
	{
		_float3 vPosition = {_float3()};
		LEVELID eLevelID= { LEVEL_END };
	}PLAYERDESC;

private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;
	
public:
	void Set_TargetTransform(CTransform* _pTargetTransform = nullptr) { m_pTargetTransform = _pTargetTransform; }
	_float3 Get_PlayerPos() { return m_pTransform->Get_Position(); }

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

	virtual void On_Maigc_Throw_Data(void* data) const override;

private:
	CShader*		m_pShader = { nullptr };
	CShader*		m_pShadowShader = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	CCustomModel*	m_pCustomModel = { nullptr };
	CRigidBody*		m_pRigidBody = { nullptr };
	CCoolTime*		m_pCooltime = { nullptr };

private:
	CPlayer_Camera* m_pPlayer_Camera = { nullptr };
	CPlayer_Information* m_pPlayer_Information = { nullptr };

	CUI_Group_Skill* m_UI_Group_Skill_01 = { nullptr };
	CAccPotion* m_pAccPotion = { nullptr };
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

	//����ü Ÿ�� ���� �Ϸ�Ǹ� ���
	CTransform* m_pTargetTransform = { nullptr };
	CGameObject* m_pTarget = { nullptr };

	_float		m_fClothPower = { 0.f };
	_float		m_fClothPowerPlus = { 0.0f };

	CMagic::MAGICDESC m_BasicDesc_Light;
	CMagic::MAGICDESC m_BasicDesc_Heavy;

	CMagicBall* m_pMagicBall = { nullptr };

	mutable function< void(void*)> m_pFrncSpellToggle = { nullptr };

	LEVELID m_eLevelID = { LEVEL_END };
	
	_float3		m_vLevelInitPosition[LEVEL_END];

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
	HRESULT Ready_MagicDesc();

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

	void Next_Spell_Action();


	void Shot_Basic_Spell();

	void Shot_Basic_Last_Spell();

	void Protego();

	void Gravity_On();
	void Gravity_Off();

	HRESULT Bind_Notify();

	void Update_Cloth(_float fTimeDelta);

	//Ÿ���� ���ϱ� ���� �Լ� (�ӽ� ��)
	void Find_Target_For_Distance();

	void Shot_Magic_Spell();

	void Shot_Levioso();
	void Shot_Confringo();
	void Shot_NCENDIO();
	void Shot_Finisher();
	void Lumos();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END