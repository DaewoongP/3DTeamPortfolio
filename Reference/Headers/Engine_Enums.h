#pragma once

namespace Engine
{
	enum CELLFLAG
	{
		// �Ϲ� �̵������� ��
		CELL_MOVE = 1 << 0,
		// Ư�� Y������ �������� ���ó���Ǵ� �������¼�.
		CELL_FALL = 1 << 1,
	};
}
