#include "..\Public\MeshEffect.h"
#include "Model.h"
#include "Texture.h"
#include "Shader.h"
#include "GameInstance.h"

CMeshEffect::CMeshEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
	m_Path[TEXTURE_PATH] = TEXT("../../Resources/Models/NonAnims/SM_SpherePrimitiveRegularNormals_01/T_Default_Material_Grid_M.png");
	m_Path[ALPHA_CLIP_TEXTURE_PATH] = TEXT("../../Resources/Effects/Textures/Gradients/Default_Gradient.png");
	m_Path[MODEL_PATH] = TEXT("../../Resources/Models/NonAnims/SM_SpherePrimitiveRegularNormals_01/SM_SpherePrimitiveRegularNormals_01.dat");
	m_Path[EMISSION_PATH] = TEXT("../../Resources/Effects/Textures/Default_Normal.png");
}

CMeshEffect::CMeshEffect(const CMeshEffect& _rhs)
	: CGameObject(_rhs)
	, m_iLevel(_rhs.m_iLevel)
	, m_isEnble(_rhs.m_isEnble)
	, m_vStartOffset(_rhs.m_vStartOffset)
	, m_vOffset(_rhs.m_vOffset)
	, m_vDeltaOffset(_rhs.m_vDeltaOffset)
	, m_vStartTiling(_rhs.m_vStartTiling)
	, m_vTililing(_rhs.m_vTililing)
	, m_vDeltaTiling(_rhs.m_vDeltaTiling)
	, m_vStartColor(_rhs.m_vStartColor)
	, m_vEndColor(_rhs.m_vEndColor)
	, m_vColor(_rhs.m_vColor)
	, m_eColorEase(_rhs.m_eColorEase)
	, m_vStartSize(_rhs.m_vStartSize)
	, m_vEndSize(_rhs.m_vEndSize)
	, m_vSize(_rhs.m_vSize)
	, m_eSizeEase(_rhs.m_eSizeEase)
	, m_vStartRot(_rhs.m_vStartRot)
	, m_vEndRot(_rhs.m_vEndRot)
	, m_vRot(_rhs.m_vRot)
	, m_eRotEase(_rhs.m_eRotEase)
	, m_vStartPos(_rhs.m_vStartPos)
	, m_vEndPos(_rhs.m_vEndPos)
	, m_vPos(_rhs.m_vPos)
	, m_ePosEase(_rhs.m_ePosEase)
	, m_TransfomationMatrix(_rhs.m_TransfomationMatrix)
	, m_fLifeTime(_rhs.m_fLifeTime)
	, m_fAge(_rhs.m_fAge)
	, m_isAlphaBlend(_rhs.m_isAlphaBlend)
	, m_strClipChannel(_rhs.m_strClipChannel)
	, m_fClipThreshold(_rhs.m_fClipThreshold)
	, m_isGlow(_rhs.m_isGlow)
	, m_isDistortion(_rhs.m_isDistortion)
	, m_isDiffuse(_rhs.m_isDiffuse)
	, m_isClipTexture(_rhs.m_isClipTexture)
	, m_isJustActionStop(_rhs.m_isJustActionStop)
	, m_isFlutter(_rhs.m_isFlutter)
	, m_vStrength(_rhs.m_vStrength)
	, m_eAnimType(_rhs.m_eAnimType)
	, m_strPassName(_rhs.m_strPassName)
	, m_strCurAnim(_rhs.m_strCurAnim)
	, m_PivotMatrix(_rhs.m_PivotMatrix)
	, m_isEmission(_rhs.m_isEmission)
	, m_fFrequency(_rhs.m_fFrequency)
	, m_vRemap(_rhs.m_vRemap)
	, m_vEmissionColor(_rhs.m_vEmissionColor)
	, m_strEmissionChannel(_rhs.m_strEmissionChannel)
	
{
	for (_uint i = 0; i < PATH_END; ++i)
		m_Path[i] = _rhs.m_Path[i];
	
	m_pTransform->Set_WorldMatrix(_rhs.m_pTransform->Get_WorldMatrix());
}

CMeshEffect::~CMeshEffect()
{

}

