#include "Golem_Combat.h"
#include "GameInstance.h"

// 테스트용 
#include "Layer.h"
#include "Dummy.h"
//
#include "Weapon_Golem_Combat.h"

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
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

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

		CSequence* pSequence_Descendo = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Descendo)
			throw TEXT("pSequence_Descendo is nullptr");
		CSequence* pSequence_Move = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Descendo)
			throw TEXT("pSequence_Descendo is nullptr");

		/* Set Decorations */

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("Failed Assemble_Behavior Selector");

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Move"), pSequence_Move)))
			throw TEXT("Failed Assemble_Behavior Sequence_Move");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Descendo"), pSequence_Descendo)))
			throw TEXT("Failed Assemble_Behavior Sequence_Descendo");

		if (FAILED(Make_Descendo(pSequence_Descendo)))
			throw TEXT("Failed Make_Descendo");
		/*if (FAILED(Make_Move(pSequence_Move)))
			throw TEXT("Failed Make_Move");*/
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_AI : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
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
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

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
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

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

	try /* Failed Check_Make_Descendo */
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
		pAction_GetUp->Set_Options(TEXT("GetUp_Front"), m_pModelCom);

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
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Descendo : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Move(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		/* Make Child Behaviors */
		CTargetDegree* pTsk_TargetDegree = dynamic_cast<CTargetDegree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_TargetDegree")));
		if (nullptr == pTsk_TargetDegree)
			throw TEXT("pTsk_TargetDegree is nullptr");
		CSelector_Degree* pSelector_Degree = dynamic_cast<CSelector_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector_Degree")));
		if (nullptr == pSelector_Degree)
			throw TEXT("pSelector_Choose_Degree is nullptr");

		CSequence* pSequence_MoveFront = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_MoveFront)
			throw TEXT("pSequence_MoveFront is nullptr");
		CSequence* pSequence_MoveBack = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_MoveBack)
			throw TEXT("pSequence_MoveBack is nullptr");
		CSequence* pSequence_MoveLeft45 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_MoveLeft45)
			throw TEXT("pSequence_MoveLeft45 is nullptr");
		CSequence* pSequence_MoveLeft90 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_MoveLeft90)
			throw TEXT("pSequence_MoveLeft90 is nullptr");
		CSequence* pSequence_MoveLeft135 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_MoveLeft135)
			throw TEXT("pSequence_MoveLeft135 is nullptr");
		CSequence* pSequence_MoveRight45 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_MoveRight45)
			throw TEXT("pSequence_MoveRight45 is nullptr");
		CSequence* pSequence_MoveRight90 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_MoveRight90)
			throw TEXT("pSequence_MoveRight90 is nullptr");
		CSequence* pSequence_MoveRight135 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_MoveRight135)
			throw TEXT("pSequence_MoveRight135 is nullptr");

		CAction* pAction_Front = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Front)
			throw TEXT("pAction_Front is nullptr");
		CAction* pAction_Back = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Back)
			throw TEXT("pAction_Back is nullptr");
		CAction* pAction_Left45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left45)
			throw TEXT("pAction_Left45 is nullptr");
		CAction* pAction_Left90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left90)
			throw TEXT("pAction_Left90 is nullptr");
		CAction* pAction_Left135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left135)
			throw TEXT("pAction_Left135 is nullptr");
		CAction* pAction_Right45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right45)
			throw TEXT("pAction_Right45 is nullptr");
		CAction* pAction_Right90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right90)
			throw TEXT("pAction_Right90 is nullptr");
		CAction* pAction_Right135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right135)
			throw TEXT("pAction_Right135 is nullptr");

		CTurn* pTsk_Turn = dynamic_cast<CTurn*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Turn")));
		if(nullptr == pTsk_Turn)
			throw TEXT("pTsk_Turn is nullptr");

		/* Set Decorations */

		/* Set Options */
		// 이부분 피벗 고치면 제대로 변경할 것 Move Front <-> back
		// 여기랑 TagetDegree 클래스에 cross 함수부분도 바꿔줘야됨
		pTsk_TargetDegree->Set_Transform(m_pTransform);
		pAction_Front->Set_Options(TEXT("Move_Back"), m_pModelCom);
		pAction_Back->Set_Options(TEXT("Idle_Turn_180"), m_pModelCom);
		pAction_Left45->Set_Options(TEXT("Move_Left_Back"), m_pModelCom);
		pAction_Left90->Set_Options(TEXT("Idle_Turn_Left_90"), m_pModelCom);
		pAction_Left135->Set_Options(TEXT("Move_Left_Front"), m_pModelCom);
		pAction_Right45->Set_Options(TEXT("Move_Right_Back"), m_pModelCom);
		pAction_Right90->Set_Options(TEXT("Idle_Turn_Right_90"), m_pModelCom);
		pAction_Right135->Set_Options(TEXT("Move_Right_Front"), m_pModelCom);
		pTsk_Turn->Set_Transform(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_TargetDegree"), pTsk_TargetDegree)))
			throw TEXT("Failed Assemble_Behavior Tsk_TargetDegree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");

		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::FRONT, pSequence_MoveFront)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree FRONT");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::BACK, pSequence_MoveBack)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree BACK");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_45, pSequence_MoveLeft45)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_45");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_90, pSequence_MoveLeft90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_135, pSequence_MoveLeft135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_45, pSequence_MoveRight45)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_45");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_90, pSequence_MoveRight90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_135, pSequence_MoveRight135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_135");

		if (FAILED(pSequence_MoveFront->Assemble_Behavior(TEXT("Action_Front"), pAction_Front)))
			throw TEXT("Failed Assemble_Childs pSequence_MoveFront Action_Front");
		if (FAILED(pSequence_MoveBack->Assemble_Behavior(TEXT("Action_Back"), pAction_Back)))
			throw TEXT("Failed Assemble_Childs pSequence_MoveBack Action_Back");
		if (FAILED(pSequence_MoveLeft45->Assemble_Behavior(TEXT("Action_Left45"), pAction_Left45)))
			throw TEXT("Failed Assemble_Childs pSequence_MoveLeft45 Action_Left45");
		if (FAILED(pSequence_MoveLeft90->Assemble_Behavior(TEXT("Action_Left90"), pAction_Left90)))
			throw TEXT("Failed Assemble_Childs pSequence_MoveLeft90 Action_Left90");
		if (FAILED(pSequence_MoveLeft135->Assemble_Behavior(TEXT("Action_Left135"), pAction_Left135)))
			throw TEXT("Failed Assemble_Childs pSequence_MoveLeft135 Action_Left135");
		if (FAILED(pSequence_MoveRight45->Assemble_Behavior(TEXT("Action_Right45"), pAction_Right45)))
			throw TEXT("Failed Assemble_Childs pSequence_MoveRight45 Action_Right45");
		if (FAILED(pSequence_MoveRight90->Assemble_Behavior(TEXT("Action_Right90"), pAction_Right90)))
			throw TEXT("Failed Assemble_Childs pSequence_MoveRight90 Action_Right90");
		if (FAILED(pSequence_MoveRight135->Assemble_Behavior(TEXT("Action_Right135"), pAction_Right135)))
			throw TEXT("Failed Assemble_Childs pSequence_MoveRight135 Action_Right135");

		if (FAILED(pAction_Front->Assemble_Behavior(TEXT("Tsk_Turn"), pTsk_Turn)))
			throw TEXT("Failed Assemble_Childs pAction_Front Tsk_Turn");
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
