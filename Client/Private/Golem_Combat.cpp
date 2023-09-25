#include "Golem_Combat.h"
#include "GameInstance.h"

#include "Weapon_Golem_Combat.h"

#include "Wait.h"
#include "Death.h"
#include "Action.h"
#include "MagicBall.h"
#include "Check_Degree.h"
#include "Random_AirHit.h"
#include "Random_Select.h"
#include "Selector_Degree.h"
#include "Sequence_Groggy.h"
#include "Sequence_Attack.h"
#include "Sequence_Levitated.h"
#include "Sequence_MoveTarget.h"

CGolem_Combat::CGolem_Combat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CGolem_Combat::CGolem_Combat(const CGolem_Combat& rhs)
	: CEnemy(rhs)
{
}

HRESULT CGolem_Combat::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGolem_Combat::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

void CGolem_Combat::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// Test Code 
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL, CInput_Device::KEY_PRESSING))
	{
		if (pGameInstance->Get_DIKeyState(DIK_4, CInput_Device::KEY_DOWN))
			m_isParring = true;
		if (pGameInstance->Get_DIKeyState(DIK_1, CInput_Device::KEY_DOWN))
			m_iCurrentSpell |= BUFF_STUPEFY;
	}

	ENDINSTANCE;
	////////////////////////////

	Set_Current_Target();

	if (nullptr != m_pRootBehavior)
		m_pRootBehavior->Tick(fTimeDelta);

	_float3 vPosition = m_pTransform->Get_Position();

	for (auto iter = m_CurrentTickSpells.begin(); iter != m_CurrentTickSpells.end(); )
	{
		if (iter->first & m_iCurrentSpell)
		{
			iter->second(vPosition, fTimeDelta);
			++iter;
		}
		else
			iter = m_CurrentTickSpells.erase(iter);
	}

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CGolem_Combat::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CGolem_Combat::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wstrMyCollisionTag = CollisionEventDesc.pThisCollisionTag;

	/* Collision Magic */
	if (wstring::npos != wstrObjectTag.find(TEXT("MagicBall")))
	{
		CMagicBall::COLLSIONREQUESTDESC* pCollisionMagicBallDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
		BUFF_TYPE eBuff = pCollisionMagicBallDesc->eBuffType;
		auto Action = pCollisionMagicBallDesc->Action;
		_float fDamage = pCollisionMagicBallDesc->fDamage;

		cout << fDamage << endl;
		m_pHealth->Damaged(fDamage);

		auto iter = m_CurrentTickSpells.find(eBuff);
		if (iter == m_CurrentTickSpells.end() && BUFF_LEVIOSO == eBuff)
		{
			m_CurrentTickSpells.emplace(eBuff, Action);
		}

		if (eBuff & m_iCurrentSpell)
			m_isHitCombo = true;

		if (eBuff & BUFF_ATTACK_LIGHT)
			m_isHitAttack = true;

		m_iCurrentSpell |= eBuff;
	}

	/* Collision Player Fig */
	if (wstring::npos != wstrMyCollisionTag.find(TEXT("Range")))
	{
		if (wstring::npos != wstrObjectTag.find(TEXT("Player")) ||
			wstring::npos != wstrObjectTag.find(TEXT("Fig")))
		{
			m_isSpawn = true;
			m_RangeInEnemies.push_back({ wstrObjectTag, CollisionEventDesc.pOtherOwner });
		}
	}
}

void CGolem_Combat::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wstrMyCollisionTag = CollisionEventDesc.pThisCollisionTag;

	if (wstring::npos != wstrMyCollisionTag.find(TEXT("Range")))
	{
		if (wstring::npos != wstrObjectTag.find(TEXT("Player")) ||
			wstring::npos != wstrObjectTag.find(TEXT("Fig")))
		{
			if (FAILED(Remove_GameObject(wstrObjectTag)))
			{
				//MSG_BOX("[CGolem_Combat] Failed OnCollisionExit : \nFailed Remove_GameObject");
				return;
			}
		}
	}
}

