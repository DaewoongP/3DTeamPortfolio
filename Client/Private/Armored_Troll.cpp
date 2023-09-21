#include "Armored_Troll.h"
#include "GameInstance.h"
#include "Weapon_Armored_Troll.h"

#include "Wait.h"
#include "Action.h"
#include "MagicBall.h"
#include "Check_Degree.h"
#include "Random_Select.h"
#include "Action_Deflect.h"
#include "Selector_Degree.h"
#include "Sequence_Groggy.h"
#include "Sequence_Attack.h"
#include "Sequence_Levitated.h"
#include "Sequence_MoveTarget.h"

CArmored_Troll::CArmored_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CArmored_Troll::CArmored_Troll(const CArmored_Troll& rhs)
	: CGameObject(rhs)
{
}

HRESULT CArmored_Troll::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CArmored_Troll::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float4x4* pWorldMatric = reinterpret_cast<_float4x4*>(pArg);
		m_pTransform->Set_WorldMatrix(*pWorldMatric);
	}
	else
		m_pTransform->Set_Position(_float3(20.f, 2.f, 20.f));

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

void CArmored_Troll::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Set_Current_Target();

	if (nullptr != m_pRootBehavior)
		m_pRootBehavior->Tick(fTimeDelta);

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CArmored_Troll::Late_Tick(_float fTimeDelta)
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

void CArmored_Troll::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//cout << "Troll Enter" << endl;
}

