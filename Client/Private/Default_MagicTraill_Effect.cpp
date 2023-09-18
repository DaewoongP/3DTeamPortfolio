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

void CDefault_MagicTraill_Effect::Stright_Move(_float3 vStartPos, _float3 vEndPos, _float fTimeAcc)
{
	_float3 movedPos = XMVectorLerp(vStartPos, vEndPos, fTimeAcc);
	m_pTransform->Set_Position(movedPos);
}

void CDefault_MagicTraill_Effect::Spin_Move(_float3 vStartPos, _float3 vEndPos, _float fTimeAcc)
{
	//직선상으로 이동시 위치해야할 position
	_float3 movedPos = XMVectorLerp(vStartPos, vEndPos, fTimeAcc);
	
	//pos 기준으로 transmatrix를 만든다.
	_float4x4 transMatirx = XMMatrixTranslation(movedPos.x, movedPos.y, movedPos.z);

	//방향벡터
	_float3 axis = XMVector3Normalize(vEndPos - vStartPos);

	//임의로 잡은 수직 벡터
	_float3 tempAxis = _float3(0, 1, 0);

	//방향에 수직인 벡터
	_float3	normal = XMVector3Cross(axis, tempAxis);

	//영점에서 normal만큼 이동한 matrix
	_float4x4 offsetMatirx = XMMatrixTranslation(normal.x, normal.y, normal.z);

	//회전 행렬 생성
	_float4x4 rotationMatrix = XMMatrixRotationAxis(axis, fTimeAcc * 100);

	//offset matrix를 transMatrix로 돌림
	_float4x4 CombineMatrix = offsetMatirx * rotationMatrix *transMatirx;
	// 위치 세팅
	m_pTransform->Set_Position(_float3(CombineMatrix.m[3][0], CombineMatrix.m[3][1], CombineMatrix.m[3][2]));
}

void CDefault_MagicTraill_Effect::Spline_Move(_float3 vWeight0, _float3 vStartPos, _float3 vEndPos, _float3 vWeight1, _float fTimeAcc)
{
	_float3 movedPos = XMVectorCatmullRom(vWeight0, vStartPos, vEndPos, vWeight1 ,fTimeAcc);
	m_pTransform->Set_Position(movedPos);
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
