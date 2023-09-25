#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "UI_Image.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)
class CPlayer;

class CUI_Group_Skill final : public CGameObject
{
private:
	enum KEYLIST
	{
		FIRST,
		SECOND,
		THIRD,
		FOURTH,
		KEYLIST_END
	};

private:
	explicit CUI_Group_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_Skill(const CUI_Group_Skill& rhs);
	virtual ~CUI_Group_Skill() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	void	Set_SpellTexture(KEYLIST eSkill, SPELL eSpell);

private:
	HRESULT Add_Prototype();
	HRESULT Add_Components(wstring wszTag);

	HRESULT Read_File(const _tchar* pFilePath, KEYLIST iIndex);
	CUI::UIDESC Load_File(const HANDLE hFile);

private:
	HRESULT Create_First(const _tchar* pFilePath);
	HRESULT Create_Component(const _tchar* pFIlePath, wstring wszTag, KEYLIST eType);

	void	Add_SpellProtoTypeTag();
	HRESULT Add_SpellProtoType();
	HRESULT Add_SpellTexture();

private:
	vector<_tchar*> m_ProtoTypeTags;
	vector<_tchar*> m_SpellProtoTypeTags;

private:
	vector<class CUI_Effect_Back*>	m_pMains;
	vector<class CUI_Back*>			m_pFrames; 
	vector<class CUI_Back*>			m_pNumbers;
	vector<class CTexture*>			m_SkillTextures;

private:
	SPELL			m_KeyList[KEYLIST_END];

public:
	static CUI_Group_Skill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END