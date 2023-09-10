#pragma once
#include "GameObject.h"


BEGIN(Engine)

class ENGINE_DLL CUI_Gruop : public CGameObject
{
protected:
	explicit CUI_Gruop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Gruop(const CUI_Gruop& rhs);
	virtual ~CUI_Gruop() = default;

public:
	const _tchar* Get_GroupName() const { return m_wszUIGroupName; }
	class CUI* Get_Parent() { return  m_pParent; }
	vector <class CUI*>* Get_Childs() { return &m_Childs; }
	//	_bool	Get_isSelected() { return m_isSelected; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	void	Set_Parent(class CUI* pParent)
	{
		if (nullptr != m_pParent)
		{
			MSG_BOX("Parent is full");
			return;
		}
		m_pParent = pParent;
	}
	void	Set_Parent();
	void	Set_Child(class CUI* pChild) { m_Childs.push_back(pChild); }
	//	void	Set_isSelected(_bool isSelected = true) { m_isSelected = isSelected; }


protected:
	_tchar	m_wszUIGroupName[MAX_STR] = {};

protected:
	class CUI* m_pParent = { nullptr };
	vector <class CUI*>	m_Childs;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END