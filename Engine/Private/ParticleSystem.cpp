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
	, m_EmissionModuleDesc(_rhs.m_EmissionModuleDesc)
	, m_ShapeModuleDesc(_rhs.m_ShapeModuleDesc)
	, m_RendererModuleDesc(_rhs.m_RendererModuleDesc)
	, m_pTexture(static_cast<CTexture*>(_rhs.m_pTexture->Clone(nullptr)))
	, m_ParticleMatrices(_rhs.m_ParticleMatrices)
	, m_iActivatedParticleNum(_rhs.m_iActivatedParticleNum)
	, m_fParticleSystemAge(_rhs.m_fParticleSystemAge)
	, m_isEnable(_rhs.m_isEnable)
	, m_StopAction(_rhs.m_StopAction)
{
	for (int i = 0; i < STATE_END; ++i)
		m_Particles[i] = _rhs.m_Particles[i];
}
HRESULT CParticleSystem::Initialize_Prototype(const _tchar* _pDirectoryPath)
{	
	// 스트링을 세이브 할 때는 문자열을 256으로 고정해서 쓰기.
	//m_MainModuleDesc.Load(_pDirectoryPath);
	//m_EmissionModuleDesc.Load(_pDirectoryPath);
	//m_ShapeModuleDesc.Load(_pDirectoryPath);
	
	// 메인모듈 생성
	Resize_Container(m_MainModuleDesc.iMaxParticles);
	Reset_AllParticles();
	Play_On_Awake();

	m_pTexture = CTexture::Create(m_pDevice, m_pContext, m_RendererModuleDesc.wstrMaterial.data());

	return S_OK;
}

