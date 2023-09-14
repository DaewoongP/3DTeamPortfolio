#include "UI_Progress1.h"
#include "GameInstance.h"

CUI_Progress::CUI_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CUI_Progress::CUI_Progress(const CUI_Progress& rhs)
	: CComponent(rhs)
{
}

HRESULT CUI_Progress::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Progress::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float3* vGauge = (_float3*)pArg;

		Initailize_Gauge(vGauge->x, vGauge->y, vGauge->z);
	}

	return S_OK;
}

void CUI_Progress::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Progress::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}


_float CUI_Progress::Get_Gauge(GAUGE eType)
{
	switch (eType)
	{
	case Engine::CUI_Progress::MIN:
		return m_fMinGauge;
	case Engine::CUI_Progress::MAX:
		return m_fMaxGauge;
	case Engine::CUI_Progress::CURRENT:
		return m_fCurrentGauge;
	case Engine::CUI_Progress::GAUGE_END:
		MSG_BOX("Failed Set Gauge");
	default:
		MSG_BOX("Failed Set Gauge");
	}
	return 0.f;
}

void CUI_Progress::Set_Gauge(_float fGauge, GAUGE eType)
{
	switch (eType)
	{
	case Engine::CUI_Progress::MIN:
		m_fMinGauge = fGauge;
		break;
	case Engine::CUI_Progress::MAX:
		m_fMaxGauge = fGauge;
		break;
	case Engine::CUI_Progress::CURRENT:
		m_fCurrentGauge = fGauge;
		break;
	case Engine::CUI_Progress::GAUGE_END:
		MSG_BOX("Failed Set Gauge");
		break;
	default:
		MSG_BOX("Failed Set Gauge");
		break;
	}

}

void CUI_Progress::Initailize_Gauge(_float fMin, _float fMax, _float fCurrent)
{
	m_fMinGauge = fMin;
	m_fMaxGauge = fMax;
	m_fCurrentGauge = fCurrent;
}

CUI_Progress* CUI_Progress::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Progress* pInstance = new CUI_Progress(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Progress");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CUI_Progress::Clone(void* pArg)
{
	CUI_Progress* pInstance = new CUI_Progress(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Progress");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Progress::Free()
{
	__super::Free();
}
