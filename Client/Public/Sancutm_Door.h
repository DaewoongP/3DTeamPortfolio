#pragma once
#include "MapObject.h"

BEGIN(Client)

class CLightStand;

class CSancutm_Door final : public CMapObject
{
private: // ���ʹ� �����ʹ� ����
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
	_float			m_fDoorSpinValue = { 0.f };	   // ���� ��ŭ ȸ���ߴ��� 

	_bool			m_isDoorAction = { false }; // ���� �������� �ƴ��� �Ǵ� ����

	vector<CLightStand*>	m_pLightStands;		// ȭ�� ����

private:
	DOORTYPE		m_eCurrentState = { DOOR_END }; // ���� ����

private:
	void Door_Action(_float fTimeDelta);

public:
	static CSancutm_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END