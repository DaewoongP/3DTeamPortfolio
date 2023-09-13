#include "Golem_Combat.h"
#include "GameInstance.h"

// 테스트용 
#include "Layer.h"
#include "Dummy.h"
//
#include "Weapon_Golem_Combat.h"

#include "Wait.h"
#include "Turn.h"
#include "Action.h"
#include "TargetDegree.h"
#include "Selector_Degree.h"

CGolem_Combat::CGolem_Combat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CGolem_Combat::CGolem_Combat(const CGolem_Combat& rhs)
	: CGameObject(rhs)
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

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

void CGolem_Combat::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (CGameInstance::GetInstance()->Get_DIKeyState(DIK_R, CInput_Device::KEY_DOWN))
		m_isParring = true;

	if (nullptr != m_pRootBehavior)
		m_pRootBehavior->Tick(fTimeDelta);

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CGolem_Combat::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CGolem_Combat::Render()
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
			wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MSG_BOX(wstrErrorMSG.c_str());

			return E_FAIL;
		}
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
		/* Add Types */
		if (FAILED(m_pRootBehavior->Add_Type("pTransform", m_pTransform)))
			throw TEXT("Failed Add_Type pTransform");
		if (FAILED(m_pRootBehavior->Add_Type("fTargetToDegree", _float())))
			throw TEXT("Failed Add_Type fTargetToDegree");
		if (FAILED(m_pRootBehavior->Add_Type("isTargetToLeft", _bool())))
			throw TEXT("Failed Add_Type isTargetToLeft");
		if (FAILED(m_pRootBehavior->Add_Type("isParring", &m_isParring)))
			throw TEXT("Failed Add_Type isParring");
		if (FAILED(m_pRootBehavior->Add_Type("isFirst", _bool())))
			throw TEXT("Failed Add_Type isFirst");

		/* 이거는 테스트 용으로 더미클래스 찾으려고 넣은 코드임 */
		CGameObject* pTestTarget = dynamic_cast<CGameObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_Debug"), TEXT("GameObject_Dummy")));
		if (nullptr == pTestTarget)
			throw TEXT("pTestTarget is nullptr");
		if (FAILED(m_pRootBehavior->Add_Type("pTarget", pTestTarget)))
			throw TEXT("Failed Add_Type pTarget");
		//////////////////////////////////////////////////////////

		/* Make Child Behaviors */
		CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector)
			throw TEXT("pSelector is nullptr");

		CSequence* pSequence_Turns = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Turns)
			throw TEXT("pSequence_Turns is nullptr");
		CSequence* pSequence_Move = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Move)
			throw TEXT("pSequence_Move is nullptr");
		CSelector* pSelector_Attacks = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_Attacks)
			throw TEXT("pSelector_Attacks is nullptr");
		CSequence* pSequence_Descendo = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Descendo)
			throw TEXT("pSequence_Descendo is nullptr");
		CSequence* pSequence_Groggy = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Groggy)
			throw TEXT("pSequence_Groggy is nullptr");

		/* Set Decorations */

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("Failed Assemble_Behavior Selector");

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Turns"), pSequence_Turns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turns");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Move"), pSequence_Move)))
			throw TEXT("Failed Assemble_Behavior Sequence_Move");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Attacks"), pSelector_Attacks)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attacks");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Groggy"), pSequence_Groggy)))
			throw TEXT("Failed Assemble_Behavior Sequence_Groggy");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Descendo"), pSequence_Descendo)))
			throw TEXT("Failed Assemble_Behavior Sequence_Descendo");

		if (FAILED(Make_Move(pSequence_Move)))
			throw TEXT("Failed Make_Move");
		if (FAILED(Make_Turns(pSequence_Turns)))
			throw TEXT("Failed Make_Turns");
		if (FAILED(Make_Groggy(pSequence_Groggy)))
			throw TEXT("Failed Make_Groggy");
		if (FAILED(Make_Attack(pSelector_Attacks)))
			throw TEXT("Failed Make_Attack");
		if (FAILED(Make_Descendo(pSequence_Descendo)))
			throw TEXT("Failed Make_Descendo");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_AI : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Add_Components()
{
	try /* Check Add_Components */
	{
		/* Com_Renderer */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
			TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw TEXT("Com_Renderer");

		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Golem_Combat"),
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

		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("SKT_RightHand"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Golem_Combat::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Golem_Combat"),
			TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGolem_Combat::SetUp_ShaderResources()
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
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
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

	if (ImGui::InputInt("animIndex##Armored", &m_iIndex))
		m_pModelCom->Change_Animation(m_iIndex);
	wstring wstrTag = m_pModelCom->Get_Animation(m_iIndex)->Get_AnimationName();
	ImGui::Text(wstrToStr(wstrTag).c_str());

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

		/* Set Options */
		pAction_Descendo1->Set_Options(TEXT("Descendo_1"), m_pModelCom);
		pAction_Descendo2->Set_Options(TEXT("Descendo_2"), m_pModelCom);
		pAction_Descendo3->Set_Options(TEXT("Descendo_3"), m_pModelCom);
		pAction_Descendo4->Set_Options(TEXT("Descendo_4"), m_pModelCom);
		pAction_GetUp->Set_Options(TEXT("Get_Up_Send_Front"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_Descendo1"), pAction_Descendo1)))
			throw TEXT("Failed Assemble_Behavior pAction_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_Descendo2"), pAction_Descendo2)))
			throw TEXT("Failed Assemble_Behavior pAction_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_Descendo3"), pAction_Descendo3)))
			throw TEXT("Failed Assemble_Behavior pAction_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_Descendo4"), pAction_Descendo4)))
			throw TEXT("Failed Assemble_Behavior pAction_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_GetUp"), pAction_GetUp)))
			throw TEXT("Failed Assemble_Behavior pAction_GetUp");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Descendo : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Turns(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turns */
	{
		/* Make Child Behaviors */
		CTargetDegree* pTsk_TargetDegree = dynamic_cast<CTargetDegree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_TargetDegree")));
		if (nullptr == pTsk_TargetDegree)
			throw TEXT("pTsk_TargetDegree is nullptr");
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

		pTsk_TargetDegree->Set_Transform(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_TargetDegree"), pTsk_TargetDegree)))
			throw TEXT("Failed Assemble_Behavior Tsk_TargetDegree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");

		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_BACK, pAction_Right_Back)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_BACK, pAction_Left_Back)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_90, pAction_Left90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_135, pAction_Left135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_90, pAction_Right90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_135, pAction_Right135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_135");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Move(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Move */
	{
		/* Make Child Behaviors */
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		CAction* pAction_Move = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Move)
			throw TEXT("pAction_Move is nullptr");

		CTargetDegree* pTsk_TargetDegree = dynamic_cast<CTargetDegree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_TargetDegree")));
		if (nullptr == pTsk_TargetDegree)
			throw TEXT("pTsk_TargetDegree is nullptr");
		CTurn* pTsk_Turn = dynamic_cast<CTurn*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Turn")));
		if(nullptr == pTsk_Turn)
			throw TEXT("pTsk_Turn is nullptr");

		/* Set Options */
		pAction_Move->Set_Options(TEXT("Move_Front"), m_pModelCom, false, 3.f, TEXT("Golem"), 3.f);
		pTsk_TargetDegree->Set_Transform(m_pTransform);
		pTsk_Turn->Set_Transform(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Move"), pAction_Move)))
			throw TEXT("Failed Assemble_Behavior Action_Move");

		if (FAILED(pAction_Move->Assemble_Behavior(TEXT("Tsk_TargetDegree"), pTsk_TargetDegree)))
			throw TEXT("Failed Assemble_Behavior Tsk_TargetDegree");
		if (FAILED(pAction_Move->Assemble_Behavior(TEXT("Tsk_Turn"), pTsk_Turn)))
			throw TEXT("Failed Assemble_Behavior Tsk_Turn");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Move : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

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

		CTurn* pTsk_Turn = dynamic_cast<CTurn*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Turn")));
		if (nullptr == pTsk_Turn)
			throw TEXT("pTsk_Turn is nullptr");
		CRandomChoose* pRandomChoose = dynamic_cast<CRandomChoose*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_RandomChoose")));
		if (nullptr == pRandomChoose)
			throw TEXT("pRandomChoose is nullptr");
		CAction* pAction_Protego_Deflect = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Protego_Deflect)
			throw TEXT("pAction_Protego_Deflect is nullptr");

		CAction* pAction_Attack_Slash = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Attack_Slash)
			throw TEXT("pAction_Attack_Slash is nullptr");
		CAction* pAction_Attack_Shoulder = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Attack_Shoulder)
			throw TEXT("pAction_Attack_Shoulder is nullptr");
		CAction* pAction_Attack_OverHand = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Attack_OverHand)
			throw TEXT("pAction_Attack_OverHand is nullptr");
		CAction* pAction_Attack_Jab = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Attack_Jab)
			throw TEXT("pAction_Attack_Jab is nullptr");

		/* Set Decorations */
		pRandomChoose->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsParring = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isParring", pIsParring)))
					return false;

				if (true == *pIsParring)
				{
					*pIsParring = false;
					if (FAILED(pBlackBoard->Set_Type("isFirst", true)))
						return false;

					return false;
				}

				return true;
			});

		pTsk_Turn->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float fTargetToDegree = { 0.f };
				if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fTargetToDegree)))
					return false;

				if (3.5f > fTargetToDegree)
					return false;

				return true;
			});

		/* Set Options */
		pTsk_Turn->Set_Degree(180.f);
		pTsk_Turn->Set_Transform(m_pTransform);

		pAction_Attack_Slash->Set_Options(TEXT("Attack_Slash_Sword"), m_pModelCom);
		pAction_Attack_Shoulder->Set_Options(TEXT("Attack_Shoulder"), m_pModelCom);
		pAction_Attack_OverHand->Set_Options(TEXT("Attack_OverHand_Sword"), m_pModelCom);
		pAction_Attack_Jab->Set_Options(TEXT("Attack_Jab"), m_pModelCom);

		pAction_Protego_Deflect->Set_Options(TEXT("Protego_Deflect"), m_pModelCom);

		/* Assemble Behaviors */
		if(FAILED(pSelector->Assemble_Behavior(TEXT("Tsk_Turn"), pTsk_Turn)))
			throw TEXT("Failed Assemble_Behavior Tsk_Turn");
		if(FAILED(pSelector->Assemble_Behavior(TEXT("RandomChoose"), pRandomChoose)))
			throw TEXT("Failed Assemble_Behavior RandomChoose");
		if(FAILED(pSelector->Assemble_Behavior(TEXT("Action_Protego_Deflect"), pAction_Protego_Deflect)))
			throw TEXT("Failed Assemble_Behavior Action_Protego_Deflect");

		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Attack_Slash"), pAction_Attack_Slash, 0.3f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Slash");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Attack_Shoulder"), pAction_Attack_Shoulder, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Shoulder");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Attack_OverHand"), pAction_Attack_OverHand, 0.3f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_OverHand");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Attack_Jab"), pAction_Attack_Jab, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Jab");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Attack : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Groggy(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Groggy */
	{
		/* Make Child Behaviors */
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		CAction* pAction_Groggy_Enter = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Groggy_Enter)
			throw TEXT("pAction_Groggy_Enter is nullptr");
		CAction* pAction_Groggy_Loop = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Groggy_Loop)
			throw TEXT("pAction_Groggy_Loop is nullptr");
		CAction* pAction_Groggy_End = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Groggy_End)
			throw TEXT("pAction_Groggy_End is nullptr");

		CWait* pTsk_Wait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
		if(nullptr == pTsk_Wait)
			throw TEXT("pTsk_Wait is nullptr");

		/* Set Decorations */

		/* Set Options */
		pAction_Groggy_Enter->Set_Options(TEXT("Groggy_Enter"), m_pModelCom);
		pAction_Groggy_Loop->Set_Options(TEXT("Groggy_Loop"), m_pModelCom, true);
		pAction_Groggy_End->Set_Options(TEXT("Groggy_End"), m_pModelCom);
		pTsk_Wait->Set_Timer(3.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Groggy_Enter"), pAction_Groggy_Enter)))
			throw TEXT("Failed Assemble_Behavior Action_Groggy_Enter");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Groggy_Loop"), pAction_Groggy_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Groggy_Loop");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Groggy_End"), pAction_Groggy_End)))
			throw TEXT("Failed Assemble_Behavior Action_Groggy_End");

		if (FAILED(pAction_Groggy_Loop->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Groggy : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
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

CGameObject* CGolem_Combat::Clone(void* pArg)
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
		Safe_Release(m_pModelCom);
		Safe_Release(m_pRenderer);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRootBehavior);
	}
}