HRESULT CGolem_Combat::Render()
{
#ifdef _DEBUG
	Tick_ImGui();
#endif // _DEBUG

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(__super::Render()))
	{
		MSG_BOX("[CGolem_Combat] Failed Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGolem_Combat::Render_Depth()
{
	return S_OK;
}

HRESULT CGolem_Combat::Make_AI()
{
	BEGININSTANCE;

	try /* Failed Check Make_AI */
	{
		if (FAILED(__super::Make_AI()))
			throw TEXT("Failed Enemy Make_AI");

		/* Make Child Behaviors */
		CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector)
			throw TEXT("pSelector is nullptr");

		CAction* pAction_Spawn = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Spawn)
			throw TEXT("pAction_Spawn is nullptr");
		CSequence* pSequence_Death = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Death)
			throw TEXT("pSequence_Death is nullptr");
		CSelector* pSelector_Alive = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_Alive)
			throw TEXT("pSelector_Alive is nullptr");
		CSelector* pSelector_Hit_Combo = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_Hit_Combo)
			throw TEXT("pSelector_Hit_Combo is nullptr");
		CSelector* pSelector_CheckSpell = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_CheckSpell)
			throw TEXT("pSelector_CheckSpell is nullptr");
		CSelector* pSelector_NormalAttack = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_NormalAttack)
			throw TEXT("pSelector_NormalAttack is nullptr");

		/* Set Decorations */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSpawn = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSpawn", pIsSpawn)))
					return false;

				return *pIsSpawn;
			});
		pSelector_Alive->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return !(pHealth->isDead());
			});

		/* Set Options */
		pAction_Spawn->Set_Options(TEXT("Spawn_Fall_1"), m_pModelCom, false, 0.f, true, true);

		/* Assemble Behaviors */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("Failed Assemble_Behavior Selector");

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Spawn"), pAction_Spawn)))
			throw TEXT("Failed Assemble_Behavior Action_Spawn");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Death"), pSequence_Death)))
			throw TEXT("Failed Assemble_Behavior Sequence_Death");
		if(FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Alive"), pSelector_Alive)))
			throw TEXT("Failed Assemble_Behavior Selector_Alive");

		if (FAILED(pSelector_Alive->Assemble_Behavior(TEXT("Selector_Hit_Combo"), pSelector_Hit_Combo)))
			throw TEXT("Failed Assemble_Behavior Selector_Hit_Combo");
		if (FAILED(pSelector_Alive->Assemble_Behavior(TEXT("Selector_CheckSpell"), pSelector_CheckSpell)))
			throw TEXT("Failed Assemble_Behavior Selector_CheckSpell");
		if (FAILED(pSelector_Alive->Assemble_Behavior(TEXT("Selector_NormalAttack"), pSelector_NormalAttack)))
			throw TEXT("Failed Assemble_Behavior Selector_NormalAttack");

		if (FAILED(Make_Death(pSequence_Death)))
			throw TEXT("Failed Make_Death");
		if (FAILED(Make_Hit_Combo(pSelector_Hit_Combo)))
			throw TEXT("Failed Make_NormalAttack");
		if (FAILED(Make_Check_Spell(pSelector_CheckSpell)))
			throw TEXT("Failed Make_NormalAttack");
		if (FAILED(Make_NormalAttack(pSelector_NormalAttack)))
			throw TEXT("Failed Make_NormalAttack");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_AI : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Notifies()
{
	return S_OK;
}

HRESULT CGolem_Combat::Add_Components()
{
	try /* Check Add_Components */
	{
		if (FAILED(__super::Add_Components()))
			throw TEXT("Failed Enemy Add_Components");

		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_Component_Model_Golem_Combat"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");

		/* For.Com_Health */
		CHealth::HEALTHDESC HealthDesc;
		HealthDesc.iMaxHP = 200;
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
			throw TEXT("Com_Health");

		/* For.Com_Weapon */
		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("SKT_RightHand"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Golem_Combat::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_Component_Weapon_Golem_Combat"),
			TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon");

		/* For.Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 2.2f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(0.7f, 1.5f);
		RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f);
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.eThisCollsion = COL_ENEMY;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_NPC | COL_NPC_RANGE | COL_MAGIC;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		m_OffsetMatrix = XMMatrixTranslation(RigidBodyDesc.vOffsetPosition.x, RigidBodyDesc.vOffsetPosition.y, RigidBodyDesc.vOffsetPosition.z);

		/* For.Collider_Range */
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		PxSphereGeometry pSphereGeomatry = PxSphereGeometry(15.f);
		RigidBodyDesc.pGeometry = &pSphereGeomatry;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Range");
		RigidBodyDesc.eThisCollsion = COL_ENEMY_RANGE;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_NPC;

		m_pRigidBody->Create_Collider(&RigidBodyDesc);
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGolem_Combat::SetUp_ShaderResources()
{
	return __super::SetUp_ShaderResources();
}

#ifdef _DEBUG
void CGolem_Combat::Tick_ImGui()
{
	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);
	POINT leftTop = { clientRect.left, clientRect.top };
	POINT rightBottom = { clientRect.right, clientRect.bottom };
	ClientToScreen(g_hWnd, &leftTop);
	ClientToScreen(g_hWnd, &rightBottom);
	int Left = leftTop.x;
	int Top = rightBottom.y;
	ImVec2 vWinpos = { _float(Left + 1280.f), _float(Top - 300.f) };
	ImGui::SetNextWindowPos(vWinpos);

	ImGui::Begin("Test Golem_Combat");

	if (ImGui::Button("Set 0, 0, 0"))
		m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	ImGui::SeparatorText("Behavior");

	vector<wstring> DebugBehaviorTags = m_pRootBehavior->Get_DebugBahaviorTags();

	for (auto& Tag : DebugBehaviorTags)
	{
		ImGui::Text(wstrToStr(Tag).c_str());
	}

	ImGui::End();
}
#endif // _DEBUG

HRESULT CGolem_Combat::Make_Descendo(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Descendo */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Descendo1 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Descendo1)
			throw TEXT("pAction_Descendo1 is nullptr");
		CAction* pAction_Descendo2 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Descendo2)
			throw TEXT("pAction_Descendo2 is nullptr");
		CAction* pAction_Descendo3 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Descendo3)
			throw TEXT("pAction_Descendo3 is nullptr");
		CAction* pAction_Descendo4 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Descendo4)
			throw TEXT("pAction_Descendo4 is nullptr");
		CAction* pAction_GetUp = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_GetUp)
			throw TEXT("pAction_GetUp is nullptr");

		/* Set Decorator */
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pCurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pCurrentSpell)))
					return false;

				if (BUFF_DESCENDO & *pCurrentSpell)
					return true;

				return false;
			});

		/* Set Options */
		pAction_Descendo1->Set_Options(TEXT("Descendo_1"), m_pModelCom);
		pAction_Descendo2->Set_Options(TEXT("Descendo_2"), m_pModelCom);
		pAction_Descendo3->Set_Options(TEXT("Descendo_3"), m_pModelCom);
		pAction_Descendo4->Set_Options(TEXT("Descendo_4"), m_pModelCom);
		pAction_GetUp->Set_Options(TEXT("Get_Up_Send_Front"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo1"), pAction_Descendo1)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo2"), pAction_Descendo2)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo3"), pAction_Descendo3)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo4"), pAction_Descendo4)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_GetUp"), pAction_GetUp)))
			throw TEXT("Failed Assemble_Behavior Action_GetUp");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Descendo : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Levioso_Combo(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try /* Failed Check Make_Descendo */
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_AirHit = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_AirHit)
			throw TEXT("pSequence_AirHit is nullptr");

		/* Set Decorator */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pIPreviusSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iPreviusSpell", pIPreviusSpell)))
					return false;

				if (BUFF_LEVIOSO & *pIPreviusSpell)
					return true;

				return false;
			});

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_AirHit"), pSequence_AirHit)))
			throw TEXT("Failed Assemble_Behavior Sequence_AirHit");

		if (FAILED(Make_Air_Hit(pSequence_AirHit)))
			throw TEXT("Failed Make_Air_Hit");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Descendo : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Air_Hit(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CSelector* pSelector_AirHit = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_AirHit)
			throw TEXT("pSelector_AirHit is nullptr");
		CAction* pAction_Splat = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Splat)
			throw TEXT("pAction_Splat is nullptr");
		CAction* pAction_GetUp = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_GetUp)
			throw TEXT("pAction_GetUp is nullptr");

		/* 평타 맞고 공중에 뜨는 액션 */
		CRandom_AirHit* pRandom_AirHit = dynamic_cast<CRandom_AirHit*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Random_AirHit")));
		if (nullptr == pRandom_AirHit)
			throw TEXT("pRandom_AirHit is nullptr");
		/* 막타 맞고 날라가는 액션 */
		CAction* pAction_Knockback = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Knockback)
			throw TEXT("pAction_Knockback is nullptr");

		CAction* pAction_Hit_1 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Hit_1)
			throw TEXT("pAction_Hit_1 is nullptr");
		CAction* pAction_Hit_3 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Hit_3)
			throw TEXT("pAction_Hit_3 is nullptr");
		CAction* pAction_Hit_4 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Hit_4)
			throw TEXT("pAction_Hit_4 is nullptr");
		CAction* pAction_Hit_5 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Hit_5)
			throw TEXT("pAction_Hit_5 is nullptr");
		CAction* pAction_Hit_6 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Hit_6)
			throw TEXT("pAction_Hit_6 is nullptr");
		CAction* pAction_Hit_8 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Hit_8)
			throw TEXT("pAction_Hit_8 is nullptr");

		/* Set Decorator*/
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_ATTACK_LIGHT & *pICurrentSpell ||
					BUFF_ATTACK_HEAVY & *pICurrentSpell)
				{
					CRigidBody* pRigidBody = { nullptr };
					if (FAILED(pBlackBoard->Get_Type("pRigidBody", pRigidBody)))
						return false;

					pRigidBody->Set_Gravity(false);
					return true;
				}

				return false;
			});
		pRandom_AirHit->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_ATTACK_LIGHT & *pICurrentSpell)
					return true;

				return false;
			});
		pRandom_AirHit->Add_Function_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsHitAttack = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isHitAttack", pIsHitAttack)))
					return false;

				if (true == *pIsHitAttack)
				{
					*pIsHitAttack = false;
					return true;
				}

				return false;
			});
		pAction_Knockback->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_ATTACK_HEAVY & *pICurrentSpell)
					return true;

				return false;
			});
		pAction_GetUp->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsHitCombo = { nullptr };
				_uint* pICurrentSpell = { nullptr };
				_uint* pIPreviusSpell = { nullptr };
				CRigidBody* pRigidBody = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isHitCombo", pIsHitCombo)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("iPreviusSpell", pIPreviusSpell)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("pRigidBody", pRigidBody)))
					return false;

				pRigidBody->Set_Gravity(true);

				*pIPreviusSpell = BUFF_NONE;
				*pICurrentSpell = BUFF_NONE;

				*pIsHitCombo = false;

				return true;
			});

		/* Set Options */
		pAction_Hit_1->Set_Options(TEXT("Air_Hit_1"), m_pModelCom);
		pAction_Hit_3->Set_Options(TEXT("Air_Hit_3"), m_pModelCom);
		pAction_Hit_4->Set_Options(TEXT("Air_Hit_4"), m_pModelCom);
		pAction_Hit_5->Set_Options(TEXT("Air_Hit_5"), m_pModelCom);
		pAction_Hit_6->Set_Options(TEXT("Air_Hit_6"), m_pModelCom);
		pAction_Hit_8->Set_Options(TEXT("Air_Hit_8"), m_pModelCom);
		pAction_Knockback->Set_Options(TEXT("Knockback_Back"), m_pModelCom);
		pAction_Splat->Set_Options(TEXT("Knockback_Back_Splat"), m_pModelCom);
		pAction_GetUp->Set_Options(TEXT("Get_Up_Send"), m_pModelCom);

		/* Assemble Childs */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_AirHit"), pSelector_AirHit)))
			throw TEXT("Failed Assemble_Behavior Selector_AirHit");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Splat"), pAction_Splat)))
			throw TEXT("Failed Assemble_Behavior Action_Splat");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_GetUp"), pAction_GetUp)))
			throw TEXT("Failed Assemble_Behavior Action_GetUp");

		if (FAILED(pSelector_AirHit->Assemble_Behavior(TEXT("Random_AirHit"), pRandom_AirHit)))
			throw TEXT("Failed Assemble_Behavior Random_AirHit");
		if (FAILED(pSelector_AirHit->Assemble_Behavior(TEXT("Action_Knockback"), pAction_Knockback)))
			throw TEXT("Failed Assemble_Behavior Action_Knockback");

		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_1"), pAction_Hit_1, 0.125f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_1");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_3"), pAction_Hit_3, 0.125f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_3");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_4"), pAction_Hit_4, 0.125f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_4");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_5"), pAction_Hit_5, 0.125f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_5");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_6"), pAction_Hit_6, 0.125f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_6");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_8"), pAction_Hit_8, 0.12f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_8");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Descendo : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Random_Idle_Move(_Inout_ CRandom_Select* pRandomSelect)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pRandomSelect)
			throw TEXT("Parameter pRandomSelect is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Idle = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Idle)
			throw TEXT("pAction_Idle is nullptr");
		CSequence_MoveTarget* pSequence_Move_1StepLeft = dynamic_cast<CSequence_MoveTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_MoveTarget")));
		if (nullptr == pSequence_Move_1StepLeft)
			throw TEXT("pSequence_Move_1StepLeft is nullptr");
		CSequence_MoveTarget* pSequence_Move_1StepRight = dynamic_cast<CSequence_MoveTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_MoveTarget")));
		if (nullptr == pSequence_Move_1StepRight)
			throw TEXT("pSequence_Move_1StepRight is nullptr");

		CWait* pTsk_Wait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
		if (nullptr == pTsk_Wait)
			throw TEXT("pTsk_Wait is nullptr");

		/* Set Options */
		pAction_Idle->Set_Options(TEXT("Idle"), m_pModelCom, true);
		pSequence_Move_1StepLeft->Set_Action_Options(TEXT("1Step_Left"), m_pModelCom);
		pSequence_Move_1StepRight->Set_Action_Options(TEXT("1Step_Right"), m_pModelCom);
		pTsk_Wait->Set_Timer(1.f);

		/* Assemble Behaviors */
		if (FAILED(pRandomSelect->Assemble_Behavior(TEXT("Action_Idle"), pAction_Idle, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Action_Idle");
		if (FAILED(pRandomSelect->Assemble_Behavior(TEXT("Sequence_Move_1StepLeft"), pSequence_Move_1StepLeft, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Move_1StepLeft");
		if (FAILED(pRandomSelect->Assemble_Behavior(TEXT("Sequence_Move_1StepRight"), pSequence_Move_1StepRight, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Move_1StepRight");

		if (FAILED(pAction_Idle->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Random_Idle_Move : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Death(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		CSelector* pSelector_Choose = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_Choose)
			throw TEXT("pSelector_Choose is nullptr");

		CSequence* pSequence_Death_Ground = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Death_Ground)
			throw TEXT("pSequence_Death_Ground is nullptr");
		CSequence* pSequence_Death_Air = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Death_Air)
			throw TEXT("pSequence_Death_Air is nullptr");

		CAction* pAction_Death_Ground = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Death_Ground)
			throw TEXT("pAction_Death_Ground is nullptr");
		CAction* pAction_Death_Ground_Loop = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Death_Ground_Loop)
			throw TEXT("pAction_Death_Ground_Loop is nullptr");
		CAction* pAction_Knockback = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Knockback)
			throw TEXT("pAction_Knockback is nullptr");
		CAction* pAction_Splat = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Splat)
			throw TEXT("pAction_Splat is nullptr"); 
		CAction* pAction_Splat_Loop = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Splat_Loop)
			throw TEXT("pAction_Splat_Loop is nullptr");

		CDeath* pTsk_Death_Ground = dynamic_cast<CDeath*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Death")));
		if (nullptr == pTsk_Death_Ground)
			throw TEXT("pTsk_Death_Ground is nullptr");
		CDeath* pTsk_Death_Air = dynamic_cast<CDeath*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Death")));
		if (nullptr == pTsk_Death_Air)
			throw TEXT("pTsk_Death_Air is nullptr");
		
		// Set Decorators
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return pHealth->isDead();
			});
		pSequence_Death_Ground->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pIPreviusSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iPreviusSpell", pIPreviusSpell)))
					return false;

				if (BUFF_LEVIOSO & *pIPreviusSpell)
					return false;

				cout << "Ground" << endl;
				return true;
			});
		pSequence_Death_Air->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pIPreviusSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iPreviusSpell", pIPreviusSpell)))
					return false;

				if (BUFF_LEVIOSO & *pIPreviusSpell)
				{
					CRigidBody* pRigidBody = { nullptr };
					if (FAILED(pBlackBoard->Get_Type("pRigidBody", pRigidBody)))
						return false;

					pRigidBody->Set_Gravity(true);

					cout << "Air" << endl;
					return true;
				}

				return false;
			});

		// Set Options 
		function<void(const _float&)> Func = [&](const _float& fTimeDelta) {this->DeathBehavior(fTimeDelta); };
		pTsk_Death_Ground->Set_DeathFunction(Func); 
		pTsk_Death_Air->Set_DeathFunction(Func);
		pAction_Knockback->Set_Options(TEXT("Knockback_Back"), m_pModelCom);
		pAction_Splat->Set_Options(TEXT("Knockback_Back_Splat"), m_pModelCom, true);
		pAction_Splat_Loop->Set_Options(TEXT("Send_Splat_Loop"), m_pModelCom, true);
		pAction_Death_Ground->Set_Options(TEXT("Death"), m_pModelCom);
		pAction_Death_Ground_Loop->Set_Options(TEXT("Send_Splat_Loop"), m_pModelCom, true);

		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Choose"), pSelector_Choose)))
			throw TEXT("Failed Assemble_Behavior Selector_Choose");

		if (FAILED(pSelector_Choose->Assemble_Behavior(TEXT("Sequence_Death_Ground"), pSequence_Death_Ground)))
			throw TEXT("Failed Assemble_Behavior Sequence_Death_Ground");
		if (FAILED(pSelector_Choose->Assemble_Behavior(TEXT("Sequence_Death_Air"), pSequence_Death_Air)))
			throw TEXT("Failed Assemble_Behavior Sequence_Death_Air");

		if (FAILED(pSequence_Death_Ground->Assemble_Behavior(TEXT("Action_Death_Ground"), pAction_Death_Ground)))
			throw TEXT("Failed Assemble_Behavior Action_Death_Ground");
		if (FAILED(pSequence_Death_Ground->Assemble_Behavior(TEXT("Action_Death_Ground_Loop"), pAction_Death_Ground_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Death_Ground_Loop");
		if (FAILED(pSequence_Death_Air->Assemble_Behavior(TEXT("Action_Knockback"), pAction_Knockback)))
			throw TEXT("Failed Assemble_Behavior Action_Knockback");
		if (FAILED(pSequence_Death_Air->Assemble_Behavior(TEXT("Action_Splat"), pAction_Splat)))
			throw TEXT("Failed Assemble_Behavior Action_Splat");
		if (FAILED(pSequence_Death_Air->Assemble_Behavior(TEXT("Action_Splat_Loop"), pAction_Splat_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Splat_Loop");

		if (FAILED(pAction_Death_Ground_Loop->Assemble_Behavior(TEXT("Tsk_Death_Ground"), pTsk_Death_Ground)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death_Ground");
		if (FAILED(pAction_Splat_Loop->Assemble_Behavior(TEXT("Tsk_Death_Air"), pTsk_Death_Air)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death_Air");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Random_Idle_Move : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CGolem_Combat::DeathBehavior(const _float& fTimeDelta)
{
	m_fDeadTimeAcc += fTimeDelta;
	if (5.f < m_fDeadTimeAcc)
		Set_ObjEvent(OBJ_DEAD);
}

HRESULT CGolem_Combat::Make_Turns(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turns */
	{
		/* Make Child Behaviors */
		CCheck_Degree* pTsk_Check_Degree = dynamic_cast<CCheck_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Degree")));
		if (nullptr == pTsk_Check_Degree)
			throw TEXT("pTsk_Check_Degree is nullptr");
		CSelector_Degree* pSelector_Degree = dynamic_cast<CSelector_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector_Degree")));
		if (nullptr == pSelector_Degree)
			throw TEXT("pSelector_Choose_Degree is nullptr");

		CAction* pAction_Right_Back = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_Back)
			throw TEXT("pAction_Right_Back is nullptr");
		CAction* pAction_Left_Back = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_Back)
			throw TEXT("pAction_Left_Back is nullptr");
		CAction* pAction_Left90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left90)
			throw TEXT("pAction_Left90 is nullptr");
		CAction* pAction_Right90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right90)
			throw TEXT("pAction_Right90 is nullptr");
		CAction* pAction_Left135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left135)
			throw TEXT("pAction_Left135 is nullptr");
		CAction* pAction_Right135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right135)
			throw TEXT("pAction_Right135 is nullptr");
		/* Set Decorations */

		/* Set Options */
		pAction_Right_Back->Set_Options(TEXT("Turn_Right_180"), m_pModelCom);
		pAction_Left_Back->Set_Options(TEXT("Turn_Left_180"), m_pModelCom);
		pAction_Left90->Set_Options(TEXT("Turn_Left_90"), m_pModelCom);
		pAction_Right90->Set_Options(TEXT("Turn_Right_90"), m_pModelCom);
		pAction_Left135->Set_Options(TEXT("Turn_Left_180"), m_pModelCom);
		pAction_Right135->Set_Options(TEXT("Turn_Right_180"), m_pModelCom);

		pTsk_Check_Degree->Set_Transform(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_Check_Degree"), pTsk_Check_Degree)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Degree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");

		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_BACK, pAction_Right_Back)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_BACK, pAction_Left_Back)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_90, pAction_Left90)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_135, pAction_Left135)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_90, pAction_Right90)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_135, pAction_Right135)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_135");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Attack(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack */
	{
		/* Make Child Behaviors */
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		CRandom_Select* pRandom_Attack = dynamic_cast<CRandom_Select*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Random_Select")));
		if (nullptr == pRandom_Attack)
			throw TEXT("pRandom_Attack is nullptr");

		CAction* pAction_Protego_Deflect = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Protego_Deflect)
			throw TEXT("pAction_Protego_Deflect is nullptr");

		CSequence_Attack* pSequence_Attack_Slash = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_Slash)
			throw TEXT("pSequence_Attack_Slash is nullptr");
		CSequence_Attack* pSequence_Attack_Shoulder = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_Shoulder)
			throw TEXT("pSequence_Attack_Shoulder is nullptr");
		CSequence_Attack* pSequence_Attack_OverHand = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_OverHand)
			throw TEXT("pSequence_Attack_OverHand is nullptr");
		CSequence_Attack* pSequence_Attack_Jab = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_Jab)
			throw TEXT("pSequence_Attack_Jab is nullptr");

		/* Set Decorations */
		pRandom_Attack->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsParring = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isParring", pIsParring)))
					return false;

				return !*pIsParring;
			});
		pAction_Protego_Deflect->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsParring = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isParring", pIsParring)))
					return false;

				return *pIsParring;
			});
		pAction_Protego_Deflect->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsParring = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isParring", pIsParring)))
					return false;

				*pIsParring = false;

				return true;
			});

		/* Set Options */
		pSequence_Attack_Slash->Set_Attack_Action_Options(TEXT("Attack_Slash_Sword"), m_pModelCom);
		pSequence_Attack_Slash->Set_Attack_Option(7.f);
		pSequence_Attack_Shoulder->Set_Attack_Action_Options(TEXT("Attack_Shoulder"), m_pModelCom);
		pSequence_Attack_Shoulder->Set_Attack_Option(4.f);
		pSequence_Attack_OverHand->Set_Attack_Action_Options(TEXT("Attack_OverHand_Sword"), m_pModelCom);
		pSequence_Attack_OverHand->Set_Attack_Option(7.5f);
		pSequence_Attack_Jab->Set_Attack_Action_Options(TEXT("Attack_Jab"), m_pModelCom);
		pSequence_Attack_Jab->Set_Attack_Option(4.5f);
		pRandom_Attack->Set_Option(5.f);

		pAction_Protego_Deflect->Set_Options(TEXT("Protego_Deflect"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("RandomChoose"), pRandom_Attack)))
			throw TEXT("Failed Assemble_Behavior RandomChoose");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Protego_Deflect"), pAction_Protego_Deflect)))
			throw TEXT("Failed Assemble_Behavior Action_Protego_Deflect");

		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_Slash"), pSequence_Attack_Slash, 0.3f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Slash");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_Shoulder"), pSequence_Attack_Shoulder, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Shoulder");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_OverHand"), pSequence_Attack_OverHand, 0.3f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_OverHand");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_Jab"), pSequence_Attack_Jab, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Jab");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Attack : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_NormalAttack(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		CSequence* pSequence_Turns = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Turns)
			throw TEXT("pSequence_Turns is nullptr");
		CSequence_MoveTarget* pSequence_MoveTarget = dynamic_cast<CSequence_MoveTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_MoveTarget")));
		if (nullptr == pSequence_MoveTarget)
			throw TEXT("pSequence_MoveTarget is nullptr");
		CSequence_MoveTarget* pSequence_MoveBackTarget = dynamic_cast<CSequence_MoveTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_MoveTarget")));
		if (nullptr == pSequence_MoveBackTarget)
			throw TEXT("pSequence_MoveBackTarget is nullptr");
		CSelector* pSelector_Attacks = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_Attacks)
			throw TEXT("pSelector_Attacks is nullptr");
		CRandom_Select* pRandom_IdleMove = dynamic_cast<CRandom_Select*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Random_Select")));
		if (nullptr == pRandom_IdleMove)
			throw TEXT("pRandom_IdleMove is nullptr");

		/* Set Decorations */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_NONE != *pICurrentSpell)
					return false;

				return true;
			});
		pSequence_MoveTarget->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float fAttackRange = { 0.f };
				_float fTargetDistance = { 0.f };
				if (FAILED(pBlackBoard->Get_Type("fAttackRange", fAttackRange)))
				{
					MSG_BOX("Failed Get_Type fAttackRange");
					return false;
				}

				if (FAILED(pBlackBoard->Get_Type("fTargetDistance", fTargetDistance)))
				{
					MSG_BOX("Failed Get_Type fTargetDistance");
					return false;
				}

				return fAttackRange < fTargetDistance;
			});
		pSequence_MoveBackTarget->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float fAttackRange = { 0.f };
				_float fTargetDistance = { 0.f };
				if (FAILED(pBlackBoard->Get_Type("fAttackRange", fAttackRange)))
				{
					MSG_BOX("Failed Get_Type fAttackRange");
					return false;
				}

				if (FAILED(pBlackBoard->Get_Type("fTargetDistance", fTargetDistance)))
				{
					MSG_BOX("Failed Get_Type fTargetDistance");
					return false;
				}

				return (fAttackRange - 1.f) > fTargetDistance;
			});
		pSelector_Attacks->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float fAttackRange = { 0.f };
				_float fTargetDistance = { 0.f };
				if (FAILED(pBlackBoard->Get_Type("fAttackRange", fAttackRange)))
				{
					MSG_BOX("Failed Get_Type fAttackRange");
					return false;
				}

				if (FAILED(pBlackBoard->Get_Type("fTargetDistance", fTargetDistance)))
				{
					MSG_BOX("Failed Get_Type fTargetDistance");
					return false;
				}

				return (fAttackRange >= fTargetDistance && (fAttackRange - 1.f) <= fTargetDistance);
			});

		/* Set Options */
		pSequence_MoveTarget->Set_Action_Options(TEXT("Move_Front"), m_pModelCom);
		pSequence_MoveBackTarget->Set_Action_Options(TEXT("Move_Back"), m_pModelCom);

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Turns"), pSequence_Turns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turns");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_MoveTarget"), pSequence_MoveTarget)))
			throw TEXT("Failed Assemble_Behavior Sequence_MoveTarget");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_MoveBackTarget"), pSequence_MoveBackTarget)))
			throw TEXT("Failed Assemble_Behavior Sequence_MoveBackTarget");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Attacks"), pSelector_Attacks)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attacks");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Random_Idle_Move"), pRandom_IdleMove)))
			throw TEXT("Failed Assemble_Behavior Random_Idle_Move");

		if (FAILED(Make_Turns(pSequence_Turns)))
			throw TEXT("Failed Make_Turns");
		if (FAILED(Make_Attack(pSelector_Attacks)))
			throw TEXT("Failed Make_Attack");
		if (FAILED(Make_Random_Idle_Move(pRandom_IdleMove)))
			throw TEXT("Failed Make_Random_Idle_Move");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_NormalAttack : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;;
}

