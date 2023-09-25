#include "Wingardium_Effect.h"
#include "GameInstance.h"
#include "Ease.h"

CWingardium_Effect::CWingardium_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CWingardium_Effect::CWingardium_Effect(const CWingardium_Effect& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CWingardium_Effect::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;

	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Wingardium_Effect_Trail")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Wingardium_Effect_Trail")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	ENDINSTANCE;
	return S_OK;
}

HRESULT CWingardium_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));
	m_pTransform->Set_Speed(20.f);

	for (int i = 0; i < TrailCount; i++)
	{
		_float3 vHigh, vLow;
		m_pTrail[i]->Set_LocalSpace(m_pTransform->Get_WorldMatrixPtr());
		m_pTrailTransform[i] = m_pTrail[i]->Get_Transform();
		Safe_AddRef(m_pTrailTransform[i]);
		m_TrailDesc[i].isFlip = ((i % 2 == 0) ? (true) : (false));
		m_TrailDesc[i].fSettingLifeTime = Random_Generator(6.f, 10.f);
		m_TrailDesc[i].fCurrnetLifeTime = m_TrailDesc[i].fSettingLifeTime;
		m_TrailDesc[i].fToY = Random_Generator(1.f, 2.f);
		m_TrailDesc[i].fToRadius = Random_Generator(0.1f, 0.5f);
		m_TrailDesc[i].fSpeed = Random_Generator(XMConvertToRadians(90), XMConvertToRadians(720));
		m_TrailDesc[i].isEnable = true;
		_float fScale = Random_Generator(0.1f, 0.5f);
		m_pTrailTransform[i]->Set_Scale(_float3(fScale, fScale, fScale));
		m_pTrail[i]->Reset_Trail(_float3(0, 0.5f, 0), _float3(0, 0.5f, 0));
	}

	for (int i = 0; i < TrailCount / 2; i++)
	{
		m_pTrailToOrigin[i]->Set_LocalSpace(m_pTransform->Get_WorldMatrixPtr());
		m_TrailToOriginDesc[i].isEnable = true;
		m_pTrailToOriginTransform[i] = m_pTrailToOrigin[i]->Get_Transform();
		Safe_AddRef(m_pTrailToOriginTransform[i]);
		m_TrailToOriginDesc[i].isFlip = ((i % 2 == 0) ? (true) : (false));
		m_TrailToOriginDesc[i].fSettingLifeTime = Random_Generator(6.f, 10.f);
		m_TrailToOriginDesc[i].fCurrnetLifeTime = m_TrailToOriginDesc[i].fSettingLifeTime;
		m_TrailToOriginDesc[i].fToRadius = Random_Generator(0.1f, 0.2f);
		m_TrailToOriginDesc[i].fSpeed = Random_Generator(XMConvertToRadians(40), XMConvertToRadians(180));
		_float fScale = Random_Generator(0.01f, 0.1f);
		m_pTrailToOriginTransform[i]->Set_Scale(_float3(fScale, fScale, fScale));
		m_pTrailToOrigin[i]->Reset_Trail(_float3(0, 0.5f, 0), _float3(0, 0.5f, 0));
	}

	return S_OK;
}

void CWingardium_Effect::Tick(_float fTimeDelta)
{
	if (!m_isEnable)
		return;
	__super::Tick(fTimeDelta);
}

void CWingardium_Effect::Late_Tick(_float fTimeDelta)
{
	if (!m_isEnable)
		return;
	__super::Late_Tick(fTimeDelta);
}

void CWingardium_Effect::Set_Position(_float3 vPos)
{
	m_pTransform->Set_Position(vPos);
}

