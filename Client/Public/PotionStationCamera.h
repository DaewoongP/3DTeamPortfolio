#pragma once
#include "Camera.h"

class CPotionStationCamera : public CCamera
{
public:
	typedef struct tagPotionStationCameraDesc
	{
		tagCameraDesc pSuperDesc;
		_float3 vAt = {};
	}POTIONSTATION_CAMERA_DESC;

private:
	explicit CPotionStationCamera() = default;
	explicit CPotionStationCamera(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	virtual ~CPotionStationCamera() = default;

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(const _float & fTimeDelta);
#ifdef _DEBUG
	void Tick_Imgui(_float fTimeDelta);
#endif // _DEBUG
private:
	_float3 m_vAt = {};
	_float3 m_vEye = {};
public:
	static CPotionStationCamera* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, void* pArg);
	virtual void Free() override;

};

