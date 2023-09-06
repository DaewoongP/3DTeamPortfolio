#include "ParticleSystem.h"
#include "GameInstance.h"
#include "Component_Manager.h"
#include "GameObject.h"

CParticleSystem::CParticleSystem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComposite(_pDevice, _pContext)
{

}
CParticleSystem::CParticleSystem(const CParticleSystem& _rhs)
	: CComposite(_rhs)
	, m_MainModuleDesc(_rhs.m_MainModuleDesc)
	, m_EmissionModuleDesc(_rhs.m_EmissionModuleDesc)
	, m_ShapeModuleDesc(_rhs.m_ShapeModuleDesc)
	, m_RendererModuleDesc(_rhs.m_RendererModuleDesc)
	, m_ParticleMatrices(_rhs.m_ParticleMatrices)
	, m_StopAction(_rhs.m_StopAction)
{
	for (int i = 0; i < STATE_END; ++i)
		m_Particles[i] = _rhs.m_Particles[i];
}
HRESULT CParticleSystem::Initialize_Prototype(const _tchar* _pDirectoryPath)
{	
	// ��Ʈ���� ���̺� �� ���� ���ڿ��� 256���� �����ؼ� ����.
	//m_MainModuleDesc.Load(_pDirectoryPath);
	//m_EmissionModuleDesc.Load(_pDirectoryPath);
	//m_ShapeModuleDesc.Load(_pDirectoryPath);
	
	//static mesh -> �Ǹ���, ��, ����
	// -> ���̳� 
	// ���θ�� ����
	Resize_Container(m_MainModuleDesc.iMaxParticles);
	//Reset_AllParticles();
	Play_On_Awake();

	return S_OK;
}

