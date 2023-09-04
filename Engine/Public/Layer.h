#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
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
	HRESULT		Add_Component(const _tchar* pComponentTag, class CComponent* pComponent);
	const unordered_map<const _tchar*, class CComponent*> Get_Components() const { return m_Components; }
	// ���̾� Ŭ����
	HRESULT			Clear_Layer();
	HRESULT			Delete_Component(const _tchar* pComponentTag);

public:
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

public:
	class CComponent* Find_Component(const _tchar* pComponentTag);

private:
	unordered_map<const _tchar*, class CComponent*>	m_Components;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END