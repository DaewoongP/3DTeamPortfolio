#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CBase
{
public:
	enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOTLIGHT, TYPE_END };

	typedef struct tagLightDesc
	{
		TYPE	eType;
		_float4	vDir;
		_float4 vLookAt;
		_float4	vPos;
		_float	fRange;
		_float	fSpotPower;

		_float4	vDiffuse;
		_float4	vAmbient;
		_float4	vSpecular;

		// Instance Light 
		_bool isIncrease;
		_float fIncreasePower;
		_float fDecreaseStartRange;
		_float fTime;
	}LIGHTDESC;

private:
	explicit CLight() = default;
	virtual ~CLight() = default;

public:
	const LIGHTDESC* Get_LightDesc() const { return &m_LightDesc; }
	void Set_LightDesc(LIGHTDESC LightDesc);
	void Set_Position(_float4 vPosition);
	void Set_Range(_float fRange);

	_bool Is_Increase() { return m_LightDesc.isIncrease; }

public:
	HRESULT Initialize(const LIGHTDESC& LightDesc);
	_bool Tick_Increase(_float fTimeDelta);
	_bool Tick_Decrease(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	LIGHTDESC					m_LightDesc;
	
private:
	_float						m_fTimeAcc = { 0.f };

public:
	static CLight* Create(const LIGHTDESC& LightDesc);
	virtual void Free() override;
};

END