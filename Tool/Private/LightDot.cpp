#include "LightDot.h"
#include"GameInstance.h"

CLightDot::CLightDot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :CGameObject(pDevice, pContext)
{
}

_float4 CLightDot::Get_Position()
{
	return m_pTransform->Get_Position().TransNorm();
}

void CLightDot::Set_Position(_float4 _vPosition)
{
	m_pTransform->Set_Position(_vPosition.xyz());

}

HRESULT CLightDot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLightDot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	
	ZEROMEM(&LightInfo);
	if (nullptr != pArg)
	{	
		LIGHTPOS* Info = (LIGHTPOS*)pArg;
		m_pTransform->Set_Position(_float3(Info->vPosition.x,Info->vPosition.y,Info->vPosition.z));
		LightInfo.vPosition = Info->vPosition;
		LightInfo.fRange = 1.f;
	}
	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CLightDot::Tick(_float fTimeDelta)
{	
	m_pCollider->Tick(m_pTransform->Get_WorldMatrix());

#ifdef _DEBUG
	m_pRenderer->Add_DebugGroup(m_pCollider);
#endif
}

HRESULT CLightDot::Render()
{
	return S_OK;
}

_bool CLightDot::RayIntersects(_float4 vOrigin, _float4 vDirection, _float& fDist)
{
	return m_pCollider->RayIntersects(vOrigin, vDirection, fDist);
}

#ifdef _DEBUG
void CLightDot::Set_Collider_Color(_float4 _vColor)
{
	NULL_CHECK(m_pCollider);

	m_pCollider->Set_Color(_vColor);
}
#endif

HRESULT CLightDot::Add_Components()
{
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CCamera_Point Add_Component : (Com_Renderer)");
		return E_FAIL;
	}


	BoundingSphereDesc.fRadius = LightInfo.fRange;
	BoundingSphereDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Sphere_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingSphereDesc)))
	{
		MSG_BOX("Failed CCamera_Point Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	return S_OK;
}

CLightDot* CLightDot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightDot* pInstance = New CLightDot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLightDot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CLightDot::Clone(void* pArg)
{
	CLightDot* pInstance = New CLightDot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLightDot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightDot::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
}
