#pragma once
#include "MapObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CVault_Torch final : public CMapObject
{
private:
	explicit CVault_Torch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVault_Torch(const CVault_Torch& rhs);
	virtual ~CVault_Torch() = default;

public:
	_uint Get_TorchIndex() const { return m_iTorchIndex; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	// Torch �ѹ���. �������� 1, 2, 3 �̷��� ��������.
	// �� �������� �� 12���� Torch�� �ִ�.
	_uint m_iTorchIndex = { 0 }; 

public:
	static CVault_Torch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END