//트레일의 실제 움직임 설정부.
void CWingardium_Effect::TrailAction(_float3 pos, _float fTimeDelta)
{
	// 딜레이 시간이 지났으면 트레일 동작시켜줘.
	if (m_TrailCurrentDelay > 0)
	{
		m_TrailCurrentDelay -= fTimeDelta;
	}
	else
	{
		_float3 vHigh, vLow;
		vHigh = _float3(0, 0.5f, 0);
		vLow = _float3(0, -0.5f, 0);
		m_pTrailTransform[m_iCurrentActionParticle]->Set_Position(_float3(0, 0, 0));
		m_pTrail[m_iCurrentActionParticle]->Reset_Trail(vHigh, vLow);

		m_pTrail[m_iCurrentActionParticle]->Reset_Trail(m_pTransform->Get_Position(), m_pTransform->Get_Position());
		m_TrailDesc[m_iCurrentActionParticle].fAnimStart_Y = 0;
		m_TrailDesc[m_iCurrentActionParticle++].isEnable = true;
		m_TrailDesc[m_iCurrentActionParticle].isEnable = true;
		(m_iCurrentActionParticle < TrailCount - 1) ? (m_iCurrentActionParticle++) : (m_iCurrentActionParticle = 0);
		m_TrailCurrentDelay = m_TrailPerfDelay;
	}

	for (_uint i = 0; i < TrailCount; i++)
	{
		//동작이 가능한 파티클이라면?
		if (m_TrailDesc[i].isEnable)
		{
			m_TrailDesc[i].fCurrnetLifeTime -= fTimeDelta;
			if (m_TrailDesc[i].fCurrnetLifeTime < 0)
			{
				//오브제의 영점
				_float3 vHigh, vLow;
				vHigh = _float3(0, 0.5f, 0);
				vLow = _float3(0, -0.5f, 0);

				m_TrailDesc[i].fSettingLifeTime = Random_Generator(6.f, 10.f);
				m_TrailDesc[i].fCurrnetLifeTime = m_TrailDesc[i].fSettingLifeTime;
				m_TrailDesc[i].fToY = Random_Generator(1.f, 2.f);
				m_TrailDesc[i].fToRadius = Random_Generator(0.1f, 0.5f);
				m_TrailDesc[i].fSpeed = Random_Generator(XMConvertToRadians(90), XMConvertToRadians(720));
				_float fScale = Random_Generator(0.1f, 0.5f);
				m_pTrailTransform[i]->Set_Scale(_float3(fScale, fScale, fScale));

				m_pTrailTransform[i]->Set_Position(_float3(0, 0, 0));
				m_pTrail[i]->Reset_Trail(vHigh, vLow);
			}

			//y를 올려줌.
			_float fElapsedTime = m_TrailDesc[i].fSettingLifeTime - m_TrailDesc[i].fCurrnetLifeTime;
			// 트레일 자체의 위치를 세팅할거임
			_float3	vTrailPos = {};
			_float fEase_Y = CEase::InOutBack(fElapsedTime, m_TrailDesc[i].fAnimStart_Y, m_TrailDesc[i].fToY, m_TrailDesc[i].fSettingLifeTime);
			// 트레일의 위치는 y
			vTrailPos.y = fEase_Y;

			// 설정된 y를 이용해 반지름 설정
			m_TrailDesc[i].fRadius = CEase::InOutBack(fEase_Y - m_TrailDesc[i].fAnimStart_Y, 0, m_TrailDesc[i].fToRadius, m_TrailDesc[i].fToY - m_TrailDesc[i].fAnimStart_Y);
			vTrailPos.x = m_TrailDesc[i].fRadius;

			// 포지션에 값 대입
			m_pTrailTransform[i]->Set_Position(vTrailPos);

			//공전
			_float4x4 rotationMatrix = XMMatrixRotationY(m_TrailDesc[i].fSpeed * fElapsedTime);
			m_pTrailTransform[i]->Set_WorldMatrix(m_pTrailTransform[i]->Get_WorldMatrix() * rotationMatrix);
		}
	}

	for (_uint i = 0; i < TrailCount / 2; i++)
	{
		//동작이 가능한 파티클이라면?
		if (m_TrailToOriginDesc[i].isEnable)
		{
			m_TrailToOriginDesc[i].fCurrnetLifeTime -= fTimeDelta;
			if (m_TrailToOriginDesc[i].fCurrnetLifeTime < 0)
			{
				//오브제의 영점
				_float3 vHigh, vLow;
				vHigh = _float3(0, 0.5f, 0);
				vLow = _float3(0, -0.5f, 0);
				m_TrailDesc[i].fSettingLifeTime = Random_Generator(6.f, 10.f);
				m_TrailToOriginDesc[i].fCurrnetLifeTime = m_TrailToOriginDesc[i].fSettingLifeTime;
				m_TrailToOriginDesc[i].fToRadius = Random_Generator(0.1f, 0.2f);
				m_TrailToOriginDesc[i].fSpeed = Random_Generator(XMConvertToRadians(40), XMConvertToRadians(180));
				_float fScale = Random_Generator(0.01f, 0.2f);
				m_pTrailToOriginTransform[i]->Set_Scale(_float3(fScale, fScale, fScale));
				m_pTrailToOriginTransform[i]->Set_Position(_float3(0, 0, 0));
				m_pTrailToOrigin[i]->Reset_Trail(vHigh, vLow);

			}

			BEGININSTANCE;
			if (pGameInstance->RayCast(m_pTransform->Get_Position(), m_pTransform->Get_Up() * -1, 100, &m_vBottom))
			{
			}
			ENDINSTANCE;

			//y를 올려줌.
			_float fElapsedTime = m_TrailToOriginDesc[i].fSettingLifeTime - m_TrailToOriginDesc[i].fCurrnetLifeTime;
			_float3	vTrailPos = {};
			_float fEase_Y = CEase::InOutBack(fElapsedTime, m_TrailToOriginDesc[i].fAnimStart_Y, m_TrailToOriginDesc[i].fToY, m_TrailToOriginDesc[i].fSettingLifeTime);
			// 트레일의 위치는 y
			vTrailPos.y = fEase_Y;

			//올라간 Y 위치를 이용해서 반지름 설정
			m_TrailToOriginDesc[i].fRadius = CEase::InOutBack(fEase_Y - m_TrailToOriginDesc[i].fAnimStart_Y, 0, m_TrailToOriginDesc[i].fToRadius, m_TrailToOriginDesc[i].fToY - m_TrailToOriginDesc[i].fAnimStart_Y);

			// 반지름만큼 trail이동.
			_float4x4 parentMatrix = m_pTransform->Get_WorldMatrix();
			vTrailPos.x = m_TrailToOriginDesc[i].fRadius;
			// 포지션에 값 대입
			m_pTrailToOriginTransform[i]->Set_Position(vTrailPos);

			//공전
			_float4x4 rotationMatrix = XMMatrixRotationY(m_TrailToOriginDesc[i].fSpeed * fElapsedTime);
			m_pTrailToOriginTransform[i]->Set_WorldMatrix(m_pTrailToOriginTransform[i]->Get_WorldMatrix() * rotationMatrix);
		}
	}

	m_pTransform->Set_Position(pos);
}

