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
	_umap<const _tchar*, class CComponent*>* Get_Components() { return &m_Components; }

public:
	HRESULT			Initialize_Level(_uint iCurrentLevelIndex);
	HRESULT			Add_Component(const _tchar* pComponentTag, class CComponent* pComponent);
	// 레이어 클리어
	HRESULT			Clear_Layer() {
		for (auto& pComponent : m_Components)
			Safe_Release(pComponent.second);

		m_Components.clear();

		return S_OK;
	}
	HRESULT			Delete_Component(const _tchar* pComponentTag);

public:
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

public:
	class CComponent* Find_Component(const _tchar* pComponentTag);

private:
	_umap<const _tchar*, class CComponent*>	m_Components;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END