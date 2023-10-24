#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "Magic.h"
#include "HitState.h"
#include "StateMachine.h"
#include "Enemy.h"
#include "ProtegoState.h"
#include"Blink_Effect.h"

#include "Event_Enter_Vault.h"
#include "Event_Dragon_Death.h"
#include "Event_Dragon_Hp_Down.h"

#include "Ease.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider;
class CCustomModel;
class CRigidBody;
class CCoolTime;
class CDefence;
class CParticleSystem;
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
class CBlink_Effect;
class CCard_Fig;
class CBroom_Stick;
END

BEGIN(Client)
class CPlayer final : public CGameObject
{
	friend CEvent_Enter_Vault;
	friend CEvent_Dragon_Death;
	friend CEvent_Dragon_Hp_Down;

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
	CTransform* Get_Player_Camera_Transform();
	CPlayer_Information* Get_Player_Information() { return m_pPlayer_Information; }
	_float3 Get_PlayerPos() { return m_pTransform->Get_Position(); }
	SPELL Get_SpellList(SKILLINPUT eType) { return m_vecSpellCheck[eType]; }
	void Set_TargetTransform(CTransform* _pTargetTransform = nullptr) { m_pTargetTransform = _pTargetTransform; }
	void Set_Protego_Collision(CTransform* _pTransform, CEnemy::ATTACKTYPE _eAttackType) const;

	void Set_Robe_Mesh_Index(const _uint& iMeshIndex) { m_iRobeMeshIndex = iMeshIndex; }
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
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CCustomModel* m_pCustomModel = { nullptr }; //스테이트
	CRigidBody* m_pRigidBody = { nullptr };
	CCoolTime* m_pCooltime = { nullptr };
	CDefence* m_pDefence = { nullptr };
	CBlink_Effect* m_pBlink = { nullptr };
	vector<CParticleSystem*> m_vecPotionParticle;
	vector<CParticleSystem*> m_vecPlayer_StateParicle;
	vector<CParticleSystem*> m_vecTest_Particle;
	_float3 Test = { 0.f,0.f,0.f };
private: /* Card Fig 전용 데이터 */
	CCard_Fig* m_pCard_Fig = { nullptr };

private:
	CPlayer_Camera* m_pPlayer_Camera = { nullptr };
	CPlayer_Information* m_pPlayer_Information = { nullptr };

	CUI_Group_Skill* m_UI_Group_Skill_01 = { nullptr };
	CUI_Group_SkillTap* m_UI_Group_SkillTap = { nullptr };
	_bool			m_isOpenSkillTap = { false };

	CTool* m_pCurTool = { nullptr };

private:
	_bool		m_isFixMouse = { false };
	CStateContext* m_pStateContext = { nullptr };

	//평타, 실드가 이미 탑재된 마법 슬롯 
	class CMagicSlot* m_pMagicSlot = { nullptr };
	CWeapon_Player_Wand* m_pWeapon = { nullptr };
	CBroom_Stick* m_pBroom = { nullptr };

	//절두체 타겟 설정 완료되면 사용
	CTransform* m_pTargetTransform = { nullptr };
	CGameObject* m_pTarget = { nullptr };

	_float		m_fClothPower = { 0.f };
	_float		m_fClothPowerPlus = { 0.0f };

	CMagic::MAGICDESC m_BasicDesc_Light;
	CMagic::MAGICDESC m_BasicDesc_Heavy;

	CMagicBall* m_pMagicBall = { nullptr };

	mutable function< void(void*)> m_pFrncSpellToggle = { nullptr };
	mutable _bool m_isUseProtego = { false };

	LEVELID m_eLevelID = { LEVEL_END };

	_float3		m_vLevelInitPosition[LEVEL_END];
	//물약 사용여부
	_bool m_isPowerUp = { false };
	_bool m_isDefUp = { false };
	_bool m_isFocusOn = { false };
	_bool m_isInvisible = { false };
	_int m_iDeffence = { 0 };


	//스킬UI에 넘겨주기
	vector<_float> m_vecCoolTimeRatio;


#pragma region 스테이트에 넘기는 변수

	//카메라룩과 플레이어룩의 차이 각을 담기위한 변수(음수일 경우 오른쪽, 양수일 경우 왼쪽)
	_float m_fLookAngle{};
	_float m_fLookAngle_Y{};
	//방향키 입력이 들어왔는지 확인하는 변수
	_bool m_isDirectionKeyPressed{ false };
	//타겟을 향하기위한 각 변수
	_float m_fTargetAngle{};

	CStateMachine::STATEMACHINEDESC m_StateMachineDesc = { CStateMachine::STATEMACHINEDESC() };

	_bool m_isFinishAnimation = { false };

