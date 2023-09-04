#include "Weapon_Forest_Troll.h"

#include "GameInstance.h"

CWeapon_Forest_Troll::CWeapon_Forest_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CParts(pDevice, pContext)
{
}

CWeapon_Forest_Troll::CWeapon_Forest_Troll(const CWeapon_Forest_Troll& rhs)
	: CParts(rhs)
{
}

HRESULT CWeapon_Forest_Troll::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CWeapon_Forest_Troll::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

void CWeapon_Forest_Troll::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Forest_Troll::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CWeapon_Forest_Troll::Render()
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
		{
			MSG_BOX("[CWeapon_Forest_Troll] Failed Render : (Bind_Material)");
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin("Mesh")))
		{
			MSG_BOX("[CWeapon_Forest_Troll] Failed Render : (Begin)");
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			MSG_BOX("[CWeapon_Forest_Troll] Failed Render : (Render)");
			return E_FAIL;
		}
	}

	return __super::Render();
}

HRESULT CWeapon_Forest_Troll::Add_Components(void* pArg)
{
	if (FAILED(Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Forest_Troll"), L"Com_Model",
		(CComponent**)&m_pModelCom, this)))
	{
		MSG_BOX("[CWeapon_Forest_Troll] Failed Add_Component (Com_Model)");
		return E_FAIL;
	}

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom, this)))
	{
		MSG_BOX("[CWeapon_Forest_Troll] Failed Add_Component (Com_Renderer)");
		return E_FAIL;
	}

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader_Mesh"),
		(CComponent**)&m_pShaderCom, this)))
	{
		MSG_BOX("[CWeapon_Forest_Troll] Failed Add_Component (Com_Shader_Mesh)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_Forest_Troll::Set_Shader_Resources()
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

CWeapon_Forest_Troll* CWeapon_Forest_Troll::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Forest_Troll* pInstance = new CWeapon_Forest_Troll(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWeapon_Forest_Troll");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CWeapon_Forest_Troll* CWeapon_Forest_Troll::Clone(void* pArg)
{
	CWeapon_Forest_Troll* pInstance = new CWeapon_Forest_Troll(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWeapon_Forest_Troll");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWeapon_Forest_Troll::Free()
{
	__super::Free();
	
	if (true == m_isCloned)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRendererCom);
	}
}
