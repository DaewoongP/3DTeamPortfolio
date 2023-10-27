#pragma once
#include "MapObject.h"
#include "Client_Defines.h"

#define MAX_FIRES  1

BEGIN(Engine)
class CBone;
class CParticleSystem;
END

BEGIN(Client)

class CFireHouse final : public CMapObject
{
private: // 어떤 집인지
	enum HOUSETYPE{ HOUSE_GEN_F,	// A 모양 지붕이 4개가 붙어있는 집
					HOUSE_GEN_G,	// 초가집처럼 생긴 집
					HOUSE_HONEY,	// 트롤 트리거 오른쪽에 있음. HoneyDukes 라는 간판이 있다.
					HOUSE_QUILL,	// 공원 바로 왼쪽에 있는 집
					HOUSE_TEASHOP,	// 트롤 트리거 왼쪽에 있음. 앞에 테이블과 의자가 있다.
					HOUSE_DB_GR,	// 가운데 다리같은 구조물이 있는 건물 -- 
					HOUSE_END
	};

private:
	explicit CFireHouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFireHouse(const CFireHouse& rhs);
	virtual ~CFireHouse() = default;

public:
	void Set_FireOn() { m_isFireOn = true; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HOUSETYPE m_eHouseType = { HOUSE_END };

private:
	vector<CParticleSystem*>	m_Particles;
	vector<_float3>				m_FirePosition;		// 불의 위치
	vector<_uint>				m_Indices;

private:
	_bool	m_isFireOn = { false };

private:
	void	FireOn();	// 불이야!

public:
	static CFireHouse* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END