HRESULT CArmored_Troll::Render()
{
#ifdef _DEBUG
	Tick_ImGui();
#endif // _DEBUG

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Bind_Material Diffuse");

			if (FAILED(m_pShaderCom->Begin("AnimMesh")))
				throw TEXT("Shader Begin AnimMesh");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CArmored_Troll::Render_Depth()
{
	return S_OK;
}

HRESULT CArmored_Troll::Make_AI()
{
	BEGININSTANCE;

	try
	{
#pragma region Add_Types
		if (FAILED(m_pRootBehavior->Add_Type("pTransform", m_pTransform)))
			throw TEXT("Failed Add_Type pTransform");
		if (FAILED(m_pRootBehavior->Add_Type("pModel", m_pModelCom)))
			throw TEXT("Failed Add_Type pModel");
		if (FAILED(m_pRootBehavior->Add_Type("isChangeAnimation", &m_isChangeAnimation)))
			throw TEXT("Failed Add_Type isChangeAnimation");

		if (FAILED(m_pRootBehavior->Add_Type("fTargetDistance", _float())))
			throw TEXT("Failed Add_Type fTargetDistance");
		if (FAILED(m_pRootBehavior->Add_Type("fAttackRange", _float())))
			throw TEXT("Failed Add_Type fAttackRange");
		if (FAILED(m_pRootBehavior->Add_Type("fTargetToDegree", _float())))
			throw TEXT("Failed Add_Type fTargetToDegree");
		if (FAILED(m_pRootBehavior->Add_Type("isTargetToLeft", _bool())))
			throw TEXT("Failed Add_Type isTargetToLeft");

		if (FAILED(m_pRootBehavior->Add_Type("isParring", &m_isParring)))
			throw TEXT("Failed Add_Type isParring");
		if (FAILED(m_pRootBehavior->Add_Type("isSpawn", &m_isSpawn)))
			throw TEXT("Failed Add_Type isSpawn");
		if (FAILED(m_pRootBehavior->Add_Type("iCurrentSpell", &m_iCurrentSpell)))
			throw TEXT("Failed Add_Type iCurrentSpell");

		m_pTarget = dynamic_cast<CGameObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_Player"), TEXT("GameObject_Player")));

		if (nullptr == m_pTarget)
			throw TEXT("m_pTarget is nullptr");
		if (FAILED(m_pRootBehavior->Add_Type("cppTarget", &m_pTarget)))
			throw TEXT("Failed Add_Type cppTarget");
#pragma endregion Add_Types

		/* Make Childs */
		CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector)
			throw TEXT("pSelector is nullptr");

		CSequence* pSequence_Turns = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Turns)
			throw TEXT("pSequence_Turns is nullptr");
		CSequence* pSequence_Attack_Charge = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Charge)
			throw TEXT("pSequence_Attack_Charge is nullptr");

		/* Bind Childs */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("RootBehavior Assemble pSelector");

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Turns"), pSequence_Turns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turns");
		if(FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Attack_Charge"), pSequence_Attack_Charge)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Charge");

		if (FAILED(Make_Turns(pSequence_Turns)))
			throw TEXT("Failed Make_Turns pSequence_Turns");
		if(FAILED(Make_Charge(pSequence_Attack_Charge)))
			throw TEXT("Failed Make_Charge pSequence_Attack_Charge");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_AI : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Add_Components()
{
	try /* Check Add_Components */
	{
		/* Com_Renderer */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
			TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw TEXT("Com_Renderer");

		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Armored_Troll"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");

		/* For.Com_Shader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			throw TEXT("Com_Shader");

		/* For.Com_RootBehavior */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RootBehavior"),
			TEXT("Com_RootBehavior"), reinterpret_cast<CComponent**>(&m_pRootBehavior))))
			throw TEXT("Com_RootBehavior");

		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 2.5f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(1.2f, 1.5f);
		RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f);
		RigidBodyDesc.pOwnerObject = this;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");

		/* For.Com_RigidBody */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("SKT_RightHand"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Armored_Troll::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = _float4x4();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Armored_Troll"),
			TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Add_Components : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CArmored_Troll::SetUp_ShaderResources()
{
	BEGININSTANCE;

	try /* Check SetUp_ShaderResources */
	{
		if (nullptr == m_pShaderCom)
			throw TEXT("m_pShaderCom is nullptr");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			throw TEXT("Failed Bind_Matrix : g_WorldMatrix");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			throw TEXT("Failed Bind_Matrix : g_ViewMatrix");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			throw TEXT("Failed Bind_Matrix : g_ProjMatrix");

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : g_fCamFar");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CArmored_Troll::Set_Current_Target()
{
	_float3 vPosition = m_pTransform->Get_Position();

	for (auto& Pair : m_RangeInEnemies)
	{
		if (nullptr == m_pTarget)
			m_pTarget = Pair.second;
		else
		{
			_float3 vSrcTargetPosition = m_pTarget->Get_Transform()->Get_Position();
			_float3 vDstTargetPosition = Pair.second->Get_Transform()->Get_Position();

			_float vSrcDistance = _float3::Distance(vPosition, vSrcTargetPosition);
			_float vDstDistance = _float3::Distance(vPosition, vDstTargetPosition);

			m_pTarget = (vSrcDistance < vDstDistance) ? m_pTarget : Pair.second;
		}
	}

	m_isRangeInEnemy = (0 < m_RangeInEnemies.size()) ? true : false;

	if (false == m_isRangeInEnemy)
	{
		m_pTarget = nullptr;
	}
}

HRESULT CArmored_Troll::Remove_GameObject(const wstring & wstrObjectTag)
{
	auto iter = find_if(m_RangeInEnemies.begin(), m_RangeInEnemies.end(), [&](auto& Pair)->_bool
		{
			if (wstring::npos != Pair.first.find(wstrObjectTag))
				return true;

			return false;
		});

	if (iter == m_RangeInEnemies.end())
		return E_FAIL;

	//Safe_Release(iter->second);
	m_RangeInEnemies.erase(iter);

	return S_OK;
}

#ifdef _DEBUG
void CArmored_Troll::Tick_ImGui()
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

	ImGui::Begin("Test Troll");

	if (ImGui::InputInt("animIndex##Armored", &m_iIndex))
		m_pModelCom->Change_Animation(m_iIndex);

	ImGui::SeparatorText("Behavior");

	vector<wstring> DebugBehaviorTags = m_pRootBehavior->Get_DebugBahaviorTags();

	for (auto& Tag : DebugBehaviorTags)
	{
		ImGui::Text(wstrToStr(Tag).c_str());
	}

	ImGui::End();
}

#endif // _DEBUG

HRESULT CArmored_Troll::Make_Turns(_Inout_ CSequence* pSequence)
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

		CAction* pAction_Left_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_45)
			throw TEXT("pAction_Left_45 is nullptr");
		CAction* pAction_Right_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_45)
			throw TEXT("pAction_Right_45 is nullptr");
		CAction* pAction_Left_90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_90)
			throw TEXT("pAction_Left_90 is nullptr");
		CAction* pAction_Right_90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_90)
			throw TEXT("pAction_Right_90 is nullptr");
		CAction* pAction_Left_135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_135)
			throw TEXT("pAction_Left_135 is nullptr");
		CAction* pAction_Right_135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_135)
			throw TEXT("pAction_Right_135 is nullptr");
		CAction* pAction_Right_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_180)
			throw TEXT("pAction_Right_180 is nullptr");
		CAction* pAction_Left_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_180)
			throw TEXT("pAction_Left_180 is nullptr");
		/* Set Decorations */

		/* Set Options */
		pAction_Left_45->Set_Options(TEXT("Idle_Turn_Left_45"), m_pModelCom);
		pAction_Right_45->Set_Options(TEXT("Idle_Turn_Right_45"), m_pModelCom);
		pAction_Left_90->Set_Options(TEXT("Idle_Turn_Left_90"), m_pModelCom);
		pAction_Right_90->Set_Options(TEXT("Idle_Turn_Right_90"), m_pModelCom);
		pAction_Left_135->Set_Options(TEXT("Idle_Turn_Left_135"), m_pModelCom);
		pAction_Right_135->Set_Options(TEXT("Idle_Turn_Right_135"), m_pModelCom);
		pAction_Left_180->Set_Options(TEXT("Idle_Turn_Left_180"), m_pModelCom);
		pAction_Right_180->Set_Options(TEXT("Idle_Turn_Right_180"), m_pModelCom);

		pTsk_Check_Degree->Set_Transform(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_Check_Degree"), pTsk_Check_Degree)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Degree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");

		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_45, pAction_Left_45)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_45");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_45, pAction_Right_45)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_45");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_90, pAction_Left_90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_90, pAction_Right_90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_135, pAction_Left_135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_135, pAction_Right_135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_135");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_BACK, pAction_Left_180)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_BACK, pAction_Right_180)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_BACK");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Turn_Run(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turn_Run */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CCheck_Degree* pTsk_Check_Degree = dynamic_cast<CCheck_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Degree")));
		if (nullptr == pTsk_Check_Degree)
			throw TEXT("pTsk_Check_Degree is nullptr");
		CSelector_Degree* pSelector_Degree = dynamic_cast<CSelector_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector_Degree")));
		if (nullptr == pSelector_Degree)
			throw TEXT("pSelector_Choose_Degree is nullptr");

		CAction* pAction_Left_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_45)
			throw TEXT("pAction_Left_45 is nullptr");
		CAction* pAction_Right_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_45)
			throw TEXT("pAction_Right_45 is nullptr");
		CAction* pAction_Left_90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_90)
			throw TEXT("pAction_Left_90 is nullptr");
		CAction* pAction_Right_90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_90)
			throw TEXT("pAction_Right_90 is nullptr");
		CAction* pAction_Left_135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_135)
			throw TEXT("pAction_Left_135 is nullptr");
		CAction* pAction_Right_135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_135)
			throw TEXT("pAction_Right_135 is nullptr");
		CAction* pAction_Right_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_180)
			throw TEXT("pAction_Right_180 is nullptr");
		CAction* pAction_Left_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_180)
			throw TEXT("pAction_Left_180 is nullptr");
		/* Set Decorations */

		/* Set Options */
		pAction_Left_45->Set_Options(TEXT("Turn_Start_Run_Left_45"), m_pModelCom);
		pAction_Right_45->Set_Options(TEXT("Turn_Start_Run_Right_45"), m_pModelCom);
		pAction_Left_90->Set_Options(TEXT("Turn_Start_Run_Left_90"), m_pModelCom);
		pAction_Right_90->Set_Options(TEXT("Turn_Start_Run_Right_90"), m_pModelCom);
		pAction_Left_135->Set_Options(TEXT("Turn_Start_Run_Left_135"), m_pModelCom);
		pAction_Right_135->Set_Options(TEXT("Turn_Start_Run_Right_135"), m_pModelCom);
		pAction_Left_180->Set_Options(TEXT("Turn_Start_Run_Left_180"), m_pModelCom);
		pAction_Right_180->Set_Options(TEXT("Turn_Start_Run_Right_180"), m_pModelCom);

		pTsk_Check_Degree->Set_Transform(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_Check_Degree"), pTsk_Check_Degree)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Degree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");

		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_45, pAction_Left_45)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_45");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_45, pAction_Right_45)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_45");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_90, pAction_Left_90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_90, pAction_Right_90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_135, pAction_Left_135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_135, pAction_Right_135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_135");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_BACK, pAction_Left_180)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_BACK, pAction_Right_180)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_BACK");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;
	return E_NOTIMPL;
}

