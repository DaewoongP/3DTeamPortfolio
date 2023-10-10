#include "Animation_Camera_Model.h"
#include "GameInstance.h"
#include "Client_Defines.h"

CAnimation_Camera_Model::CAnimation_Camera_Model(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice,_pContext)
{

}

CAnimation_Camera_Model::CAnimation_Camera_Model(const CAnimation_Camera_Model& rhs)
	: CGameObject(rhs)

{

}

HRESULT CAnimation_Camera_Model::Initialize_Protofftype()
{
	return S_OK;
}

HRESULT CAnimation_Camera_Model::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	//컴포넌트 추가
	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Add_Components : CAnimation_Camera_Model");

		return E_FAIL;
	}

	if (FAILED(Ready_BonData()))
	{
		MSG_BOX("Failed Ready_BonData : CAnimation_Camera_Model");

		return E_FAIL;
	}


	ANIMATION_CAMERA_MODEL_DESC* pAnimation_Camera_Model_Desc = static_cast<ANIMATION_CAMERA_MODEL_DESC*>(pArg);

	//부모가 할당한 것을 지우고
	Safe_Release(m_pTransform);



	//플레이어 것으로 장착
	m_pTransform = pAnimation_Camera_Model_Desc->pTargetTransform;

	Safe_AddRef(m_pTransform);

	m_pAnimCameraModel->Change_Animation(TEXT("Cam_Start_Animation"));

	return S_OK;
}

HRESULT CAnimation_Camera_Model::Initialize_Level(_uint _iLevelIndex)
{
	return S_OK;
}

void CAnimation_Camera_Model::Tick(_float fTimeDelta)
{
	//애니메이션 재생
	m_pAnimCameraModel->Play_Animation(fTimeDelta);

	//eye,up,at갱신
	Update_Up_Eye_At();
}

void CAnimation_Camera_Model::Update_Up_Eye_At()
{
	_float4x4 skt_camWorldMatrix = 	
		m_pEyePoint->Get_OffsetMatrix() * 
		m_pEyePoint->Get_CombinedTransformationMatrix() *
		m_pAnimCameraModel->Get_PivotFloat4x4() * 
		m_pTransform->Get_WorldMatrix();


	_float4x4 lookatWorldMatrix =
		m_pUpPoint->Get_OffsetMatrix() *
		m_pUpPoint->Get_CombinedTransformationMatrix() *
		m_pAnimCameraModel->Get_PivotFloat4x4() *
		m_pTransform->Get_WorldMatrix();

	//Eye
	m_vEye = skt_camWorldMatrix.Translation();

	//up
	m_vUp = lookatWorldMatrix.Translation();
	m_vUp -= m_vEye;
	//m_vUp = _float3(0.0f,1.0f,0.0f);

	_float3 vBoneLook = skt_camWorldMatrix.Right();

	vBoneLook.Normalize();

	//vBoneLook *= -1;

	//At
	m_vAt = m_vEye + vBoneLook.Cross(m_vUp);

	m_vEye.y -= 0.5f;
	m_vAt.y -= 0.5f;
}

HRESULT CAnimation_Camera_Model::Add_Components()
{
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Model_Animation_Camera"),TEXT("Com_Aminmation_Camera_Model"),
		reinterpret_cast<CComponent**>(&m_pAnimCameraModel))))
	{
		MSG_BOX("Failed Add Com_Aminmation_Camera_Model");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAnimation_Camera_Model::Ready_BonData()
{
	m_pEyePoint = m_pAnimCameraModel->Get_Bone(TEXT("skt_cam"));

	if (nullptr == m_pEyePoint)
	{
		MSG_BOX("Failed get Bone skt_cam");

		return E_FAIL;
	}

	m_pUpPoint = m_pAnimCameraModel->Get_Bone(TEXT("lookat"));

	if (nullptr == m_pUpPoint)
	{
		MSG_BOX("Failed get Bone lookat");

		return E_FAIL;
	}

	return S_OK;
}
CAnimation_Camera_Model* CAnimation_Camera_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnimation_Camera_Model* pInstance = New CAnimation_Camera_Model(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimation_Camera_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAnimation_Camera_Model::Clone(void* pArg)
{
	CAnimation_Camera_Model* pInstance = New CAnimation_Camera_Model(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimation_Camera_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation_Camera_Model::Free()
{
	CGameObject::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pAnimCameraModel);
		Safe_Release(m_Animation_Camera_Model_Desc.pTargetTransform);
	}
}
