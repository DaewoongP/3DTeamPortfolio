#pragma once
#include "Camera.h"

class PotionStationCamera : public CCamera
{
public:
	typedef struct tagPotionStationCameraDesc
	{
		tagCameraDesc pSuperDesc;
		_float3 vAt = {};
	}POTIONSTATION_CAMERA_DESC;

private:
	explicit PotionStationCamera() = default;
	explicit PotionStationCamera(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	virtual ~PotionStationCamera() = default;

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(const _float & fTimeDelta);

private:
private:
	_float3 m_vAt = {};
	_float3 m_vEye = {};
public:
	static PotionStationCamera* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, void* pArg);
	virtual void Free() override;

};

