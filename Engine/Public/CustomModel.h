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
class CShader;
class CTransform;
class CMeshParts;
END

BEGIN(Engine)

class ENGINE_DLL CCustomModel : public CModel
{
public:
	enum MESHTYPE { HAT, HAIR, HEAD, MASK, ARM, ROBE, TOP, PANTS, SOCKS, SHOES, MESH_END };
	typedef struct tagPartsDesc
	{
		CMeshParts* pMeshParts = { nullptr };
		wstring wstrPrototypeTag = { TEXT("") };
	}PARTSDESC;

#pragma region �ڵ� �߷�
private:
	explicit CCustomModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCustomModel(const CCustomModel& rhs);
	virtual ~CCustomModel() = default;

public:
	// ���� �޽� ���� ��ȯ
	_uint								Get_NumMeshes(const _uint& _iMeshPartsIndex) const;
	array<PARTSDESC, MESH_END>			Get_MeshParts() { return m_MeshParts; }
	void								Set_WindVelocity(_float3 vWindVelocity);
	// �κ� ���� ���Ƿ� ����
	HRESULT								Set_Top_For_Robe(const _uint& _iLevelIndex);
	// �κ� ���� ���Ƿ� ����
	HRESULT								Set_Top_For_NonRobe(const _uint& _iLevelIndex);
	_bool								is_Equip_Robe() const { return m_isRobeTop; }
	// ���� �޽� Tick ȣ��
	virtual void Tick(const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex, _float _fTimeDelta);
#pragma endregion

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const wstring& wstrModelFilePath, _float4x4 PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(const _uint& iMeshPartsIndex, const _uint& iMeshIndex);

public:
	HRESULT	Add_MeshParts(const _uint& iLevelIndex, const wstring& wstrPrototypeTag,
		MESHTYPE eMeshPartsType, const _float4& vColor = _float4(1.f, 1.f, 1.f, 1.f), 
		const _tchar* szClothDataFilePath = nullptr);
	virtual HRESULT Bind_Material(CShader* pShader, const char* pConstantName,
		const _uint& iMeshPartsIndex, const _uint& iMeshIndex, Engine::TextureType MaterialType);
	virtual HRESULT Bind_BoneMatrices(CShader* pShader, const char* pConstantName, 
		const _uint& iMeshPartsIndex, const _uint& iMeshIndex);
	HRESULT Bind_Color(CShader* pShader, const char* pConstantName, const _uint& iMeshPartsIndex);

private: /* For.MeshParts */
	array<PARTSDESC, MESH_END>	m_MeshParts = { nullptr };
	// ���� �κ� ���� ��������
	_bool m_isRobeTop = { false }; 

public:
	static CCustomModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		TYPE eType, const wstring& wstrModelFilePath, _float4x4 PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END