HRESULT CWingardium_Effect::Add_Components()
{
	_tchar comTag_trail[TrailCount][MAX_PATH] = { TEXT("Com_Trail1"), TEXT("Com_Trail2"), TEXT("Com_Trail3"),TEXT("Com_Trail4"),TEXT("Com_Trail5")
	,TEXT("Com_Trail6") ,TEXT("Com_Trail7") ,TEXT("Com_Trail8") };

	for (int i = 0; i < TrailCount; i++)
	{
		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Wingardium_Effect_Trail")
			, comTag_trail[i], (CComponent**)&m_pTrail[i])))
		{
			__debugbreak();
			return E_FAIL;
		}
	}

	_tchar comTag_trailToOrigin[TrailCount / 2][MAX_PATH] = { TEXT("Com_Trail9"), TEXT("Com_Trail10"), TEXT("Com_Trail11"),TEXT("Com_Trail12") };
	for (int i = 0; i < TrailCount / 2; i++)
	{
		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Wingardium_Effect_Trail")
			, comTag_trailToOrigin[i], (CComponent**)&m_pTrailToOrigin[i])))
		{
			__debugbreak();
			return E_FAIL;
		}
	}
	return S_OK;
}

CWingardium_Effect* CWingardium_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CWingardium_Effect* pInstance = New CWingardium_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CWingardium_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWingardium_Effect::Clone(void* pArg)
{
	CWingardium_Effect* pInstance = New CWingardium_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWingardium_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWingardium_Effect::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		for (int i = 0; i < TrailCount; i++)
		{
			Safe_Release(m_pTrailTransform[i]);
			Safe_Release(m_pTrail[i]);
		}

		for (int i = 0; i < TrailCount / 2; i++)
		{
			Safe_Release(m_pTrailToOriginTransform[i]);
			Safe_Release(m_pTrailToOrigin[i]);
		}
	}
}
