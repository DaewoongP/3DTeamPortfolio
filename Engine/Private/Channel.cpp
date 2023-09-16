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
	// ä�ΰ� �̸��� ���� ���� �ε����� ã�� ��������� ����.
	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue) {
		if (!lstrcmp(m_szName, pValue->Get_Name()))
			return true;
		else
		{
			++m_iBoneIndex;
			return false;
		}
		});

	// SRT �� ���� ������������ ���� Ű�������� ã�� ����.
	// ��������� �������� �������� ���� ������ ������ ���������� �������� �������� ���������� �װ��� �����ϸ� �ǹǷ�. ���� ū ���� �����Ѵ�.	
	m_iNumKeyFrames = max(Channel.iNumScalingKeys, Channel.iNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, Channel.iNumPositionKeys);

	
	_float3	vScale;
	ZEROMEM(&vScale);
	_float4	vRotation;
	ZEROMEM(&vRotation);
	_float3	vTranslation;
	ZEROMEM(&vTranslation);

	// SRT�� ���� ���� �������� ���� ���� ��ȸ�ϸ� �� �����ӿ����� ���°��� ����.
	// ���� �ش� �����ӿ��� ���� ���� ��� ���������� ó���� ������ ����.
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

HRESULT CChannel::Initialize(const Engine::CHANNEL_GCM& Channel, const CModel::BONES& Bones)
{
	lstrcpy(m_szName, Channel.szName);
	// ä�ΰ� �̸��� ���� ���� �ε����� ã�� ��������� ����.
	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue) {
		if (!lstrcmp(m_szName, pValue->Get_Name()))
			return true;
		else
		{
			++m_iBoneIndex;
			return false;
		}
		});

	// SRT �� ���� ������������ ���� Ű�������� ã�� ����.
	// ��������� �������� �������� ���� ������ ������ ���������� �������� �������� ���������� �װ��� �����ϸ� �ǹǷ�. ���� ū ���� �����Ѵ�.	
	m_iNumKeyFrames = Channel.iNumMartixKeys;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		MATRIXFRAME				Keyframe;

		Keyframe.vScale = Channel.iMatrixFrame[i].vScale;
		Keyframe.vRotation = Channel.iMatrixFrame[i].vRotation;
		Keyframe.vTranslation = Channel.iMatrixFrame[i].vTranslation;
		Keyframe.fTime = Channel.iMatrixFrame[i].fTime;

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

	// ������ Ű������ �ϰ�� ���º�ȯ�� �״�� ����.
	if (fTimeAcc >= LastKeyFrame.fTime)
	{
		vScale = LastKeyFrame.vScale;
		vRotation = LastKeyFrame.vRotation;
		vTranslation = LastKeyFrame.vTranslation;
		// return LastKeyFrame;
	}
	else
	{
		// ���� TimeAcc ���� (Index ~ Index + 1) �ȿ� �����ϴ��� �˻�.
		// ���� �ȿ� �������� ������� �������� ���� ������ �˻�.
		while (fTimeAcc >= m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime)
			++(*pCurrentKeyFrameIndex);

		// TimeAcc - Index.Time / (Index.Time ~ Index.Time + 1)
		_float		fRatio = (fTimeAcc - m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].fTime) /
			(m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime - m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].fTime);

		// ���� �ε����� ���� �ε��� ���� ���º�ȯ���� ���������ϱ� ���� �� ����.
		_float3		vSourScale = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vScale;
		_float4		vSourRotation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vRotation;
		_float3		vSourTranslation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vTranslation;

		_float3		vDestScale = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale;
		_float4		vDestRotation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation;
		_float3		vDestTranslation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation;

		// �������� �Լ�. Rotation�� ��� Quaternion ���¿��� Slerp �Լ� ���.
		vScale = vScale.Lerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vTranslation = vTranslation.Lerp(vSourTranslation, vDestTranslation, fRatio);
	}

	// ���������Ǿ� ���۵� ���͸� ��Ŀ� ��� ä���� �̸��� ���� ���� �ε����� �� ����.
	_float4x4	TransformationMatrix = XMMatrixAffineTransformation(vScale, _float4(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Invalidate_TransformationMatrix_Lerp(CModel::BONES& Bones, _float fTimeAcc, _uint* pCurrentKeyFrameIndex, _float fLerpTimeAcc)
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

			// ���� �ε����� ���� �ε��� ���� ���º�ȯ���� ���������ϱ� ���� �� ����.
			_float3		vSourScale = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vScale;
			_float4		vSourRotation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vRotation;
			_float3		vSourTranslation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex)].vTranslation;

			_float3		vDestScale = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale;
			_float4		vDestRotation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation;
			_float3		vDestTranslation = m_MatrixKeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation;

			vSourRotation = XMQuaternionNormalize(vSourRotation);
			vDestRotation = XMQuaternionNormalize(vDestRotation);

			// �������� �Լ�. Rotation�� ��� Quaternion ���¿��� Slerp �Լ� ���.
			vScale = vScale.Lerp(vSourScale, vDestScale, fRatio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
			vTranslation = vTranslation.Lerp(vSourTranslation, vDestTranslation, fRatio);
		}
	}
	//������ų �� ������ �ڵ�
	if (fLerpTimeAcc > ANIMATIONLERPTIME)
		fLerpTimeAcc = ANIMATIONLERPTIME;
	
	_float		fRatio = fLerpTimeAcc / ANIMATIONLERPTIME;
	_float3		vSourScale = Bones[m_iBoneIndex]->Get_TransformationMatrix_Scale();
	_float4		vSourRotation = XMQuaternionRotationMatrix(Bones[m_iBoneIndex]->Get_TransformationMatrix());
	_float3		vSourTranslation = Bones[m_iBoneIndex]->Get_TransformationMatrix_Position();

	//�̰� �ɸ� ū�ϳ�.
	//if (m_iBoneIndex == 3)
	//	return;
	vSourRotation = XMQuaternionNormalize(vSourRotation);
	vRotation = XMQuaternionNormalize(vRotation);

	vScale = vScale.Lerp(vSourScale, vScale, fRatio);
	_float4 vFinalRotation = XMQuaternionSlerp(vSourRotation, vRotation, fRatio);
	vTranslation = vTranslation.Lerp(vSourTranslation, vTranslation, fRatio);

	vFinalRotation = XMQuaternionNormalize(vFinalRotation);

	_float4x4	TransformationMatrix = XMMatrixAffineTransformation(vScale, _float4(0.f, 0.f, 0.f, 1.f), vFinalRotation, vTranslation);
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);	

	////Ƣ�°� ������ ����� ���� �߰��ؾ��ҵ���.
	//_float4 vAfterRotation = XMQuaternionRotationMatrix(Bones[m_iBoneIndex]->Get_TransformationMatrix());
	//if (m_iBoneIndex==3/*vAfterRotation != vFinalRotation*/)
	//{
	//	vFinalRotation = XMVectorSet(0, 0, 0, 0);
	//	//vFinalRotation = XMQuaternionSlerp(vSourRotation, vRotation, fRatio);
	//	vFinalRotation.x = Lerp(vSourRotation.x, vRotation.x, fRatio*-1);
	//	//vFinalRotation.y = Lerp(vSourRotation.y, vRotation.y, fRatio);
	//	//vFinalRotation.z = Lerp(vSourRotation.z, vRotation.z, fRatio);
	//	vFinalRotation.w = Lerp(vSourRotation.w, vRotation.w, fRatio*-1);
	//	
	//	vFinalRotation = XMQuaternionNormalize(vFinalRotation);

	//	_float4x4 scaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
	//	_float4x4 rotationMatrix = XMMatrixRotationQuaternion(vFinalRotation);
	//	_float4x4 transMatrix = XMMatrixTranslation(vTranslation.x, vTranslation.y, vTranslation.z);

	//	_float4x4	TransformationMatrix = scaleMatrix * rotationMatrix * transMatrix;
	//	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
	//}
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

CChannel* CChannel::Create(const Engine::CHANNEL_GCM& Channel, const CModel::BONES& Bones)
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
