#pragma once

/* =============================================== */
// 
//	정 : 박대웅
//	부 :
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
	// 이거 꼭 Initialize_Engine에 넣어서 초기화 실행해주세요
    HRESULT Initialize();
	// 파일경로에 있는 사운드를 자동으로 파일이름을 태그값으로 입력해줌
	HRESULT Add_Sounds(const _tchar* pSoundFilePath);
	
public:
	// forceplay 활성화하면 현재 재생중인 사운드 무시하고 실행함.
	// default 매개변수 설정하는것을 추천함.
	_int Play_Sound(const _tchar* pSoundTag, _float fVolume);
	// 랜덤으로 실행할 사운드 개수와 태그 입력
	// sound%d.wav, 10 -> 10개중에 랜덤 사운드 재생
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