#pragma once
#include "GameObject.h"
#include "UI.h"

BEGIN(Engine)

class ENGINE_DLL CUI_Gruop : public CGameObject
{
	struct UI_DATA
	{
		UI_DATA()
		{
			isSave = { false };

			vCombinedXY = { 0.f, 0.f };
			m_fX = { 0.f };
			m_fY = { 0.f };
			m_fZ = { 0.f };

			m_fSizeX = { 0.f };
			m_fSizeY = { 0.f };

			ZeroMemory(m_wszTextureName, MAX_PATH);

			m_eType = { CUI::UI_ID::UI_ID_END };
			m_isParent = { false };

			m_isAlpha = { false };
			m_vColor = { 1.f, 1.f, 1.f, 1.f };
			ZeroMemory(m_wszAlphaTexturePrototypeTag, MAX_PATH);
			ZeroMemory(m_wszAlphaTextureFilePath, MAX_PATH);
		}

		_bool				isSave;

		_float2				vCombinedXY;
		_float				m_fX;
		_float				m_fY;
		_float				m_fZ;

		_float				m_fSizeX;
		_float				m_fSizeY;

		_tchar				m_wszTextureName[MAX_PATH];
		_tchar				m_wszTexturePath[MAX_PATH];

		CUI::UI_ID	m_eType;

		_bool				m_isParent;

		_bool				m_isAlpha;
		_float4				m_vColor;
		_tchar				m_wszAlphaTexturePrototypeTag[MAX_PATH] = TEXT("");
		_tchar				m_wszAlphaTextureFilePath[MAX_PATH] = TEXT("");
	};

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


	HRESULT	Save(const _tchar* pFileDirectory);
	HRESULT	Load(const _tchar* pFileDirectory);

protected:
	_tchar	m_wszUIGroupName[MAX_STR] = {};


	_tchar* m_wszUIProtoTypeTag;

protected:
	class CUI* m_pParent = { nullptr };
	vector <class CUI*>	m_Childs;

private:
	UI_DATA		m_UIDesc;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END