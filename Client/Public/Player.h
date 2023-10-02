#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "Magic.h"
#include "HitState.h"
#include "StateMachine.h"

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
	CCustomModel*	m_pCustomModel = { nullptr }; //스테이트
	CRigidBody*		m_pRigidBody = { nullptr };
	CCoolTime*		m_pCooltime = { nullptr };

private:
	CPlayer_Camera* m_pPlayer_Camera = { nullptr };
	CPlayer_Information* m_pPlayer_Information = { nullptr };

	CUI_Group_Skill* m_UI_Group_Skill_01 = { nullptr };
	CAccPotion* m_pAccPotion = { nullptr };
private:
	

	_bool		m_isFixMouse = { false };
	CStateContext* m_pStateContext = { nullptr };
	
	//평타, 실드가 이미 탑재된 마법 슬롯 
	class CMagicSlot*	m_pMagicSlot = { nullptr };
	CWeapon_Player_Wand*	m_pWeapon = { nullptr };

	//절두체 타겟 설정 완료되면 사용
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

#pragma region 스테이트에 넘기는 변수

	//카메라룩과 플레이어룩의 차이 각을 담기위한 변수(음수일 경우 오른쪽, 양수일 경우 왼쪽)
	_float m_fLookAngle{};
	//방향키 입력이 들어왔는지 확인하는 변수
	_bool m_isDirectionKeyPressed { false };
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
	HRESULT Ready_StateMachine();

public:
	// 마법에 함수가 잘 들어가나 테스트용도입니다.
	// 추후에 마법 사용시 지팡이가 빛나게 하는 파티클 action을 여기에 넣어주면 될거같음.
	void MagicTestTextOutput();

#ifdef _DEBUG
private:
	void Tick_ImGui();
	_bool m_isGravity = { false };
#endif // _DEBUG

private:
	//카메라와 플레이어의 각을 검사해서 저장한다.
	void UpdateLookAngle();
	//타겟과의 각을 구하기 위한함수
	void Update_Target_Angle();

	void Update_Cloth(_float fTimeDelta);

	//타겟을 정하기 위한 함수 (임시 용)
	void Find_Target_For_Distance();
	
#pragma region 노티파이 사용함수

	HRESULT Bind_Notify();

	void Gravity_On();
	void Gravity_Off();

	void Shot_Levioso();
	void Shot_Confringo();
	void Shot_NCENDIO();
	void Shot_Finisher();
	void Lumos();

	void Finish_Animation();

	void Next_Spell_Action();

	void Shot_Basic_Spell();

	void Shot_Basic_Last_Spell();

	void Protego();

#pragma endregion

#pragma region 스테이트 변경 함수

	void Go_Roll();

	void Go_Jump();

	void Go_MagicCast(void * _pArg);

	void Go_Protego(void * _pArg);

	void Go_Hit(void* _pArg);

	void Go_Switch_Start();

	void Go_Switch_Loop();







#pragma endregion

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END