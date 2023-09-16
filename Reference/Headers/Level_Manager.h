#pragma once

/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)

private:
	explicit CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	_uint Get_CurrentLevelIndex() const { return m_iLevelIndex; }
	class CLevel* Get_CurrentLevel() { return m_pCurrentLevel; }
	list<const _tchar*> Get_Layers(const _tchar* pSceneTag);
	
public:
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	void	Tick(_float fTimeDelta);
	HRESULT Render();

public:
	HRESULT Add_Scene(const _tchar* pSceneTag, const _tchar* pLayerTag);

private:
	class CLevel*	m_pCurrentLevel = { nullptr };
	_uint			m_iLevelIndex = { 0 };

private:
	unordered_map<const _tchar*, list<const _tchar*>>	m_Scenes;

public:
	virtual void Free() override;
};

END