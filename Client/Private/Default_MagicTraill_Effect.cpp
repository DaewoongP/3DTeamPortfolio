#include "Default_MagicTraill_Effect.h"
#include "GameInstance.h"
#include "Trail.h"

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

void CDefault_MagicTraill_Effect::Ready_Stright(_float3 vTargerPosition, _float3 vStartPosition, _float fTrailLifeTime, _float fDistance)
{
	m_vStartPostion = vStartPosition;
	m_vEndPostion = vTargerPosition;
	m_fTrailLifeTime = fTrailLifeTime;
	m_fLerpAcc = 0.f;
	m_fTimeScalePerDitance = fDistance / _float3(m_vEndPostion - m_vStartPostion).Length();
	m_pTrail->Reset_Trail(vStartPosition + _float3(0, 0.5f, 0), vStartPosition - _float3(0, 0.5f, 0));
}

void CDefault_MagicTraill_Effect::Ready_Spline(_float3 vTargerPosition, _float3 vStartPosition, _float fTrailLifeTime,_float fDistance, _float3 vAxis)
{
	m_vStartPostion = vStartPosition;
	m_vEndPostion = vTargerPosition;
	m_fTrailLifeTime = fTrailLifeTime;
	m_fLerpAcc = 0.f;

	// 플레이어가 타겟을 보는 vector를 구함.
	_float3 vDir = XMVector3Normalize(m_vEndPostion - m_vStartPostion);
	// 임의의 축을 구함.
	_float3 tempAxis = _float3(1, 1, 1);
	// 외적
	_float3	normal = XMVector3Cross(vDir, tempAxis);

	//진행 경로만큼 뒤로 이동한 뒤
	m_vSplineLerp[0] = m_vStartPostion - vDir;
	//임의의 랜덤 값을 구하고
	_float fRandom = Random_Generator(-20.f, 20.f);
	// 외적 방향으로 튄다.
	m_vSplineLerp[0] += _float3(normal.x * fRandom, normal.y * fabsf(fRandom), normal.z * fRandom);

	//진행 경로만큼 뒤로 이동한 뒤
	m_vSplineLerp[1] = m_vStartPostion + vDir;
	//임의의 랜덤 값을 구하고
	fRandom = Random_Generator(-20.f, 20.f);
	// 외적 방향으로 튄다.
	m_vSplineLerp[1] += _float3(normal.x * fRandom, normal.y * fabsf(fRandom), normal.z * fRandom);
	m_fTimeScalePerDitance = fDistance / _float3(m_vEndPostion - m_vStartPostion).Length();
	m_pTrail->Reset_Trail(vStartPosition + _float3(0,0.5f,0), vStartPosition - _float3(0, 0.5f, 0));
}

void CDefault_MagicTraill_Effect::Ready_Spin(_float3 vTargerPosition, _float3 vStartPosition, _float fTrailLifeTime, _float fDistance)
{
	m_vStartPostion = vStartPosition;
	m_vEndPostion = vTargerPosition;
	m_fTrailLifeTime = fTrailLifeTime;
	m_fLerpAcc = 0.f;
	m_fTimeScalePerDitance = fDistance / _float3(m_vEndPostion - m_vStartPostion).Length();
	m_pTrail->Reset_Trail(vStartPosition + _float3(0, 0.5f, 0), vStartPosition - _float3(0, 0.5f, 0));
}

HRESULT CDefault_MagicTraill_Effect::Initialize_Prototype(const _tchar* wszFilePath, _uint iLevel)
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
			, CTrail::Create(m_pDevice, m_pContext, wszFilePath, m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Ball_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Ball_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Basic_GlowBall"), m_iLevel))))
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
	
	//trail Setting
	m_pTrailTransform = m_pTrail->Get_Transform();
	Safe_AddRef(m_pTrailTransform);
	m_pTrailTransform->Set_Position(initDesc->vInitPosition);

	//glowBall Setting
	m_pGlowBallTransform = m_pGlowBall->Get_Transform();
	Safe_AddRef(m_pGlowBallTransform);
	m_pGlowBallTransform->Set_Position(initDesc->vInitPosition);

	m_pTrail->Reset_Trail(initDesc->vInitPosition, initDesc->vInitPosition);

	return S_OK;
}

void CDefault_MagicTraill_Effect::Tick(_float fTimeDelta)
{
	if (!m_isEnable)
		return;
	__super::Tick(fTimeDelta);
}

void CDefault_MagicTraill_Effect::Late_Tick(_float fTimeDelta)
{
	if (!m_isEnable)
		return;

	__super::Late_Tick(fTimeDelta);
	m_pTrailTransform->Set_Position(m_pTransform->Get_Position());
	m_pGlowBallTransform->Set_Position(m_pTransform->Get_Position());
}

