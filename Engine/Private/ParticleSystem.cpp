#include "ParticleSystem.h"
#include "GameInstance.h"
#include "Component_Manager.h"
#include "Ease.h"
#include "ParticleSystemPool.h"

CParticleSystem::CParticleSystem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{

}
CParticleSystem::CParticleSystem(const CParticleSystem& _rhs)
	: CGameObject(_rhs)
	, m_MainModuleDesc(_rhs.m_MainModuleDesc)
	, m_EmissionModuleDesc(_rhs.m_EmissionModuleDesc)
	, m_ShapeModuleDesc(_rhs.m_ShapeModuleDesc)
	, m_ColorOverLifeTimeModuleDesc(_rhs.m_ColorOverLifeTimeModuleDesc)
	, m_SizeOverLifeTimeModuleDesc(_rhs.m_SizeOverLifeTimeModuleDesc)
	, m_RotationOverLifetimeModuleDesc(_rhs.m_RotationOverLifetimeModuleDesc)
	, m_TextureSheetAnimationModuleDesc(_rhs.m_TextureSheetAnimationModuleDesc)
	, m_RendererModuleDesc(_rhs.m_RendererModuleDesc)
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

	if (FAILED(this->Load(_pDirectoryPath)))
	{
		MSG_BOX("Failed to Load ParticleSystem but You can progress");
	}

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

	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_TextureSheetAnimationModuleDesc.wstrNormalPath.c_str());
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_TextureSheetAnimationModuleDesc.wstrNormalPath.c_str()))))
			return E_FAIL;
	}

	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_RendererModuleDesc.wstrGraientTexture.c_str());
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_RendererModuleDesc.wstrGraientTexture.c_str()))))
			return E_FAIL;
	}

	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_RendererModuleDesc.wstrDistortionTexture.c_str());
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_RendererModuleDesc.wstrDistortionTexture.c_str()))))
			return E_FAIL;
	}

	pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
		, TEXT("../../Resources/Effects/Textures/Noises/VFX_T_Noise04_D.png"));

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_Shader_VtxRectColIdxInstance")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, TEXT("Prototype_Component_Shader_VtxRectColIdxInstance")
			, CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectColIdxInstance.hlsl")
				, VTXPARTICLE_DECL::Elements, VTXPARTICLE_DECL::iNumElements))))
			return E_FAIL;
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_VIBuffer_Rect_Particle_Instance")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, TEXT("Prototype_Component_VIBuffer_Rect_Particle_Instance")
			, CVIBuffer_Rect_Particle_Instance::Create(m_pDevice, m_pContext))))
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

	if (m_isTrace)
	{
		_float4x4 PositionMatrix = m_TraceOffsetMatrix * (*m_pTraceBindBoneMatrix) * m_TracePivotMatrix * (*m_pTraceWorldMatrix);
		m_pTransform->Set_Position(PositionMatrix.Translation());

		_float4x4 WorldMatrix = *m_pTraceWorldMatrix;
		WorldMatrix.m[3][0] = 0;
		WorldMatrix.m[3][1] = 0;
		WorldMatrix.m[3][2] = 0;

		Get_ShapeModuleRef().ShapeMatrix *= WorldMatrix;
	}

	// Stop��ư�� ���� �� ��� ��ƼŬ���� �Ҹ��ϸ� �ڵ����� Disable�� �ȴ�.
	if (true == m_isStop && true == Is_AllDead())
		Action_By_StopOption();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_EmissionModuleDesc.vCurPos = m_pTransform->Get_Position();
	m_ParticleMatrices.clear();

	// ����ӵ��� ���� TimeDelta ����
	Simulation_Speed(_fTimeDelta);

	m_MainModuleDesc.fParticleSystemAge += _fTimeDelta;
	for (auto& Particle : m_Particles[DELAY])
		Particle.fAge += _fTimeDelta;

	// ��ƼŬ ���� ó��
	Action_By_Age();

	// ��ƼŬ �غ�.
	m_EmissionModuleDesc.Action(this, _fTimeDelta);

	// ī�޶� ��ƼŬ �ý����� ���� ���������� ��.
	_float4 vCamPosition;
	if (true == m_ShapeModuleDesc.isChase)
		vCamPosition = XMVector3TransformCoord(*pGameInstance->Get_CamPosition(), m_pTransform->Get_WorldMatrix_Inverse());
	else
		vCamPosition = *pGameInstance->Get_CamPosition();

	// ��ƼŬ ���� ����
	for (auto Particle_iter = m_Particles[ALIVE].begin(); Particle_iter != m_Particles[ALIVE].end();)
	{
		VTXPARTICLEINSTANCE InstDesc;
		_float3 vPos;

		// ��ƼŬ ���� ó��
		Particle_iter->fAge += _fTimeDelta;
		if (Particle_iter->fAge >= Particle_iter->fLifeTime)
			Particle_iter->isAlive = false;

		// ������������ ������ ������.
		vPos = Particle_iter->WorldMatrix.Translation();
		_float3 vPrevPos = vPos;

		// ��ġ�� �ӵ��� ���ؼ� ���� ��ġ�� ����.
		vPos = vPos + Particle_iter->vVelocity * _fTimeDelta;

		// �߷°� ����
		Particle_iter->fGravityAccel += m_MainModuleDesc.fGravityModifier * _fTimeDelta;
		vPos.y -= Particle_iter->fGravityAccel * 0.1f; // 0.1f�� ���� �̼��ϰ� �����ϱ� ���� �����.

		// ��ġ ����
		Particle_iter->WorldMatrix.Translation(vPos);

		m_VelocityOverLifeTimeModuleDesc.Action(Particle_iter, m_pTransform->Get_WorldMatrixPtr(), _fTimeDelta);

		// LifeTime ���ø��
		if (true == m_MainModuleDesc.is3DStartSize)
			m_SizeOverLifeTimeModuleDesc.Action(m_MainModuleDesc.v3DSizeXYZ, Particle_iter, _fTimeDelta);
		else
		{
			_float3 vSize = _float3(m_MainModuleDesc.fStartSize, m_MainModuleDesc.fStartSize, m_MainModuleDesc.fStartSize);
			m_SizeOverLifeTimeModuleDesc.Action(vSize, Particle_iter, _fTimeDelta);
		}

		m_RotationOverLifetimeModuleDesc.Action(Particle_iter, _fTimeDelta);
		m_ColorOverLifeTimeModuleDesc.Action(Particle_iter, _fTimeDelta);

		// �ؽ�ó ��Ʈ ���
		m_TextureSheetAnimationModuleDesc.Action(Particle_iter, _fTimeDelta);

		// SRT ����
		_float4x4 ScaleMatrix = _float4x4::MatrixScale(Particle_iter->vScale);
		_float4x4 BillBoardMatrix = _float4x4();
		_float4x4 RotationMatrix = _float4x4();
		_float4x4 DirectionMatrix = _float4x4();

		{
			BillBoardMatrix = LookAt(vPos, vCamPosition.xyz(), m_RendererModuleDesc.isDeleteY);
			if (true == m_MainModuleDesc.is3DStartAngle)
			{
				RotationMatrix = _float4x4::MatrixFromQuaternion(XMQuaternionRotationRollPitchYaw(
					XMConvertToRadians(m_MainModuleDesc.vStartAngle3D.x),
					XMConvertToRadians(m_MainModuleDesc.vStartAngle3D.y),
					XMConvertToRadians(m_MainModuleDesc.vStartAngle3D.z)));
			}
			else
				RotationMatrix = _float4x4::MatrixRotationAxis(_float3(vCamPosition - vPos), XMConvertToRadians(Particle_iter->fAngle));
		}

		if (true == m_MainModuleDesc.isDirectionRotation) // ���� �������� ȸ��
		{
			_float3 vDir = vPos - vPrevPos;
			vDir.z = 0;
			vDir.Normalize();

			_float fDot = XMVectorGetX(XMVector3Dot(vDir, Particle_iter->WorldMatrix.Up()));
			_float fRadian = acosf(fDot);

			if (XMVectorGetY(XMVector3Cross(vDir, BillBoardMatrix.Look())) < 0)
				fRadian = 2 * XMVectorGetX(g_XMPi) - fRadian;

			_float4x4 rotationMatrix = XMMatrixRotationAxis(BillBoardMatrix.Look(), fRadian);
			DirectionMatrix = rotationMatrix;
		}

		_float4x4 TranslationMatrix = _float4x4::MatrixTranslation(vPos);
		_float4x4 TransfomationMatrix = ScaleMatrix * BillBoardMatrix * DirectionMatrix * RotationMatrix * TranslationMatrix;

		InstDesc.vRight = TransfomationMatrix.Right().TransNorm();
		InstDesc.vUp = TransfomationMatrix.Up().TransNorm();
		InstDesc.vLook = TransfomationMatrix.Look().TransNorm();
		InstDesc.vTranslation = TransfomationMatrix.Translation().TransCoord();
		InstDesc.vColor = Particle_iter->vColor;
		InstDesc.iCurrentIndex = Particle_iter->iCurIndex;
		InstDesc.vVelocity = Particle_iter->vVelocity;
		PlayEvent(Particle_iter);
		m_ParticleMatrices.push_back(InstDesc);
		++Particle_iter;
	}

	m_pBuffer->Set_DrawNum(_uint(m_Particles[ALIVE].size()));
	m_pBuffer->Tick(m_ParticleMatrices.data(), m_pBuffer->Get_DrawNum());
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
	{
		if (true == m_RendererModuleDesc.isGlow)
		{
			m_pRenderer->Add_RenderGroup(CRenderer::RENDER_GLOW, this);
		}
		if (true == m_RendererModuleDesc.isDistortion)
		{
			m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);
		}
		if (m_RendererModuleDesc.isDiffuse)
		{
			if ("Default" == m_RendererModuleDesc.strPass ||
				"TextureSheetAnimation" == m_RendererModuleDesc.strPass ||
				"MotionBlur" == m_RendererModuleDesc.strPass ||
				"Default_Depth_Disable" == m_RendererModuleDesc.strPass)
			{
				m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
			}
			else
			{
				m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
			}
		}
	}
}

