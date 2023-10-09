#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_Slot;
class CUI_Back;
class CUI_Effect_Back;
class CUI_Group_Cursor;

class CUI_Group_SkillTap final : public CGameObject
{
private:
	enum SKILLTAP
	{
		ARRESTOMOMENTUM, GALCIUS, LEVIOSO, TRANSFORMATIONOVERLAND,
		ACCIO, DEPULSO, DESCENDO, FILPENDO,
		CONFRINGO, DIFFINDO, EXPELLIAMUS, EXPULSO,
		INCENDIO, DISILLUSIONMENT, LUMOS, REPARE,
		WINGARDIUM, AVADAKEDAVRA, CRUCIO, IMPERIUS,
		SKILLTAP_END
	};

private:
	explicit CUI_Group_SkillTap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_SkillTap(const CUI_Group_SkillTap& rhs);
	virtual ~CUI_Group_SkillTap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Prototype();

private:
	HRESULT Create_Component_Select(const _tchar* pFIlePath, wstring wszTag, SKILLTAP eType);
	HRESULT Add_Components_Select(wstring wszTag);
	HRESULT Read_File_Select(const _tchar* pFilePath, SKILLTAP iIndex);
	CUI::UIDESC Load_File(const HANDLE hFile, _bool isDDS = false);

private:
	HRESULT	Ready_DefaultTexture();

	HRESULT	Ready_SkillTextures();

	HRESULT Ready_SkillSelect();
	HRESULT Ready_SkillSlot();



private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	// 선택가능한 UI
	vector<CUI_Effect_Back*>			m_pSelectFrames;
	vector<CUI_Back*>					m_pSelectIcons;
	vector<CUI_Effect_Back*>			m_pSelectBacks;

	// 왼쪽 실질적인 UI
	vector<CUI_Effect_Back*>			m_pSlotMains;
	vector<CUI_Back*>					m_pSlotFrames;
	vector<CUI_Back*>					m_pSlotNumbers;
	vector<CTexture*>					m_SkillTextures;


	CUI_Back*						m_pBack;
	CUI_Group_Cursor*				m_pCursor;

private:
	_bool		m_isOpen = { false };

public:
	static CUI_Group_SkillTap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END