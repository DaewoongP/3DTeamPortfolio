#include "../Public/Sound_Manager.h"
#include <io.h>

IMPLEMENT_SINGLETON(CSound_Manager)

HRESULT CSound_Manager::Initialize()
{
	// fmod system ��ü ����
	if (FMOD_OK != FMOD::System_Create(&m_pSystem))
	{
		MSG_BOX("Failed Create fmod System");
		return E_FAIL;
	}
	// �ý��� �ʱ�ȭ
	if (FMOD_OK != m_pSystem->init(MAX_CHANNEL, FMOD_INIT_NORMAL, NULL))
	{
		MSG_BOX("Failed Initialize FMOD");
		return E_FAIL;
	}

	return S_OK;
}

void CSound_Manager::Tick()
{
	if (nullptr != m_pSystem)
		m_pSystem->update();
}

HRESULT CSound_Manager::Add_Sounds(const _tchar* pSoundFilePath)
{
	// ���� ���� ����
	if (FAILED(Load_SoundFile(pSoundFilePath)))
		return E_FAIL;

	return S_OK;
}

_bool CSound_Manager::Is_Playing(_int iChannel)
{
	if (0 > iChannel ||
		MAX_CHANNEL < iChannel)
		return false;

	_bool isPlay = { true };
	m_Channels[iChannel]->isPlaying(&isPlay);

	return isPlay;
}

_int CSound_Manager::Play_Sound(const _tchar* pSoundTag, _float fVolume)
{
	FMOD::Sound* pSound = Find_Sound(pSoundTag);
	
	if (nullptr == pSound)
	{
#ifdef _DEBUG
		MSG_BOX("Failed Find_Sound");
#endif // _DEBUG
		return -1;
	}

	_bool isPlay = { true };

	// false�϶� ������̸� �н�, ������� �ƴϸ� ���� ���� ���.
	_uint iChecked = { 0 };
	while (isPlay)
	{
		if (MAX_CHANNEL == m_iChannel)
			m_iChannel = 0;

		m_Channels[m_iChannel]->isPlaying(&isPlay);

		if (true == isPlay)
		{
			++m_iChannel;
		}
		
		++iChecked;
		// 32�� ���������� ����
		if (iChecked >= MAX_CHANNEL)
			return -1;
	}
	
	m_pSystem->playSound(pSound, nullptr, false, &m_Channels[m_iChannel]);

	// ����ó��
	if (nullptr == m_Channels[m_iChannel])
	{
		MSG_BOX("Failed Set Channel");
		return -1;
	}
	
	if (FMOD_OK != m_Channels[m_iChannel]->setVolume(fVolume))
		return -1;

	return m_iChannel;
}

_int CSound_Manager::Play_BGM(const _tchar* pSoundTag, _float fVolume)
{
	FMOD::Sound* pSound = Find_Sound(pSoundTag);

	if (nullptr == pSound)
		return -1;

	_bool isPlay = { true };

	_uint iChecked = { 0 };
	while (isPlay)
	{
		if (MAX_CHANNEL == m_iChannel)
			m_iChannel = 0;

		m_Channels[m_iChannel]->isPlaying(&isPlay);

		if (true == isPlay)
		{
			++m_iChannel;
		}

		++iChecked;
		// 32�� ���������� ����
		if (iChecked >= MAX_CHANNEL)
			return -1;
	}

	if (FMOD_OK != m_pSystem->playSound(pSound, nullptr, false, &m_Channels[m_iChannel]))
		return -1;

	m_Channels[m_iChannel]->setMode(FMOD_LOOP_NORMAL);
	m_Channels[m_iChannel]->setVolume(fVolume);

	return m_iChannel;
}

_int CSound_Manager::Play_Sound(const _tchar* pSoundTag, _uint iNumSounds, _float fVolume)
{
	_tchar	szSoundName[MAX_STR] = TEXT("");
	wsprintf(szSoundName, pSoundTag, rand() % iNumSounds);

	return Play_Sound(szSoundName, fVolume);
}

