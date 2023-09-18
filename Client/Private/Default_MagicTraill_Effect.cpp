#include "Default_MagicTraill_Effect.h"
#include "GameInstance.h"

CDefault_MagicTraill_Effect::CDefault_MagicTraill_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDefault_MagicTraill_Effect::CDefault_MagicTraill_Effect(const CDefault_MagicTraill_Effect& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
{
	lstrcpy(m_wszName, rhs.m_wszName);
}

HRESULT CDefault_MagicTraill_Effect::Initialize_Prototype(const _tchar* wszFilePath,_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;

	_char filePath[MAX_PATH] = "";
	_char fileName[MAX_PATH] = "";
	WCharToChar(wszFilePath, filePath);
	_splitpath_s(filePath, nullptr, 0, nullptr, 0, fileName, MAX_PATH, nullptr, 0);

	CharToWChar(fileName, m_wszName);

	_tchar wszProtoTag[MAX_PATH] = TEXT("Prototype_GameObject_Default_Magic_Trail");
	wsprintf(wszProtoTag, TEXT("%s%s"), wszProtoTag, m_wszName);

	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, wszProtoTag))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, wszProtoTag
			, CTrail::Create(m_pDevice, m_pContext, wszFilePath , m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;
	return S_OK;
}

HRESULT CDefault_MagicTraill_Effect::Initialize(void* pArg)
{
	INITDESC* initDesc = static_cast<INITDESC*>(pArg);
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	m_pTransform->Set_Position(initDesc->vInitPosition);
	m_pTrailTransform = m_pTrail->Get_Transform();
	Safe_AddRef(m_pTrailTransform);
	m_pTrailTransform->Set_Position(initDesc->vInitPosition);

	//m_pTrail->Reset_Trail(*static_cast<Vector3*>(pArg), *static_cast<Vector3*>(pArg));
	
	return S_OK;
}

void CDefault_MagicTraill_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CDefault_MagicTraill_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	m_pTrailTransform->Set_Position(m_pTransform->Get_Position());
}

void CDefault_MagicTraill_Effect::Set_Position(_float3 vPos)
{
	m_pTransform->Set_Position(vPos);
}

void CDefault_MagicTraill_Effect::Action_Magic(TRAILACTIONTYPE eType, _float3 vStartPos, _float3 vEndPos, _float fTimeAcc,_float fTimeDelta)
{
	switch (eType)
	{
	case Client::CDefault_MagicTraill_Effect::TRAILACTION_STRIGHT:
		Stright_Move(vStartPos, vEndPos, fTimeAcc, fTimeDelta);
		break;
	case Client::CDefault_MagicTraill_Effect::TRAILACTION_SPIN:
		Spin_Move(vStartPos, vEndPos, fTimeAcc, fTimeDelta);
		break;
	}
}

void CDefault_MagicTraill_Effect::Stright_Move(_float3 vStartPos, _float3 vEndPos, _float fTimeAcc, _float fTimeDelta)
{
	_float3 movedPos = XMVectorLerp(vStartPos, vEndPos, fTimeAcc);
	m_pTransform->Set_Position(movedPos);
}

void CDefault_MagicTraill_Effect::Spin_Move(_float3 vStartPos, _float3 vEndPos, _float fTimeAcc, _float fTimeDelta)
{
	// 두 점 start 와 end 의 외적
	_float3 axis = XMVector3Cross(XMVector3Normalize(vStartPos), XMVector3Normalize(vEndPos));
	// 외적 axis를 position에 더함.
	m_pTransform->Set_Position(axis);
	// axis 축을 기준으로 돌림.
	m_pTransform->Turn(axis, fTimeDelta);

	//직선상으로 이동시 위치해야할 position
	_float3 movedPos = XMVectorLerp(vStartPos, vEndPos, fTimeAcc);
	//movedpos로 위치 이동
	m_pTransform->Set_Position(m_pTransform->Get_Position() + movedPos);
}

HRESULT CDefault_MagicTraill_Effect::Add_Components()
{
	_tchar protoTag[MAX_PATH] = TEXT("Prototype_GameObject_Default_Magic_Trail");
	wsprintf(protoTag, TEXT("%s%s"), protoTag, m_wszName);

	if (FAILED(CComposite::Add_Component(m_iLevel, protoTag
		, TEXT("Com_Trail"), (CComponent**)&m_pTrail)))
		return E_FAIL;

	return S_OK;
}

CDefault_MagicTraill_Effect* CDefault_MagicTraill_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* wszFilePath, _uint iLevel)
{
	CDefault_MagicTraill_Effect* pInstance = New CDefault_MagicTraill_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wszFilePath, iLevel)))
	{
		MSG_BOX("Failed to Created CDefault_MagicTraill_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDefault_MagicTraill_Effect::Clone(void* pArg)
{
	CDefault_MagicTraill_Effect* pInstance = New CDefault_MagicTraill_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDefault_MagicTraill_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefault_MagicTraill_Effect::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pTrail);
		Safe_Release(m_pTrailTransform);
	}
}
