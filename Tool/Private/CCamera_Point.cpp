#include "CCamera_Point.h"
#include "GameInstance.h"

CCamera_Point::CCamera_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice,pContext)
{
}

_float4 CCamera_Point::Get_Position()
{
	return m_pTransform->Get_Position().TransCoord();
}

void CCamera_Point::Set_Position(_float4 _vPosition)
{
	m_pTransform->Set_Position(_vPosition.xyz());
}

HRESULT CCamera_Point::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Point::Initialize(void* pArg)
{
	CAMERAPOINTDESC CameraPointDesc = *(CAMERAPOINTDESC*)pArg;

	m_pTransform->Set_Position(CameraPointDesc.vPosition.xyz());

	return S_OK;
}

void CCamera_Point::Tick(_float _TimeDelta)
{
#ifdef _DEBUG
	//렌더러에 던지기만 할듯
	m_pRenderer->Add_DebugGroup(m_pCollider);
#endif // _DEBUG
}

HRESULT CCamera_Point::Render()
{
	return S_OK;
}


HRESULT CCamera_Point::Add_Components()
{
	///* Com_Renderer */
	//if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
	//	TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	//{
	//	MSG_BOX("Failed CTest_Player Add_Component : (Com_Renderer)");
	//	return E_FAIL;
	//}

	///* For.Com_Model */
	//if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Fiona"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//{
	//	MSG_BOX("Failed CTest_Player Add_Component : (Com_Model)");
	//	return E_FAIL;
	//}

	///* For.Com_Shader */
	//if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//{
	//	MSG_BOX("Failed CTest_Player Add_Component : (Com_Shader)");
	//	return E_FAIL;
	//}

	return S_OK;
}

CCamera_Point* CCamera_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CCamera_Point* pInstance = new CCamera_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CCamera_Point");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Point::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

CGameObject* CCamera_Point::Clone(void* pArg)
{
	return nullptr;
}
