#pragma once

/* =============================================== */
//	[CCustomModel]
//	- Ŀ���͸���¡ ���� ��
//  - Ŀ���͸���¡ ���� �ִϸ��̼��� �����Ѵٰ� �����ϰ� �������
// 
//	�� : �ּ�ȯ
//	�� : ��ö��
//
/* =============================================== */

#include "Component.h"
#include "MeshParts.h"

BEGIN(Engine)

class ENGINE_DLL CCustomModel : public CComponent
{
public:
	enum MESHTYPE { HEAD, ARM, ROBE, UPPERBODY, UNDERBODY, SOCKS, SHOES, MESH_END };

private:
	explicit CCustomModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCustomModel(const CCustomModel& rhs);
	virtual ~CCustomModel() = default;

public:
	const _float4x4* Get_PivotFloat4x4Ptr() const {
		return &m_PivotMatrix;
	}
	_float4x4 Get_PivotFloat4x4() const {
		return m_PivotMatrix;
	}
	_uint Get_CurrentAnimIndex() const {
		return m_MeshParts[HEAD]->Get_CurrentAnimIndex();
	}
	// ���� �޽� ���� ��ȯ
	_uint Get_NumMeshes(const _uint& _iMeshPartsIndex) const {
		if (nullptr == m_MeshParts[_iMeshPartsIndex])
			return 0;

		return m_MeshParts[_iMeshPartsIndex]->Get_NumMeshes();
	}
	// ���� �ִϸ��̼� ���� ��ȯ
	_uint Get_NumAnimations() const {
		return m_MeshParts[HEAD]->Get_NumAnimations();
	}
	const class CBone* Get_Bone(const _tchar* pBoneName);
	// ���� �������� �ִϸ��̼�
	class CAnimation* Get_Animation() const {
		return m_MeshParts[HEAD]->Get_Animation();
	}
	class CAnimation* Get_Animation(const _uint& _iAnimationIndex) const {
		return m_MeshParts[HEAD]->Get_Animation(_iAnimationIndex);
	}
	const _float4x4* Get_BoneCombinedTransformationMatrixPtr(_uint iIndex);
	_float4x4 Get_BoneCombinedTransformationMatrix(_uint iIndex);
	HRESULT Add_MeshParts(const _uint& _iMeshPartsIndex, const wstring& _wstrMeshPartsFilePath);

public:
	virtual HRESULT Initialize_Prototype(_float4x4 PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex);

public:
	// Lerp�� �������� �ʰ� �ش� �ִϸ��̼� �ʱ���·� �ٷ� �����Ѵ�.
	void	Reset_Animation(const _uint& _iAnimIndex) {
		//m_MeshParts[HEAD]->Reset_Animation(_iAnimIndex);
		m_MeshParts[ARM]->Reset_Animation(_iAnimIndex);
	}
	void	Play_Animation(const _float& _fTimeDelta) {
		//m_MeshParts[HEAD]->Play_Animation(_fTimeDelta);
		m_MeshParts[ARM]->Play_Animation(_fTimeDelta);
	}
	HRESULT Find_BoneIndex(const _tchar* _pBoneName, _uint* _iIndex);

public:
	HRESULT Bind_Material(class CShader* _pShader, const char* _pConstantName,
		const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex, Engine::TextureType _MaterialType);
	HRESULT Bind_BoneMatrices(class CShader* _pShader, const char* _pConstantName, const _uint& _iMeshPartsIndex, const _uint& _iMeshIndex);

private: /* For.MeshParts */
	array<class CMeshParts*, MESH_END>	m_MeshParts = { nullptr };

private:
	_float4x4						m_PivotMatrix;

public:
	static CCustomModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float4x4 PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END