#pragma once
#include "Composite.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CModel_LOD final : public CComposite
{
public:
	typedef struct tagLODModelDesc
	{
		_uint			iLevelIndex = { 0 };
		_tchar			szPrototypeName[MAX_STR] = TEXT("");
		CModel::TYPE	eType = { CModel::TYPE_END };
	}LODDESC;

private:
	explicit CModel_LOD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CModel_LOD(const CModel_LOD& rhs);
	virtual ~CModel_LOD() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components(const LODDESC& LodDesc);

public:
	static CModel_LOD* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* szModelFilePath, _uint iNumModel = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END