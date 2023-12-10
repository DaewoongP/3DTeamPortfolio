#include "Quest_Bone.h"
#include "GameInstance.h"

CQuest_Bone::CQuest_Bone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CQuest(pDevice, pContext)
{
}

_bool CQuest_Bone::Is_Finished()
{
	if (false == m_isClear)
		return false;

	return true;
}

HRESULT CQuest_Bone::Initialize()
{
	return S_OK;
}

void CQuest_Bone::Tick(_float fTimeDelta)
{
}

void CQuest_Bone::Late_Tick(_float fTimeDelta)
{
}

HRESULT CQuest_Bone::Render()
{
	return S_OK;
}

void CQuest_Bone::Clear_Quest()
{
	m_isClear = true;
}

CQuest_Bone* CQuest_Bone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Bone* pInstance = New CQuest_Bone(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CQuest_Bone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuest_Bone::Free()
{
	__super::Free();
}
