#pragma once

#include "Tool.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CCoolTime;
class CHealth;
END

BEGIN(Client)

class CChineseChompingCabbageItem final : public CTool
{
private:
	CChineseChompingCabbageItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChineseChompingCabbageItem(const CChineseChompingCabbageItem& rhs);
	virtual ~CChineseChompingCabbageItem() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Use(_float3 vPlayPos) override;
	virtual void CreateTool() override;
private:
	HRESULT Add_Components();

public:
	static CChineseChompingCabbageItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;


};
END
