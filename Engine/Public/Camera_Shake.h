#pragma once
/* =============================================== */
// 
//	정 : 장현우
//	부 :
//
/* =============================================== */

#include "Component.h"
#include "Camera_Manager.h"
#include "Transform.h"

BEGIN(Engine)

class CPipeLine;

END

BEGIN(Engine)

class ENGINE_DLL CCamera_Shake final : public CComponent
{
public:
	typedef struct tagCamera_Shake_Desc
	{
		_bool isDistanceOption = { false };
		CTransform* pTransform = { nullptr };
		CCamera_Manager::SHAKE_PRIORITY eShake_Priority = { CCamera_Manager::SHAKE_PRIORITY_END };
		CCamera_Manager::SHAKE_INFO_DESC Shake_Info_Desc = { CCamera_Manager::SHAKE_INFO_DESC() };
	}CAMERA_SHAKE_DESC;

private:
	explicit CCamera_Shake(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CCamera_Shake(const CCamera_Shake& rhs);
	virtual ~CCamera_Shake() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	//거리 비례 파워 측정용
	//Min, Max, Power
	void Ready_Shake(_float _fMax, _float _fMin, _float _fPower);
	
	//right 축을 random % 30 각 으로 회전 시킨 각으로 쉐이크
	void RandomRightAxisShake();
	//up 축을 random % 30 각 으로 회전 시킨 각으로 쉐이크
	void RandomUpAxisShake();

	//원하는 축으로 지속시간동안 한번만 갔다오는 쉐이크(각축의 반대 방향을 원한다면 구조체의 파워를 음수로 설정한다.)
	void GoAndComeBackShake();
	
	void Shake();

private:
	//거리 비례 파워 변수
	_float					m_fMaxCameraShakeDistance = { 0.0f };
	_float					m_fMinCameraShakeDistance = { 0.0f };
	_float					m_fDistanceRatio = { 0.0f };

	_float					m_fShakePower = { 0.0f };
	
	CCamera_Manager*		m_pCamera_Manager = { nullptr };
	CPipeLine*				m_pPipeLine = { nullptr };

	CAMERA_SHAKE_DESC m_Camera_Shake_Desc = { CAMERA_SHAKE_DESC() };

private:
	//Power
	void Shake_Power();


public:
	static CCamera_Shake* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END