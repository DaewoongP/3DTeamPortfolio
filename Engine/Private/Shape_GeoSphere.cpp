#include "Shape_GeoSphere.h"
#include "GameInstance.h"
#include "VIBuffer_GeoSphere.h"

CShape_GeoSphere::CShape_GeoSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CShape_GeoSphere::CShape_GeoSphere(const CShape_GeoSphere& rhs)
	: CGameObject(rhs)
{
}

HRESULT CShape_GeoSphere::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShape_GeoSphere::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	return S_OK;
}

HRESULT CShape_GeoSphere::Render()
{
#ifdef _DEBUG
	// Tick_ImGui();
#endif // _DEBUG

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin("Default");

	m_pBuffer->Render();

	return S_OK;
}

HRESULT CShape_GeoSphere::Add_Components()
{
	/* For.Com_Buffer */
	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_VIBuffer_GeoSphere"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer))))
	{
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Shader_Geometry"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShape_GeoSphere::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
	{
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CShape_GeoSphere* CShape_GeoSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShape_GeoSphere* pInstance = New CShape_GeoSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CGeometry_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShape_GeoSphere::Clone(void* pArg)
{
	CShape_GeoSphere* pInstance = New CShape_GeoSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CGeometry_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShape_GeoSphere::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}
