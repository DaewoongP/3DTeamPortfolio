#pragma once
#include "MapObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CParticleSystem;
END

BEGIN(Client)

class CFireHouse final : public CMapObject
{
private: // � ������
	enum HOUSETYPE{ HOUSE_GEN_F,	// A ��� ������ 4���� �پ��ִ� ��
					HOUSE_GEN_G,	// �ʰ���ó�� ���� ��
					HOUSE_HONEY,	// Ʈ�� Ʈ���� �����ʿ� ����. HoneyDukes ��� ������ �ִ�.
					HOUSE_QUILL,	// ���� �ٷ� ���ʿ� �ִ� ��
					HOUSE_TEASHOP,	// Ʈ�� Ʈ���� ���ʿ� ����. �տ� ���̺��� ���ڰ� �ִ�.
					HOUSE_DB_GR,		// ��� �ٸ����� �������� �ִ� �ǹ�
					HOUSE_END
	};

private:
	explicit CFireHouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFireHouse(const CFireHouse& rhs);
	virtual ~CFireHouse() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HOUSETYPE m_eHouseType = { HOUSE_END };

public:
	static CFireHouse* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END