#include "ParticleSystem.h"


CParticleSystem::CParticleSystem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{

}

CParticleSystem::CParticleSystem(const CParticleSystem& _rhs)
	: CComponent(_rhs)
{

}

MAIN_MODULE CParticleSystem::Get_MainModule_Desc() const
{
	if (nullptr == m_pMainModule)
		return MAIN_MODULE();

	return m_pMainModule->m_MainModuleDesc;
}

HRESULT CParticleSystem::Initialize_Prototype(const _tchar* _pPariticleSystemFilePath)
{	
	// 메인모듈 생성
	m_pMainModule->Create(_pPariticleSystemFilePath);

	return S_OK;
}

HRESULT CParticleSystem::Initialize(void* _pArg)
{
	return S_OK;
}

HRESULT CParticleSystem::Render()
{
	return S_OK;
}

CParticleSystem* CParticleSystem::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pPariticleSystemFilePath)
{
	CParticleSystem* pInstance = new CParticleSystem(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pPariticleSystemFilePath)))
	{
		MSG_BOX("Failed to Created CParticleSystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CParticleSystem::Clone(void* _pArg)
{
	CParticleSystem* pInstance = new CParticleSystem(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned CParticleSystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticleSystem::Free()
{
	__super::Free();
	Safe_Release(m_pMainModule);
}
