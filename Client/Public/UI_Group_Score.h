#pragma once

#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CHealth;
END

BEGIN(Client)
class CUI_Back;
class CUI_Font;

class CUI_Group_Score final : public CGameObject
{
private:
	enum UISCOREENUM
	{
		FIRST,
		SECOND,
		THIRD,
		FOURTH,
		FIFTH,
		SIXTH,
		SEVENTH,
		EIGHTH,
		NINTH,
		UISCORE_END
	};

public:
	struct UISCOREDESC
	{
		_tchar		wszFirstName[MAX_PATH] = TEXT("");
		_tchar		wszSecondName[MAX_PATH] = TEXT("");
		_tchar		wszThirdName[MAX_PATH] = TEXT("");
		_tchar		wszFourthName[MAX_PATH] = TEXT("");
		_tchar		wszFifthName[MAX_PATH] = TEXT("");
		_tchar		wszSixthName[MAX_PATH] = TEXT("");
		_tchar		wszSeventhName[MAX_PATH] = TEXT("");
		_tchar		wszEighthName[MAX_PATH] = TEXT("");
		_tchar		wszNinthName[MAX_PATH] = TEXT("");

		vector<_uint>*		pScore = { nullptr };
	};

	struct UISCORE
	{
		CUI_Back* pBack = { nullptr };
		CUI_Font* pName = { nullptr };
		CUI_Font* pScore = { nullptr };

		_uint* fScore = { nullptr };
	};

private:
	explicit CUI_Group_Score(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_Score(const CUI_Group_Score& rhs);
	virtual ~CUI_Group_Score() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	_tchar				m_wszFirstName[MAX_PATH] = TEXT("");
	_tchar				m_wszSecondName[MAX_PATH] = TEXT("");
	_tchar				m_wszThirdName[MAX_PATH] = TEXT("");
	_tchar				m_wszFourthName[MAX_PATH] = TEXT("");

	vector<_float2>		m_vOffset;
	vector<UISCORE>		m_UIScores;

	_uint				Text1 = 0;
	_uint				Text2 = 0;
	_uint				Text3 = 0;
	_uint				Text4 = 0;
	_uint				Text5 = 0;
	_uint				Text6 = 0;
	_uint				Text7 = 0;
	_uint				Text8 = 0;
	_uint				Text9 = 0;


private:
	HRESULT		Add_Prototype();

	HRESULT		Create_UI_Score(_tchar* pFontFont, _uint* fScore, UISCOREENUM eType);
	HRESULT		Add_Components(_tchar* pFontFont, UISCORE* pUI, UISCOREENUM eType);

	HRESULT		Add_Font(const _tchar* pFont, UISCORE* pUI);
	HRESULT		Ready_Offset();

private:
	void		Update_Score(_float fTimeDelta);

public:
	static CUI_Group_Score* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

// 160 75