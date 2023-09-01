#include "ParticleSystem.h"
#include "GameInstance.h"
#include "GameObject.h"

CParticleSystem::CParticleSystem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{

}

CParticleSystem::CParticleSystem(const CParticleSystem& _rhs)
	: CComponent(_rhs)
	, m_MainModuleDesc(_rhs.m_MainModuleDesc)
	, m_ParticleDescs(_rhs.m_ParticleDescs)
	, m_ParticleMatrices(_rhs.m_ParticleMatrices)
	, m_iNumInstance(_rhs.m_iNumInstance)
{
	
}

HRESULT CParticleSystem::Initialize_Prototype(_uint iNumInstance)
{	
	// ���θ�� ����
	Vector_Size_Allocate(iNumInstance);
	Reset_AllParticle();
	Play_On_Awake();
	
	return S_OK;
}

HRESULT CParticleSystem::Initialize(void* _pArg)
{
	return S_OK;
}

void CParticleSystem::Tick(_float _fTimeDelta, CVIBuffer_Point_Color_Instance* pBuffer)
{
	if (false == IsEnable())
		return;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_ParticleMatrices.clear();
	TimeDelta_Calculate(_fTimeDelta);
	m_fParticleSystemAge += _fTimeDelta;
	Action_By_Duration();

	for (auto& ParticleDesc : m_ParticleDescs)
	{
		COL_P_INSTANCE colInstDesc;
		_float4 vPos;
		ParticleDesc.fAge += _fTimeDelta;

		// ��ƼŬ ���� ���� �ൿ ����
		if (ParticleDesc.fAge > ParticleDesc.fLifeTime)
		{
			Action_By_LifteTime(ParticleDesc);
		}
		
		// ��ƼŬ �������� ���� ���ϴ� �ɼ�
		if (true == Continue_Option(ParticleDesc))
		{
			continue;
		}

		// ������������ ��ġ���� ������.
		vPos = ParticleDesc.WorldMatrix.Translation().TransCoord();

		// ��ġ�� �ӵ��� ���ؼ� ���� ��ġ�� ����.
		vPos = vPos + ParticleDesc.vVelocity * _fTimeDelta;

		// �߷°� ����
		vPos.y -= m_MainModuleDesc.fGravityModifier * _fTimeDelta;

		// ��ġ ����
		ParticleDesc.WorldMatrix.Translation(vPos.xyz());

		// SRT ����
		_float4x4 ScaleMatrix = _float4x4::MatrixScale(ParticleDesc.vScale);
		_float4x4 TransMatrix = _float4x4::MatrixTranslation(vPos.xyz());
		_float4x4 TransfomationMatrix = ScaleMatrix * TransMatrix;

		colInstDesc.InstanceLocalMatrix = TransfomationMatrix;
		colInstDesc.vInstanceColor = { 1.f, 1.f, 1.f, 1.f };
		m_ParticleMatrices.push_back(colInstDesc);
	}

	pBuffer->Tick(m_ParticleMatrices.data());
	Safe_Release(pGameInstance);
}

HRESULT CParticleSystem::Render()
{
	return S_OK;
}

void CParticleSystem::Play_Particle()
{
	Enable();
	
	if (true == m_MainModuleDesc.isLooping && false == m_MainModuleDesc.isPrewarm)
	{
		Reset_AllParticle();
	}
}

void CParticleSystem::Stop_Particle()
{
	Disable();
}

void CParticleSystem::Bind_ParticleValue(CShader* pShader)
{
	//pShader->Bind_RawValue("g_vColor", ); 
}


void CParticleSystem::Enable()
{
	m_isEnable = true;
}

void CParticleSystem::Disable()
{
	m_isEnable = false;
}

void CParticleSystem::Save(const _tchar* _pDirectoryPath)
{
	m_MainModuleDesc.Save(_pDirectoryPath);
}

void CParticleSystem::Load(const _tchar* _pDirectoryPath)
{
	m_MainModuleDesc.Load(_pDirectoryPath);
}

