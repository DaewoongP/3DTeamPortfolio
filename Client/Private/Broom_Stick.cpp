#include "Broom_Stick.h"
#include "GameInstance.h"
#include "Player.h"
#include"ParticleSystem.h"

CBroom_Stick::CBroom_Stick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CParts(pDevice, pContext)
{
}

CBroom_Stick::CBroom_Stick(const CBroom_Stick& rhs)
	: CParts(rhs)
{
}

HRESULT CBroom_Stick::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	
	return S_OK;
}

HRESULT CBroom_Stick::Initialize(void* pArg)
{
	CWEAPON_PLAYER_BROOM_DESC* pCweapon_Player_Broom_Desc = static_cast<CWEAPON_PLAYER_BROOM_DESC*>(pArg);
	
	if (FAILED(__super::Initialize(&pCweapon_Player_Broom_Desc->ParentMatrixDesc)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_pParticle->Play(m_OffsetMatrix.Translation());
	m_pParticle_Local->Play(m_OffsetMatrix.Translation());

 	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	m_OffsetMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)
		* XMMatrixRotationY(XMConvertToRadians(100)) * XMMatrixRotationZ(XMConvertToRadians(110))
		* XMMatrixTranslation(0.53f, 0.f, -0.99f);

	_float4x4 RotationMatrix = XMMatrixRotationQuaternion(
		XMQuaternionRotationAxis(XMVector3Normalize(_float3(1, 0, 0)), 1 * -10.f));

	_float3 vRight = m_OffsetMatrix.Right();
	_float3 vUp = m_OffsetMatrix.Up();
	_float3 vLook = m_OffsetMatrix.Look();

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	memcpy(&m_OffsetMatrix.m[0][0], &vRight, sizeof(_float3));
	memcpy(&m_OffsetMatrix.m[1][0], &vUp, sizeof(_float3));
	memcpy(&m_OffsetMatrix.m[2][0], &vLook, sizeof(_float3));
	
	m_pPlayerRigidBody = pCweapon_Player_Broom_Desc->pPlayerRigidBody;
	Safe_AddRef(m_pPlayerRigidBody);
	m_pParticle->Get_EmissionModuleRef().fRateOverTime = 0;
	m_pParticle_Local->Get_EmissionModuleRef().fRateOverTime = 0;

	return S_OK;
}

void CBroom_Stick::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBroom_Stick::Late_Tick(_float fTimeDelta)
{
	CGameObject::Late_Tick(fTimeDelta);

	_float4x4 CombinedWorldMatrix;
	// ��ü �����»�(������ * �� * �ǹ�) * �θ��� ����
	CombinedWorldMatrix = m_OffsetMatrix * m_ParentMatrixDesc.OffsetMatrix * *m_ParentMatrixDesc.pCombindTransformationMatrix * m_ParentMatrixDesc.PivotMatrix *
		  *m_ParentMatrixDesc.pParentWorldMatrix;
	m_pTransform->Set_WorldMatrix(CombinedWorldMatrix);
	
	_float4x4 TempMatrix = static_cast<CGameObject*>(m_pOwner)->Get_Transform()->Get_WorldMatrix();
	TempMatrix.m[3][0] = 0;
	TempMatrix.m[3][1] = 0;
	TempMatrix.m[3][2] = 0;
	m_pParticle->Get_ShapeModuleRef().ShapeMatrix =
		m_pParticle->Get_ShapeModuleRef().ShapeMatrixInit * TempMatrix;
	m_pParticle->Get_Transform()->Set_Position(m_pTransform->Get_Position() + m_pTransform->Get_Look()* + 3);
	m_pParticle->Get_EmissionModuleRef().fRateOverDistance = m_pPlayerRigidBody->Get_Current_Velocity().Length();

	m_pParticle_Local->Get_ShapeModuleRef().ShapeMatrix =
		m_pParticle_Local->Get_ShapeModuleRef().ShapeMatrixInit * TempMatrix;
	m_pParticle_Local->Get_Transform()->Set_Position(m_pTransform->Get_Position() + m_pTransform->Get_Look()* + 3);
	m_pParticle_Local->Get_EmissionModuleRef().fRateOverTime = m_pPlayerRigidBody->Get_Current_Velocity().Length();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBroom_Stick::Render()
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Failed Bind_Material : g_DiffuseTexture");

			if (FAILED(m_pShaderCom->Begin("Mesh")))
				throw TEXT("Failed Begin : Mesh");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Failed Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CBroom_Stick] Failed Render : \n");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
			__debugbreak();

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBroom_Stick::Add_Components(void* pArg)
{
	try /* Check Add_Components */
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Weapon_Player_Broom"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom, this)))
			throw TEXT("Failed Add_Component : Com_Model");

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
			(CComponent**)&m_pRendererCom, this)))
			throw TEXT("Failed Add_Component : Com_Renderer");

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader_Mesh"),
			(CComponent**)&m_pShaderCom, this)))
			throw TEXT("Failed Add_Component : Com_Shader_Mesh");

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Broom_Broom_Stick"),
			TEXT("Com_Broom_Broom_Stick"), reinterpret_cast<CComponent**>(&m_pParticle))))
		{
			MSG_BOX("Failed Add_GameObject : (Com_Broom_Broom_Stick)");
			__debugbreak();
			return E_FAIL;
		}

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Broom_Broom_Stick_Local"),
			TEXT("Com_Broom_Stick_Local"), reinterpret_cast<CComponent**>(&m_pParticle_Local))))
		{
			MSG_BOX("Failed Add_GameObject : (Com_Broom_Stick_Local)");
			__debugbreak();
			return E_FAIL;
		}
		
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CBroom_Stick] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBroom_Stick::Set_Shader_Resources()
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
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CBroom_Stick] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

CBroom_Stick* CBroom_Stick::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_Stick* pInstance = New CBroom_Stick(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBroom_Stick");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBroom_Stick* CBroom_Stick::Clone(void* pArg)
{
	CBroom_Stick* pInstance = New CBroom_Stick(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBroom_Stick");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBroom_Stick::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRendererCom);
		Safe_Release(m_pParticle);
		Safe_Release(m_pParticle_Local);
		Safe_Release(m_pPlayerRigidBody);
	}
}
