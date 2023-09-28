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


	enum COLLISIONFLAG
	{
		COL_NONE			= 0,
		COL_PLAYER			= 1 << 0,
		COL_ENEMY			= 1 << 1,
		COL_ENEMY_RANGE		= 1 << 2,
		COL_NPC				= 1 << 3,
		COL_NPC_RANGE		= 1	<< 4,
		COL_WEAPON			= 1 << 5,
		COL_MAGIC			= 1 << 6,
		COL_STATIC			= 1 << 7,
		COL_TRIGGER			= 1 << 8,
		COL_ITEM			= 1 << 9,
		// COL~~ = 1 << 10,
		// ... ��� �߰����ֽø� �˴ϴ�!! ���ڸ� ��� ���������ּ���

		COL_TEST = 1 << 29, // �浹 ������ �ִ�ġ�Դϴ�. ����� �߰��ϼŵ� �˴ϴ�.
							// �� TEST�� ����� �߰��ϽǶ��� ������ �������ּ���
	};

	enum ISCOLLISION
	{
		END_COL,
		USE_COL,
	};
}
