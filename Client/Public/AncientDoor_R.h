#pragma once
#include "MapObject.h"

BEGIN(Engine)
class CRigidBody;;
END

BEGIN(Client)

class CPlayer;

class CAncientDoor_R final : public CMapObject
{
private: // INSIDE는 안에서 일어나는, OUTSIDE는 밖에서 일어나는 상황이다.
	enum DOORSTATE { OPEN_OUTSIDE, OPEN_INSIDE, CLOSE_OUTSIDE, CLOSE_INSIDE, DOOR_END };

protected:
	explicit CAncientDoor_R(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAncientDoor_R(const CAncientDoor_R& rhs);
	virtual ~CAncientDoor_R() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CRigidBody* m_pRigidBody = { nullptr };
	CPlayer* m_pPlayer = { nullptr };			 // 플레이어 주소

private:
	_float			m_fDist_From_Player = { 0.f }; // 문과 플레이어 거리
	_float			m_fDoorSpinValue = { 0.f };	   // 문이 얼만큼 회전했는지 

	_bool			m_isDoorAction = { false }; // 문의 움직일지 아닐지 판단 여부

private:
	DOORSTATE		m_eCurrentState = { DOOR_END }; // 문의 상태

private:
	void Door_Action(_float fTimeDelta);

public:
	static CAncientDoor_R* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#define DOORTURN 90.f // 문 회전값