	_uint m_iMoveType = { (_uint)MOVETYPE_END };

	_uint m_iActionType = { (_uint)ACTION_END };

	_float m_fRotationSpeed = { 0.0f };

	function<void()> m_funcFinishAnimation = { nullptr };

	_bool m_isReadySpell = { true };
	
#pragma endregion

#pragma region 카메라 쉐이크

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

	const char* m_pEases[CEase::EASE_END] = {
		"InQuad", "OutQuad", "InOutQuad"
		, "InSine", "OutSine", "InOutSine"
		, "InQuint", "OutQuint", "InOutQuint"
		, "InQuart", "OutQuart", "InOutQuart"
		, "NoneLinear", "InLinear", "OutLinear", "InOut_Linear"
		, "InExpo", "OutExpo", "InOutExpo"
		, "InElastic", "OutElastic", "InOutElastic"
		, "InCubic", "OutCubic", "InOutCubic"
		, "InCirc", "OutCirc", "InOutCirc"
		, "InBounce", "OutBounce", "InOutBounce"
		, "InBack", "OutBack", "InOutBack"
	};

#pragma endregion

	_float m_fTargetViewRange = { 0.0f };

	vector<SPELL> m_vecSpellCheck;
	

	_bool m_isFlying = { false };
	_bool m_isLumosOn = { false };

	_bool m_isBlink = { false };


	//프로테고 const 이주...
	_bool m_isPrepareProtego = { false };
	
	mutable _bool m_isCollisionEnterProtego = { false };

	mutable CProtegoState::PROTEGOSTATEDESC m_ProtegoStateDesc = { CProtegoState::PROTEGOSTATEDESC() };

	//루모스 끄기
	_bool m_isPreLumos = { false };

	// 로브 업데이트 메쉬 인덱스
	_uint m_iRobeMeshIndex = { 0 };

	_float m_fEyePlusDistanceForHover = { 0.0f };
	//스위칭용
	SPELL m_pFlySpell[4] = { LEVIOSO,FLIPENDO,ACCIO,DESCENDO  };
	SPELL m_pNonFlySpell[4] = { SPELL(0) };
	CParticleSystem* m_pWindParticle = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	HRESULT Add_Magic();

private:
	void Key_Input(_float fTimeDelta);
	void Key_input_Flying(_float fTimeDelta);
	void Fix_Mouse();

private:
	HRESULT Ready_MeshParts();
	HRESULT Ready_Camera();
	HRESULT Ready_MagicDesc();
	HRESULT Ready_StateMachine();

public:
	// 마법에 함수가 잘 들어가나 테스트용도입니다.
	// 추후에 마법 사용시 지팡이가 빛나게 하는 파티클 action을 여기에 넣어주면 될거같음.
	void MagicTestTextOutput();

#ifdef _DEBUG
private:
	void Tick_TestShake();
#endif // _DEBUG

private:
	//카메라와 플레이어의 각을 검사해서 저장한다.
	void UpdateLookAngle();
	//타겟과의 각을 구하기 위한함수
	void Update_Target_Angle();

	void Update_Cloth(_float fTimeDelta);

	void Clear_Target();

	//타겟을 정하기 위한 함수 (임시 용)
	void Find_Target_For_Distance();
	void Find_Target_For_ViewSpace();

	//비행시 어떤 행동을 하는지에 따른 Distance;
	void Update_Hover_Eye_Distance();
	
#pragma region 노티파이 사용함수

	HRESULT Bind_Notify();

	void Gravity_On();
	void Gravity_Off();

	void Shot_Magic_Spell_Button_1();
	void Shot_Magic_Spell_Button_2();
	void Shot_Magic_Spell_Button_3();
	void Shot_Magic_Spell_Button_4();
	
	

	_uint Special_Action(_uint _iButton);

	void Finisher();

	void Stupefy();

	void Lumos();

	void Finish_Animation();

	void Next_Spell_Action();

	void Shot_Basic_Spell();

	void Shot_Basic_Last_Spell();

	void Protego();
	void Add_Layer_Item();
	void Drink_Potion();

	void Add_Potion();

	void Drink_Heal_Potion();

	void Blink_Start();
	void Blink_End();

	void Healing();

	void Landing();

	void Fly_Effect();

#pragma endregion

#pragma region 스테이트 변경 함수

	void Go_Roll(void* _pArg = nullptr);

	void Go_Jump();

	void Go_MagicCast(void * _pArg);

	void Go_Protego(void * _pArg);

	void Go_Hit(void* _pArg);

	void Go_Switch_Start();

	void Go_Switch_Loop();

	void Prepare_Protego();

	void Go_Use_Item();

	void Go_Use_Potion();

#pragma endregion

	void Update_Skill_CoolTime();


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END