HRESULT CParticleSystem::Render()
{
	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(m_RendererModuleDesc.strPass.data())))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

void CParticleSystem::Play(_float3 vPosition)
{
	if (true == IsEnable())
		Restart();

	m_isStop = false;

	Enable(vPosition);
}
void CParticleSystem::Stop()
{
	m_isStop = true;
}
HRESULT CParticleSystem::Setup_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 WorldMatrix;
	if (true == m_ShapeModuleDesc.isChase)
		WorldMatrix = m_pTransform->Get_WorldMatrix();

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

		if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw "g_fCamFar";

		_float fTimeAcc = pGameInstance->Get_World_TimeAcc();
		if (FAILED(m_pShader->Bind_RawValue("g_fTime", &fTimeAcc, sizeof(_float))))
			throw "g_fTime";

		_int iClipChannel = { 3 };
		if (m_ShapeModuleDesc.strClipChannel == "Red") { iClipChannel = 0; }
		else if (m_ShapeModuleDesc.strClipChannel == "Green") { iClipChannel = 1; }
		else if (m_ShapeModuleDesc.strClipChannel == "Blue") { iClipChannel = 2; }
		else if (m_ShapeModuleDesc.strClipChannel == "Alpha") { iClipChannel = 3; }

		if (FAILED(m_pShader->Bind_RawValue("g_iClipChannel", &iClipChannel, sizeof(_int))))
			throw "g_iClipChannel";

		if (true == m_TextureSheetAnimationModuleDesc.isActivate)
		{
			if (FAILED(m_pShader->Bind_RawValue("g_iWidthLength", &m_TextureSheetAnimationModuleDesc.iWidthLength, sizeof(_uint))))
				throw "g_iWidthLength";

			if (FAILED(m_pShader->Bind_RawValue("g_iHeightLength", &m_TextureSheetAnimationModuleDesc.iHeightLength, sizeof(_uint))))
				throw "g_iHeightLength";

			if (FAILED(m_pShader->Bind_RawValue("g_isUseNormalTexture", &m_TextureSheetAnimationModuleDesc.isUseNormalTexture, sizeof(_bool))))
				throw "g_isUseNormalTexture";
		}
		if (FAILED(m_pNormalTexture->Bind_ShaderResource(m_pShader, "g_NormalTexture")))
			throw "g_NormalTexture";

		if (FAILED(m_pGradientTexture->Bind_ShaderResource(m_pShader, "g_GradientTexture")))
			throw "g_GradientTexture";

		if (FAILED(m_pNoiseTexture->Bind_ShaderResource(m_pShader, "g_NoiseTexture")))
			throw "g_NoiseTexture";

		if (FAILED(m_pShader->Bind_RawValue("g_isUseGradientTexture", &m_RendererModuleDesc.isUseGradientTexture, sizeof(_bool))))
			throw "g_isUseGradientTexture";

		if (FAILED(m_pShader->Bind_RawValue("g_isTextureSheetAnimationActivated", &m_TextureSheetAnimationModuleDesc.isActivate, sizeof(_bool))))
			throw "g_isTextureSheetAnimationActivated";
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
void CParticleSystem::Enable(_float3 vPos)
{
	m_pTransform->Set_Position(vPos);
	Get_EmissionModuleRef().Setting_PrevPos(vPos);
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
	if (FAILED(m_TextureSheetAnimationModuleDesc.Save(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_ColorOverLifeTimeModuleDesc.Save(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_SizeOverLifeTimeModuleDesc.Save(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_RotationOverLifetimeModuleDesc.Save(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_RendererModuleDesc.Save(_pDirectoryPath)))
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
	if (FAILED(m_TextureSheetAnimationModuleDesc.Load(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_ColorOverLifeTimeModuleDesc.Load(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_SizeOverLifeTimeModuleDesc.Load(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_RotationOverLifetimeModuleDesc.Load(_pDirectoryPath)))
		return E_FAIL;
	if (FAILED(m_RendererModuleDesc.Load(_pDirectoryPath)))
		return E_FAIL;

	return S_OK;
}

void CParticleSystem::Simulation_Speed(_float& fTimeDelta)
{
	// ��� �ӵ��� ���� Ÿ�ӵ�Ÿ �� ����.
	fTimeDelta *= m_MainModuleDesc.fSimulationSpeed;
}
void CParticleSystem::Add_Notify(PT_NOTIFY PTNotify)
{
	for (_uint i = 0; i < STATE_END; ++i)
	{
		for (auto iter = m_Particles[i].begin(); iter != m_Particles[i].end(); ++iter)
		{
			iter->Add_Notify(PTNotify);
		}
	}
}
void CParticleSystem::PlayEvent(const PARTICLE_IT& Particle_iter)
{
	for (auto iter = Particle_iter->Events.begin(); iter != Particle_iter->Events.end();)
	{
		if (false == iter->CanPlay)
		{
			++iter;
			continue;
		}

		if (iter->StartCondition(Particle_iter))
		{
			iter->Event();

			if (true == iter->isPlayOnce)
			{
				iter->CanPlay = false;
			}
		}

		++iter;
	}
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
		if (false == iter->isAlive)
		{
			//Reset_Particle(iter);
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
	if ("Disable" == m_MainModuleDesc.strStopAction)
	{
		Disable();
		Restart();
	}
	else if ("Destroy" == m_MainModuleDesc.strStopAction)
	{
		Set_ObjEvent(CGameObject::OBJ_DEAD);
	}
	else if ("Callback" == m_MainModuleDesc.strStopAction)
	{
		if (m_StopAction) // �ݹ��Լ��� �ּҰ� �ִ��� �˻�.
			m_StopAction();
		Disable();
		Restart();
	}
	else if ("Pool" == m_MainModuleDesc.strStopAction)
	{
		if (TEXT("") == m_szParticleTag) // �� ũ���� ������
			return;

		Set_ObjEvent(CGameObject::OBJ_DEAD);
		CParticleSystemPool* pPool = CParticleSystemPool::GetInstance();
		Safe_AddRef(pPool);
		pPool->Return_Particle(this);
		Restart();
		Safe_Release(pPool);
	}
}

void CParticleSystem::ResetStartPosition(PARTICLE_IT& _particle_iter)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// ���� �ʱ�ȭ
	_float3 vDirection;
	_float3 vPosition;
	_float fSpeed;

	// �ӵ� ����
	if (true == m_MainModuleDesc.isStartSpeedRange)
		fSpeed = Random_Generator(m_MainModuleDesc.vStartSpeedRange.x, m_MainModuleDesc.vStartSpeedRange.y);
	else
		fSpeed = m_MainModuleDesc.fStartSpeed;

	// ��ġ�� ���� ����
	if ("Sphere" == m_ShapeModuleDesc.strShape)
	{
		vDirection = _float3(0.f, 0.f, 1.f);
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
		if (true == m_ShapeModuleDesc.isLengthRange)
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
		vDirection = XMVector3TransformCoord(vDirection, _float4x4::MatrixRotationX(fTheta) * _float4x4::MatrixRotationY(fPhi));
		vPosition = fLength * vDirection;
	}
	else if ("Circle" == m_ShapeModuleDesc.strShape)
	{
		vDirection = _float3(0.f, 0.f, 1.f);
		_float fLength = 0.f;
		_float fTheta = 0.f;

		_float fMinLength = 0.f;
		_float fMaxLength = 0.f;
		_float fMinTheta = 0.f;
		_float fMaxTheta = 360.f;

		// ���� ����
		if (true == m_ShapeModuleDesc.isLengthRange)
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

		// ���� ����
		fTheta = XMConvertToRadians(fTheta);

		// ī�޶� ���� ������ ���
		if (true == m_ShapeModuleDesc.isCameraAxis)
		{
			_float3 vCamPos = pGameInstance->Get_CamPosition()->xyz();
			_float4x4 RULMatrix = pGameInstance->RightUpLook_In_Vectors(vPosition, vCamPos);
			vDirection = XMVector3TransformCoord(RULMatrix.Right(), _float4x4::MatrixRotationAxis(RULMatrix.Look(), fTheta));
		}
		else // xz��� ���� ������ ���
		{
			vDirection = XMVector3TransformCoord(_float3(0.f, 0.f, 1.f), _float4x4::MatrixRotationY(fTheta));
		}

		// ��ġ ����
		vPosition = fLength * vDirection;
		//(*_particle_iter).WorldMatrix.Translation();
	}
	else if ("Cone" == m_ShapeModuleDesc.strShape)
	{
		Clamp(m_ShapeModuleDesc.fAngle, 0.f, 90.f);
		_float fTopRadius = m_ShapeModuleDesc.fBaseRadius + m_ShapeModuleDesc.fConeLength * tanf(XMConvertToRadians(m_ShapeModuleDesc.fAngle));
		Clamp(m_ShapeModuleDesc.fBaseRadius, 0.f, fTopRadius);
		_float fAngle = Random_Generator(0.f, m_ShapeModuleDesc.fAngle);
		fAngle = XMConvertToRadians(fAngle);

		_float fMinBaseRadius = m_ShapeModuleDesc.fBaseRadius * (1.f - m_ShapeModuleDesc.fRadiusThickness);
		_float fMaxBaseRadius = m_ShapeModuleDesc.fBaseRadius;
		_float2 vResult = pGameInstance->PolarToCartesian(Random_Generator(fMinBaseRadius, fMaxBaseRadius), Random_Generator(0.f, 360.f));
		vPosition.x = vResult.x;
		vPosition.z = vResult.y;

		_float fTheta = fAngle; // ���Դ��� �߽������ ����
		_float fPhi = Random_Generator(0.f, 2 * XM_PI); // ���Դ��� ���̽� ��鿡���� ����

		vDirection = pGameInstance->PolarToCartesian(1.f, fTheta, fPhi);
	}
	else if ("Edge" == m_ShapeModuleDesc.strShape)
	{
		_float3 vRightPos = m_ShapeModuleDesc.vLength * _float3(1.f, 0.f, 0.f) * 0.5f;
		_float3 vLeftPos = vRightPos * -1.f;
		
		if ("Random" == m_ShapeModuleDesc.strPhiMode)
		{
			if (m_ShapeModuleDesc.fPhiSpread <= 0.01f) // Spread�� 0.f�� ���
			{
				vPosition.x = Random_Generator(vRightPos.x, vLeftPos.x);
				vPosition.y = Random_Generator(vRightPos.y, vLeftPos.y);
				vPosition.z = Random_Generator(vRightPos.z, vLeftPos.z);
			}
			//else // Spread�� 0.f���� Ŭ ���
			//{
			//	_float3 vLength = m_ShapeModuleDesc.fPhiSpread * _float3(-1.f, 0.f, 0.f);
			//	vRightPos += 
			// 	
			// 	
			//}
		}
		
		vDirection = _float3(0.f, 0.f, 1.f);
	}
	vPosition = XMVector3TransformCoord(vPosition, m_ShapeModuleDesc.ShapeMatrix);
	vDirection = XMVector3TransformNormal(vDirection, m_ShapeModuleDesc.ShapeMatrix);
	vDirection.Normalize();
	if (m_ShapeModuleDesc.isChase == false)
	{
		vPosition += m_pTransform->Get_Position();
	}
	// ��ġ ����
	_particle_iter->WorldMatrix.Translation(vPosition);

	// �ӵ�(���� * ���ǵ�) ����
	_particle_iter->vVelocity = (vDirection * fSpeed).TransNorm();

	Safe_Release(pGameInstance);
}

HRESULT CParticleSystem::Add_Components()
{
	FAILED_CHECK_RETURN(CComposite::Add_Component(0, TEXT("Prototype_Component_Renderer")
		, TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel
		, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_RendererModuleDesc.wstrMaterialPath.c_str()).c_str()
		, TEXT("Com_MainTexture")
		, reinterpret_cast<CComponent**>(&m_pMainTexture)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel
		, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_ShapeModuleDesc.wstrClipTexturePath.c_str()).c_str()
		, TEXT("Com_ClipTexture")
		, reinterpret_cast<CComponent**>(&m_pClipTexture)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel
		, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_TextureSheetAnimationModuleDesc.wstrNormalPath.c_str()).c_str()
		, TEXT("Com_NormalTexture")
		, reinterpret_cast<CComponent**>(&m_pNormalTexture)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel
		, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_RendererModuleDesc.wstrGraientTexture.c_str()).c_str()
		, TEXT("Com_GradientTexture")
		, reinterpret_cast<CComponent**>(&m_pGradientTexture)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel
		, ToPrototypeTag(TEXT("Prototype_Component_Texture"), TEXT("../../Resources/Effets/Textures/Noises/VFX_T_Noise04_D.png")).c_str()
		, TEXT("Com_NoiseTexture")
		, reinterpret_cast<CComponent**>(&m_pNoiseTexture)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Shader_VtxRectColIdxInstance")
		, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_VIBuffer_Rect_Particle_Instance")
		, TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer), &m_MainModuleDesc.iMaxParticles), E_FAIL);

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
void CParticleSystem::Reset_Particle(PARTICLE_IT& _particle_iter)
{
	_particle_iter->isAlive = true;
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

	// ����ȸ�� ------------- �Ⱦ�
	if (false == m_MainModuleDesc.is3DStartAngle)
	{
		if (true == m_MainModuleDesc.isStartAngleRange)
			_particle_iter->fAngle = Random_Generator(m_MainModuleDesc.vStartAngleRange.x, m_MainModuleDesc.vStartAngleRange.y);
		else
			_particle_iter->fAngle = m_MainModuleDesc.fStartAngle;
	}
	else
		_particle_iter->fAngle = m_MainModuleDesc.fStartAngle;

	// ���ӵ� ����
	m_RotationOverLifetimeModuleDesc.Reset(_particle_iter);

	// ȸ�� �� �ݴ�������� �������� ����.
	if (true == RandomBool(m_MainModuleDesc.fFlipAngle))
	{
		_particle_iter->fAngle *= -1.f;
		_particle_iter->fAngularVelocity *= -1.f;
	}


	// ���� ũ�� ����
	if (true == m_MainModuleDesc.is3DStartSize)
	{
		_particle_iter->vStartScale = _float3(m_MainModuleDesc.v3DSizeXYZ.x, m_MainModuleDesc.v3DSizeXYZ.y, m_MainModuleDesc.v3DSizeXYZ.z);
		_particle_iter->vScale = _particle_iter->vStartScale;
	}
	else
	{
		_float fScale;

		if (true == m_MainModuleDesc.isStartSizeRange)
			fScale = Random_Generator(m_MainModuleDesc.vStartSizeRange.x, m_MainModuleDesc.vStartSizeRange.y);
		else
			fScale = m_MainModuleDesc.fStartSize;

		_particle_iter->vStartScale = _float3(fScale, fScale, fScale);
		_particle_iter->vScale = _particle_iter->vStartScale;
	}
	// ���� ȸ��
	//_particle_iter->WorldMatrix = 

	// ���� ��ġ
	ResetStartPosition(_particle_iter);

	// ���� ����
	if (false == m_MainModuleDesc.isStartColorRange)
	{
		_particle_iter->vColor = m_MainModuleDesc.vStartColor;
	}
	else
	{
		if (RandomBool(0.5f))
		{
			_particle_iter->vColor = m_MainModuleDesc.vStartColor;
			_particle_iter->vStartColor = m_MainModuleDesc.vStartColor;
		}
		else
		{
			_particle_iter->vColor = m_MainModuleDesc.vStartColor2;
			_particle_iter->vStartColor = m_MainModuleDesc.vStartColor2;
		}
	}
	//_particle_iter->fAngle = m_MainModuleDesc.f

	_particle_iter->fGravityAccel = { 0.f };

	m_TextureSheetAnimationModuleDesc.Reset(_particle_iter);
	m_VelocityOverLifeTimeModuleDesc.Reset(_particle_iter, m_pTransform->Get_WorldMatrixPtr());

	for (auto& Event : _particle_iter->Events)
		Event.Reset();
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
PARTICLE_IT CParticleSystem::Wating_One_Particle()
{
	if (true == m_isStop)
		return m_Particles[WAIT].end();

	if (m_Particles[WAIT].empty())
	{
		return m_Particles[WAIT].end();
	}

	if (m_Particles[ALIVE].size() >= m_MainModuleDesc.iMaxParticles)
	{
		return m_Particles[WAIT].end();
	}

	PARTICLE_IT particle_iter = m_Particles[WAIT].begin();
	Reset_Particle(particle_iter);
	::TransitionTo(particle_iter, m_Particles[WAIT], m_Particles[DELAY]);
	return particle_iter;
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
	// ��� �׷��� WAIT���� �̵�
	for (auto iter = m_Particles[DEAD].begin(); iter != m_Particles[DEAD].end();)
		iter = TransitionTo(iter, m_Particles[DEAD], m_Particles[WAIT]);
	for (auto iter = m_Particles[ALIVE].begin(); iter != m_Particles[ALIVE].end();)
		iter = TransitionTo(iter, m_Particles[ALIVE], m_Particles[WAIT]);
	for (auto iter = m_Particles[DELAY].begin(); iter != m_Particles[DELAY].end();)
		iter = TransitionTo(iter, m_Particles[DELAY], m_Particles[WAIT]);

	// ��ƼŬ ����
	for (_uint i = 0; i < STATE_END; ++i)
	{
		for (auto& Particle : m_Particles[i])
		{
			Particle.Restart();
		}
	}

	m_MainModuleDesc.Restart();
	m_EmissionModuleDesc.Restart();
	m_ShapeModuleDesc.Restart();
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

	if (true == m_isCloned)
	{
		Safe_Release(m_pRenderer);
		Safe_Release(m_pMainTexture);
		Safe_Release(m_pClipTexture);
		Safe_Release(m_pNormalTexture);
		Safe_Release(m_pGradientTexture);
		Safe_Release(m_pNoiseTexture);
		Safe_Release(m_pBuffer);
		Safe_Release(m_pShader);
		Safe_Release(m_pModel);
	}

	for (_uint i = 0; i < STATE_END; ++i)
	{
		m_Particles[i].clear();
	}
}