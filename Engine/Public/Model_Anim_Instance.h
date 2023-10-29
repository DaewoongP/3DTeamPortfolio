#pragma once

// 정 : 주성환
// anim mesh 인스턴싱을 위한 Model 클래스

#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Anim_Instance : public CModel
{
private:
	CModel_Anim_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Anim_Instance(const CModel_Anim_Instance& rhs);
	virtual ~CModel_Anim_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	static CModel_Anim_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath,
		_float4x4* pInstanceMatrix, _uint iInstanceCnt, _float4x4 PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END