void CDefault_MagicTraill_Effect::Set_Position(_float3 vPos)
{
	m_pTransform->Set_Position(vPos);
}

_bool CDefault_MagicTraill_Effect::Stright_Move(_float fTimeDelta)
{
	if (m_fLerpAcc >= 1.0f)
	{
		m_isEnd = true;
		return m_isEnd;
	}
	m_fLerpAcc += fTimeDelta / m_fTrailLifeTime * m_fTimeScalePerDitance;
	if (m_fLerpAcc > 1.0f)
		m_fLerpAcc = 1.0f;
		
	m_fLerpAcc += fTimeDelta / m_fTrailLifeTime * m_fTimeScalePerDitance;
	_float3 movedPos = XMVectorLerp(m_vStartPostion, m_vEndPostion, m_fLerpAcc);
	m_pTransform->Set_Position(movedPos);
	return m_isEnd;
}

_bool CDefault_MagicTraill_Effect::Spin_Move(_float fTimeDelta)
{
	if (m_fLerpAcc >= 1.0f)
	{
		m_isEnd = true;
		return m_isEnd;
	}
	m_fLerpAcc += fTimeDelta / m_fTrailLifeTime *m_fTimeScalePerDitance;
	if (m_fLerpAcc >= 1.0f)
		m_fLerpAcc = 1.0f;
	//직선상으로 이동시 위치해야할 position
	_float3 movedPos = XMVectorLerp(m_vStartPostion, m_vEndPostion, m_fLerpAcc);

	//pos 기준으로 transmatrix를 만든다.
	_float4x4 transMatirx = XMMatrixTranslation(movedPos.x, movedPos.y, movedPos.z);

	//방향벡터
	_float3 axis = XMVector3Normalize(m_vEndPostion - m_vStartPostion);

	//임의로 잡은 수직 벡터
	_float3 tempAxis = _float3(0, 1, 0);

	//방향에 수직인 벡터
	_float3	normal = XMVector3Normalize(XMVector3Cross(axis, tempAxis))*0.5f;

	//영점에서 normal만큼 이동한 matrix
	_float4x4 offsetMatirx = XMMatrixTranslation(normal.x, normal.y, normal.z);

	//회전 행렬 생성
	_float4x4 rotationMatrix = XMMatrixRotationAxis(axis, m_fLerpAcc * 20);

	//offset matrix를 transMatrix로 돌림
	_float4x4 CombineMatrix = offsetMatirx * rotationMatrix * transMatirx;
	// 위치 세팅
	m_pTransform->Set_Position(_float3(CombineMatrix.m[3][0], CombineMatrix.m[3][1], CombineMatrix.m[3][2]));
	return m_isEnd;
}

_bool CDefault_MagicTraill_Effect::Spline_Move(_float fTimeDelta)
{
	if (m_fLerpAcc >= 1.0f)
	{
		m_isEnd = true;
		return m_isEnd;
	}
	m_fLerpAcc += fTimeDelta / m_fTrailLifeTime * m_fTimeScalePerDitance;
	if (m_fLerpAcc > 1.0f)
		m_fLerpAcc = 1.0f;

	_float3 movedPos = XMVectorCatmullRom(m_vSplineLerp[0], m_vStartPostion, m_vEndPostion, m_vSplineLerp[1], m_fLerpAcc);
	m_pTransform->Set_Position(movedPos);
	return m_isEnd;
}

void CDefault_MagicTraill_Effect::Set_Trail_HeadColor(_float3 vColor)
{
	m_pTrail->Set_Trail_HeadColor(vColor);
}

void CDefault_MagicTraill_Effect::Set_Trail_TailColor(_float3 vColor)
{
	m_pTrail->Set_Trail_TailColor(vColor);
}

void CDefault_MagicTraill_Effect::Set_Glow_BallColor(_float3 vColor)
{
	MAIN_MODULE& MainModule = m_pGlowBall->Get_MainModuleRef();
	MainModule.vStartColor = _float4(vColor.x, vColor.y, vColor.z,1.f);
}

void CDefault_MagicTraill_Effect::GlowBall_Dead()
{
	m_pGlowBall->Disable();
}

HRESULT CDefault_MagicTraill_Effect::Add_Components()
{
	_tchar protoTag[MAX_PATH] = TEXT("Prototype_GameObject_Default_Magic_Trail");
	wsprintf(protoTag, TEXT("%s%s"), protoTag, m_wszName);

	if (FAILED(CComposite::Add_Component(m_iLevel, protoTag
		, TEXT("Com_Trail"), (CComponent**)&m_pTrail)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Default_Magic_Ball_Particle")
		, TEXT("Com_BloomBall"), (CComponent**)&m_pGlowBall)))
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

		Safe_Release(m_pGlowBall);
		Safe_Release(m_pGlowBallTransform);
	}
}