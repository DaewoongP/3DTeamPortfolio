#pragma once

/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
//
/* =============================================== */

#include "Base.h"

#define	MAX_CHANNEL		32

BEGIN(Engine)

class CSound_Manager final : public CBase
{
    DECLARE_SINGLETON(CSound_Manager)

private:
    explicit CSound_Manager() = default;
    virtual ~CSound_Manager() = default;

public:
	// �̰� �� Initialize_Engine�� �־ �ʱ�ȭ �������ּ���
    HRESULT Initialize();
	// ���ϰ�ο� �ִ� ���带 �ڵ����� �����̸��� �±װ����� �Է�����
	HRESULT Add_Sounds(const _tchar* pSoundFilePath);
	
public:
	// forceplay Ȱ��ȭ�ϸ� ���� ������� ���� �����ϰ� ������.
	// default �Ű����� �����ϴ°��� ��õ��.
	_int Play_Sound(const _tchar* pSoundTag, _float fVolume);
	// �������� ������ ���� ������ �±� �Է�
	// sound%d.wav, 10 -> 10���߿� ���� ���� ���
	_int Play_Sound(const _tchar* pSoundTag, _uint iNumSounds, _float fVolume);
	_int Play_BGM(const _tchar* pSoundTag, _float fVolume);
	HRESULT Stop_Sound(_int iChannel);
	HRESULT Pause_Sound(_int iChannel);
	HRESULT Restart_Sound(_int iChannel);
	HRESULT Stop_AllSound();
	HRESULT Set_ChannelVolume(_int iChannel, _float fVolume);

private:
	HRESULT Load_SoundFile(const _tchar* pSoundFile);
	FMOD::Sound* Find_Sound(const _tchar* pSoundTag);

private:
	_umap<_tchar*, FMOD::Sound*>		m_Sounds;
	FMOD::Channel*						m_Channels[MAX_CHANNEL];
	FMOD::System*						m_pSystem = { nullptr };
	
public:
    virtual void Free() override;
};

END