HRESULT CSound_Manager::Stop_Sound(_int iChannel)
{
	if (0 > iChannel ||
		MAX_CHANNEL < iChannel)
		return E_FAIL;

	m_Channels[iChannel]->stop();

	return S_OK;
}

HRESULT CSound_Manager::Pause_Sound(_int iChannel)
{
	if (0 > iChannel ||
		MAX_CHANNEL < iChannel)
		return E_FAIL;

	m_Channels[iChannel]->setPaused(true);

	return S_OK;
}

HRESULT CSound_Manager::Restart_Sound(_int iChannel)
{
	if (0 > iChannel ||
		MAX_CHANNEL <= iChannel)
		return E_FAIL;

	m_Channels[iChannel]->setPaused(false);

	return S_OK;
}

HRESULT CSound_Manager::Stop_AllSound()
{
	for (auto& pChannel : m_Channels)
		pChannel->stop();

	return S_OK;
}

HRESULT CSound_Manager::Set_ChannelVolume(_int iChannel, _float fVolume)
{
	if (0 > iChannel ||
		MAX_CHANNEL <= iChannel)
		return E_FAIL;

	m_Channels[iChannel]->setVolume(fVolume);

	return S_OK;
}

_float CSound_Manager::Get_ChannelVolume(_int iChannel)
{
	if (0 > iChannel ||
		MAX_CHANNEL <= iChannel)
		return 0.f;

	_float fVolume = { 0.f };
	m_Channels[iChannel]->getVolume(&fVolume);

	return fVolume;
}

HRESULT CSound_Manager::Load_SoundFile(const _tchar* pSoundFile)
{
	std::lock_guard<std::mutex> lock(mtx);

	_wfinddata_t FindDataValue;

	_tchar szAllFiles[MAX_PATH] = TEXT("");
	lstrcpy(szAllFiles, pSoundFile);
	lstrcat(szAllFiles, TEXT("*.*"));

	INT64 handle = _wfindfirst(szAllFiles, &FindDataValue);

	if (-1 == handle)
		return E_FAIL;

	_int iResult = { 0 };

	_tchar szFullPath[MAX_PATH] = TEXT("");

	// ������ ���������� ��ȸ.
	while (-1 != iResult)
	{
		lstrcpy(szFullPath, pSoundFile);
		lstrcat(szFullPath, FindDataValue.name);

		// system file
		if (!lstrcmp(FindDataValue.name, TEXT(".")) ||
			!lstrcmp(FindDataValue.name, TEXT("..")))
		{
			iResult = _wfindnext(handle, &FindDataValue);
			continue;
		}
		
		FMOD::Sound* pSound = { nullptr };
		
		_char szFmodPath[MAX_PATH] = "";
		WideCharToMultiByte(CP_ACP, 0, szFullPath, -1, szFmodPath, MAX_PATH, nullptr, nullptr);
		
		if (FMOD_OK == m_pSystem->createSound(szFmodPath, FMOD_DEFAULT, nullptr, &pSound))
		{
			_int iLength = lstrlen(FindDataValue.name) + 1;

			_tchar* szSoundKey = New _tchar[iLength];
			ZeroMemory(szSoundKey, sizeof(_tchar) * iLength);

			lstrcpy(szSoundKey, FindDataValue.name);

			m_Sounds.emplace(szSoundKey, pSound);
		}
		else
		{
			MSG_BOX("Failed Create Sound");
			return E_FAIL;
		}

		iResult = _wfindnext(handle, &FindDataValue);
	}

	_findclose(handle);

	return S_OK;
}

FMOD::Sound* CSound_Manager::Find_Sound(const _tchar* pSoundTag)
{
	auto iter = find_if(m_Sounds.begin(), m_Sounds.end(), CTag_Finder(pSoundTag));

	if (iter == m_Sounds.end())
		return nullptr;

	return iter->second;
}

void CSound_Manager::Free()
{
	for (auto& pair : m_Sounds)
	{
		delete[] pair.first;
		pair.second->release();
	}
	m_Sounds.clear();

	m_pSystem->release();
	m_pSystem->close();
}