HRESULT CMeshEffect::Initialize_Prototype(const _tchar* pFilePath, _uint _iLevel, _float4x4 PivotMatrix)
{
	__super::Initialize_Prototype();
	// 여기서 m_Path로드해주기.

	if (FAILED(this->Load(pFilePath)))
	{
		if (0 != lstrcmp(pFilePath, TEXT("")))
		{
			MSG_BOX("Failed to Load MeshEffect");
			return E_FAIL;
		}
	}

	m_PivotMatrix = PivotMatrix;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 컴포넌트 조사 후 추가.
	wstring ProtoTag;
	m_iLevel = _iLevel;

	// 필요한 텍스처 조사하고 원본 추가하는 로직
	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_Path[TEXTURE_PATH].c_str());
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_Path[TEXTURE_PATH].c_str()));
	}

	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_Path[EMISSION_PATH].c_str());
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_Path[EMISSION_PATH].c_str()));
	}

	// 필요한 텍스처 조사하고 원본 추가하는 로직
	//ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_Path[ALPHA_CLIP_TEXTURE_PATH].c_str());
	//if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	//{
	//	pGameInstance->Add_Prototype(m_iLevel
	//		, ProtoTag.data()
	//		, CTexture::Create(m_pDevice, m_pContext, m_Path[ALPHA_CLIP_TEXTURE_PATH].c_str()));
	//}

	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Model"), m_Path[MODEL_PATH].c_str());

	// 필요한 모델 조사이 레벨에 있는지 조사.
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		// 있으면 
		fs::path fsFilePath = m_Path[MODEL_PATH];
		string strPath = fsFilePath.parent_path().parent_path().stem().string();
		if ("NonAnims" == strPath)
			m_eAnimType = CModel::TYPE_NONANIM;
		else
			m_eAnimType = CModel::TYPE_ANIM;

		pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CModel::Create(m_pDevice, m_pContext, m_eAnimType, m_Path[MODEL_PATH].c_str(), m_PivotMatrix));
	}


	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_Shader_DefaultEffect")))
	{
		pGameInstance->Add_Prototype(m_iLevel
			, TEXT("Prototype_Component_Shader_DefaultEffect")
			, CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DefaultEffect.hlsl")
				, VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements));
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CMeshEffect::Initialize(void* _pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;
	
	return S_OK;
}

