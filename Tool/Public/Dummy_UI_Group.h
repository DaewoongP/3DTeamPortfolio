#pragma once
#include "UI_Group.h"
#include "Tool_Defines.h"

BEGIN(Tool)

class CDummy_UI_Group final : public CGameObject
{
public:
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

		_uint			m_eType;

		_bool				m_isParent;

		_bool				m_isAlpha;
		_float4				m_vColor;
		_tchar				m_wszAlphaTexturePrototypeTag[MAX_PATH] = TEXT("");
		_tchar				m_wszAlphaTextureFilePath[MAX_PATH] = TEXT("");
	};
private:
	explicit CDummy_UI_Group(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDummy_UI_Group(const CDummy_UI_Group& rhs);
	virtual ~CDummy_UI_Group() = default;

public:
	const _tchar* Get_GroupName() const { return m_wszUIGroupName; }
	class CDummy_UI* Get_Parent() { return  m_pParent; }
	vector <class CDummy_UI*>* Get_Childs() { return &m_Childs; }

	void	Set_Parent(class CDummy_UI* pParent)
	{
		if (nullptr != m_pParent)
		{
			MSG_BOX("Parent is full");
			return;
		}
		m_pParent = pParent;
	}
	void	Set_Parent();
	void	Set_Child(class CDummy_UI* pChild) { m_Childs.push_back(pChild); }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	HRESULT	Save(const _tchar* pFileDirectory);
	HRESULT	Load(const _tchar* pFileDirectory);
	void	Delete(class CDummy_UI* pDeleteUI);
	void	Clear();

private:
	_tchar	m_wszUIGroupName[MAX_STR] = {};
	_tchar* m_wszUIProtoTypeTag;

private:
	class CDummy_UI* m_pParent = { nullptr };
	vector <class CDummy_UI*>	m_Childs;

private:
	UI_DATA		m_UIDesc;

public:
	static CDummy_UI_Group* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END