HRESULT CArmored_Troll::Make_Attack(_Inout_ CSelector* pSelector)
{ 
	return S_OK;
}

HRESULT CArmored_Troll::Make_Charge(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Charge_Enter = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Charge_Enter)
			throw TEXT("pAction_Charge_Enter is nullptr");
		CAction* pAction_Charge_Loop = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Charge_Loop)
			throw TEXT("pAction_Charge_Loop is nullptr");
		CAction* pAction_Charge_End = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Charge_End)
			throw TEXT("pAction_Charge_End is nullptr");

		CWait* pTsk_Wait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
		if (nullptr == pTsk_Wait)
			throw TEXT("pTsk_Wait is nullptr");

		/* Set Decorations */

		/* Set Options */
		pAction_Charge_Enter->Set_Options(TEXT("Attack_Charge_Enter"), m_pModelCom);
		pAction_Charge_Loop->Set_Options(TEXT("Attack_Charge_Loop"), m_pModelCom, true);
		pAction_Charge_End->Set_Options(TEXT("Attack_Charge_End_Turn_Left_180"), m_pModelCom);

		pTsk_Wait->Set_Timer(2.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Charge_Enter"), pAction_Charge_Enter)))
			throw TEXT("Failed Assemble_Childs Action_Charge_Enter");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Charge_Loop"), pAction_Charge_Loop)))
			throw TEXT("Failed Assemble_Childs Action_Charge_Loop");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Charge_End"), pAction_Charge_End)))
			throw TEXT("Failed Assemble_Childs Action_Charge_End");

		if (FAILED(pAction_Charge_Loop->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Charge : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_NormalAttack(_Inout_ CSelector* pSelector)
{
	return S_OK;
}

HRESULT CArmored_Troll::Make_Check_Spell(_Inout_ CSelector* pSelector)
{
	return S_OK;
}

HRESULT CArmored_Troll::Make_Random_Idle_Move(CRandom_Select* pRandomSelect)
{
	return S_OK;
}

CArmored_Troll* CArmored_Troll::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArmored_Troll* pInstance = New CArmored_Troll(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CArmored_Troll");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CArmored_Troll::Clone(void* pArg)
{
	CArmored_Troll* pInstance = New CArmored_Troll(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CArmored_Troll");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CArmored_Troll::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pWeapon);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pRenderer);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRigidBody);
		Safe_Release(m_pRootBehavior);
	}
}
