#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "Magic.h"
#include "HitState.h"
#include "StateMachine.h"
#include "Enemy.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider;
class CCustomModel;
class CRigidBody;
class CCoolTime;
class CDefence;
END

BEGIN(Client)
class CPlayer_Camera;
class CWeapon_Player_Wand;
class CStateContext;
class CPlayer_Information;
class CUI_Group_Skill;
class CUI_Group_SkillTap;
class CMagicBall;
class CMaximaPotion;
class CFocusPotion;
class CEdurusPotion;
class CInvisibilityPotion;
class CWiggenweldPotion;
class CTool;
END

BEGIN(Client)
class CPlayer final : public CGameObject
{
public:
	enum MOVETYPE
	{
		MOVETYPE_NONE,
		MOVETYPE_WALK,
		MOVETYPE_JOGING,
		MOVETYPE_SPRINT,
		MOVETYPE_END
	};

	enum ACTIONTYPE
	{
		ACTION_NONE,
		ACTION_CASUAL,
		ACTION_CMBT,
		ACTION_END
	};

	enum SKILLINPUT
	{
		SKILLINPUT_1,
		SKILLINPUT_2,
		SKILLINPUT_3,
		SKILLINPUT_4,
		SKILLINPUT_END
	};

private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;
	
public:
	CPlayer_Information* Get_Player_Information() { return m_pPlayer_Information; }
	_float3 Get_PlayerPos() { return m_pTransform->Get_Position(); }
	SPELL Get_SpellList(SKILLINPUT eType) { return m_vecSpellCheck[eType]; }
	void Set_TargetTransform(CTransform* _pTargetTransform = nullptr) { m_pTargetTransform = _pTargetTransform; }
	void Set_Protego_Collision(CEnemy::ATTACKTYPE _eAttackType, CTransform* _pTransform);


	void Set_PowerUp(_bool isPowerUp) { m_isPowerUp = isPowerUp; }
	void Set_DefUp(_bool isDefUp) { m_isDefUp = isDefUp; }
	void Set_FocusOn(_bool isFocus) { m_isFocusOn = isFocus; }
	void Set_Invisible(_bool isInvisible) { m_isInvisible = isInvisible; }

	void Set_Spell_Botton(_uint _Button, SPELL _eSpell);

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
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;



	void Potion_Duration(_float fTimeDelta);
	virtual void On_Maigc_Throw_Data(void* data) const override;
	_bool Is_Action_Camera_Playing();

private:
	CShader*		m_pShader = { nullptr };
	CShader*		m_pShadowShader = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	CCustomModel*	m_pCustomModel = { nullptr }; //������Ʈ
	CRigidBody*		m_pRigidBody = { nullptr };
	CCoolTime*		m_pCooltime = { nullptr };
	CDefence*		m_pDefence = { nullptr };

private:
	CPlayer_Camera* m_pPlayer_Camera = { nullptr };
	CPlayer_Information* m_pPlayer_Information = { nullptr };

	CUI_Group_Skill* m_UI_Group_Skill_01 = { nullptr };
	CUI_Group_SkillTap* m_UI_Group_SkillTap = { nullptr };

	CTool* m_pCurTool = { nullptr };

private:
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
	//���� ��뿩��
	_bool m_isPowerUp = { false };
	_bool m_isDefUp = { false };
	_bool m_isFocusOn = { false };
	_bool m_isInvisible = { false };
	_int m_iDeffence = { 0 };


	//��ųUI�� �Ѱ��ֱ�
	vector<_float> m_vecCoolTimeRatio;

	
#pragma region ������Ʈ�� �ѱ�� ����

	//ī�޶��� �÷��̾���� ���� ���� ������� ����(������ ��� ������, ����� ��� ����)
	_float m_fLookAngle{};
	//����Ű �Է��� ���Դ��� Ȯ���ϴ� ����
	_bool m_isDirectionKeyPressed { false };
	//Ÿ���� ���ϱ����� �� ����
	_float m_fTargetAngle{};

	CStateMachine::STATEMACHINEDESC m_StateMachineDesc = { CStateMachine::STATEMACHINEDESC() };

	_bool m_isFinishAnimation = { false };

	_uint m_iMoveType = { (_uint)MOVETYPE_END };

	_uint m_iActionType = { (_uint)ACTION_END };

	_float m_fRotationSpeed = { 0.0f };

	function<void()> m_funcFinishAnimation = { nullptr };

	_bool m_isReadySpell = { true };
	
#pragma endregion

#pragma region ī�޶� ����ũ

	vector<char*> m_vecEaseList;

	_int m_iShake_Type = { 0 };
	_int m_iShake_Axis = { 0 };
	_int m_iEase = { 0 };

	_float m_fShakeSpeed = { 5.0f };
	_float m_fShakeDuration = { 1.0f };
	_float m_fShakePower = { 0.01f };

	_int m_iShakePower = { 0 };

	_float m_fx = { 1.0f };
	_float m_fy = { 1.0f };
	_float m_fz = { 1.0f };

#pragma endregion

	_float m_fTargetViewRange = { 0.0f };

	vector<SPELL> m_vecSpellCheck;
	


	_bool m_isLumosOn = { false };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	HRESULT Add_Magic();

private:
	void Key_Input(_float fTimeDelta);
	void Fix_Mouse();

private:
	HRESULT Ready_MeshParts();
	HRESULT Ready_Camera();
	HRESULT Ready_MagicDesc();
	HRESULT Ready_StateMachine();

public:
	// ������ �Լ��� �� ���� �׽�Ʈ�뵵�Դϴ�.
	// ���Ŀ� ���� ���� �����̰� ������ �ϴ� ��ƼŬ action�� ���⿡ �־��ָ� �ɰŰ���.
	void MagicTestTextOutput();

#ifdef _DEBUG
private:
	void Tick_ImGui();
	void Tick_Magic_ImGui();
	_bool m_isGravity = { false };

	void Tick_TestShake();
#endif // _DEBUG

private:
	//ī�޶�� �÷��̾��� ���� �˻��ؼ� �����Ѵ�.
	void UpdateLookAngle();
	//Ÿ�ٰ��� ���� ���ϱ� �����Լ�
	void Update_Target_Angle();

	void Update_Cloth(_float fTimeDelta);

	void Clear_Target();

	//Ÿ���� ���ϱ� ���� �Լ� (�ӽ� ��)
	void Find_Target_For_Distance();
	void Find_Target_For_ViewSpace();
	
#pragma region ��Ƽ���� ����Լ�

	HRESULT Bind_Notify();

	void Gravity_On();
	void Gravity_Off();

	void Shot_Magic_Spell_Button_1();
	void Shot_Magic_Spell_Button_2();
	void Shot_Magic_Spell_Button_3();
	void Shot_Magic_Spell_Button_4();
	
	

	_uint Special_Action(_uint _iButton);

	void Finisher();

	void Lumos();

	void Finish_Animation();

	void Next_Spell_Action();

	void Shot_Basic_Spell();

	void Shot_Basic_Last_Spell();

	void Protego();
	void Add_Layer_Item();
	void Drink_Potion();
#pragma endregion

#pragma region ������Ʈ ���� �Լ�

	void Go_Roll(void* _pArg = nullptr);

	void Go_Jump();

	void Go_MagicCast(void * _pArg);

	void Go_Protego(void * _pArg);

	void Go_Hit(void* _pArg);

	void Go_Switch_Start();

	void Go_Switch_Loop();

#pragma endregion

	void Update_Skill_CoolTime();


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END