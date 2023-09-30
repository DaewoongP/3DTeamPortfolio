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
	if (false == m_isEnable)
		return;

	if (m_fCurCoolTime >= m_fMaxCoolTime)
	{
		m_fCurCoolTime = 0.f;
		m_isEnable = false;
	}

	m_fCurCoolTime += _fTimeDelta;
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
