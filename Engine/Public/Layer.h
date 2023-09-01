#pragma once
/* =============================================== */
// 
//	정 : 박대웅
//	부 :
//
/* =============================================== */


#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT			Add_GameObjects(const _tchar* pGameObjectTag, class CGameObject* pGameObject);
	const unordered_map<const _tchar*, class CGameObject*> Get_GameObjects() const { return m_GameObjects; }
	// 레이어 클리어
	HRESULT			Clear_Layer();
public:
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

public:
	class CGameObject* Find_GameObject(const _tchar* pGameObjectTag);

private:
	unordered_map<const _tchar*, class CGameObject*>	m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END