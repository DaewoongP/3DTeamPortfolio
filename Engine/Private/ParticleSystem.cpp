#include "ParticleSystem.h"
#include "MainModule.h"

CParticleSystem::CParticleSystem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{

}

CParticleSystem::CParticleSystem(const CParticleSystem& _rhs)
	: CComponent(_rhs)
{

}

HRESULT CParticleSystem::Initialize_Prototype(const _tchar* _pPariticleSystemFilePath)
{	
	// ���θ�� ����
	m_Modules.resize(MODULE_END);

	// �ʼ� ��� 3������ �����ϰ� ����.
	m_Modules[MAIN] = ModuleFactory(MAIN);
	m_Modules[EMMISION] = ModuleFactory(EMMISION);
	m_Modules[SHAPE] = ModuleFactory(SHAPE);

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

void CParticleSystem::Save(const _tchar* _pDirectoryPath)
{
	for (_uint i = 0; i < MODULE_END; ++i)
	{
		if (nullptr != m_Modules[i])
			m_Modules[i]->SaveModule(_pDirectoryPath);
	}
}

void CParticleSystem::Load(const _tchar* _pDirectoryPath)
{
	HRESULT hr;
	for (_uint i = 0; i < MODULE_END; ++i)
	{
		// ������ �����ϴ� ����� ����.
		Safe_Release(m_Modules[i]);

		// i���� ���� �´� ��� ����
		m_Modules[i] = ModuleFactory(static_cast<MODULE>(i));

		// NULL�� ��� ��ŵ
		if (nullptr == m_Modules[i])
			continue;

		// �ε� �Լ� ȣ��
		hr = m_Modules[i]->LoadModule(_pDirectoryPath);

		// ��� �ε� ���� �� ����(���� �Ϻη� ��� �ȸ��� �����)
		if (FAILED(hr))
		{
			Safe_Release(m_Modules[i]);
		}
	}
}

CParticleModule* CParticleSystem::ModuleFactory(MODULE _eModule)
{
	CParticleModule* pModule = { nullptr };
	switch (_eModule)
	{
	case MAIN:
		pModule = CMainModule::Create(this);
		break;
	case EMMISION:
		//pModule = CEmmisionModule::Create();
		break;
	case SHAPE:
		break;
	case VELOCITY_OVER_LIFETIME:
		break;
	case LIMIT_VELOCITY_OVER_LIFETIME:
		break;
	case INHERIT_VELOCITY:
		break;
	case FORCE_OVER_LIFETIME:
		break;
	case COLOR_OVER_LIFETIME:
		break;
	case COLOR_BY_SPEED:
		break;
	case SIZE_OVER_LIFETIME:
		break;
	case SIZE_BY_SPEED:
		break;
	case ROTATION_OVER_ROTATION:
		break;
	case ROTATION_BY_SPEED:
		break;
	case EXTERNAL_FORCES:
		break;
	case NOISE:
		break;
	case COLLISION:
		break;
	case TRIGGERS:
		break;
	case SUB_EMITTERS:
		break;
	case TEXTURE_SHEET_ANIMATION:
		break;
	case LIGHTS:
		break;
	case TRAILS:
		break;
	case CUSTOM_DATA:
		break;
	case RENDERER:
		break;
	case MODULE_END:
		break;
	default:
		pModule = nullptr;
		break;
	}

	return pModule;
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
	
	for (auto Module : m_Modules)
		Safe_Release(Module);
}
