#pragma once
#include "MapObject.h"

BEGIN(Client)

class CPlayer;

class CDoor final : public CMapObject
{
private: // INSIDE�� �ȿ��� �Ͼ��, OUTSIDE�� �ۿ��� �Ͼ�� ��Ȳ�̴�.
	enum DOORSTATE {OPEN_OUTSIDE, OPEN_INSIDE, CLOSE_OUTSIDE, CLOSE_INSIDE, DOOR_END};

protected:
	explicit CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDoor(const CDoor& rhs);
	virtual ~CDoor() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CPlayer* m_pPlayer = { nullptr };			 // �÷��̾� �ּ�

private:
	_float			m_fDist_From_Player = { 0.f }; // ���� �÷��̾� �Ÿ�
	_float			m_fDoorSpinValue = { 0.f };	   // ���� ��ŭ ȸ���ߴ��� 

	_bool			m_isDoorAction = { false }; // ���� �������� �ƴ��� �Ǵ� ����

private:
	DOORSTATE		m_eCurrentState = { DOOR_END }; // ���� ����

private:
	void Door_Action(_float fTimeDelta);

public:
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

#define DOORTURN 90.f // �� ȸ����