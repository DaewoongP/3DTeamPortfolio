#include "..\Public\Player.h"
#include "GameInstance.h"
#include "Player_Camera.h"
#include "Magic.h"
#include "Weapon_Player_Wand.h"
#include "StateContext.h"
#include "IdleState.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed Player Initialize");

		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Player Add_Components");

		return E_FAIL;
	}

	if (FAILED(Ready_MeshParts()))
	{
		MSG_BOX("Failed Ready Player Mesh Parts");

		return E_FAIL;
	}

	/*if (FAILED(Ready_Camera()))
	{
		MSG_BOX("Failed Ready Player Caemra");

		return E_FAIL;
	}*/

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));


	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Key_Input(fTimeDelta);

	UpdateLookAngle();

	m_pStateContext->Tick(fTimeDelta);

	m_pCustomModel->Tick(CCustomModel::ROBE, 2, fTimeDelta);

	/*m_pCustomModel->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);*/
	/*m_pCustomModel->Play_Animation(fTimeDelta, CModel::UNDERBODY);*/
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pStateContext->Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}

#ifdef _DEBUG
	Tick_ImGui();
#endif // _DEBUG
}

void CPlayer::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	cout << "Player Enter" << endl;
}

void CPlayer::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	cout << "stay" << endl;
}

void CPlayer::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	cout << "Exit" << endl;
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	for (_uint iPartsIndex = 0; iPartsIndex < CCustomModel::MESH_END; ++iPartsIndex)
	{
		_uint		iNumMeshes = m_pCustomModel->Get_NumMeshes(iPartsIndex);

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pCustomModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iPartsIndex, i);

			m_pCustomModel->Bind_Material(m_pShader, "g_DiffuseTexture", iPartsIndex, i, DIFFUSE);

			m_pShader->Begin("AnimMeshNonCull");

			m_pCustomModel->Render(iPartsIndex, i);
		}
	}

	return S_OK;
}

HRESULT CPlayer::Render_Depth()
{
	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* For.Com_Model_CustomModel_Player */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_CustomModel_Player"),
		TEXT("Com_Model_CustomModel_Player"), reinterpret_cast<CComponent**>(&m_pCustomModel))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Model_CustomModel_Player)");
		return E_FAIL;
	}

	CStateContext::STATECONTEXTDESC StateContextDesc{};

	StateContextDesc.pOwnerModel = m_pCustomModel;
	StateContextDesc.pOwnerLookAngle = &m_fLookAngle;
	StateContextDesc.pIsDirectionPressed = &m_isDirectionKeyPressed;
	StateContextDesc.pPlayerTransform = m_pTransform;

	/* For.Com_StateContext */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_StateContext"),
		TEXT("Com_StateContext"), reinterpret_cast<CComponent**>(&m_pStateContext),&StateContextDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_StateContext)");
		return E_FAIL;
	}

	const CBone* pBone = m_pCustomModel->Get_Bone(TEXT("SKT_RightHand"));
	if (nullptr == pBone)
		throw TEXT("pBone is nullptr");

	CWeapon_Player_Wand::PARENTMATRIXDESC ParentMatrixDesc;
	ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.PivotMatrix = m_pCustomModel->Get_PivotFloat4x4();
	ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

	if (FAILED(Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Player_Wand"),
		TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
		throw TEXT("Com_Weapon");

	/* For.Com_Magic*/
	//CMagic::MAGICDESC magicInitDesc;
	//magicInitDesc.eBuffType = CMagic::BUFF_NONE;
	//magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
	//magicInitDesc.eMagicType = CMagic::MT_NOTHING;
	//magicInitDesc.eMagicTag = BASICCAST;
	//magicInitDesc.fCoolTime = 1.f;
	//magicInitDesc.fDamage = 10.f;
	//magicInitDesc.fCastDistance = 1000;
	//magicInitDesc.fBallDistance = 30;
	//magicInitDesc.fLifeTime = 0.1f;

	CMagic::MAGICDESC magicInitDesc;
	magicInitDesc.eBuffType = CMagic::BUFF_SHILED;
	magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
	magicInitDesc.eMagicType = CMagic::MT_ALL;
	magicInitDesc.eMagicTag = CONFRINGO;
	magicInitDesc.fCoolTime = 1.f;
	magicInitDesc.fDamage = 0.f;
	magicInitDesc.fCastDistance = 1000;
	magicInitDesc.fBallDistance = 30;
	magicInitDesc.fLifeTime = 0.2f;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Magic"),
		TEXT("Com_Magic"), reinterpret_cast<CComponent**>(&m_pMagic), &magicInitDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_Magic)");
		return E_FAIL;
	}
	m_pMagic->Add_ActionFunc([&] {(*this).MagicTestTextOutput(); });

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_UP))
	{
		m_pTransform->Go_Straight(fTimeDelta);
	}
	
	if (pGameInstance->Get_DIKeyState(DIK_DOWN))
	{
		m_pTransform->Go_Backward(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_LEFT))
	{
		//m_pTransform->Turn(_float3(0.f, -1.f, 0.f), fTimeDelta);
		m_pTransform->Go_Left(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_RIGHT))
	{
		//m_pTransform->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);
		m_pTransform->Go_Right(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		//m_pRigidBody->Add_Force(m_pTransform->Get_Up() * 30.f, PxForceMode::eIMPULSE);
	}

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		if (m_pMagic != nullptr)
		{
			// 목표의 transform과 시작 위치를 가져와야합니다.
			// 아직 타겟 설정하는게 없어서 널로 넣었음.
			// 지팡이 위치를 2번째 인자 pos로 넣어야하는데 지팡이도 없어서 그냥 pTransform->Get_Position()로 넣음.
			// 임의로 아무거나 집어오겠음.
			
			/* 이거는 테스트 용으로 더미클래스 찾으려고 넣은 코드를 훔쳐온거임 */
			CGameObject* pTestTarget = dynamic_cast<CGameObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_Debug"), TEXT("GameObject_Dummy")));
			if (nullptr == pTestTarget)
				throw TEXT("pTestTarget is nullptr");

			m_pMagic->Magic_Cast(pTestTarget->Get_Transform(), m_pWeapon);
		}
	}

	ENDINSTANCE;
}

