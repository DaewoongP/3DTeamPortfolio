#include "House_Elf.h"
#include "GameInstance.h"

CHouse_Elf::CHouse_Elf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CHouse_Elf::CHouse_Elf(const CHouse_Elf& rhs)
	: CGameObject(rhs)
{
}

HRESULT CHouse_Elf::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHouse_Elf::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	ELFINITDESC* pDesc = reinterpret_cast<ELFINITDESC*>(pArg);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_WorldMatrix(pDesc->WorldMatrix);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	m_pModelCom->Change_Animation(pDesc->wstrAnimationTag);
	if (wstring::npos != pDesc->wstrAnimationTag.find(TEXT("Walk")))
		m_isWalk = true;

	return S_OK;
}

void CHouse_Elf::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (true == m_isWalk)
		Walk(fTimeDelta);

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CHouse_Elf::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CHouse_Elf::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Bind_Material Diffuse");

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, NORMALS)))
				throw TEXT("Bind_Material Normal");

			if (6 == i)
			{
				if (FAILED(m_pShaderCom->Begin("AnimMeshColor")))
					throw TEXT("Shader Begin AnimMeshColor");
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin("AnimMesh")))
					throw TEXT("Shader Begin AnimMesh");
			}

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CHouse_Elf] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CHouse_Elf::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShadowShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pShadowShaderCom->Begin("Shadow")))
				throw TEXT("Shader Begin AnimMesh");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CHouse_Elf] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CHouse_Elf::Add_Components()
{
	try /* Check Add_Components */
	{
		/* Com_Renderer */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
			TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw TEXT("Com_Renderer");

		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(LEVEL_SMITH, TEXT("Prototype_Component_Model_Doby"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");

		/* For.Com_Shader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			throw TEXT("Com_Shader");

		/* For.Com_ShadowShader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
			TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShaderCom))))
			throw TEXT("Com_ShadowShader");

		/* Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.isGravity = true;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vInitRotation = m_pTransform->Get_Quaternion();
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxCapsuleGeometry MyGeometry = PxCapsuleGeometry(0.5f, 0.5f);
		RigidBodyDesc.pGeometry = &MyGeometry;
		RigidBodyDesc.vOffsetPosition = _float3(0.f, MyGeometry.halfHeight + MyGeometry.radius, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
		RigidBodyDesc.vDebugColor = _float4(1.f, 105 / 255.f, 180 / 255.f, 1.f); // hot pink
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.eThisCollsion = COL_NPC;
		RigidBodyDesc.eCollisionFlag = COL_STATIC | COL_PLAYER;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Body");

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
		{
			__debugbreak();
			return E_FAIL;
		}
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CHouse_Elf] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHouse_Elf::SetUp_ShaderResources()
{
	BEGININSTANCE;

	try /* Check SetUp_ShaderResources */
	{
		if (nullptr == m_pShaderCom)
			throw TEXT("m_pShaderCom is nullptr");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			throw TEXT("Failed Bind_Matrix : g_WorldMatrix");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			throw TEXT("Failed Bind_Matrix : g_ViewMatrix");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			throw TEXT("Failed Bind_Matrix : g_ProjMatrix");

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : g_fCamFar");

		_float3 vHairColor = { 0.7f, 0.7f, 0.7f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vHairColor", &vHairColor, sizeof(_float3))))
			throw TEXT("Failed Bind_RawValue : g_vHairColor");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CHouse_Elf] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CHouse_Elf::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	BEGININSTANCE;

	try /* Check SetUp_ShaderResources */
	{
		if (nullptr == m_pShadowShaderCom)
			throw TEXT("m_pShaderCom is nullptr");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			throw TEXT("Failed Bind_Matrix : g_WorldMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
			throw TEXT("Failed Bind_Matrix : g_ViewMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
			throw TEXT("Failed Bind_Matrix : g_ProjMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : g_fCamFar");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CHouse_Elf] Failed SetUp_ShadowShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CHouse_Elf::Walk(const _float& fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;

	if (false == m_isTurn && 20.f < m_fTimeDelta)
	{
		m_pModelCom->Change_Animation(TEXT("Turn_Left_180"));
		m_isTurn = true;
	}
	else if (true == m_isTurn && true == m_pModelCom->Is_Finish_Animation())
	{
		m_pModelCom->Change_Animation(TEXT("Walk_1"));
		m_fTimeDelta = 0.f;
		m_isTurn = false;
	}
}

CHouse_Elf* CHouse_Elf::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHouse_Elf* pInstance = New CHouse_Elf(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHouse_Elf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHouse_Elf::Clone(void* pArg)
{
	CHouse_Elf* pInstance = New CHouse_Elf(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CHouse_Elf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHouse_Elf::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShadowShaderCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pRigidBody);
}