void CParticleSystem::TimeDelta_Calculate(_float& fTimeDelta)
{
	// ��� �ӵ��� ���� Ÿ�ӵ�Ÿ �� ����.
	fTimeDelta *= m_MainModuleDesc.fSimulationSpeed;
}

void CParticleSystem::Action_By_LifteTime(PARTICLE_DESC& _particleDesc)
{
	// ����on : ��ƼŬ �ʱ�ȭ
	if (false == m_MainModuleDesc.isLooping)
	{
		Reset_Particle(_particleDesc);
	}
	// ����off : ��ƼŬ ���� ����. 
	else
	{
		Dead_Particle(_particleDesc);
	}
}

void CParticleSystem::Action_By_Duration()
{
	// ������ ���� �ȵ����� ����
	if (m_fParticleSystemAge <= m_MainModuleDesc.fDuration)
		return;

	// ������ �Ǿ����� ��ü ����.
	CGameObject* pOwner = dynamic_cast<CGameObject*>(m_pOwner);
	if (nullptr != pOwner)
	{
		pOwner->Set_ObjEvent(CGameObject::OBJ_DEAD);
	}
}

void CParticleSystem::Action_By_StopOption()
{
	switch (m_MainModuleDesc.eStopAction)
	{
	case MAIN_MODULE::STOP_OPTION::SO_NONE:
		break;
	case MAIN_MODULE::STOP_OPTION::SO_DISABLE:
		Disable();
		break;
	case MAIN_MODULE::STOP_OPTION::SO_DESTROY:
		//m_pOwner->SetDead();
		break;
	case MAIN_MODULE::STOP_OPTION::SO_CALLBACK:
		m_StopAction();
		break;
	default:
		break;
	}
}

void CParticleSystem::Play_On_Awake()
{
	if (false == m_MainModuleDesc.isPlayOnAwake)
	{
		for (auto& ParticleDesc : m_ParticleDescs)
			ParticleDesc.isAlive = false;
	}
}

void CParticleSystem::Vector_Size_Allocate(_uint iNumInstance)
{
	m_iNumInstance = iNumInstance;
	
	m_ParticleMatrices.reserve(m_iNumInstance);
	m_ParticleDescs.resize(m_iNumInstance);
}

_bool CParticleSystem::Continue_Option(PARTICLE_DESC& _particleDesc)
{
	if (false == _particleDesc.isAlive)
	{
		return true;
	}
	
	// �ִ� ��ƼŬ �� <= ���� Ȱ��ȭ �� ��ƼŬ ��
	if (m_MainModuleDesc.iMaxParticles <= m_iActivatedParticleNum)
	{
		return true;
	}

	return false;
}

void CParticleSystem::Reset_Particle(PARTICLE_DESC& _particleDesc)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_particleDesc.fAge = 0.f;
	_particleDesc.fLifeTime = Random_Generator(m_MainModuleDesc.fStartLifeTime.x, m_MainModuleDesc.fStartLifeTime.y);
	_particleDesc.vAccel = _float4();
	_particleDesc.WorldMatrix.Translation(_float3(0.f, 0.f, 0.f));

	_float fSpeed = Random_Generator(m_MainModuleDesc.fStartSpeed.x, m_MainModuleDesc.fStartSpeed.y);
	_particleDesc.vVelocity = pGameInstance->Get_RandomVectorInSphere(fSpeed);

	Safe_Release(pGameInstance);
}

void CParticleSystem::Reset_AllParticle()
{
	for (auto& ParticleDesc : m_ParticleDescs)
	{
		Reset_Particle(ParticleDesc);
	}
}

void CParticleSystem::Dead_Particle(PARTICLE_DESC& _particleDesc)
{
	_particleDesc.isAlive = false;
	++m_iDeadCount;
}

CParticleSystem* CParticleSystem::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint iNumInstance)
{
	CParticleSystem* pInstance = new CParticleSystem(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
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
}
