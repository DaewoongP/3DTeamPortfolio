#pragma once
#include "MapObject.h"

BEGIN(Client)

class CLightStand;

class CSancutm_Door final : public CMapObject
{
private: // 왼쪽문 오른쪽문 구분
	enum DOORTYPE {LEFTDOOR, RIGHTDOOR, DOOR_END};

private:
	explicit CSancutm_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSancutm_Door(const CSancutm_Door& rhs);
	virtual ~CSancutm_Door() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	const _float	m_fDoorTurn = { 90.f };		   // 문의 최대 회전값
	_float			m_fDoorSpinValue = { 0.f };	   // 문이 얼만큼 회전했는지 

	_bool			m_isDoorAction = { false }; // 문의 움직일지 아닐지 판단 여부

	vector<CLightStand*>	m_pLightStands;		// 화로 모음

private:
	DOORTYPE		m_eCurrentState = { DOOR_END }; // 문의 종류

private:
	void Door_Action(_float fTimeDelta);

public:
	static CSancutm_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END