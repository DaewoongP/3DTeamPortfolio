#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CGame_Manager final : public CBase
{
	DECLARE_SINGLETON(CGame_Manager)
private:
	explicit CGame_Manager() = default;
	virtual ~CGame_Manager() = default;

public:
	CGameObject* Get_GameObject(const _tchar* pStaticObjectTag);
	HRESULT Set_GameObject(const _tchar* pStaticObjectTag, CGameObject* pGameObject);

private:
	CGameObject* Find_Object(const _tchar* pStaticObjectTag);

private:
	_umap<const _tchar*, CGameObject*>	m_StaticObjects;

public:
	virtual void Free() override;
};

END