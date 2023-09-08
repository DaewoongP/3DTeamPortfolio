#include "Assasin_Goblin.h"
#include "GameInstance.h"
#include "PhysXConverter.h"

CAssassin_Goblin::CAssassin_Goblin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CAssassin_Goblin::CAssassin_Goblin(const CAssassin_Goblin& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAssassin_Goblin::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAssassin_Goblin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

void CAssassin_Goblin::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CAssassin_Goblin::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CAssassin_Goblin::Render()
{
#ifdef _DEBUG
	Tick_ImGui();
#endif // _DEBUG

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE);

		m_pShaderCom->Begin("AnimMesh");

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CAssassin_Goblin::Render_Depth()
{
	return S_OK;
}

HRESULT CAssassin_Goblin::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("[CAssassin_Goblin] Failed Add_Components : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody))))
	{
		MSG_BOX("[CAssassin_Goblin] Failed Add_Components : (Com_RigidBody)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_TestModel"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("[CAssassin_Goblin] Failed Add_Components : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("[CAssassin_Goblin] Failed Add_Components : (Com_Shader)");
		return E_FAIL;
	}

	//const CBone* pBone = m_pModelCom->Get_Bone(TEXT("RightHand"));
	//if (nullptr == pBone)
	//{
	//	MSG_BOX("[CAssassin_Goblin] Failed Add_Components : (pBone is nullptr)");
	//	return E_FAIL;
	//}

	//CWeapon_Armored_Troll::PARENTMATRIXDESC ParentMatrixDesc;
	//ParentMatrixDesc.OffsetMatrix = /*pBone->Get_OffsetMatrix();*/ _float4x4();
	//ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	//ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	//ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

	//if (FAILED(Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Armored_Troll"),
	//	TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
	//{
	//	MSG_BOX("[CAssassin_Goblin] Failed Add_Components : (Com_Weapon)");
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CAssassin_Goblin::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

#ifdef _DEBUG
void CAssassin_Goblin::Tick_ImGui()
{
	ImGui::Begin("Test Troll");

	if (ImGui::InputInt("animIndex", &m_iIndex))
		m_pModelCom->Reset_Animation(m_iIndex);

	ImGui::End();
}
#endif // _DEBUG

CAssassin_Goblin* CAssassin_Goblin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAssassin_Goblin* pInstance = New CAssassin_Goblin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAssassin_Goblin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAssassin_Goblin::Clone(void* pArg)
{
	CAssassin_Goblin* pInstance = New CAssassin_Goblin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAssassin_Goblin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAssassin_Goblin::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRenderer);
		Safe_Release(m_pRigidBody);
	}
}
