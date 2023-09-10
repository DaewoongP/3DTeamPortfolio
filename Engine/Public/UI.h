#pragma once
#include "GameObject.h"
#include "UI_Group.h"

BEGIN(Engine)

class ENGINE_DLL CUI :  public CGameObject
{
public:
	enum UI_ID { NONE, BUTTON, PROGRESS, UI_ID_END };

protected:
	explicit CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	_float2		Get_vCombinedXY() { return m_vCombinedXY; }
	_float2		Get_fXY() { return _float2(m_fX, m_fY); }
	_float		Get_fZ() { return m_fZ; }
	_float2		Get_fSize() { return _float2(m_fSizeX, m_fSizeY); }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_fXY(_float fX, _float fY)
	{
		m_fX = fX;
		m_fY = fY;
	}
	void Set_fZ(_float fZ) { m_fZ = fZ; }
	void Set_Size(_float fX, _float fY)
	{
		m_fSizeX = fX;
		m_fSizeY = fY;
	}

	void Set_bParent() { m_isParent = true; }
	void Set_Parent(CUI * pParent, _bool isSave = false)
	{
		if (nullptr == m_pParent)
		{
			if (isSave)
			{
				m_pParent = pParent;
			}
			else
			{
				m_pParent = pParent;
				Set_fXY(0.f, 0.f);
			}
		}
	}
	void	Set_vColor(_float4 vColor) { m_vColor = vColor; }
	void	Set_bAlpha(_bool bAlpha) { m_isAlpha = bAlpha; }
	void	Set_AlphaTextureTag(_tchar * pTag)
	{
		lstrcpy(m_wszAlphaTexturePrototypeTag, pTag);
	}
	void	Set_AlphaTexturePath(_tchar * pPath)
	{
		lstrcpy(m_wszAlphaTextureFilePath, pPath);
	}
	void	Set_eUIID(UI_ID eType) { m_eUIType = eType; }


protected: // None이 사용하는 변수
	_float2			m_vCombinedXY = { 0.f, 0.f };
	// 윈도우창의 실제 x좌표
	_float			m_fX = { 650.f };
	// 윈도우창의 실제 y좌표
	_float			m_fY = { 360.f };
	// UI 거리 체크용 z값
	_float			m_fZ = { 0.f };
	// 텍스처 x사이즈 설정
	_float			m_fSizeX = { 100.f };
	// 텍스처 y사이즈 설정
	_float			m_fSizeY = { 100.f };

	_float			m_fWinSizeX = { 1280.f };
	_float			m_fWinSizeY = { 640.f };


	_float4x4		m_ViewMatrix;
	_float4x4		m_ProjMatrix;

protected:
	_tchar			m_wszTextureName[MAX_PATH] = {};
	_tchar			m_wszTexturePath[MAX_PATH] = {};

	UI_ID			m_eUIType = { UI_ID_END };
	_bool			m_isParent = { false };
	CUI*			m_pParent = { nullptr };

protected:
	_bool			m_isAlpha = { false };
	_float4			m_vColor = { 1.f, 1.f, 1.f, 1.f };
	_tchar			m_wszAlphaTexturePrototypeTag[MAX_PATH] = TEXT("");
	_tchar			m_wszAlphaTextureFilePath[MAX_PATH] = TEXT("");

protected:
	_bool			m_isSave = { false };

public:
	_bool	Is_In_Rect(HWND hWnd);

public:
	_float2	UIPos_To_WorldPos(_float fX, _float fY);
	_float2 WorldPos_To_UIPos(_float fX, _float fY);

protected:
	HRESULT Change_Position(_float fX, _float fY);
	HRESULT Change_Scale(_float fX, _float fY);

public:
	HRESULT	Save(HANDLE hFile, _ulong& dwByte);
	HRESULT	Load(HANDLE hFile, _ulong& dwByte);

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END