#include "Broom_Stick_DarkWizard.h"
#include "GameInstance.h"
#include "Player.h"
#include"ParticleSystem.h"

CBroom_Stick_DarkWizard::CBroom_Stick_DarkWizard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CParts(pDevice, pContext)
{
}

CBroom_Stick_DarkWizard::CBroom_Stick_DarkWizard(const CBroom_Stick_DarkWizard& rhs)
	: CParts(rhs)
{
}

HRESULT CBroom_Stick_DarkWizard::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBroom_Stick_DarkWizard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

HRESULT CBroom_Stick_DarkWizard::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	if (FAILED(__super::Initialize_Level(iCurrentLevelIndex)))
		return E_FAIL;

	_float3 vRadians = _float3(-81.5f, 155.f, -36.5f);
	vRadians.x = XMConvertToRadians(vRadians.x);
	vRadians.y = XMConvertToRadians(vRadians.y);
	vRadians.z = XMConvertToRadians(vRadians.z);

	_float4 vQuaternion = XMQuaternionRotationRollPitchYaw(vRadians.x, vRadians.y, vRadians.z);

	m_OffsetMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) *
		XMMatrixRotationQuaternion(vQuaternion) *
		XMMatrixTranslation(0.8f, 0.45f, -1.15f);

	return S_OK;
}

void CBroom_Stick_DarkWizard::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

#ifdef _DEBUG
	//ADD_IMGUI([&] { this->Tick_Debug(); });
#endif // _DEBUG
}

void CBroom_Stick_DarkWizard::Late_Tick(_float fTimeDelta)
{
	CGameObject::Late_Tick(fTimeDelta);

	_float4x4 CombinedWorldMatrix;
	// 자체 오프셋뼈(오프셋 * 뼈 * 피벗) * 부모의 월드
	CombinedWorldMatrix = m_OffsetMatrix * 
		m_ParentMatrixDesc.OffsetMatrix * 
		*m_ParentMatrixDesc.pCombindTransformationMatrix * 
		m_ParentMatrixDesc.PivotMatrix *
		*m_ParentMatrixDesc.pParentWorldMatrix;
	m_pTransform->Set_WorldMatrix(CombinedWorldMatrix);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBroom_Stick_DarkWizard::Render()
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Failed Bind_Material : g_DiffuseTexture");

			if (FAILED(m_pShaderCom->Begin("Mesh")))
				throw TEXT("Failed Begin : Mesh");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Failed Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CBroom_Stick_DarkWizard] Failed Render : \n");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
			__debugbreak();

			return E_FAIL;
		}
	}

	return S_OK;
}

#ifdef _DEBUG

void CBroom_Stick_DarkWizard::Tick_Debug()
{
	ImGui::Begin("TestShake");

	wstring wstrTag = m_pTag;
	string strTag = wstrToStr(wstrTag);

	ImGui::DragFloat3(strTag.c_str(), (_float*)&m_vPosition, 0.05f, -100.f, 10000.f);
	strTag += "Degree";
	
	_float3 vRadians;
	ImGui::DragFloat3(strTag.c_str(), (_float*)&m_vAngle, 0.5f, -360.f, 360.f);
	vRadians.x = XMConvertToRadians(m_vAngle.x);
	vRadians.y = XMConvertToRadians(m_vAngle.y);
	vRadians.z = XMConvertToRadians(m_vAngle.z);

	_float4 vQuaternion = XMQuaternionRotationRollPitchYaw(vRadians.x, vRadians.y, vRadians.z);

	m_OffsetMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) *
		XMMatrixRotationQuaternion(vQuaternion) *
		XMMatrixTranslation(m_vPosition.x, m_vPosition.y, m_vPosition.z);

	ImGui::End();
}

#endif // _DEBUG

HRESULT CBroom_Stick_DarkWizard::Add_Components()
{
	try /* Check Add_Components */
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
			(CComponent**)&m_pRendererCom, this)))
			throw TEXT("Failed Add_Component : Com_Renderer");

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader_Mesh"),
			(CComponent**)&m_pShaderCom, this)))
			throw TEXT("Failed Add_Component : Com_Shader_Mesh");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CBroom_Stick_DarkWizard] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBroom_Stick_DarkWizard::Add_Components_Level(_uint iCurrentLevelIndex)
{
	try
	{
		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_Broom_Stick_DarkWizard"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom, this)))
			throw TEXT("Failed Add_Component : Com_Model");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CBroom_Stick_DarkWizard] Failed Add_Components_Level : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBroom_Stick_DarkWizard::Set_Shader_Resources()
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
		wstring wstrErrorMSG = TEXT("[CBroom_Stick_DarkWizard] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

CBroom_Stick_DarkWizard* CBroom_Stick_DarkWizard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_Stick_DarkWizard* pInstance = New CBroom_Stick_DarkWizard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBroom_Stick_DarkWizard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBroom_Stick_DarkWizard* CBroom_Stick_DarkWizard::Clone(void* pArg)
{
	CBroom_Stick_DarkWizard* pInstance = New CBroom_Stick_DarkWizard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBroom_Stick_DarkWizard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBroom_Stick_DarkWizard::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRendererCom);
		//Safe_Release(m_pParticle);
	}
}