void CMeshEffect::Tick(_float _fTimeDelta)
{
	if (false == m_isEnble)
		return;
	m_fAge += _fTimeDelta;

	if (nullptr != m_pModel && CModel::TYPE_ANIM == m_eAnimType)
		m_pModel->Play_Animation(_fTimeDelta);
	m_vOffset.x += m_vDeltaOffset.x * _fTimeDelta;
	m_vOffset.y += m_vDeltaOffset.y * _fTimeDelta;
	m_vTililing.x += m_vDeltaTiling.x * _fTimeDelta;
	m_vTililing.y += m_vDeltaTiling.y * _fTimeDelta;

	_float fRatio = m_fAge / m_fLifeTime;

	//m_vColor = _float4::Lerp(m_vStartColor, m_vEndColor, fRatio);
	//m_vSize = _float3::Lerp(m_vStartSize, m_vEndSize, fRatio);
	//m_vRot = _float3::Lerp(m_vStartRot, m_vEndRot, fRatio);
	if (!m_isJustActionStop)
	{
		{ // Color
			_float fChangeAmount;
			fChangeAmount = m_vEndColor.x - m_vStartColor.x; // R
			m_vColor.x = CEase::Ease(m_eColorEase, m_fAge, m_vStartColor.x, fChangeAmount, m_fLifeTime);
			fChangeAmount = m_vEndColor.y - m_vStartColor.y; // G
			m_vColor.y = CEase::Ease(m_eColorEase, m_fAge, m_vStartColor.y, fChangeAmount, m_fLifeTime);
			fChangeAmount = m_vEndColor.z - m_vStartColor.z; // B
			m_vColor.z = CEase::Ease(m_eColorEase, m_fAge, m_vStartColor.z, fChangeAmount, m_fLifeTime);
			fChangeAmount = m_vEndColor.w - m_vStartColor.w; // A
			m_vColor.w = CEase::Ease(m_eColorEase, m_fAge, m_vStartColor.w, fChangeAmount, m_fLifeTime);
		}
		{ // Size
			_float fChangeAmount;
			fChangeAmount = m_vEndSize.x - m_vStartSize.x; // X
			m_vSize.x = CEase::Ease(m_eSizeEase, m_fAge, m_vStartSize.x, fChangeAmount, m_fLifeTime);
			fChangeAmount = m_vEndSize.y - m_vStartSize.y; // Y
			m_vSize.y = CEase::Ease(m_eSizeEase, m_fAge, m_vStartSize.y, fChangeAmount, m_fLifeTime);
			fChangeAmount = m_vEndSize.z - m_vStartSize.z; // Z
			m_vSize.z = CEase::Ease(m_eSizeEase, m_fAge, m_vStartSize.z, fChangeAmount, m_fLifeTime);
		}
		{ // Rot
			_float fChangeAmount;
			fChangeAmount = m_vEndRot.x - m_vStartRot.x; // X
			m_vRot.x = CEase::Ease(m_eRotEase, m_fAge, m_vStartRot.x, fChangeAmount, m_fLifeTime);
			fChangeAmount = m_vEndRot.y - m_vStartRot.y; // Y
			m_vRot.y = CEase::Ease(m_eRotEase, m_fAge, m_vStartRot.y, fChangeAmount, m_fLifeTime);
			fChangeAmount = m_vEndRot.z - m_vStartRot.z; // Z
			m_vRot.z = CEase::Ease(m_eRotEase, m_fAge, m_vStartRot.z, fChangeAmount, m_fLifeTime);
		}
		{ // Pos
			_float fChangeAmount;
			fChangeAmount = m_vEndPos.x - m_vStartPos.x; // X
			m_vPos.x = CEase::Ease(m_ePosEase, m_fAge, m_vStartPos.x, fChangeAmount, m_fLifeTime);
			fChangeAmount = m_vEndPos.y - m_vStartPos.y; // Y
			m_vPos.y = CEase::Ease(m_ePosEase, m_fAge, m_vStartPos.y, fChangeAmount, m_fLifeTime);
			fChangeAmount = m_vEndPos.z - m_vStartPos.z; // Z
			m_vPos.z = CEase::Ease(m_ePosEase, m_fAge, m_vStartPos.z, fChangeAmount, m_fLifeTime);
		}
	}

	_float4x4 ScaleMatrix = _float4x4::MatrixScale(m_vSize);
	_float4x4 RotMatrix = _float4x4::MatrixFromQuaternion(XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(m_vRot.x),
		XMConvertToRadians(m_vRot.y),
		XMConvertToRadians(m_vRot.z)));
	_float4x4 TranslationMatrix = _float4x4::MatrixTranslation(m_vPos);
	m_TransfomationMatrix = ScaleMatrix * RotMatrix * TranslationMatrix;

	if (m_fAge >= m_fLifeTime)
		m_isEnble = false;
}

void CMeshEffect::Late_Tick(_float _fTimeDelta)
{
	if (false == m_isEnble)
		return;

	if (nullptr != m_pRenderer)
	{
		if (m_isDiffuse)
		{
			if (true == m_isAlphaBlend)
				m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
			else
				m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		}

		if (m_isGlow)
		{
			m_pRenderer->Add_RenderGroup(CRenderer::RENDER_GLOW, this);
		}

		if (m_isDistortion)
		{
			m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);
		}
	}
}

HRESULT CMeshEffect::Render()
{
	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(m_strPassName.c_str())))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CMeshEffect::Play(_float3 vPos)
{
	SetJustActionStop(false);
	m_pTransform->Set_Position(vPos);
	m_isEnble = true;
	Reset();
}

void CMeshEffect::Stop()
{
	m_isEnble = false;
}

void CMeshEffect::Reset()
{
	m_vOffset = m_vStartOffset;
	m_vTililing = m_vStartTiling;
	m_fAge = 0.f;
	m_vSize = m_vStartSize;
	m_vRot = m_vStartRot;
	m_vPos = m_vStartPos;
}

void CMeshEffect::Set_PassName(string strPassName)
{
	m_strPassName = strPassName;
}

void CMeshEffect::Set_Path(wstring wstrPath, PATH ePath)
{
	Clamp(ePath, TEXTURE_PATH, static_cast<PATH>(PATH_END - 1));
	m_Path[ePath] = wstrPath;
}

