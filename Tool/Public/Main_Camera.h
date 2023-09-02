#pragma once
#include "Tool_Defines.h"
#include "Camera.h"

BEGIN(Tool)

class CMain_Camera : public CCamera
{
private:
	explicit CMain_Camera() = default;
	virtual ~CMain_Camera() = default;

	//for. Get, Set
public:
	const _float Get_MoveSeed() const { return m_fMoveSpeed; }
	

public:
	void Set_MoveSpeed(const _float& _fMoveSpeed);



public:
	HRESULT Initialize(void* pArg);
	void Tick(_float fTimeDelta);


private:
	//카메라 이동 속도
	_float m_fMoveSpeed = { 0.0f };

private:
	void	Key_Input(_float _TimeDelta);
	void	Mouse_Input(_float _TimeDelta);


public:
	static CMain_Camera* Create();
	virtual void Free() override;
	 
};

END

