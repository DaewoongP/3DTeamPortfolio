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
		CModel::TYPE	eModelType = { CModel::TYPE_END };
		_float4x4		PivotMatrix = XMMatrixIdentity();
	}LODDESC;

public:
	_uint Get_NumMeshes() const;
	vector<CMesh*>* Get_MeshesVec() { return m_pCurrentModel->Get_MeshesVec(); }
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, Engine::TextureType MaterialType);
	void Set_MaxLodDistance(_float fMaxDistance) { m_fMaxLodDistance = fMaxDistance; }

private:
	explicit CModel_LOD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CModel_LOD(const CModel_LOD& rhs);
	virtual ~CModel_LOD() = default;

public:
	virtual HRESULT Initialize_Prototype(LODDESC& LodDesc, const _tchar* szModelFilePath, _uint iNumModel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_float fTimeDelta);

private:
	// 프로토타입 이름, 레벨
	vector<pair<_tchar*, _uint>>			m_LodPrototypes;
	vector<_tchar*>							m_ModelComponentTags;

private:
	CModel*									m_pCurrentModel = { nullptr };
	_float									m_fMaxLodDistance = { 0.f };
	_uint									m_iNumModels = { 0 };

private:
	HRESULT Add_Components();
	void Check_CameraDistance();

public:
	static CModel_LOD* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LODDESC& LodDesc, const _tchar* szModelFilePath, _uint iNumModel = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END