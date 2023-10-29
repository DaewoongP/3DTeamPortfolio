#include "Event_Sky_Enter.h"
#include "GameInstance.h"

CEvent_Sky_Enter::CEvent_Sky_Enter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Sky_Enter::CEvent_Sky_Enter(const CEvent_Sky_Enter& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Sky_Enter::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	BEGININSTANCE;

	pGameInstance->Add_Timer(TEXT("Sky_Enter_CutScene_Fade_Out"),false,1.0f);
	pGameInstance->Add_Timer(TEXT("Sky_Enter_CutScene_Play"),false,11.0f);

	pGameInstance->Read_CutSceneCamera(TEXT("Sky_Enter"), TEXT("../../Resources/GameData/CutScene/Sky_Start.cut"));

	ENDINSTANCE;

	return S_OK;
}

void CEvent_Sky_Enter::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Check_Event_Play_CutScene();
}

void CEvent_Sky_Enter::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Sky_Enter::Check_Event_Play_CutScene()
{
	BEGININSTANCE;

	switch (m_eSky_Enter_Sequence)
	{
		//���̵� �ƿ�
	case Client::CEvent_Sky_Enter::SKY_ENTER_SEQUENCE_FADE_OUT:
	{
		//���Խ�
		if (true == m_isEnter)
		{
			//���̵� �ƿ�
			m_pRenderer->FadeOut(1.0f, _float4(1.0f, 1.0f, 1.0f, 1.0f));
			
			//���� ǥ��
			m_isEnter = false;
		}

		//Ÿ�̸� üũ
		if (true == pGameInstance->Check_Timer(TEXT("Sky_Enter_CutScene_Fade_Out")))
		{
			m_isEnter = true;
			
			m_eSky_Enter_Sequence = SKY_ENTER_SEQUENCE_PLAY_CUTSCENE;
			
		}
	}
		break;
		//�ƾ� ��� �� ���̵� ��
	case Client::CEvent_Sky_Enter::SKY_ENTER_SEQUENCE_PLAY_CUTSCENE:
	{
		//���Խ�
		if (true == m_isEnter)
		{
			//���̵� ��
			m_pRenderer->FadeIn(1.0f, _float4(1.0f, 1.0f, 1.0f, 1.0f));
			//Ÿ�̸� ����
			pGameInstance->Reset_Timer(TEXT("Sky_Enter_CutScene_Play"));
			//���� ǥ��
			m_isEnter = false;
			//�ƾ� ���
			pGameInstance->Add_CutScene(TEXT("Sky_Enter"));
		}

		//Ÿ�̸� ����
		if (true == pGameInstance->Check_Timer(TEXT("Sky_Enter_CutScene_Play")))
		{
			m_eSky_Enter_Sequence = SKY_ENTER_SEQUENCE_FADE_IN;
			
			//���̵� �ƿ�
			m_pRenderer->FadeOut(1.0f, _float4(1.0f, 1.0f, 1.0f, 1.0f));
			//Ÿ�̸� ����
			pGameInstance->Reset_Timer(TEXT("Sky_Enter_CutScene_Fade_Out"));
			
			m_isEnter = true;
		}
	}
	break;
	//���̵� �� 
	case Client::CEvent_Sky_Enter::SKY_ENTER_SEQUENCE_FADE_IN:
	{
		//���Խ�
		if (true == m_isEnter)
		{
			//���� ǥ��
			m_isEnter = false;
		}

		//Ÿ�̸� üũ
		if (true == pGameInstance->Check_Timer(TEXT("Sky_Enter_CutScene_Fade_Out")))
		{
			m_isEnter = true;

			m_eSky_Enter_Sequence = SKY_ENTER_SEQUENCE_END;

			//���̵� ��
			m_pRenderer->FadeIn(1.0f, _float4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}
		break;
	case Client::CEvent_Sky_Enter::SKY_ENTER_SEQUENCE_END:
	{
		if (true == m_isEnter)
			break;

		m_isEnter = true;
		m_eSky_Enter_Sequence = SKY_ENTER_SEQUENCE_FADE_OUT;
		//Ÿ�̸� ����
		pGameInstance->Reset_Timer(TEXT("Sky_Enter_CutScene_Fade_Out"));
			
	}
	break;
	default:
		break;
	}

	ENDINSTANCE;
}

HRESULT CEvent_Sky_Enter::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		__debugbreak();
		return E_FAIL;
	}


	return S_OK;
}

CEvent_Sky_Enter* CEvent_Sky_Enter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Sky_Enter* pInstance = New CEvent_Sky_Enter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Sky_Enter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Sky_Enter* CEvent_Sky_Enter::Clone(void* pArg)
{
	CEvent_Sky_Enter* pInstance = New CEvent_Sky_Enter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Sky_Enter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Sky_Enter::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pRenderer);
	}
}