HRESULT CGolem_Combat::Make_Hit_Combo(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSelector* pSelector_Levioso_Combo = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_Levioso_Combo)
			throw TEXT("pSelector_Levioso_Combo is nullptr");

		/* Set Decorations */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsHitCombo = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isHitCombo", pIsHitCombo)))
					return false;

				return *pIsHitCombo;
			});
		pSelector->Add_Fail_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsHitCombo = { nullptr };
				_uint* pICurrentSpell = { nullptr };
				_uint* pIPreviusSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isHitCombo", pIsHitCombo)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("iPreviusSpell", pIPreviusSpell)))
					return false;

				// 이부분은 애매한 처리임. 나중에 몬스터에 맞은 마법값이 이상하게 동작할 경우 확인할 것
				*pICurrentSpell = BUFF_NONE;
				*pIPreviusSpell = BUFF_NONE;

				*pIsHitCombo = false;

				return true;
			});

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Levioso_Combo"), pSelector_Levioso_Combo)))
			throw TEXT("Failed Assemble_Behavior Selector_Levioso_Combo");

		if (FAILED(Make_Levioso_Combo(pSelector_Levioso_Combo)))
			throw TEXT("Failed Make_Levioso_Combo");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Hit : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Check_Spell(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSequence_Groggy* pSequence_Groggy = dynamic_cast<CSequence_Groggy*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Groggy")));
		if (nullptr == pSequence_Groggy)
			throw TEXT("pSequence_Groggy is nullptr");
		CSequence_Levitated* pSequence_Levitated = dynamic_cast<CSequence_Levitated*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Levitated")));
		if (nullptr == pSequence_Levitated)
			throw TEXT("pSequence_Levitated is nullptr");
		CSequence* pSequence_Descendo = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Descendo)
			throw TEXT("pSequence_Descendo is nullptr");

		/* Set Decorations */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsHitCombo = { nullptr };
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isHitCombo", pIsHitCombo)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_NONE == *pICurrentSpell ||
					true == *pIsHitCombo)
					return false;

				return true;
			});

		/* Set_Options */
		pSequence_Groggy->Set_LoopTime(3.f);

		/* Stupefy */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Groggy"), pSequence_Groggy)))
			throw TEXT("Failed Assemble_Behavior Sequence_Groggy");
		/* Levioso */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Levitated"), pSequence_Levitated)))
			throw TEXT("Failed Assemble_Behavior Sequence_Levitated");
		/* Descendo */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Descendo"), pSequence_Descendo)))
			throw TEXT("Failed Assemble_Behavior Sequence_Descendo");

		if (FAILED(Make_Descendo(pSequence_Descendo)))
			throw TEXT("Failed Make_Descendo");

		if (FAILED(pSequence_Levitated->Assemble_Random_Select_Behavior(TEXT("Levitated_Loop_1"), 0.2f, 4.f)))
			throw TEXT("Failed Assemble_Random_Select_Behavior");
		if (FAILED(pSequence_Levitated->Assemble_Random_Select_Behavior(TEXT("Levitated_Loop_2"), 0.2f, 4.f)))
			throw TEXT("Failed Assemble_Random_Select_Behavior");
		if (FAILED(pSequence_Levitated->Assemble_Random_Select_Behavior(TEXT("Levitated_Loop_3"), 0.2f, 4.f)))
			throw TEXT("Failed Assemble_Random_Select_Behavior");
		if (FAILED(pSequence_Levitated->Assemble_Random_Select_Behavior(TEXT("Levitated_Loop_4"), 0.2f, 4.f)))
			throw TEXT("Failed Assemble_Random_Select_Behavior");
		if (FAILED(pSequence_Levitated->Assemble_Random_Select_Behavior(TEXT("Levitated_Loop_5"), 0.2f, 4.f)))
			throw TEXT("Failed Assemble_Random_Select_Behavior");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Check_Spell : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;;
}

CGolem_Combat* CGolem_Combat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGolem_Combat* pInstance = New CGolem_Combat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CGolem_Combat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGolem_Combat* CGolem_Combat::Clone(void* pArg)
{
	CGolem_Combat* pInstance = New CGolem_Combat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CGolem_Combat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGolem_Combat::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pWeapon);
	}
}
