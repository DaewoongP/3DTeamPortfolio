#include "..\Public\Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

void CChannel::Delete_Translation()
{
	_float3 vInitPos = m_MatrixKeyFrames[0].vTranslation;

	for (auto& KeyFrame : m_MatrixKeyFrames)
		KeyFrame.vTranslation = vInitPos;
}

void CChannel::Delete_Rotation()
{
	_float4 vInitRotation = m_MatrixKeyFrames[0].vRotation;

	for (auto& KeyFrame : m_MatrixKeyFrames)
		KeyFrame.vRotation = vInitRotation;
}

HRESULT CChannel::Initialize(const Engine::CHANNEL& Channel, const CModel::BONES& Bones)
{
	lstrcpy(m_szName, Channel.szName);
	// 채널과 이름이 같은 뼈의 인덱스를 찾아 멤버변수에 저장.
	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue) {
		if (!lstrcmp(m_szName, pValue->Get_Name()))
			return true;
		else
		{
			++m_iBoneIndex;
			return false;
		}
		});

	// SRT 중 가장 많은프레임을 가진 키프레임을 찾아 저장.
	// 상대적으로 적은수의 프레임을 가진 변수는 어차피 마지막값을 기준으로 프레임이 끝날때까지 그값을 유지하면 되므로. 가장 큰 값만 저장한다.
	m_iNumKeyFrames = max(Channel.iNumScalingKeys, Channel.iNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, Channel.iNumPositionKeys);

	_float3	vScale;
	ZEROMEM(&vScale);
	_float4	vRotation;
	ZEROMEM(&vRotation);
	_float3	vTranslation;
	ZEROMEM(&vTranslation);

	// SRT중 가장 많은 프레임을 가진 값을 순회하며 각 프레임에서의 상태값을 저장.
	// 만약 해당 프레임에서 값이 없을 경우 마지막으로 처리한 값으로 저장.
	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		MATRIXFRAME				Keyframe;

		if (Channel.iNumScalingKeys > i)
		{
			vScale = Channel.ScalingKeys[i].vValue;
			Keyframe.fTime = Channel.ScalingKeys[i].fTime;
		}

		if (Channel.iNumRotationKeys > i)
		{
			vRotation = Channel.RotationKeys[i].vValue;
			Keyframe.fTime = Channel.RotationKeys[i].fTime;
		}

		if (Channel.iNumPositionKeys > i)
		{
			vTranslation = Channel.PositionKeys[i].vValue;
			Keyframe.fTime = Channel.PositionKeys[i].fTime;
		}

		Keyframe.vScale = vScale;
		Keyframe.vRotation = vRotation;
		Keyframe.vTranslation = vTranslation;

		m_MatrixKeyFrames.push_back(Keyframe);
	}

	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(CModel::BONES& Bones, _float fTimeAcc, _Inout_ _uint* pCurrentKeyFrameIndex)
{
	if (0.f == fTimeAcc)
		*pCurrentKeyFrameIndex = 0;

	MATRIXFRAME LastKeyFrame = m_MatrixKeyFrames.back();

	_float3	vScale;
	ZEROMEM(&vScale);
	_float4	vRotation;
	ZEROMEM(&vRotation);
	_float3	vTranslation;
	ZEROMEM(&vTranslation);

	// 마지막 키프레임 일경우 상태변환을 그대로 유지.
	if (fTimeAcc >= LastKeyFrame.fTime)
	{
		vScale = LastKeyFrame.vScale;
		vRotation = LastKeyFrame.vRotation;
		vTranslation = LastKeyFrame.vTranslation;
		// return LastKeyFrame;
	}
	else
	{
		// 현재 TimeAcc 값이 (Index ~ Index + 1) 안에 존재하는지 검사.
		// 만약 안에 존재하지 않을경우 증가시켜 다음 프레임 검사.
		while (fTimeAcc >= m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime)
			++(*pCurrentKeyFrameIndex);

		// TimeAcc - Index.Time / (Index.Time ~ Index.Time + 1)
		_float		fRatio = (fTimeAcc - m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].fTime) /
			(m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime - m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].fTime);

		// 현재 인덱스와 다음 인덱스 까지 상태변환들을 선형보간하기 위한 값 저장.
		_float3		vSourScale = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vScale;
		_float4		vSourRotation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vRotation;
		_float3		vSourTranslation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vTranslation;

		_float3		vDestScale = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale;
		_float4		vDestRotation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation;
		_float3		vDestTranslation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation;

		// 선형보간 함수. Rotation의 경우 Quaternion 형태여서 Slerp 함수 사용.
		vScale = vScale.Lerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vTranslation = vTranslation.Lerp(vSourTranslation, vDestTranslation, fRatio);
	}

	// 선형보간되어 제작된 벡터를 행렬에 담아 채널의 이름과 같은 뼈의 인덱스에 값 전달.
	_float4x4	TransformationMatrix = XMMatrixAffineTransformation(vScale, _float4(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Invalidate_TransformationMatrix_Lerp(CModel::BONES& Bones, _float fTimeAcc, _uint* pCurrentKeyFrameIndex, _double LerpTimeAcc)
{
	if (0.f == fTimeAcc)
		*pCurrentKeyFrameIndex = 0;

	MATRIXFRAME LastKeyFrame = m_MatrixKeyFrames.back();

	_float3	vScale;
	ZEROMEM(&vScale);
	_float4	vRotation;
	ZEROMEM(&vRotation);
	_float3	vTranslation;
	ZEROMEM(&vTranslation);

	//기존 뼈 움직임코드
	{
		if (fTimeAcc >= LastKeyFrame.fTime)
		{
			vScale = LastKeyFrame.vScale;
			vRotation = LastKeyFrame.vRotation;
			vTranslation = LastKeyFrame.vTranslation;
		}
		else
		{
			while (fTimeAcc >= m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime)
				++(*pCurrentKeyFrameIndex);

			// TimeAcc - Index.Time / (Index.Time ~ Index.Time + 1)
			_float		fRatio = (fTimeAcc - m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].fTime) /
				(m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime - m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].fTime);

			// 현재 인덱스와 다음 인덱스 까지 상태변환들을 선형보간하기 위한 값 저장.
			_float3		vSourScale = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vScale;
			_float4		vSourRotation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vRotation;
			_float3		vSourTranslation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vTranslation;

			_float3		vDestScale = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale;
			_float4		vDestRotation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation;
			_float3		vDestTranslation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation;

			// 선형보간 함수. Rotation의 경우 Quaternion 형태여서 Slerp 함수 사용.
			vScale = vScale.Lerp(vSourScale, vDestScale, fRatio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
			vTranslation = vTranslation.Lerp(vSourTranslation, vDestTranslation, fRatio);
		}
	}
	//러프시킬 뼈 움직임 코드
	
	if (LerpTimeAcc > ANIMATIONLERPTIME)
		LerpTimeAcc = ANIMATIONLERPTIME;
	
	_float fRatio = LerpTimeAcc / ANIMATIONLERPTIME;
	_float3		vSourScale = Bones[m_iBoneIndex]->Get_TransformationMatrix_Scale();
	_float4		vSourRotation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vRotation;
	_float3		vSourTranslation = Bones[m_iBoneIndex]->Get_TransformationMatrix_Position();

	/*XMVectorSet(Bones[m_iBoneIndex]->Get_TransformationMatrix().ToEuler().x, Bones[m_iBoneIndex]->Get_TransformationMatrix().ToEuler().y, Bones[m_iBoneIndex]->Get_TransformationMatrix().ToEuler().z, 0);
	_float3		vSourScale = Bones[m_iBoneIndex]->Get_Scale();
	_float4		vSourRotation = _float4(0.f, 0.f, 0.f, 0.f);
	_float3		vSourTranslation = Bones[m_iBoneIndex]->Get_Position();
	XMStoreFloat4(&vSourRotation, XMQuaternionRotationMatrix(XMLoadFloat4x4(&Bones[m_iBoneIndex]->Get_TransformationMatrix())));*/



	vScale = vScale.Lerp(vSourScale, vScale, fRatio);
	vRotation = XMQuaternionSlerp(vSourRotation, vRotation, fRatio);
	vTranslation = vTranslation.Lerp(vSourTranslation, vTranslation, fRatio);

	_float4x4	TransformationMatrix = XMMatrixAffineTransformation(vScale, _float4(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Lerp_TransformationMatrix(CModel::BONES& Bones, CChannel* pCurrentChannel, _float fDuration, _float fTimeAcc, _uint iCurrentKeyFrameIndex)
{
	_float3	vScale;
	ZEROMEM(&vScale);
	_float4	vRotation;
	ZEROMEM(&vRotation);
	_float3	vTranslation;
	ZEROMEM(&vTranslation);

	while (fTimeAcc >= fDuration)
		return;

	_float		fRatio = fTimeAcc / fDuration;

	_float3		vSourScale = m_MatrixKeyFrames[iCurrentKeyFrameIndex].vScale;
	_float4		vSourRotation = m_MatrixKeyFrames[iCurrentKeyFrameIndex].vRotation;
	_float3		vSourTranslation = m_MatrixKeyFrames[iCurrentKeyFrameIndex].vTranslation;

	_float3		vDestScale = pCurrentChannel->m_MatrixKeyFrames[0].vScale;
	_float4		vDestRotation = pCurrentChannel->m_MatrixKeyFrames[0].vRotation;
	_float3		vDestTranslation = pCurrentChannel->m_MatrixKeyFrames[0].vTranslation;

	// 선형보간 함수. Rotation의 경우 Quaternion 형태여서 Slerp 함수 사용.
	vScale = vScale.Lerp(vSourScale, vDestScale, fRatio);
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
	vTranslation = vTranslation.Lerp(vSourTranslation, vDestTranslation, fRatio);

	// 선형보간되어 제작된 벡터를 행렬에 담아 채널의 이름과 같은 뼈의 인덱스에 값 전달.
	_float4x4	TransformationMatrix = XMMatrixAffineTransformation(vScale, _float4(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const Engine::CHANNEL& Channel, const CModel::BONES& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(Channel, Bones)))
	{
		MSG_BOX("Failed to Created CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
}
