#include "ParticleSystem.h"
#include "GameInstance.h"
#include "Component_Manager.h"
#include "Ease.h"
CParticleSystem::CParticleSystem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{

}
CParticleSystem::CParticleSystem(const CParticleSystem& _rhs)
	: CGameObject(_rhs)
	, m_MainModuleDesc(_rhs.m_MainModuleDesc)
	, m_EmissionModuleDesc(_rhs.m_EmissionModuleDesc)
	, m_ShapeModuleDesc(_rhs.m_ShapeModuleDesc)
	, m_RendererModuleDesc(_rhs.m_RendererModuleDesc)
	, m_ParticleMatrices(_rhs.m_ParticleMatrices)
	, m_StopAction(_rhs.m_StopAction)
	, m_iLevel(_rhs.m_iLevel)
{
	for (int i = 0; i < STATE_END; ++i)
		m_Particles[i] = _rhs.m_Particles[i];
}
HRESULT CParticleSystem::Initialize_Prototype(const _tchar* _pDirectoryPath, _uint _iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	this->Load(_pDirectoryPath);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	m_iLevel = _iLevel;
	// �ʿ��� ���� �ؽ�ó�� ������ ���� �ؽ�ó�� ����� ����
	wstring ProtoTag; 
	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_RendererModuleDesc.wstrMaterialPath.c_str());
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_RendererModuleDesc.wstrMaterialPath.c_str()))))
			return E_FAIL;
	}

	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_ShapeModuleDesc.wstrClipTexturePath.c_str());
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_ShapeModuleDesc.wstrClipTexturePath.c_str()))))
			return E_FAIL;
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_Shader_VtxRectColInstance")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, TEXT("Prototype_Component_Shader_VtxRectColInstance")
			, CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectColInstance.hlsl")
			,VTXRECTCOLORINSTANCE_DECL::Elements, VTXRECTCOLORINSTANCE_DECL::iNumElements))))
			return E_FAIL;
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_VIBuffer_Rect_Color_Instance")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, TEXT("Prototype_Component_VIBuffer_Rect_Color_Instance")
			, CVIBuffer_Rect_Color_Instance::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}
