#pragma once
#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)

private:
	explicit CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pObjectTag, void* pArg);
	class CGameObject* Find_GameObject_In_Layer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pGameObjectTag);
	void	Clear_LevelResources(_uint iLevelIndex);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

	
public:
	void	Tick(_float fTimeDelta);
	void	Late_Tick(_float fTimeDelta);

private:
	typedef unordered_map<const _tchar*, class CLayer*>		LAYERS;
	unordered_map<const _tchar*, class CGameObject*>	m_Prototypes;	

private:
	LAYERS*				m_pLayers = { nullptr };
	_uint				m_iNumLevels = { 0 };

private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END