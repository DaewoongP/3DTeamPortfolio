#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CInfo_Main final : public CGameObject
{
public:
	enum INFOLIST
	{
		GEAR,
		INVENTORY,
		//TALENT,
		COLLECTION,
		ACTION,
		SETTING,
		OWLMAIL,
		QUEST,
		MAP,
		INFOLIST_END
	};

private:
	explicit CInfo_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInfo_Main(const CInfo_Main& rhs);
	virtual ~CInfo_Main() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Prototype();
	//	HRESULT Add_Components(const _tchar* wszTag);
	HRESULT Add_Components(wstring wszTag);

	HRESULT Read_File(const _tchar* pFilePath, INFOLIST iIndex);
	CUI::UIDESC Load_File(const HANDLE hFile);

private:
	HRESULT Create_First(void* pArg);
	//	HRESULT Create_Component(const _tchar* pFIlePath, const _tchar* wszTag, KEYLIST eType);
	HRESULT Create_Component(const _tchar* pFIlePath, wstring wszTag, INFOLIST eType);

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CTexture*			m_pTexture = { nullptr };

private:
	vector<class CUI_Effect_Back*>	m_pFrames;
	vector<class CUI_Effect_Back*>	m_pMains;
	vector<class CUI_Back*>			m_pBacks;
	vector<class CUI_Back*>			m_pLines;

public:
	static CInfo_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END