HRESULT CPlayer::Ready_MeshParts()
{
	//Head
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME,
		TEXT("Prototype_Component_MeshPart_Player_Head"),
		CCustomModel::HEAD)))
	{
		MSG_BOX("Failed Add MeshPart Head");

		return E_FAIL;
	}

	//Arm
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME, 
		TEXT("Prototype_Component_MeshPart_Player_Arm"),
		CCustomModel::ARM)))
	{
		MSG_BOX("Failed Add MeshPart Arm");

		return E_FAIL;
	}

	//Robe
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME,
		TEXT("Prototype_Component_MeshPart_Robe01"),
		CCustomModel::ROBE), TEXT("../../Resources/GameData/ClothData/Test.cloth")))
	{
		MSG_BOX("Failed Add MeshPart Robe");
	
		return E_FAIL;
	}

	//Top
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME,
		TEXT("Prototype_Component_MeshPart_Player_Top"),
		CCustomModel::TOP)))
	{
		MSG_BOX("Failed Add MeshPart Top");

		return E_FAIL;
	}

	//Pants
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME,
		TEXT("Prototype_Component_MeshPart_Player_Pants"),
		CCustomModel::PANTS)))
	{
		MSG_BOX("Failed Add MeshPart Pants");

		return E_FAIL;
	}

	//Socks
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME,
		TEXT("Prototype_Component_MeshPart_Player_Socks"),
		CCustomModel::SOCKS)))
	{
		MSG_BOX("Failed Add MeshPart Socks");

		return E_FAIL;
	}

	//Shoes
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_MAINGAME,
		TEXT("Prototype_Component_MeshPart_Player_Shoes"),
		CCustomModel::SHOES)))
	{
		MSG_BOX("Failed Add MeshPart Shoes");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Ready_Camera()
{
	//어떤 뼈에 붙일 것인가.
	_uint iBoneIndex{ 0 };

	m_pCustomModel->Find_BoneIndex(TEXT("SKT_HeadCamera"), &iBoneIndex);

	CCamera::CAMERADESC CameraDesc;

	CameraDesc.m_fAspect = _float(g_iWinSizeX) / _float(g_iWinSizeY);
	CameraDesc.m_fFovY = XMConvertToRadians(90.f);
	CameraDesc.m_fNear = 0.1f;
	CameraDesc.m_fFar = 1000.f;

	CPlayer_Camera::PLAYERCAMERADESC PlayerCameraDesc;

	PlayerCameraDesc.CameraDesc = CameraDesc;
	//PlayerCameraDesc.pFollowTargetBoneMatrix = m_pCustomModel->Get_BoneCombinedTransformationMatrixPtr(iBoneIndex);
	PlayerCameraDesc.pFollowTargetMatrix = m_pTransform->Get_WorldMatrixPtr();

	m_pPlayer_Camera = CPlayer_Camera::Create(m_pDevice,m_pContext, &PlayerCameraDesc);

	Safe_AddRef(m_pPlayer_Camera);

	NULL_CHECK_RETURN_MSG(m_pPlayer_Camera, E_FAIL, TEXT("Failed Create Player Camera"));

	BEGININSTANCE;

	pGameInstance->Add_Camera(TEXT("Player_Camera"), (CCamera*)m_pPlayer_Camera);

	pGameInstance->Set_Camera(TEXT("Player_Camera"));

	ENDINSTANCE;

	return S_OK;
}


void CPlayer::MagicTestTextOutput()
{
	cout << "마법 발동" << endl;
}

#ifdef _DEBUG

void CPlayer::Tick_ImGui()
{
	//ImGui::Begin("Player");
	
	//ImGui::End();
}

#endif // _DEBUG

void CPlayer::UpdateLookAngle()
{
	BEGININSTANCE;
	
	m_isDirectionKeyPressed = false;

	//키 입력에 따라 비교 각이 바뀐다.

	_float3 vNextLook{};

	//앞 키 > 룩
	if (pGameInstance->Get_DIKeyState(DIK_W, CInput_Device::KEY_PRESSING) || 
		pGameInstance->Get_DIKeyState(DIK_W,CInput_Device::KEY_DOWN))
	{
		vNextLook = m_pPlayer_Camera->Get_CamLookXZ();
		m_isDirectionKeyPressed = true;
	}
	//뒷 키 > -룩
	else if (pGameInstance->Get_DIKeyState(DIK_S, CInput_Device::KEY_PRESSING) ||
		pGameInstance->Get_DIKeyState(DIK_S, CInput_Device::KEY_DOWN))
	{
		vNextLook = -m_pPlayer_Camera->Get_CamLookXZ();
		m_isDirectionKeyPressed = true;
	}
	//오른쪽 키 > 라이트
	else if (pGameInstance->Get_DIKeyState(DIK_D, CInput_Device::KEY_PRESSING) ||
		pGameInstance->Get_DIKeyState(DIK_D, CInput_Device::KEY_DOWN))
	{
		vNextLook = m_pPlayer_Camera->Get_CamRightXZ();
		m_isDirectionKeyPressed = true;
	}
	//왼쪽 키 > -라이트
	else if (pGameInstance->Get_DIKeyState(DIK_A, CInput_Device::KEY_PRESSING) ||
		pGameInstance->Get_DIKeyState(DIK_A, CInput_Device::KEY_DOWN))
	{
		vNextLook = -m_pPlayer_Camera->Get_CamRightXZ();
		m_isDirectionKeyPressed = true;
	}

	//플레이어의 룩을 가지고 온다.
	_float3 vPlayerLook = m_pTransform->Get_Look();

	//y값 지우고
	vPlayerLook = XMVectorSetY(vPlayerLook, 0.0f);

	vPlayerLook.Normalize();

	//내적한다.cos(-1 ~ 1)
	_float fLookAngle = vPlayerLook.Dot(vNextLook);

	//라디안 화
	m_fLookAngle = acosf(fLookAngle);

	//좌우 구분을 위한 외적
	if (0.0f > vPlayerLook.Cross(vNextLook).y)
	{
		m_fLookAngle *= -1;
	}

	ENDINSTANCE;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = New CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = New CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	CGameObject::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pShader);
		Safe_Release(m_pRenderer);
		Safe_Release(m_pCustomModel);
		Safe_Release(m_pPlayer_Camera);
		Safe_Release(m_pMagic);
		Safe_Release(m_pWeapon);
		Safe_Release(m_pStateContext);

	}
}