HRESULT CParticleSystem::Initialize(void* _pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CParticleSystem::Tick(_float _fTimeDelta, CTransform* pTransform)
{
	if (false == IsEnable())
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_EmissionModuleDesc.vCurPos = pTransform->Get_Position();
	m_ParticleMatrices.clear();
	// ����ӵ��� ���� TimeDelta ����
	Simulation_Speed(_fTimeDelta);

	// ��⿡ �ʿ��� TimeDelta ó��.
	Sum_TimeDelta(_fTimeDelta);
	
	// ��ü ���� ó��
	Action_By_Duration();

	// ��ƼŬ ���� ó��
	Action_By_Age();

	// ��ƼŬ �ʴ� ����
	Action_By_RateOverTime();

	// ��ƼŬ �̵� �Ÿ��� ���� ����
	Action_By_Distance();

	// Burst�ɼǿ� ���� ��ƼŬ ����
	Action_By_Bursts();

	_float4 vCamPosition = XMVector3TransformCoord(*pGameInstance->Get_CamPosition(), pTransform->Get_WorldMatrix_Inverse());
	_float3 vCamUp = *pGameInstance->Get_CamUp();

	// ��ƼŬ ���� ����
	for (auto iter = m_Particles[ALIVE].begin(); iter != m_Particles[ALIVE].end();)
	{
		COL_INSTANCE colInstDesc;
		_float4 vPos;

		// ������������ ������ ������.
		vPos = iter->WorldMatrix.Translation().TransCoord();

		// ��ġ�� �ӵ��� ���ؼ� ���� ��ġ�� ����.
		vPos = vPos + iter->vVelocity * _fTimeDelta;

		// �߷°� ����
		iter->fGravityAccel += m_MainModuleDesc.fGravityModifier * _fTimeDelta;
		vPos.y -= iter->fGravityAccel * 0.01f; // 0.01f�� ���� �̼��ϰ� �����ϱ� ���� �����.

		// ��ġ ����
		iter->WorldMatrix.Translation(vPos.xyz());
		
		// SRT ����
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

	m_pBuffer->Set_DrawNum(_uint(m_Particles[ALIVE].size()));
	m_pBuffer->Tick(m_ParticleMatrices.data(), m_pBuffer->Get_DrawNum());
	m_EmissionModuleDesc.vPrevPos = m_EmissionModuleDesc.vCurPos;
	Safe_Release(pGameInstance);
}

HRESULT CParticleSystem::Render()
{
	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Default")))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}
void CParticleSystem::Play()
{
	Enable();
	
	if (true == m_MainModuleDesc.isLooping && false == m_MainModuleDesc.isPrewarm)
	{
		Reset_AllParticles();
	}
}
void CParticleSystem::Stop()
{
	Disable();
}
HRESULT CParticleSystem::Setup_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pTransform = dynamic_cast<CGameObject*>(m_pOwner)->Get_Transform();
	if (nullptr != pTransform)
	{
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", pTransform->Get_WorldMatrixPtr())))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", pGameInstance->Get_CamPosition(), sizeof(_float4))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_Texture")))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(m_pClipTexture->Bind_ShaderResource(m_pShader, "g_ClipTexture")))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}
void CParticleSystem::Enable()
{
	m_MainModuleDesc.isEnable = true;
}
void CParticleSystem::Disable()
{
	m_MainModuleDesc.isEnable = false;
}
void CParticleSystem::Save(const _tchar* _pDirectoryPath)
{
	m_MainModuleDesc.Save(_pDirectoryPath);
}
void CParticleSystem::Load(const _tchar* _pDirectoryPath)
{
	m_MainModuleDesc.Load(_pDirectoryPath);
}
void CParticleSystem::Simulation_Speed(_float& fTimeDelta)
{
	// ��� �ӵ��� ���� Ÿ�ӵ�Ÿ �� ����.
	fTimeDelta *= m_MainModuleDesc.fSimulationSpeed;
}
void CParticleSystem::Action_By_Age()
{
	// DELAY�ð� �Ϸ�� �ֵ��� ALIVE�� �̵�
	for (auto iter = m_Particles[DELAY].begin(); iter != m_Particles[DELAY].end();)
	{
		if (iter->fAge > iter->fGenTime)
		{
			iter = TransitionTo(iter, m_Particles[DELAY], m_Particles[ALIVE]);
		}
		else
			++iter;
	}

	// ���� �ɼǿ� ���� ���� ó��
	for (auto iter = m_Particles[ALIVE].begin(); iter != m_Particles[ALIVE].end();)
	{
		if (iter->fAge > iter->fLifeTime)
		{
			Reset_Particle(iter);
			// ���� Ȱ��ȭ �� Wait���� �̵�
			if (true == m_MainModuleDesc.isLooping)
			{
				iter = TransitionTo(iter, m_Particles[ALIVE], m_Particles[WAIT]);
			}
			// ���� ��Ȱ��ȭ �� Dead�� �̵�
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
	// ������ ���� �ȵ����� ����
	if (m_MainModuleDesc.fParticleSystemAge <= m_MainModuleDesc.fDuration)
		return;

	// ������ �Ǿ����� ��ü ����ó��.
	Action_By_StopOption();
}
void CParticleSystem::Action_By_StopOption()
{
	if (m_MainModuleDesc.strStopAction == "Disable")
	{
		Disable();
	}
	else if (m_MainModuleDesc.strStopAction == "Destroy")
	{
		CGameObject* pOwner = dynamic_cast<CGameObject*>(m_pOwner);
		if (nullptr != pOwner)
		{
			pOwner->Set_ObjEvent(CGameObject::OBJ_DEAD);
		}
	}
	else if (m_MainModuleDesc.strStopAction == "Callback")
	{

	}
}
void CParticleSystem::Action_By_RateOverTime()
{
	if (m_EmissionModuleDesc.fRateOverTime <= 0.001f)
		return;

	// �ʴ� N���� �����ϴ� �ڵ�
	_float fTimePerParticle = 1.f / m_EmissionModuleDesc.fRateOverTime;
	while (m_EmissionModuleDesc.fRateOverTimeAcc >= fTimePerParticle)
	{
		Wating_One_Particle();
		m_EmissionModuleDesc.fRateOverTimeAcc -= fTimePerParticle;
	}
}
void CParticleSystem::Action_By_Distance()
{
	_float fPositionDelta = { _float3(m_EmissionModuleDesc.vCurPos - m_EmissionModuleDesc.vPrevPos).Length() };
	if (fPositionDelta <= 0.001f)
		return;

	for (_uint i = 0; i < _uint(fPositionDelta * m_EmissionModuleDesc.fRateOverDistance); ++i)
	{
		Wating_One_Particle();
	}
}
void CParticleSystem::Action_By_Bursts()
{
	for (auto& Burst : m_EmissionModuleDesc.Bursts)
	{
		if (false == Burst.isTrigger && Burst.fTriggerTimeAcc >= Burst.fTime)
		{
			if (RandomBool(Burst.fProbability))
			{
				Burst.isTrigger = true;
				Burst.iCycleCount = 0;
			}
		}

		if (true == Burst.isTrigger)
		{
			// ������ �������� ��ƼŬ�� �߻���Ű�� �ڵ�.
			if (Burst.fIntervalTimeAcc >= Burst.fInterval)
			{
				for (_uint i = 0; i < Burst.iCount.x; ++i)
				{
					Wating_One_Particle();
				}
				Burst.fIntervalTimeAcc = 0.f;
				++Burst.iCycleCount;
			}

			// ��� ����Ŭ�� ���� Trigger�� ��Ȱ��ȭ
			if (Burst.iCycleCount >= Burst.iCycles)
			{
				Burst.isTrigger = false;
				Burst.fTriggerTimeAcc = 0.f;
			}
		}
	}
}
HRESULT CParticleSystem::Add_Components()
{
	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Renderer")
		, TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(0
		, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_RendererModuleDesc.wstrMaterialPath.c_str()).c_str()
		, TEXT("Com_MainTexture")
		, reinterpret_cast<CComponent**>(&m_pTexture))))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(0
		, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_ShapeModuleDesc.wstrClipTexture.c_str()).c_str()
		, TEXT("Com_ClipTexture")
		, reinterpret_cast<CComponent**>(&m_pClipTexture))))
		return E_FAIL;

	wstring wstrShaderTag = TEXT("Prototype_Component_");
	wstrShaderTag += m_RendererModuleDesc.wstrShaderTag;
	if (FAILED(CComposite::Add_Component(0, wstrShaderTag.c_str()
		, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Rect_Color_Instance")
		, TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer), &m_MainModuleDesc.iMaxParticles)))
		return E_FAIL;

	return S_OK;
}
void CParticleSystem::Play_On_Awake()
{
	if (false == m_MainModuleDesc.isPlayOnAwake)
	{
		Enable();
	}
}
void CParticleSystem::Resize_Container(_uint iNumMaxParticle)
{	
	m_ParticleMatrices.reserve(iNumMaxParticle);
	m_Particles[WAIT].resize(iNumMaxParticle);
}
void CParticleSystem::Sum_TimeDelta(const _float& _fTimeDelta)
{
	// ����ó��
	m_MainModuleDesc.fParticleSystemAge += _fTimeDelta;
	for (auto& Particle : m_Particles[ALIVE])
		Particle.fAge += _fTimeDelta;
	for (auto& Particle : m_Particles[DELAY])
		Particle.fAge += _fTimeDelta;
	for (auto& Burst : m_EmissionModuleDesc.Bursts)
	{
		if (true == Burst.isTrigger)
		{
			Burst.fIntervalTimeAcc += _fTimeDelta;
		}

		Burst.fTriggerTimeAcc += _fTimeDelta;
	}
	m_EmissionModuleDesc.fRateOverTimeAcc += _fTimeDelta;
}
void CParticleSystem::Reset_Particle(PARTICLE_IT& _particle_iter)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_particle_iter->fAge = 0.f;

	// Delay ����
	if (true == m_MainModuleDesc.isStartDelayRange)
		_particle_iter->fGenTime = Random_Generator(m_MainModuleDesc.vStartDelayRange.x, m_MainModuleDesc.vStartDelayRange.y);
	else
		_particle_iter->fGenTime = m_MainModuleDesc.fStartDelay;

	// LifeTime ����
	if (true == m_MainModuleDesc.isStartLifeTimeRange)
		_particle_iter->fLifeTime = Random_Generator(m_MainModuleDesc.vStartLifeTimeRange.x, m_MainModuleDesc.vStartLifeTimeRange.y);
	else
		_particle_iter->fLifeTime = m_MainModuleDesc.fStartLifeTime;

	// Speed ����
	_float fSpeed;
	if (true == m_MainModuleDesc.isStartSpeedRange)
		fSpeed = Random_Generator(m_MainModuleDesc.vStartSpeedRange.x, m_MainModuleDesc.vStartSpeedRange.y);
	else
		fSpeed = m_MainModuleDesc.fStartSpeed;

	// Speed�� ���� Velocity ����
	_particle_iter->vVelocity = pGameInstance->Get_RandomVectorInSphere(fSpeed);

	// �ʰ� ���¸�ŭ ���� �߰�.
	_particle_iter->fLifeTime += _particle_iter->fGenTime; 

	_particle_iter->vAccel = _float4();

	if (false == m_MainModuleDesc.is3DStartRotation)
	{
		if (true == m_MainModuleDesc.isStartRotationRange)
			_particle_iter->fAngle = Random_Generator(m_MainModuleDesc.vStartRotationRange.x, m_MainModuleDesc.vStartRotationRange.y);
		else
			_particle_iter->fAngle = m_MainModuleDesc.fStartRotation;
	}
	if (true == RandomBool(m_MainModuleDesc.fFlipRotation))
		_particle_iter->fAngle *= -1.f;

	// Size ����
	if (true == m_MainModuleDesc.is3DStartSize)
	{	
		_particle_iter->vScale = _float3(m_MainModuleDesc.f3DSizeXYZ.x, m_MainModuleDesc.f3DSizeXYZ.y, m_MainModuleDesc.f3DSizeXYZ.z);
	}
	else
	{
		_float fScale;

		if (true == m_MainModuleDesc.isStartSizeRange)
			fScale = Random_Generator(m_MainModuleDesc.vStartSizeRange.x, m_MainModuleDesc.vStartSizeRange.y);
		else
			fScale = m_MainModuleDesc.fStartSize;

		_particle_iter->vScale = _float3(fScale, fScale, fScale);
	}

	_particle_iter->vColor = m_MainModuleDesc.vStartColor;
	//_particle_iter->fAngle = m_MainModuleDesc.f
	_particle_iter->WorldMatrix.Translation(_float3(0.f, 0.f, 0.f));
	
	
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
	if (m_Particles[WAIT].empty())
	{
		return false;
	}

	if (m_Particles[ALIVE].size() >= m_MainModuleDesc.iMaxParticles)
	{
		return false;
	}

	PARTICLE_IT particle_iter = m_Particles[WAIT].begin();
	Reset_Particle(particle_iter);
	TransitionTo(particle_iter, m_Particles[WAIT], m_Particles[DELAY]);
	return true;
}
CParticleSystem::PARTICLE_IT CParticleSystem::TransitionTo(PARTICLE_IT& _particle_iter, list<PARTICLE>& _source, list<PARTICLE>& _dest)
{
	
	PARTICLE_IT next_it = std::next(_particle_iter);
	_dest.splice(_dest.end(), _source, _particle_iter); // ����Ʈ ���� �̵� �Լ�.
	return next_it; // for�� ��ȸ�� ���� ���� iter�� ��ȯ.(erase�� �� next��ȯ�ϴ� ����)
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
void CParticleSystem::Restart()
{
	for (auto iter = m_Particles[DEAD].begin(); iter != m_Particles[DEAD].end();)
	{
		Reset_Particle(iter);
		iter = TransitionTo(iter, m_Particles[DEAD], m_Particles[WAIT]);
		m_MainModuleDesc.fParticleSystemAge = 0.f;
	}
}
void CParticleSystem::ChangeMainTexture(const _tchar* pTexturePath)
{
	m_RendererModuleDesc.wstrMaterialPath = pTexturePath;
	Safe_Release(m_pTexture);
	m_pTexture = CTexture::Create(m_pDevice, m_pContext, m_RendererModuleDesc.wstrMaterialPath.data());
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
CComposite* CParticleSystem::Clone(void* _pArg)
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
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTexture);
	Safe_Release(m_pClipTexture);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);

	for (_uint i = 0; i < STATE_END; ++i)
	{
		m_Particles[i].clear();
	}
}