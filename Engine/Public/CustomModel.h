#pragma once

/* =============================================== */
//	[CCustomModel]
//	- Ŀ���͸���¡ ���� ��
//  - Ŀ���͸���¡ ���� ���� �ִϸ��̼� ������ ������ ��
// 
//	�� : �ּ�ȯ
//	�� : ��ö��
//
/* =============================================== */

#include "Model.h"

BEGIN(Engine)
class CTransform;
class ENGINE_DLL CCustomModel : public CModel
{
public:
	enum MESHTYPE { HAT, HAIR, HEAD, MASK, ARM, ROBE, TOP, PANTS, SOCKS, SHOES, MESH_END };

private:
	explicit CCustomModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCustomModel(const CCustomModel& rhs);
	virtual ~CCustomModel() = default;

public:
	// ���� �޽� ���� ��ȯ
	_uint								Get_NumMeshes(const _uint& _iMeshPartsIndex) const;
	array<class CMeshParts*, MESH_END>	Get_MeshParts() const { return m_MeshParts; }
	void								Set_WindVelocity(_float3 vWindVelocity);

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const wstring& _wstrModelFilePath, _float4x4 _PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	// ���� �޽� Tick ȣ��
	virtual void Tick(const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex, _float _fTimeDelta);
	virtual HRESULT Render(const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex);

public:
	HRESULT	Add_MeshParts(const _uint& _iLevelIndex, const wstring& _wstrPrototypeTag,
		MESHTYPE _eMeshPartsType, const _float4& _vColor = _float4(1.f, 1.f, 1.f, 1.f), const _tchar* _szClothDataFilePath = nullptr);
	virtual HRESULT Bind_Material(class CShader* _pShader, const char* _pConstantName,
		const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex, Engine::TextureType _MaterialType);
	virtual HRESULT Bind_BoneMatrices(class CShader* _pShader, const char* _pConstantName, const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex);
	HRESULT Bind_Color(class CShader* _pShader, const char* _pConstantName, const _uint& _iMeshPartsIndex);

private: /* For.MeshParts */
	array<class CMeshParts*, MESH_END>	m_MeshParts = { nullptr };

public:
	static CCustomModel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, TYPE eType, const wstring& _wstrModelFilePath, _float4x4 _PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END