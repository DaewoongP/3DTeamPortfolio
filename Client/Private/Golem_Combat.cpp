#include "Golem_Combat.h"
#include "GameInstance.h"

#include "Weapon_Golem_Combat.h"

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
	m_pTransform->Set_RigidBody(m_pRigidBody);

	return S_OK;
}

void CGolem_Combat::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pRootBehavior)
		m_pRootBehavior->Tick(fTimeDelta);

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta);
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

		/* Com_RigidBody */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody))))
			throw TEXT("Com_RigidBody");

		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Golem_Combat"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");

		/* For.Com_Shader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			throw TEXT("Com_Shader");

		/* For.Com_RootBehavior */
		/*if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RootBehavior"),
			TEXT("Com_RootBehavior"), reinterpret_cast<CComponent**>(&m_pRootBehavior))))
			throw TEXT("Com_RootBehavior");*/

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
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Add_Components : ");
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
		m_pModelCom->Reset_Animation(m_iIndex);

	ImGui::SeparatorText("Behavior");

	/*vector<wstring> DebugBehaviorTags = m_pRootBehavior->Get_DebugBahaviorTags();

	for (auto& Tag : DebugBehaviorTags)
	{
		ImGui::Text(wstrToStr(Tag).c_str());
	}*/

	ImGui::End();
}
#endif // _DEBUG

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
		Safe_Release(m_pRigidBody);
		Safe_Release(m_pRootBehavior);
	}
}