_bool CMeshEffect::IsEnable()
{
	return m_isEnble;
}

HRESULT CMeshEffect::Add_Components()
{
	/* For.Com_Renderer */
	FAILED_CHECK_RETURN(CComposite::Add_Component(0, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer), E_FAIL);

	/* For.Com_Model */
	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Model"), m_Path[MODEL_PATH].c_str()).c_str()
		, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)), E_FAIL);

	/* For.Com_MainTexture */
	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_Path[TEXTURE_PATH].c_str()).c_str()
		, TEXT("Com_MainTexture"), reinterpret_cast<CComponent**>(&m_pTexture)), E_FAIL);

	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_Path[EMISSION_PATH].c_str()).c_str()
		, TEXT("Com_EmissionTexture"), reinterpret_cast<CComponent**>(&m_pEmissionTexture)), E_FAIL);

	/* For.Com_AlphaClipTexture */
	//FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_Path[TEXTURE_PATH].c_str()).c_str()
	//	, TEXT("Com_ClipTexture"), reinterpret_cast<CComponent**>(&m_pClipTexture)), E_FAIL);

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Shader_DefaultEffect")
		, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader)), E_FAIL);

	return S_OK;
}

HRESULT CMeshEffect::Setup_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->Bind_Matrix("g_TransformationMatrix", &m_TransfomationMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPos", pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vOffset", &m_vOffset, sizeof m_vOffset)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vTililing", &m_vTililing, sizeof m_vTililing)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	_float fTimeAcc = pGameInstance->Get_World_TimeAcc();
	if (FAILED(m_pShader->Bind_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(fTimeAcc))))
		return E_FAIL;

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_MaterialTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_isEmission", &m_isEmission, sizeof(m_isEmission))))
		return E_FAIL;

	if (m_isEmission)
	{
		if (FAILED(m_pEmissionTexture->Bind_ShaderResource(m_pShader, "g_EmissionTexture")))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fFrequency", &m_fFrequency, sizeof(m_fFrequency))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vRemap", &m_vRemap, sizeof(m_vRemap))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vEmissionColor", &m_vEmissionColor, sizeof(m_vEmissionColor))))
			return E_FAIL;

		_int iEmissionChannel = { 3 };
		if (m_strEmissionChannel == "Red") { iEmissionChannel = 0; }
		else if (m_strEmissionChannel == "Green") { iEmissionChannel = 1; }
		else if (m_strEmissionChannel == "Blue") { iEmissionChannel = 2; }
		else if (m_strEmissionChannel == "Alpha") { iEmissionChannel = 3; }
		if (FAILED(m_pShader->Bind_RawValue("g_iEmissionChannel", &iEmissionChannel, sizeof(_int))))
			return E_FAIL;
	}

	_int iClipChannel = { 3 };
	if (m_strClipChannel == "Red") { iClipChannel = 0; }
	else if (m_strClipChannel == "Green") { iClipChannel = 1; }
	else if (m_strClipChannel == "Blue") { iClipChannel = 2; }
	else if (m_strClipChannel == "Alpha") { iClipChannel = 3; }

	if (FAILED(m_pShader->Bind_RawValue("g_iClipChannel", &iClipChannel, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fClipThreshold", &m_fClipThreshold, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_isFlutter", &m_isFlutter, sizeof(m_isFlutter))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vStrength", &m_vStrength, sizeof(m_vStrength))))
		return E_FAIL;

	return S_OK;
}
HRESULT CMeshEffect::Save(const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	WriteFile(hFile, &m_isEnble, sizeof(m_isEnble), &dwByte, nullptr);
	WriteFile(hFile, &m_vOffset, sizeof(m_vOffset), &dwByte, nullptr);
	WriteFile(hFile, &m_vDeltaOffset, sizeof(m_vDeltaOffset), &dwByte, nullptr);
	WriteFile(hFile, &m_vTililing, sizeof(m_vTililing), &dwByte, nullptr);
	WriteFile(hFile, &m_vDeltaTiling, sizeof(m_vDeltaTiling), &dwByte, nullptr);
	WriteFile(hFile, &m_vStartColor, sizeof(m_vStartColor), &dwByte, nullptr);
	WriteFile(hFile, &m_vEndColor, sizeof(m_vEndColor), &dwByte, nullptr);
	WriteFile(hFile, &m_eColorEase, sizeof(m_eColorEase), &dwByte, nullptr);
	WriteFile(hFile, &m_vStartSize, sizeof(m_vStartSize), &dwByte, nullptr);
	WriteFile(hFile, &m_vEndSize, sizeof(m_vEndSize), &dwByte, nullptr);
	WriteFile(hFile, &m_eSizeEase, sizeof(m_eSizeEase), &dwByte, nullptr);
	WriteFile(hFile, &m_vStartRot, sizeof(m_vStartRot), &dwByte, nullptr);
	WriteFile(hFile, &m_vEndRot, sizeof(m_vEndRot), &dwByte, nullptr);
	WriteFile(hFile, &m_eRotEase, sizeof(m_eRotEase), &dwByte, nullptr);
	WriteFile(hFile, &m_vStartPos, sizeof(m_vStartPos), &dwByte, nullptr);
	WriteFile(hFile, &m_vEndPos, sizeof(m_vEndPos), &dwByte, nullptr);
	WriteFile(hFile, &m_ePosEase, sizeof(m_ePosEase), &dwByte, nullptr);
	WriteFile(hFile, &m_fLifeTime, sizeof(m_fLifeTime), &dwByte, nullptr);
	WriteFile(hFile, &m_eAnimType, sizeof(m_eAnimType), &dwByte, nullptr);
	WriteFile(hFile, m_strPassName.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	for (_uint i = 0; i < 3; ++i)
	{
		WriteFile(hFile, m_Path[i].data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	}

	WriteFile(hFile, &m_isClipTexture, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_PivotMatrix, sizeof(_float4x4), &dwByte, nullptr);
	WriteFile(hFile, m_strCurAnim.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	m_pTransform->Write_File(hFile, &dwByte);
	WriteFile(hFile, &m_vStartOffset, sizeof(m_vStartOffset), &dwByte, nullptr);
	WriteFile(hFile, &m_vStartTiling, sizeof(m_vStartTiling), &dwByte, nullptr);
	WriteFile(hFile, &m_isAlphaBlend, sizeof(m_isAlphaBlend), &dwByte, nullptr);
	WriteFile(hFile, m_strClipChannel.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_fClipThreshold, sizeof(m_fClipThreshold), &dwByte, nullptr);
	WriteFile(hFile, &m_isGlow, sizeof(m_isGlow), &dwByte, nullptr);
	WriteFile(hFile, &m_isDistortion, sizeof(m_isDistortion), &dwByte, nullptr);
	WriteFile(hFile, &m_isDiffuse, sizeof(m_isDiffuse), &dwByte, nullptr);
	WriteFile(hFile, &m_isFlutter, sizeof(m_isFlutter), &dwByte, nullptr);
	WriteFile(hFile, &m_vStrength, sizeof(m_vStrength), &dwByte, nullptr);
	WriteFile(hFile, m_Path[EMISSION_PATH].data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_isEmission, sizeof(m_isEmission), &dwByte, nullptr);
	WriteFile(hFile, &m_fFrequency, sizeof(m_fFrequency), &dwByte, nullptr);
	WriteFile(hFile, &m_vRemap, sizeof(m_vRemap), &dwByte, nullptr);
	WriteFile(hFile, &m_vEmissionColor, sizeof(m_vEmissionColor), &dwByte, nullptr);
	WriteFile(hFile, m_strEmissionChannel.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}
HRESULT CMeshEffect::Load(const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;
	_char szBuffer[MAX_PATH];
	_tchar wszBuffer[MAX_PATH];

	ReadFile(hFile, &m_isEnble, sizeof(m_isEnble), &dwByte, nullptr);
	ReadFile(hFile, &m_vOffset, sizeof(m_vOffset), &dwByte, nullptr);
	ReadFile(hFile, &m_vDeltaOffset, sizeof(m_vDeltaOffset), &dwByte, nullptr);
	ReadFile(hFile, &m_vTililing, sizeof(m_vTililing), &dwByte, nullptr);
	ReadFile(hFile, &m_vDeltaTiling, sizeof(m_vDeltaTiling), &dwByte, nullptr);
	ReadFile(hFile, &m_vStartColor, sizeof(m_vStartColor), &dwByte, nullptr);
	ReadFile(hFile, &m_vEndColor, sizeof(m_vEndColor), &dwByte, nullptr);
	ReadFile(hFile, &m_eColorEase, sizeof(m_eColorEase), &dwByte, nullptr);
	ReadFile(hFile, &m_vStartSize, sizeof(m_vStartSize), &dwByte, nullptr);
	ReadFile(hFile, &m_vEndSize, sizeof(m_vEndSize), &dwByte, nullptr);
	ReadFile(hFile, &m_eSizeEase, sizeof(m_eSizeEase), &dwByte, nullptr);
	ReadFile(hFile, &m_vStartRot, sizeof(m_vStartRot), &dwByte, nullptr);
	ReadFile(hFile, &m_vEndRot, sizeof(m_vEndRot), &dwByte, nullptr);
	ReadFile(hFile, &m_eRotEase, sizeof(m_eRotEase), &dwByte, nullptr);
	ReadFile(hFile, &m_vStartPos, sizeof(m_vStartPos), &dwByte, nullptr);
	ReadFile(hFile, &m_vEndPos, sizeof(m_vEndPos), &dwByte, nullptr);
	ReadFile(hFile, &m_ePosEase, sizeof(m_ePosEase), &dwByte, nullptr);
	ReadFile(hFile, &m_fLifeTime, sizeof(m_fLifeTime), &dwByte, nullptr);
	ReadFile(hFile, &m_eAnimType, sizeof(m_eAnimType), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	m_strPassName = szBuffer;
	for (_uint i = 0; i < 3; ++i)
	{
		ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		m_Path[i] = wszBuffer;
	}
	ReadFile(hFile, &m_isClipTexture, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &m_PivotMatrix, sizeof(_float4x4), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	m_strCurAnim = szBuffer;
	m_pTransform->Read_File(hFile, &dwByte);
	ReadFile(hFile, &m_vStartOffset, sizeof(m_vStartOffset), &dwByte, nullptr);
	ReadFile(hFile, &m_vStartTiling, sizeof(m_vStartTiling), &dwByte, nullptr);
	ReadFile(hFile, &m_isAlphaBlend, sizeof(m_isAlphaBlend), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	m_strClipChannel = szBuffer;
	ReadFile(hFile, &m_fClipThreshold, sizeof(m_fClipThreshold), &dwByte, nullptr);
	ReadFile(hFile, &m_isGlow, sizeof(m_isGlow), &dwByte, nullptr);
	ReadFile(hFile, &m_isDistortion, sizeof(m_isDistortion), &dwByte, nullptr);
	ReadFile(hFile, &m_isDiffuse, sizeof(m_isDiffuse), &dwByte, nullptr);
	ReadFile(hFile, &m_isFlutter, sizeof(m_isFlutter), &dwByte, nullptr);
	ReadFile(hFile, &m_vStrength, sizeof(m_vStrength), &dwByte, nullptr);
	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	if (0 == dwByte)
	{
		CloseHandle(hFile);
		return S_OK;
	}

	m_Path[EMISSION_PATH] = wszBuffer;
	ReadFile(hFile, &m_isEmission, sizeof(m_isEmission), &dwByte, nullptr);
	ReadFile(hFile, &m_fFrequency, sizeof(m_fFrequency), &dwByte, nullptr);
	ReadFile(hFile, &m_vRemap, sizeof(m_vRemap), &dwByte, nullptr);
	ReadFile(hFile, &m_vEmissionColor, sizeof(m_vEmissionColor), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	m_strEmissionChannel = szBuffer;
	CloseHandle(hFile);

	return S_OK;
}
CMeshEffect* CMeshEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* pFilePath, _uint _iLevel, _float4x4 PivotMatrix)
{
	CMeshEffect* pInstance = New CMeshEffect(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath, _iLevel, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CMeshEffect::Clone(void* _pArg)
{
	CMeshEffect* pInstance = New CMeshEffect(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned CMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CMeshEffect::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pTexture);
		Safe_Release(m_pClipTexture);
		Safe_Release(m_pEmissionTexture);
		Safe_Release(m_pModel);
		Safe_Release(m_pShader);
		Safe_Release(m_pRenderer);
	}
}
