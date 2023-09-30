#include "CoolTime.h"

CCoolTime::CCoolTime(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCoolTime::CCoolTime(const CCoolTime& rhs)
	: CComponent(rhs)
{
}

HRESULT CCoolTime::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		COOLTIME_DESC Desc = *static_cast<COOLTIME_DESC*>(pArg);
		m_fMaxCoolTime = Desc.fMaxCoolTime;
	}
	return S_OK;
}

void CCoolTime::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	if (false == m_isEnable)
		return;

	if (m_fCurCoolTime >= 15.f)
	{
		m_fCurCoolTime = 0.f;
		m_isEnable = false;
	}

	if (15.f <= m_fDrugTime)
	{
		m_isAccelerlator = false;
		m_fDrugTime = 0.f;
	}

	if (true == m_isAccelerlator)
	{
		m_fCurCoolTime += _fTimeDelta*1.2f;
		m_fDrugTime += _fTimeDelta;
	}
	else 
		m_fCurCoolTime += _fTimeDelta;
	cout << m_fCurCoolTime << endl;


}

void CCoolTime::Play_CoolTime()
{
	if (true == m_isEnable)
		return;

	m_isEnable = true;
}

CCoolTime* CCoolTime::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCoolTime* pInstance = New CCoolTime(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCoolTime");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCoolTime* CCoolTime::Clone(void* pArg)
{
	CCoolTime* pInstance = New CCoolTime(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCoolTime");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCoolTime::Free()
{
	__super::Free();
}
