#pragma once
#include "MapObject.h"

BEGIN(Client)

class CLightStand;

class CSanctum_Door final : public CMapObject
{
private: // ���ʹ� �����ʹ� ����
	enum DOORTYPE {LEFTDOOR, RIGHTDOOR, DOOR_END};

private:
	explicit CSanctum_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSanctum_Door(const CSanctum_Door& rhs);
	virtual ~CSanctum_Door() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	const _float	m_fDoorTurn = { 90.f };		   // ���� �ִ� ȸ����
	_float			m_fDoorSpinValue = { 0.f };	   // ���� ��ŭ ȸ���ߴ��� 

	_bool			m_isDoorAction = { false }; // ���� �������� �ƴ��� �Ǵ� ����

	vector<CLightStand*>	m_pLightStands;		// ȭ�� ����

private:
	DOORTYPE		m_eDoorType = { DOOR_END }; // ���� ����

private:
	void Check_FireOn();					// ȭ�ο� ���� �پ����� Ȯ���ϴ� �Լ�
	void Door_Action(_float fTimeDelta);	// ���ǿ� ���� ���� ������ �Լ�

public:
	static CSanctum_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END