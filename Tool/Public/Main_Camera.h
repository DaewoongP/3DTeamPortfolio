#pragma once
#include "Tool_Defines.h"
#include "Camera.h"

BEGIN(Tool)

class CMain_Camera : public CCamera
{
private:
	explicit CMain_Camera() = default;
	explicit CMain_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMain_Camera() = default;

	//for. Get, Set
public:
	const _float& Get_MoveSeed() const { return m_fMoveSpeed; }
	

public:
	void Set_MoveSpeed(const _float& _fMoveSpeed);
	void IsLimitButton();
	void Set_Position(_float3 _vPos);

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(const _float& fTimeDelta);


private:
	//카메라 이동 속도
	_float m_fMoveSpeed = { 0.0f };

	//카메라 방향 제한
	_bool m_isLimit{ false };
	bool test = { false };

private:
	void	Key_Input(_float _TimeDelta);
	void	Mouse_Input(_float _TimeDelta);

	

public:
	static CMain_Camera* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg);
	virtual void Free() override;
	 
};

END

