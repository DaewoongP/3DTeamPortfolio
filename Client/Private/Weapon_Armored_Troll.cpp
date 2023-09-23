#include "Weapon_Armored_Troll.h"

#include "GameInstance.h"

CWeapon_Armored_Troll::CWeapon_Armored_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CParts(pDevice, pContext)
{
}

CWeapon_Armored_Troll::CWeapon_Armored_Troll(const CWeapon_Armored_Troll& rhs)
	: CParts(rhs)
{
}

HRESULT CWeapon_Armored_Troll::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CWeapon_Armored_Troll::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

void CWeapon_Armored_Troll::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Armored_Troll::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CWeapon_Armored_Troll::Render()
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

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
			wstring wstrErrorMSG = TEXT("[CWeapon_Armored_Troll] Failed Render : \n");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
			__debugbreak();

			return E_FAIL;
		}
	}

	return __super::Render();
}

HRESULT CWeapon_Armored_Troll::Add_Components(void* pArg)
{
	try /* Check Add_Components */
	{
		if (FAILED(Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_Component_Model_Weopon_Armored_Troll"), L"Com_Model",
			(CComponent**)&m_pModelCom, this)))
			throw TEXT("Failed Add_Component : Com_Model");

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
			(CComponent**)&m_pRendererCom, this)))
			throw TEXT("Failed Add_Component : Com_Renderer");

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader_Mesh"),
			(CComponent**)&m_pShaderCom, this)))
			throw TEXT("Failed Add_Component : Com_Shader_Mesh");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CWeapon_Armored_Troll] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_Armored_Troll::Set_Shader_Resources()
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

CWeapon_Armored_Troll* CWeapon_Armored_Troll::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Armored_Troll* pInstance = New CWeapon_Armored_Troll(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWeapon_Armored_Troll");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CWeapon_Armored_Troll* CWeapon_Armored_Troll::Clone(void* pArg)
{
	CWeapon_Armored_Troll* pInstance = New CWeapon_Armored_Troll(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWeapon_Armored_Troll");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWeapon_Armored_Troll::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRendererCom);
	}
}
