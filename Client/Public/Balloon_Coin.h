#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CBalloon_Coin final : public CGameObject
{
public:
	enum COIN {
		COIN_BLACK, COIN_BRONZE, COIN_SILVER, COIN_GOLD, COIN_RAINBOW, COIN_END
	};

	typedef struct tagCoinDesc
	{
		COIN	eCoinColorType;
		_float2 vScale;
		_float3	vPosition;
		_float	fCoinSpeed;
	}COINDESC;

private:
	explicit CBalloon_Coin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBalloon_Coin(const CBalloon_Coin& rhs);
	virtual ~CBalloon_Coin() = default;

public:
	void Reset();

public:
	HRESULT Initialize(void* pArg) override;
	void Tick(_float fTimeDelta) override;
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	CShader*						m_pShader = { nullptr };
	CTexture*						m_pTexture = { nullptr };
	CRenderer*						m_pRenderer = { nullptr };
	CVIBuffer_Point_Instance*		m_pBuffer = { nullptr };

private:
	_uint							m_iCoinMaxIndex = { 0 };
	_uint							m_iCoinIndex = { 0 };
	COIN							m_eCoinColor = { COIN_END };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBalloon_Coin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END