HRESULT CParticleSystem::Initialize(void* _pArg)
{
	__super::Initialize(_pArg);

	if (FAILED(Add_Components()))
		return E_FAIL;

	Resize_Container(m_MainModuleDesc.iMaxParticles);
	Play_On_Awake();

	return S_OK;
}
void CParticleSystem::Tick(_float _fTimeDelta)
{
	if (false == IsEnable())
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_EmissionModuleDesc.vCurPos = m_pTransform->Get_Position();
	m_ParticleMatrices.clear();
	// ����ӵ��� ���� TimeDelta ����
	Simulation_Speed(_fTimeDelta);

	// ��⿡ �ʿ��� TimeDelta ó��.
	Sum_TimeDelta(_fTimeDelta);

	// ��ƼŬ ���� ó��
	Action_By_Age();

	// ��ƼŬ �ʴ� ����
	Action_By_RateOverTime();

	// ��ƼŬ �̵� �Ÿ��� ���� ����
	Action_By_Distance();

	// Burst�ɼǿ� ���� ��ƼŬ ����
	Action_By_Bursts();

	_float4 vCamPosition = XMVector3TransformCoord(*pGameInstance->Get_CamPosition(), m_pTransform->Get_WorldMatrix_Inverse());
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
		vPos.y -= iter->fGravityAccel * 0.1f; // 0.1f�� ���� �̼��ϰ� �����ϱ� ���� �����.

		// ��ġ ����
		iter->WorldMatrix.Translation(vPos.xyz());
		Action_By_RotationOverLifeTime(iter, _fTimeDelta);
		
		Action_By_ColorOverLifeTime(iter, _fTimeDelta);

		// SRT ����
		_float4x4 ScaleMatrix = _float4x4::MatrixScale(iter->vScale);
		_float4x4 BillBoardMatrix = LookAt(vPos.xyz(), vCamPosition.xyz());
		_float4x4 RotationMatrix = _float4x4::MatrixRotationAxis(_float3(vPos.xyz() - vCamPosition), XMConvertToRadians(iter->fAngle));
		_float4x4 TransMatrix = _float4x4::MatrixTranslation(vPos.xyz());
		_float4x4 TransfomationMatrix = ScaleMatrix * BillBoardMatrix * RotationMatrix * TransMatrix;

		colInstDesc.vRight = TransfomationMatrix.Right().TransNorm();
		colInstDesc.vUp = TransfomationMatrix.Up().TransNorm();
		colInstDesc.vLook = TransfomationMatrix.Look().TransNorm();
		colInstDesc.vTranslation = TransfomationMatrix.Translation().TransCoord();
		colInstDesc.vColor = iter->vColor;

		m_ParticleMatrices.push_back(colInstDesc);
		++iter;
	}

	m_pBuffer->Set_DrawNum(_uint(m_Particles[ALIVE].size()));
	m_pBuffer->Tick(m_ParticleMatrices.data(), m_pBuffer->Get_DrawNum(), true, m_pTransform->Get_WorldMatrix_Inverse());
	m_EmissionModuleDesc.vPrevPos = m_EmissionModuleDesc.vCurPos;
	Safe_Release(pGameInstance);

	// ��ü ���� ó��
	Action_By_Duration();
}
void CParticleSystem::Late_Tick(_float _fTimeDelta)
{
	if (false == IsEnable())
		return;

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
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

	Restart();
}
void CParticleSystem::Stop()
{
	Disable();

	for (auto iter = m_Particles[ALIVE].begin(); iter != m_Particles[ALIVE].end();)
	{
		iter = ::TransitionTo(iter, m_Particles[ALIVE], m_Particles[WAIT]);
	}
}
HRESULT CParticleSystem::Setup_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 WorldMatrix = m_pTransform->Get_WorldMatrix();
	if (nullptr != m_pOwner)
	{
		CTransform* pOwnerTransform = dynamic_cast<CGameObject*>(m_pOwner)->Get_Transform();
		if (nullptr != pOwnerTransform)
		{
			WorldMatrix *= pOwnerTransform->Get_WorldMatrix();
		}
	}

	try
	{
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			throw "g_WorldMatrix";

		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			throw "g_ViewMatrix";

		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			throw "g_ProjMatrix";

		if (FAILED(m_pMainTexture->Bind_ShaderResource(m_pShader, "g_Texture")))
			throw "g_Texture";

		if (FAILED(m_pClipTexture->Bind_ShaderResource(m_pShader, "g_ClipTexture")))
			throw "g_ClipTexture";

		if (FAILED(m_pShader->Bind_RawValue("g_fClipThreshold", &m_ShapeModuleDesc.fClipThreshold, sizeof(_float))))
			throw "g_fClipThreshold";

		_int iClipChannel = { 3 };
		if (m_ShapeModuleDesc.strClipChannel == "Red") { iClipChannel = 0; }
		else if (m_ShapeModuleDesc.strClipChannel == "Green") { iClipChannel = 1; }
		else if (m_ShapeModuleDesc.strClipChannel == "Blue") { iClipChannel = 2; }
		else if (m_ShapeModuleDesc.strClipChannel == "Alpha") { iClipChannel = 3; }

		if (FAILED(m_pShader->Bind_RawValue("g_iClipChannel", &iClipChannel, sizeof(_int))))
			throw "g_iClipChannel";
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed SetupResources : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
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
HRESULT CParticleSystem::Save(const _tchar* _pDirectoryPath)
{
	if (FAILED(m_MainModuleDesc.Save(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_EmissionModuleDesc.Save(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_ShapeModuleDesc.Save(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_RendererModuleDesc.Save(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_RotationOverLifetimeModuleDesc.Save(_pDirectoryPath)))
		return E_FAIL;
	return S_OK;
}
HRESULT CParticleSystem::Load(const _tchar* _pDirectoryPath)
{
	if (FAILED(m_MainModuleDesc.Load(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_EmissionModuleDesc.Load(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_ShapeModuleDesc.Load(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_RendererModuleDesc.Load(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_RotationOverLifetimeModuleDesc.Load(_pDirectoryPath)))
		return E_FAIL;
	return S_OK;
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
	if (m_MainModuleDesc.fDuration >= m_MainModuleDesc.fParticleSystemAge)
		return;

	// ������ �Ǿ����� ��ü ����ó��.
	Action_By_StopOption();
}
void CParticleSystem::Action_By_StopOption()
{
	if (m_MainModuleDesc.strStopAction == "Disable")
	{
		Stop();
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
		if (false == Burst.isTrigger && Burst.fTriggerTimeAcc >= Burst.fTime && 0 == Burst.iCycleCount)
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
			}
		}
	}
}
void CParticleSystem::Action_By_RotationOverLifeTime(PARTICLE_IT& _particle_iter, _float fTimeDelta)
{
	if (false == m_RotationOverLifetimeModuleDesc.isActivate)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float3 vLook = pGameInstance->Get_CamPosition()->xyz() - _particle_iter->WorldMatrix.Translation();
	vLook.Normalize();
	_float3 vUp = { 0.f, 1.f, 0.f };
	_float3 vRight = vUp.Cross(vLook);
	vUp = vLook.Cross(vRight);

	_particle_iter->fCircularMotionAngle += m_RotationOverLifetimeModuleDesc.fSpeed * fTimeDelta;

	_float fX = m_RotationOverLifetimeModuleDesc.fRadius * cos(_particle_iter->fCircularMotionAngle);
	_float fY = m_RotationOverLifetimeModuleDesc.fRadius * sin(_particle_iter->fCircularMotionAngle);

	if (true == m_RotationOverLifetimeModuleDesc.isFlipOption)
	{
		if (RandomBool(m_RotationOverLifetimeModuleDesc.fFlipProperty))
		{
			fX = -fX;
			fY = -fY;
		}
	}

	_float3 vPos = _particle_iter->WorldMatrix.Translation();
	vPos += fX * vRight + fY * vUp;

	_particle_iter->WorldMatrix.Translation(vPos);
	Safe_Release(pGameInstance);
}
void CParticleSystem::Action_By_ColorOverLifeTime(PARTICLE_IT& _particle_iter, _float fTimeDelta)
{
	if (false == m_ColorOverLifeTimeModuleDesc.isActivate)
		return;
	_float4 changeAmount = m_ColorOverLifeTimeModuleDesc.vEndColor - m_ColorOverLifeTimeModuleDesc.vStartColor;

	_particle_iter->vColor.x = CEase::Ease(m_ColorOverLifeTimeModuleDesc.eEase, _particle_iter->fAge
		, m_ColorOverLifeTimeModuleDesc.vStartColor.x
		, changeAmount.x
		, _particle_iter->fLifeTime);

	_particle_iter->vColor.y = CEase::Ease(m_ColorOverLifeTimeModuleDesc.eEase, _particle_iter->fAge
		, m_ColorOverLifeTimeModuleDesc.vStartColor.y
		, changeAmount.y
		, _particle_iter->fLifeTime);

	_particle_iter->vColor.z = CEase::Ease(m_ColorOverLifeTimeModuleDesc.eEase, _particle_iter->fAge
		, m_ColorOverLifeTimeModuleDesc.vStartColor.z
		, changeAmount.z
		, _particle_iter->fLifeTime);

	_particle_iter->vColor.w = CEase::Ease(m_ColorOverLifeTimeModuleDesc.eEase, _particle_iter->fAge
		, m_ColorOverLifeTimeModuleDesc.vStartColor.w
		, changeAmount.w
		, _particle_iter->fLifeTime);

	//_particle_iter->vColor = m_MainModuleDesc.vStartColor * _float3::Lerp(m_ColorOverLifeTimeModuleDesc.vStartColor.xyz()
	//	, m_ColorOverLifeTimeModuleDesc.vEndColor.xyz(), _particle_iter->fAge / _particle_iter->fLifeTime).TransCoord();

	// ���̾�� ����, ������ ����� ���ְ�,
	// Ǯ�Ŵ��� -> ���۷��� ī��Ʈ�� 1�� ������ �־.
}
void CParticleSystem::ResetStartPosition(PARTICLE_IT& _particle_iter)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_particle_iter->WorldMatrix = _float4x4();
	_float4x4 TranslationMatrix = _float4x4::MatrixTranslation(m_ShapeModuleDesc.vPosition);
	_float4x4 RotationMatrix = _float4x4::MatrixFromQuaternion(XMQuaternionRotationRollPitchYaw(m_ShapeModuleDesc.vRotation.x, m_ShapeModuleDesc.vRotation.y, m_ShapeModuleDesc.vRotation.z));
	_float4x4 ScaleMatrix = _float4x4::MatrixScale(m_ShapeModuleDesc.vScale);
	_float4x4 ShapeMatrix = ScaleMatrix * RotationMatrix * TranslationMatrix;

	if ("Sphere" == m_ShapeModuleDesc.strShape)
	{
		_float3 vDir = _float3(0.f, 0.f, 1.f);
		_float fLength = 0.f;
		_float fTheta = 0.f;
		_float fPhi = 0.f;
		_float fMinLength = 0.f;
		_float fMaxLength = 0.f;
		_float fMinPhi = 0.f;
		_float fMaxPhi = 360.f;
		_float fMinTheta = 0.f;
		_float fMaxTheta = 360.f;

		// ���� ����
		if (true == m_ShapeModuleDesc.isfLengthRange)
		{
			fMinLength = m_ShapeModuleDesc.vLength.x;
			fMaxLength = m_ShapeModuleDesc.vLength.y;
			fLength = Random_Generator(fMinLength, fMaxLength);
		}
		else
		{
			fLength = m_ShapeModuleDesc.vLength.y;
		}

		// Phi���� ����.
		if ("Random" == m_ShapeModuleDesc.strPhiMode)
		{
			fMinPhi = m_ShapeModuleDesc.vPhi.x;
			fMaxPhi = m_ShapeModuleDesc.vPhi.y;
			fPhi = Random_Generator(fMinPhi, fMaxPhi);
		}
		else if ("Loop" == m_ShapeModuleDesc.strPhiMode)
		{
			m_ShapeModuleDesc.fLoopPhi += m_ShapeModuleDesc.fPhiInterval;
			if (m_ShapeModuleDesc.fLoopPhi >= m_ShapeModuleDesc.vPhi.y)
				m_ShapeModuleDesc.fLoopPhi -= m_ShapeModuleDesc.vPhi.y - m_ShapeModuleDesc.vPhi.x;

			fPhi = m_ShapeModuleDesc.fLoopPhi;
		}

		// Theta���� ����
		if ("Random" == m_ShapeModuleDesc.strThetaMode)
		{
			fMinTheta = m_ShapeModuleDesc.vTheta.x;
			fMaxTheta = m_ShapeModuleDesc.vTheta.y;
			fTheta = Random_Generator(fMinTheta, fMaxTheta);
		}
		else if ("Loop" == m_ShapeModuleDesc.strThetaMode)
		{
			m_ShapeModuleDesc.fLoopTheta += m_ShapeModuleDesc.fThetaInterval;
			if (m_ShapeModuleDesc.fLoopTheta >= m_ShapeModuleDesc.vTheta.y)
				m_ShapeModuleDesc.fLoopTheta -= m_ShapeModuleDesc.vTheta.y - m_ShapeModuleDesc.vTheta.x;

			fTheta = m_ShapeModuleDesc.fLoopTheta;
		}

		// ��ġ ����
		fTheta = XMConvertToRadians(fTheta);
		fPhi = XMConvertToRadians(fPhi);

		_float3 vLook = _float3(0.f, 0.f, 1.f);
		vDir = XMVector3TransformCoord(vDir, _float4x4::MatrixRotationX(fTheta) * _float4x4::MatrixRotationY(fPhi));
		
		(*_particle_iter).WorldMatrix.Translation(fLength * vDir);
		(*_particle_iter).WorldMatrix *= ShapeMatrix;
	}
	else if("Circle" == m_ShapeModuleDesc.strShape)
	{
		_float3 vDir = _float3(0.f, 0.f, 1.f);
		_float fLength = 0.f;
		_float fTheta = 0.f;

		_float fMinLength = 0.f;
		_float fMaxLength = 0.f;
		_float fMinTheta = 0.f;
		_float fMaxTheta = 360.f;

		// ���� ����
		if (true == m_ShapeModuleDesc.isfLengthRange)
		{
			fMinLength = m_ShapeModuleDesc.vLength.x;
			fMaxLength = m_ShapeModuleDesc.vLength.y;
			fLength = Random_Generator(fMinLength, fMaxLength);
		}
		else
		{
			fLength = m_ShapeModuleDesc.vLength.y;
		}

		// Theta���� ����
		if ("Random" == m_ShapeModuleDesc.strThetaMode)
		{
			fMinTheta = m_ShapeModuleDesc.vTheta.x;
			fMaxTheta = m_ShapeModuleDesc.vTheta.y;
			fTheta = Random_Generator(fMinTheta, fMaxTheta);
		}
		else if ("Loop" == m_ShapeModuleDesc.strThetaMode)
		{
			m_ShapeModuleDesc.fLoopTheta += m_ShapeModuleDesc.fThetaInterval;
			if (m_ShapeModuleDesc.fLoopTheta >= m_ShapeModuleDesc.vTheta.y)
				m_ShapeModuleDesc.fLoopTheta -= m_ShapeModuleDesc.vTheta.y - m_ShapeModuleDesc.vTheta.x;

			fTheta = m_ShapeModuleDesc.fLoopTheta;
		}

		_float3 vPos = _particle_iter->WorldMatrix.Translation();
		_float3 vCamPos = pGameInstance->Get_CamPosition()->xyz();
		_float4x4 ResultMatrix = pGameInstance->RightUpLook_In_Vectors(vPos, vCamPos);

		// ��ġ ����
		fTheta = XMConvertToRadians(fTheta);

		_float3 vLook = _float3(0.f, 0.f, 1.f);
		vDir = XMVector3TransformCoord(ResultMatrix.Right(), _float4x4::MatrixRotationAxis(ResultMatrix.Look(), fTheta));

		(*_particle_iter).WorldMatrix.Translation(fLength * vDir);
		(*_particle_iter).WorldMatrix *= ShapeMatrix;
	}

	Safe_Release(pGameInstance);

	// fRadiusThickness�� ���� �ܰ����� ������ ��ü������ ���� ������ ����.
	//fMinLength = m_ShapeModuleDesc.fRadius * (1.f - m_ShapeModuleDesc.fRadiusThickness);
	//fMaxLength = m_ShapeModuleDesc.fRadius;
}
void CParticleSystem::ResetVelocity(PARTICLE_IT& _particle_iter)
{
	_float fSpeed;
	if (true == m_MainModuleDesc.isStartSpeedRange)
		fSpeed = Random_Generator(m_MainModuleDesc.vStartSpeedRange.x, m_MainModuleDesc.vStartSpeedRange.y);
	else
		fSpeed = m_MainModuleDesc.fStartSpeed;

	if ("Sphere" == m_ShapeModuleDesc.strShape || "Circle" == m_ShapeModuleDesc.strShape)
	{
		_float4 vDir = _particle_iter->WorldMatrix.Translation().TransCoord() - _float4();
		vDir.Normalize();
		_particle_iter->vVelocity = vDir * fSpeed;
	}
}
HRESULT CParticleSystem::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	wstring wstrTag;
	try
	{
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Renderer")
			, TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw(TEXT("Com_Renderer"));

		if (FAILED(CComposite::Add_Component(m_iLevel
			, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_RendererModuleDesc.wstrMaterialPath.c_str()).c_str()
			, TEXT("Com_MainTexture")
			, reinterpret_cast<CComponent**>(&m_pMainTexture))))
			throw(TEXT("Com_MainTexture"));

		if (FAILED(CComposite::Add_Component(m_iLevel
			, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_ShapeModuleDesc.wstrClipTexturePath.c_str()).c_str()
			, TEXT("Com_ClipTexture")
			, reinterpret_cast<CComponent**>(&m_pClipTexture))))
			throw(TEXT("Com_ClipTexture"));
		
		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Shader_VtxRectColInstance")
			, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
			throw(TEXT("Com_Shader"));

		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_VIBuffer_Rect_Color_Instance")
			, TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer), &m_MainModuleDesc.iMaxParticles)))
			throw(TEXT("Com_Buffer"));
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed Add_Component in CParticleSystem: ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
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
	m_Particles[ALIVE].clear();
	m_Particles[WAIT].resize(iNumMaxParticle);
	m_Particles[DEAD].clear();
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

	// �ʰ� ���¸�ŭ ���� �߰�.
	_particle_iter->fLifeTime += _particle_iter->fGenTime;
	_particle_iter->vAccel = _float4();

	// ���� ��ġ
	ResetStartPosition(_particle_iter);
	ResetVelocity(_particle_iter);

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


	_particle_iter->fGravityAccel = { 0.f };

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
	::TransitionTo(particle_iter, m_Particles[WAIT], m_Particles[DELAY]);
	return true;
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
	// DEAD->WAIT
	for (auto iter = m_Particles[DEAD].begin(); iter != m_Particles[DEAD].end();)
	{
		Reset_Particle(iter);
		iter = TransitionTo(iter, m_Particles[DEAD], m_Particles[WAIT]);
	}

	// ������
	for (auto& Particle : m_Particles[ALIVE])
		Particle.fAge = 0.f;
	for (auto& Particle : m_Particles[DELAY])
		Particle.fAge = 0.f;

	m_MainModuleDesc.Restart();
	m_EmissionModuleDesc.Restart();
	m_ShapeModuleDesc.Restart();
	m_RendererModuleDesc.Restart();
	m_RotationOverLifetimeModuleDesc.Restart();
}
CParticleSystem* CParticleSystem::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath, _uint m_iLevel)
{
	CParticleSystem* pInstance = new CParticleSystem(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pDirectoryPath, m_iLevel)))
	{
		MSG_BOX("Failed to Created CParticleSystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CParticleSystem::Clone(void* _pArg)
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
	Safe_Release(m_pMainTexture);
	Safe_Release(m_pClipTexture);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);

	for (_uint i = 0; i < STATE_END; ++i)
	{
		m_Particles[i].clear();
	}
}