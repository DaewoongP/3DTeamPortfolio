#pragma once
#include "UI_Group.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_Group_Skill final : public CUI_Group
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

public:
	struct UISKILLDESC
	{
		HFILE			hfile;
		_float2			vCombinedXY = { 0.f, 0.f };
		_float			fX = { 0.f };
		_float			fY = { 0.f };
		_float			fZ = { 0.f };
		_float			fSizeX = { 0.f };
		_float			fSizeY = { 0.f };
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
	HRESULT Add_ProtoType();
	void	Add_SpellProtoTypeTag();
	HRESULT Add_SpellProtoType();
	HRESULT Add_SpellTexture();

private:
	void	Load_Skill_1(void* pArg);
	HRESULT	Load_Skill_2();
	HRESULT	Load_Skill_3();
	HRESULT	Load_Skill_4();

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
	UISKILLDESC		m_SkillDesc;

	HFILE			m_hFile;

public:
	static CUI_Group_Skill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END