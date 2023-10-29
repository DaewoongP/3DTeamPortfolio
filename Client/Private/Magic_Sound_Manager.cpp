#include "Magic_Sound_Manager.h"
#include "GameInstance.h"

CMagic_Sound_Manager::CMagic_Sound_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

HRESULT CMagic_Sound_Manager::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	BEGININSTANCE;
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Dark_Wizard_Male_A/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Dark_Wizard_Male_B/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Dark_Wizard_Male_C/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Dark_Wizard_Male_D/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Player/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Eleazar_Fig/"));
	ENDINSTANCE;
#pragma region OWNER_BLACKMAGIC_A
	m_szSoundTag[OWNER_BLACKMAGIC_A][SPELL::PROTEGO].push_back(TEXT("darkwizardmalea_00414.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_A][SPELL::PROTEGO].push_back(TEXT("DarkWizardMaleA_00414-01.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_A][SPELL::PROTEGO].push_back(TEXT("DarkWizardMaleA_00414-02.wav"));

	m_szSoundTag[OWNER_BLACKMAGIC_A][SPELL::DIFFINDO].push_back(TEXT("darkwizardmalea_00434.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_A][SPELL::DIFFINDO].push_back(TEXT("darkwizardmalea_00434-01.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_A][SPELL::DIFFINDO].push_back(TEXT("darkwizardmalea_00434-02.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_A][SPELL::DIFFINDO].push_back(TEXT("DarkWizardMaleA_00434-03.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_A][SPELL::DIFFINDO].push_back(TEXT("DarkWizardMaleA_00434-04.wav"));
#pragma endregion
#pragma region OWNER_BLACKMAGIC_B
	m_szSoundTag[OWNER_BLACKMAGIC_B][SPELL::PROTEGO].push_back(TEXT("darkwizardmaleb_00414.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_B][SPELL::PROTEGO].push_back(TEXT("darkwizardmaleb_00414.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_B][SPELL::PROTEGO].push_back(TEXT("DarkWizardMaleB_00414-02.wav"));

	m_szSoundTag[OWNER_BLACKMAGIC_B][SPELL::CONFRINGO].push_back(TEXT("darkwizardmaleb_00442.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_B][SPELL::CONFRINGO].push_back(TEXT("darkwizardmaleb_00442-01.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_B][SPELL::CONFRINGO].push_back(TEXT("darkwizardmaleb_00442-02.wav"));
#pragma endregion
#pragma region OWNER_BLACKMAGIC_C
	m_szSoundTag[OWNER_BLACKMAGIC_C][SPELL::PROTEGO].push_back(TEXT("darkwizardmalec_00414.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_C][SPELL::PROTEGO].push_back(TEXT("DarkWizardMaleC_00414-01.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_C][SPELL::PROTEGO].push_back(TEXT("DarkWizardMaleC_00414-02.wav"));

	m_szSoundTag[OWNER_BLACKMAGIC_C][SPELL::CONFRINGO].push_back(TEXT("darkwizardmalec_00442.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_C][SPELL::CONFRINGO].push_back(TEXT("darkwizardmalec_00442-01.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_C][SPELL::CONFRINGO].push_back(TEXT("darkwizardmalec_00442-02.wav"));
#pragma endregion
#pragma region OWNER_BLACKMAGIC_D
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::FLIPENDO].push_back(TEXT("darkwizardmaled_00410.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::FLIPENDO].push_back(TEXT("darkwizardmaled_00410-01.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::FLIPENDO].push_back(TEXT("darkwizardmaled_00410-02.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::FLIPENDO].push_back(TEXT("DarkWizardMaleD_00410-03.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::FLIPENDO].push_back(TEXT("DarkWizardMaleD_00410-04.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::FLIPENDO].push_back(TEXT("DarkWizardMaleD_00410-05.wav"));

	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::LEVIOSO].push_back(TEXT("darkwizardmaled_00413.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::LEVIOSO].push_back(TEXT("DarkWizardMaleD_00413-01.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::LEVIOSO].push_back(TEXT("DarkWizardMaleD_00413-02.wav"));

	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::PROTEGO].push_back(TEXT("darkwizardmaled_00414.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::PROTEGO].push_back(TEXT("DarkWizardMaleD_00414-01.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::PROTEGO].push_back(TEXT("DarkWizardMaleD_00414-02.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::PROTEGO].push_back(TEXT("DarkWizardMaleD_00414-03.wav"));
	
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::DIFFINDO].push_back(TEXT("darkwizardmaled_00434.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::DIFFINDO].push_back(TEXT("darkwizardmaled_00434-01.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::DIFFINDO].push_back(TEXT("darkwizardmaled_00434-02.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::DIFFINDO].push_back(TEXT("DarkWizardMaleD_00434-03.wav"));

	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::CONFRINGO].push_back(TEXT("darkwizardmaled_00442.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::CONFRINGO].push_back(TEXT("darkwizardmaled_00442-01.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::CONFRINGO].push_back(TEXT("darkwizardmaled_00442-02.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::CONFRINGO].push_back(TEXT("DarkWizardMaleD_00442-03.wav"));

	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::EXPELLIARMUS].push_back(TEXT("darkwizardmaled_00444.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::EXPELLIARMUS].push_back(TEXT("DarkWizardMaleD_00444-01.wav"));
	m_szSoundTag[OWNER_BLACKMAGIC_D][SPELL::EXPELLIARMUS].push_back(TEXT("DarkWizardMaleD_00444-02.wav"));
#pragma endregion
#pragma region OWNER_PLAYER
	m_szSoundTag[OWNER_PLAYER][SPELL::ACCIO].push_back(TEXT("playermale_00400.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::ACCIO].push_back(TEXT("playermale_00400-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::ACCIO].push_back(TEXT("playermale_00400-02.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::ALOHOMORA].push_back(TEXT("playermale_00401.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::ALOHOMORA].push_back(TEXT("playermale_00401-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::ALOHOMORA].push_back(TEXT("playermale_00401-02.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::ARRESTOMOMENTUM].push_back(TEXT("playermale_00402.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::ARRESTOMOMENTUM].push_back(TEXT("playermale_00402-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::ARRESTOMOMENTUM].push_back(TEXT("playermale_00402-02.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::DEPULSO].push_back(TEXT("playermale_00403.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::DEPULSO].push_back(TEXT("playermale_00403-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::DEPULSO].push_back(TEXT("playermale_00403-02.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::DEPULSO].push_back(TEXT("playermale_00403-03.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::EXPELLIARMUS].push_back(TEXT("playermale_00408.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::EXPELLIARMUS].push_back(TEXT("playermale_00408-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::EXPELLIARMUS].push_back(TEXT("playermale_00408-02.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::EXPELLIARMUS].push_back(TEXT("playermale_00408-03.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::FLIPENDO].push_back(TEXT("playermale_00410.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::FLIPENDO].push_back(TEXT("playermale_00410-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::FLIPENDO].push_back(TEXT("playermale_00410-02.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::NCENDIO].push_back(TEXT("playermale_00412.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::NCENDIO].push_back(TEXT("playermale_00412-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::NCENDIO].push_back(TEXT("playermale_00412-02.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::NCENDIO].push_back(TEXT("playermale_00412-03.wav"));
	
	m_szSoundTag[OWNER_PLAYER][SPELL::LEVIOSO].push_back(TEXT("playermale_00413.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::LEVIOSO].push_back(TEXT("playermale_00413-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::LEVIOSO].push_back(TEXT("playermale_00413-02.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::PROTEGO].push_back(TEXT("playermale_00414.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::PROTEGO].push_back(TEXT("playermale_00414-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::PROTEGO].push_back(TEXT("playermale_00414-02.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::PROTEGO].push_back(TEXT("playermale_00414-03.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::STUPEFY].push_back(TEXT("playermale_00417.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::STUPEFY].push_back(TEXT("playermale_00417-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::STUPEFY].push_back(TEXT("playermale_00417-02.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::STUPEFY].push_back(TEXT("playermale_00417-03.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::STUPEFY].push_back(TEXT("playermale_00417-04.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::STUPEFY].push_back(TEXT("playermale_00417-05.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::LUMOS].push_back(TEXT("playermale_00420.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::LUMOS].push_back(TEXT("playermale_00420-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::LUMOS].push_back(TEXT("playermale_00420-02.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::IMPERIO].push_back(TEXT("playermale_00432.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::IMPERIO].push_back(TEXT("playermale_00432-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::IMPERIO].push_back(TEXT("playermale_00432-02.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::IMPERIO].push_back(TEXT("playermale_00432-03.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::DIFFINDO].push_back(TEXT("playermale_00434.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::DIFFINDO].push_back(TEXT("playermale_00434-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::DIFFINDO].push_back(TEXT("playermale_00434-02.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::CONFRINGO].push_back(TEXT("playermale_00442.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::CONFRINGO].push_back(TEXT("playermale_00442-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::CONFRINGO].push_back(TEXT("playermale_00442-02.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::PETRIFICUSTOTALUS].push_back(TEXT("playermale_00444.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::PETRIFICUSTOTALUS].push_back(TEXT("playermale_00444-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::PETRIFICUSTOTALUS].push_back(TEXT("playermale_00444-02.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::PETRIFICUSTOTALUS].push_back(TEXT("playermale_00444-03.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::EVAESCO].push_back(TEXT("playermale_00456.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::EVAESCO].push_back(TEXT("playermale_00456-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::EVAESCO].push_back(TEXT("playermale_00456-02.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::EVAESCO].push_back(TEXT("playermale_00456-03.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::DESCENDO].push_back(TEXT("playermale_00458.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::DESCENDO].push_back(TEXT("playermale_00458-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::DESCENDO].push_back(TEXT("playermale_00458-02.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::BOMBARDA].push_back(TEXT("playermale_00459.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::BOMBARDA].push_back(TEXT("playermale_00459-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::BOMBARDA].push_back(TEXT("playermale_00459-02.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::BOMBARDA].push_back(TEXT("playermale_00459-03.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::BOMBARDA].push_back(TEXT("playermale_00459-04.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::CRUCIO].push_back(TEXT("playermale_00461.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::CRUCIO].push_back(TEXT("playermale_00461-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::CRUCIO].push_back(TEXT("playermale_00461-02.wav"));

	m_szSoundTag[OWNER_PLAYER][SPELL::EXPELLIARMUS].push_back(TEXT("DarkWizardMaleD_00444-01.wav"));
	m_szSoundTag[OWNER_PLAYER][SPELL::EXPELLIARMUS].push_back(TEXT("DarkWizardMaleD_00444-02.wav"));
#pragma endregion
#pragma region OWNER_FIG
	m_szSoundTag[OWNER_FIG][SPELL::ACCIO].push_back(TEXT("eleazarfig_00400.wav"));

	m_szSoundTag[OWNER_FIG][SPELL::DEPULSO].push_back(TEXT("eleazarfig_00403.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::DEPULSO].push_back(TEXT("eleazarfig_00403-01.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::DEPULSO].push_back(TEXT("eleazarfig_00403-02.wav"));

	m_szSoundTag[OWNER_FIG][SPELL::LEVIOSO].push_back(TEXT("eleazarfig_00413.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::LEVIOSO].push_back(TEXT("EleazarFig_00413-01.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::LEVIOSO].push_back(TEXT("EleazarFig_00413-02.wav"));

	m_szSoundTag[OWNER_FIG][SPELL::PROTEGO].push_back(TEXT("eleazarfig_00414.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::PROTEGO].push_back(TEXT("EleazarFig_00414-01.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::PROTEGO].push_back(TEXT("EleazarFig_00414-02.wav"));

	m_szSoundTag[OWNER_FIG][SPELL::LUMOS].push_back(TEXT("eleazarfig_00420.wav"));

	m_szSoundTag[OWNER_FIG][SPELL::DIFFINDO].push_back(TEXT("eleazarfig_00434.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::DIFFINDO].push_back(TEXT("eleazarfig_00434-01.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::DIFFINDO].push_back(TEXT("eleazarfig_00434-02.wav"));

	m_szSoundTag[OWNER_FIG][SPELL::CONFRINGO].push_back(TEXT("eleazarfig_00442.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::CONFRINGO].push_back(TEXT("eleazarfig_00442-01.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::CONFRINGO].push_back(TEXT("eleazarfig_00442-02.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::CONFRINGO].push_back(TEXT("eleazarfig_00442-03.wav"));

	m_szSoundTag[OWNER_FIG][SPELL::DESCENDO].push_back(TEXT("eleazarfig_00458.wav"));

	m_szSoundTag[OWNER_FIG][SPELL::FINISHER].push_back(TEXT("eleazarfig_13364.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::CRUCIO].push_back(TEXT("eleazarfig_12222.wav"));
	m_szSoundTag[OWNER_FIG][SPELL::BOMBARDA].push_back(TEXT("eleazarfig_12222.wav"));
#pragma endregion
	return S_OK;
}

void CMagic_Sound_Manager::Spell_Magic(OWNERTYPE eOwner,SPELL eSpell,_float fVolum)
{
	if (eOwner >= OWNER_END ||
		eSpell >= SPELL_END||
		m_szSoundTag[eOwner][eSpell].size() == 0)
		return;

	//Play_Sound(const _tchar * pSoundTag, _uint iNumSounds, CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay)
	BEGININSTANCE;
	pGameInstance->Play_Sound(m_szSoundTag[eOwner][eSpell][rand() % m_szSoundTag[eOwner][eSpell].size()].c_str(), fVolum);
	ENDINSTANCE;
}

CMagic_Sound_Manager* CMagic_Sound_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMagic_Sound_Manager* pInstance = New CMagic_Sound_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMagic_Sound_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMagic_Sound_Manager* CMagic_Sound_Manager::Clone(void* pArg)
{
	AddRef();
	return this;
}

void CMagic_Sound_Manager::Free()
{
	__super::Free();
}