HRESULT CParticleSystem::Initialize(void* _pArg)
{
	return S_OK;
}
void CParticleSystem::Tick(_float _fTimeDelta, CVIBuffer_Rect_Color_Instance* pBuffer)
{
	if (false == IsEnable())
		return;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_ParticleMatrices.clear();
	TimeDelta_Calculate(_fTimeDelta);

	// 객체 수명처리
	m_fParticleSystemAge += _fTimeDelta;
	Action_By_Duration();

	// 파티클 수명 처리.
	for (auto& Particle : m_Particles[ALIVE])
		Particle.fAge += _fTimeDelta;
	for (auto& Particle : m_Particles[WAIT])
		Particle.fAge += _fTimeDelta;
	Action_By_LifteTime();
	
	// MAX_PARTICLE_NUM 만들고(상수 보이면 수정하기), 이미션 모듈 작업중.
	m_EmissionModuleDesc.fRateOverTimeAcc += _fTimeDelta;
	Wating_One_Particle();
	if (m_EmissionModuleDesc.fRateOverTimeAcc >= 1.f)
	{
		m_EmissionModuleDesc.fRateOverTimeAcc = 0.f;
	}

	_float4 vCamPosition = *pGameInstance->Get_CamPosition();
	_float3 vCamUp = *pGameInstance->Get_CamUp();

	// 파티클 연산 시작
	for (auto iter = m_Particles[ALIVE].begin(); iter != m_Particles[ALIVE].end();)
	{
		COL_INSTANCE colInstDesc;
		_float4 vPos;

		// 이전프레임의 값들을 가져옴.
		vPos = iter->WorldMatrix.Translation().TransCoord();

		// 위치에 속도를 더해서 최종 위치를 정함.
		vPos = vPos + iter->vVelocity * _fTimeDelta;

		// 중력값 적용
		iter->fGravityAccel += m_MainModuleDesc.fGravityModifier * _fTimeDelta;
		vPos.y -= iter->fGravityAccel * 0.01f; // 0.01f는 값을 미세하게 조정하기 위한 상수값.

		// 위치 갱신
		iter->WorldMatrix.Translation(vPos.xyz());

		iter->vScale = m_MainModuleDesc.f3DSizeXYZ;
		
		// SRT 연산
		_float4x4 ScaleMatrix = _float4x4::MatrixScale(iter->vScale);
		_float4x4 BillBoardMatrix = LookAt(vPos.xyz(), vCamPosition.xyz());
		_float4x4 RotationMatrix = _float4x4::MatrixRotationAxis(_float3(vPos.xyz() - vCamPosition), XMConvertToRadians(iter->fAngle));
		_float4x4 TransMatrix = _float4x4::MatrixTranslation(vPos.xyz());
		_float4x4 TransfomationMatrix = ScaleMatrix * BillBoardMatrix * RotationMatrix * TransMatrix;

		colInstDesc.InstanceLocalMatrix = TransfomationMatrix;
		colInstDesc.vInstanceColor = iter->vColor;
		m_ParticleMatrices.push_back(colInstDesc);
		++iter;
	}

	pBuffer->Set_DrawNum(_uint(m_Particles[ALIVE].size()));
	pBuffer->Tick(m_ParticleMatrices.data(), pBuffer->Get_DrawNum());
	
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
		Reset_AllParticles();
	}
}
void CParticleSystem::Stop_Particle()
{
	Disable();
}
HRESULT CParticleSystem::Bind_ParticleValue(CShader* pShader)
{
	if (FAILED(m_pTexture->Bind_ShaderResource(pShader, "g_Texture")))
		return E_FAIL;
	
	return S_OK;
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
void CParticleSystem::Action_By_LifteTime()
{
	// Gen시간 완료된 애들은 Alive로 이동
	for (auto iter = m_Particles[WAIT].begin(); iter != m_Particles[WAIT].end();)
	{
		if (iter->fAge > iter->fGenTime)
		{
			iter = TransitionTo(iter, m_Particles[WAIT], m_Particles[ALIVE]);
		}
		else
			++iter;
	}

	// 루프 옵션에 따른 수명 처리
	for (auto iter = m_Particles[ALIVE].begin(); iter != m_Particles[ALIVE].end();)
	{
		if (iter->fAge > iter->fLifeTime)
		{
			Reset_Particle(iter);
			// 루프 활성화 시 Wait으로 이동
			if (true == m_MainModuleDesc.isLooping)
			{
				iter = TransitionTo(iter, m_Particles[ALIVE], m_Particles[WAIT]);
			}
			// 루프 비활성화 시 Dead로 이동
			else
			{
				iter = TransitionTo(iter, m_Particles[ALIVE], m_Particles[DEAD]);
			}
		}
		else
		{
			++iter;
		}
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
	if (m_MainModuleDesc.strStopAction == "None")
	{

	}
	else if (m_MainModuleDesc.strStopAction == "Disable")
	{
		Disable();
	}
	else if (m_MainModuleDesc.strStopAction == "Destroy")
	{

	}
	else if (m_MainModuleDesc.strStopAction == "Callback")
	{

	}
}
void CParticleSystem::Play_On_Awake()
{
	if (false == m_MainModuleDesc.isPlayOnAwake)
	{
		for (auto iter = m_Particles[WAIT].begin(); iter != m_Particles[WAIT].end();)
		{
			iter = TransitionTo(iter, m_Particles[WAIT], m_Particles[WAIT]);
		}
	}
}
void CParticleSystem::Resize_Container(_uint iNumMaxParticle)
{	
	m_ParticleMatrices.reserve(iNumMaxParticle);
	m_Particles[WAIT].resize(iNumMaxParticle);
	m_Particles[ALIVE];
}
void CParticleSystem::Reset_Particle(PARTICLE_IT& _particle_iter)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_particle_iter->fAge = 0.f;
	_particle_iter->fGenTime = Random_Generator(m_MainModuleDesc.fStartDelay.x, m_MainModuleDesc.fStartDelay.y);
	_particle_iter->fLifeTime = Random_Generator(m_MainModuleDesc.fStartLifeTime.x, m_MainModuleDesc.fStartLifeTime.y);
	_particle_iter->fLifeTime += _particle_iter->fGenTime; // 늦게 나온만큼 수명 추가.
	_particle_iter->vAccel = _float4();

	if (false == m_MainModuleDesc.is3DStartRotation)
	{
		_particle_iter->fAngle = Random_Generator(m_MainModuleDesc.fStartRotation.x, m_MainModuleDesc.fStartRotation.y);
	}

	if (false == m_MainModuleDesc.is3DStartSize)
	{
		_float fScale = Random_Generator(m_MainModuleDesc.fStartSize.x, m_MainModuleDesc.fStartSize.y);
		_particle_iter->vScale = _float3(fScale, fScale, fScale);
	}
	else
	{
		_particle_iter->vScale = _float3(
			m_MainModuleDesc.f3DRotationXYZ.x ,
			m_MainModuleDesc.f3DRotationXYZ.y ,
			m_MainModuleDesc.f3DRotationXYZ.z);
	}
		
	if (true == RandomBool(m_MainModuleDesc.fFlipRotation))
		_particle_iter->fAngle *= -1.f;

	_particle_iter->vColor = m_MainModuleDesc.vStartColor;
	//_particle_iter->fAngle = m_MainModuleDesc.f
	_particle_iter->WorldMatrix.Translation(_float3(0.f, 0.f, 0.f));
	_float fSpeed = Random_Generator(m_MainModuleDesc.fStartSpeed.x, m_MainModuleDesc.fStartSpeed.y);
	_particle_iter->vVelocity = pGameInstance->Get_RandomVectorInSphere(fSpeed);
	_particle_iter->fGravityAccel = { 0.f };
	Safe_Release(pGameInstance);
}
void CParticleSystem::Reset_Particles(STATE eGroup)
{
	for (auto it = m_Particles[eGroup].begin(); it != m_Particles[eGroup].end(); ++it)
	{
		Reset_Particle(it);
	}
}
void CParticleSystem::Reset_AllParticles()
{
	for (_uint i = 0; i < STATE_END; ++i)
	{
		for (auto it = m_Particles[i].begin(); it != m_Particles[i].end(); ++it)
		{
			Reset_Particle(it);
		}
	}
}
_bool CParticleSystem::Wating_One_Particle()
{
	if (m_Particles[DEAD].empty())
	{
		return false;
	}
	PARTICLE_IT particle_iter = m_Particles[DEAD].begin();
	Reset_Particle(particle_iter);
	TransitionTo(particle_iter, m_Particles[DEAD], m_Particles[WAIT]);
	return true;
}
CParticleSystem::PARTICLE_IT CParticleSystem::TransitionTo(PARTICLE_IT& _particle_iter, list<PARTICLE>& _source, list<PARTICLE>& _dest)
{
	PARTICLE_IT next_it = std::next(_particle_iter);
	_dest.splice(_dest.end(), _source, _particle_iter); // 리스트 원소 이동 함수.
	return next_it;
}
_float4x4 CParticleSystem::LookAt(_float3 vPos, _float3 _vTarget, _bool _isDeleteY)
{
	_float4x4 BiliboardMatrix = _float4x4();
	_float3 vLook = _vTarget - vPos;

	if (true == _isDeleteY)
		vLook.y = 0.f;
	
	_float3 vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_float3 vUp = XMVector3Cross(vLook, vRight);

	vRight.Normalize();
	vUp.Normalize();
	vLook.Normalize();
	BiliboardMatrix.Right(vRight);
	BiliboardMatrix.Up(vUp);
	BiliboardMatrix.Look(vLook);

	return BiliboardMatrix;
}
void CParticleSystem::ReviveAll()
{
	for (auto iter = m_Particles[DEAD].begin(); iter != m_Particles[DEAD].end();)
	{
		Reset_Particle(iter);
		iter = TransitionTo(iter, m_Particles[DEAD], m_Particles[WAIT]);
	}
}
void CParticleSystem::ChangeMainTexture(const _tchar* pTexturePath)
{
	m_RendererModuleDesc.wstrMaterial = pTexturePath;
	Safe_Release(m_pTexture);
	m_pTexture = CTexture::Create(m_pDevice, m_pContext, m_RendererModuleDesc.wstrMaterial.data());
}
void CParticleSystem::RemakeBuffer(_uint iNumInstance)
{
	iNumInstance = min(iNumInstance, MAX_PARTICLE_NUM);
	CVIBuffer_Rect_Color_Instance* pBuffer = dynamic_cast<CVIBuffer_Rect_Color_Instance*>(m_pOwner->Find_Component(TEXT("Com_Buffer")));
	if (nullptr != pBuffer)
	{
		pBuffer->Initialize(&iNumInstance);
	}
}
CParticleSystem* CParticleSystem::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath)
{
	CParticleSystem* pInstance = new CParticleSystem(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pDirectoryPath)))
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
	Safe_Release(m_pTexture);
	m_Particles[ALIVE].clear();
	m_Particles[DEAD].clear();
}