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
	// 메인모듈 생성
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

		// 파티클 수명에 따른 행동 결정
		if (ParticleDesc.fAge > ParticleDesc.fLifeTime)
		{
			Action_By_LifteTime(ParticleDesc);
		}
		
		// 파티클 연산할지 말지 정하는 옵션
		if (true == Continue_Option(ParticleDesc))
		{
			continue;
		}

		// 이전프레임의 위치값을 가져옴.
		vPos = ParticleDesc.WorldMatrix.Translation().TransCoord();

		// 위치에 속도를 더해서 최종 위치를 정함.
		vPos = vPos + ParticleDesc.vVelocity * _fTimeDelta;

		// 중력값 적용
		vPos.y -= m_MainModuleDesc.fGravityModifier * _fTimeDelta;

		// 위치 갱신
		ParticleDesc.WorldMatrix.Translation(vPos.xyz());

		// SRT 연산
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
	// 재생 속도에 따른 타임델타 값 변경.
	fTimeDelta *= m_MainModuleDesc.fSimulationSpeed;
}

void CParticleSystem::Action_By_LifteTime(PARTICLE_DESC& _particleDesc)
{
	// 루프on : 파티클 초기화
	if (false == m_MainModuleDesc.isLooping)
	{
		Reset_Particle(_particleDesc);
	}
	// 루프off : 파티클 수명 다함. 
	else
	{
		Dead_Particle(_particleDesc);
	}
}

void CParticleSystem::Action_By_Duration()
{
	// 수명이 아직 안됐으면 리턴
	if (m_fParticleSystemAge <= m_MainModuleDesc.fDuration)
		return;

	// 수명이 되었으면 객체 삭제.
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
	
	// 최대 파티클 수 <= 현재 활성화 된 파티클 수
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
