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
		COL_PLAYER			= 1 << 0,
		COL_ENEMY			= 1 << 1,
		COL_ENEMY_RANGE		= 1 << 2,
		COL_NPC				= 1 << 3,
		COL_NPC_RANGE		= 1	<< 4,
		COL_WEAPON			= 1 << 5,
		// COL~~ = 1 << 6,
		// ... ��� �߰����ֽø� �˴ϴ�!! ���ڸ� ��� ���������ּ���

		COL_TEST = 1 << 29, // �浹 ������ �ִ�ġ�Դϴ�. ����� �߰��ϼŵ� �˴ϴ�.
							// �� TEST�� ����� �߰��ϽǶ��� ������ �������ּ���

		COL_ALL = (1 << 30) - 1, // �ٸ� ��� ��ü�� �浹
	};

	enum ISCOLLISION
	{
		END_COL,
		USE_COL,
	};
}
