#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI :  public CGameObject
{
public:
	typedef struct tagUIDesc
	{
		_float2 vCombinedXY;
		_float	fX = { 0.f };
		_float	fY = { 0.f };
		_float	fZ = { 0.f };
		_float	fSizeX = { 0.f };
		_float	fSizeY = { 0.f };
		_tchar szTexturePath[MAX_PATH] = TEXT("");
		_tchar szAlphaTexturePath[MAX_PATH] = TEXT("");
		_float4 vColor;
	}UIDESC;

protected:
	explicit CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	_float2 Get_XY() { return _float2(m_fX, m_fY); }
	void Set_Parent(CUI* pUIParent);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Change_Position(_float fX, _float fY, _float fZ, _uint iWinSizeX = 1280.f, _uint iWinSizeY = 720.f);
	HRESULT Change_Scale(_float fX, _float fY);
	HRESULT Load(UIDESC UIDesc);

protected:
	_float2			m_vCombinedXY = { 0.f, 0.f };
	// 윈도우창의 실제 x좌표
	_float			m_fX = { 0.f };
	// 윈도우창의 실제 y좌표
	_float			m_fY = { 0.f };
	// UI 거리 체크용 z값
	_float			m_fZ = { 0.f };
	// 텍스처 x사이즈 설정
	_float			m_fSizeX = { 0.f };
	// 텍스처 y사이즈 설정
	_float			m_fSizeY = { 0.f };

	_float4x4		m_ViewMatrix;
	_float4x4		m_ProjMatrix;

protected:
	CUI*					m_pParent = { nullptr };
	vector<class CTexture*>	m_Textures;
	vector<class CTexture*>	m_AlphaTextures;

protected:
	void Make_Matrix(_float fWinSizeX, _float fWinSizeY);
	void Make_CombinedXY();

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END