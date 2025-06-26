#include<iostream>
#include<easyx.h>
#include<vector>
#include<ctime>
#include<cstdlib>
#include<windows.h>
#include<tchar.h>

using namespace std;
#define SIZE 9//�����׳��Ĵ�С

#define SIZE_REC 20//����ÿ�����ӵĴ�С

#define WIDTH (SIZE+2)*SIZE_REC//������Ϸ���ڵĿ��
#define HEIGHT SIZE*SIZE_REC//������Ϸ���ڵĸ߶�

#define RATE 0.13//�����Ѷ�ϵ��

int bombcount = SIZE * SIZE * RATE;//�����׵�����
int bomb_show = bombcount;//��Ļ����ʾ���׵�����

//�׳���ÿ��Ԫ�صĿ����ʾ״̬
enum CellState {
	COVERED,
	UNCOVERED,
	FLAGGED
};

//�׳��е�ÿ��Ԫ�ص�״̬
typedef struct map
{
	CellState state;//�����ʾ״̬
	bool isbomb;//�Ƿ�����
	int countbomb;//��Χ�м�����
}MAP;

//��ʼ���׳�
void initMap(vector<vector<MAP>>& m)
{
	for (int i = 0;i < SIZE;i++)
	{
		for (int j = 0;j < SIZE;j++)
		{
			m[i][j].state = COVERED;
			m[i][j].isbomb = FALSE;
			m[i][j].countbomb = 0;
		}
	}
}

//�Ҽ������ʾС����
void mark(vector<vector<MAP>>& m, int x, int y)
{
	setfillcolor(LIGHTGRAY);
	fillrectangle(x * SIZE_REC, y * SIZE_REC, (x + 1) * SIZE_REC, (y + 1) * SIZE_REC);//����С���Σ�4���������ֱ������Ͻǵĵ�����½ǵĵ㣩

	//���������ε���������
	POINT pts[] = { {(x + 0.5) * SIZE_REC,y * SIZE_REC},
		{(x + 0.5) * SIZE_REC,(y + 0.5) * SIZE_REC},
		{(x + 1) * SIZE_REC,(y + 0.25) * SIZE_REC} };
	setfillcolor(RED);//���������ɫΪ��ɫ
	fillpolygon(pts, 3);//����������
	line((x + 0.5) * SIZE_REC, (y + 0.5) * SIZE_REC, (x + 0.5) * SIZE_REC, (y + 1) * SIZE_REC);//�������
}

//��ӡ�׳�
void printMap(vector<vector<MAP>>& m)
{
	//������һ�δ�ӡ�Ķ���
	clearrectangle(SIZE * SIZE_REC, 0, WIDTH, SIZE_REC);

	//��ӡ�׵�����
	settextcolor(YELLOW);
	settextstyle(SIZE_REC, 0, _T("Arial"));//��������߶�Ϊ20���������Ӧ��_T֧��Unicode���룬����ΪArial
	TCHAR endstr[20];//TCHAR��һ���꣬����Unicode�汾��Windows�б�����Ϊwchar_t���ڷ�Unicode�汾�б�����Ϊchar����ʹ�ó����ܹ����ݱ��뻷���Զ�ѡ����ʵ��ַ����͡�
	_stprintf_s(endstr, _T("%d"), bomb_show);//���д���ʹ��sprintf_s����������bomb_showת��Ϊ�ַ��������洢��endstr�����С�_stprintf_s��sprintf_s��Unicode�汾��ͬ����Ϊ��֧��Unicode����
	outtextxy(SIZE * SIZE_REC, 0, endstr);//���д�����ָ����λ���������

	//��ӡ�׳�
	for (int i = 0;i < SIZE;i++)
	{
		for (int j = 0;j < SIZE;j++)
		{
			if (m[i][j].state == COVERED)
			{
				setfillcolor(LIGHTGRAY);
				fillrectangle(j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC);//����С����
			}
			else if (m[i][j].state == UNCOVERED)
			{
				if (m[i][j].isbomb)
				{
					setfillcolor(RED);
					fillrectangle(j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC);//����С����
					settextcolor(BLACK);
					RECT rc = { j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC };
					setbkmode(TRANSPARENT);
					drawtext('X', &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//����X,DT_CENTER: �����־��ʾ�ı����ھ���������ˮƽ���л��ơ�DT_VCENTER: �����־��ʾ�ı����ھ��������ڴ�ֱ���л��ơ�DT_SINGLELINE : �����־��ʾ�ı�����Ϊ���д����������з�������ζ�ż�ʹ�ı����ݰ������з�����Ҳ�ᱻ���ԣ��ı���һֱ������ͬһ���ϣ�ֱ���ﵽ���ε��ұ߽�Ϊֹ��
				}
				else
				{
					setfillcolor(DARKGRAY);
					fillrectangle(j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC);//����С����
					if (m[i][j].countbomb != 0)
					{
						if (m[i][j].countbomb == 1)
						{
							settextcolor(BLUE);
						}
						else if (m[i][j].countbomb == 2)
						{
							settextcolor(GREEN);
						}
						else
						{
							settextcolor(RED);
						}
						RECT r = { j * SIZE_REC,i * SIZE_REC,(j + 1) * SIZE_REC,(i + 1) * SIZE_REC };
						setbkmode(TRANSPARENT);
						TCHAR c = _T('0') + m[i][j].countbomb;
						drawtext(c, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//��������		
					}
				}
			}
			else if (m[i][j].state == FLAGGED)
			{
				mark(m, j, i);//����С����
			}
		}
	}
}

//���ɵ���
void generateBombs(vector<vector<MAP>>& m, int bombcount, int x, int y)//��֤�û���һ���������λ�ò�����
{
	int count = 0;
	while (count < bombcount)
	{
		int bomb_x = rand() % SIZE;//ȡģ��Ϊ�˱�֤ȡ��0-8֮��������
		int bomb_y = rand() % SIZE;
		if (bomb_x == x && bomb_y == y)
		{
			continue;
		}
		else if (m[bomb_y][bomb_x].isbomb)
		{
			continue;
		}
		else
		{
			m[bomb_y][bomb_x].isbomb = true;
			count++;
		}
	}
}

//��Ϸʧ�ܴ�ӡ����������Ϣ
void printBombs(vector<vector<MAP>>& m)
{
	for (int i = 0;i < SIZE;i++)
	{
		for (int j = 0;j < SIZE;j++)
		{
			if (m[i][j].isbomb)
			{
				setfillcolor(RED);
				fillrectangle(j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC);//����С����
				settextcolor(BLACK);
				settextstyle(SIZE_REC, 0, _T("Arial"));
				RECT rc = { j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC };
				setbkmode(TRANSPARENT);
				drawtext('X', &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
		}
	}
}

//�������޸�Ϊδ����
void dfs(vector<vector<MAP>>& m, int i, int j)
{
	int s = 0;
	if (i < 0 || j < 0 || i >= SIZE || j >= SIZE || m[i][j].state == UNCOVERED || m[i][j].state == FLAGGED)
		return;
	else if (m[i][j].isbomb)
	{
		m[i][j].state = UNCOVERED;
		return;
	}
	else
	{
		if (i + 1 < SIZE && m[i + 1][j].isbomb)
			s++;
		if (i - 1 >= 0 && m[i - 1][j].isbomb)
			s++;
		if (j + 1 < SIZE && m[i][j + 1].isbomb)
			s++;
		if (j - 1 >= 0 && m[i][j - 1].isbomb)
			s++;
		if (i - 1 >= 0 && j + 1 < SIZE && m[i - 1][j + 1].isbomb)
			s++;
		if (i + 1 < SIZE && j - 1 >= 0 && m[i + 1][j - 1].isbomb)
			s++;
		if (i + 1 < SIZE && j + 1 < SIZE && m[i + 1][j + 1].isbomb)
			s++;
		if (i - 1 >= 0 && j - 1 >= 0 && m[i - 1][j - 1].isbomb)
			s++;
		if (s > 0)
		{
			m[i][j].countbomb = s;
			m[i][j].state = UNCOVERED;
			return;
		}
		else
		{
			m[i][j].state = UNCOVERED;
			dfs(m, i + 1, j);
			dfs(m, i - 1, j);
			dfs(m, i, j - 1);
			dfs(m, i, j + 1);
			dfs(m, i + 1, j + 1);
			dfs(m, i + 1, j - 1);
			dfs(m, i - 1, j - 1);
			dfs(m, i - 1, j + 1);
		}
	}
}//�Ե�����ǵĸ��Ӻ��������dfs��
void uncoverCell(vector<vector<MAP>>& m, int x, int y)
{
	dfs(m, y, x);
}

//��Ϸ�Ƿ��ʤ
bool isWin(vector<vector<MAP>>& m)
{
	for (int i = 0;i < SIZE;i++)
	{
		for (int j = 0;j < SIZE;j++)
		{
			if ((m[i][j].state == FLAGGED && !m[i][j].isbomb) || (m[i][j].state == COVERED && m[i][j].isbomb))
			{
				return false;
			}
		}
	}
	return true;
}

//�����������¼�
void mouseClick(vector<vector<MAP>>& m, ExMessage& msg, bool& gameover, bool& isstart)
{
	//��ȡ�����λ��
	int x = msg.x / SIZE_REC;
	int y = msg.y / SIZE_REC;
	if (x >= 0 && x < SIZE && y >= 0 && y < SIZE)
	{
		if (m[y][x].state == COVERED)
		{
			if (m[y][x].isbomb)
			{
				uncoverCell(m, x, y);
				printMap(m);
				settextcolor(RED);
				settextstyle(50, 0, _T("Arial"));//��������߶�Ϊ20���������Ӧ��_T֧��Unicode���룬����ΪArial
				TCHAR endstr[20];//TCHAR��һ���꣬����Unicode�汾��Windows�б�����Ϊwchar_t���ڷ�Unicode�汾�б�����Ϊchar����ʹ�ó����ܹ����ݱ��뻷���Զ�ѡ����ʵ��ַ����͡�
				_stprintf_s(endstr, _T("��"));//���д���ʹ��sprintf_s�������ַ���"Game Over!"ת��Ϊ�ַ��������洢��endstr�����С�_stprintf_s��sprintf_s��Unicode�汾��ͬ����Ϊ��֧��Unicode����
				outtextxy(WIDTH / 2 - SIZE_REC, HEIGHT / 2 - SIZE_REC, endstr);//���д�����ָ����λ���������
				Sleep(1000);
				printBombs(m);
				settextcolor(RED);
				settextstyle(50, 0, _T("Arial"));//��������߶�Ϊ20���������Ӧ��_T֧��Unicode���룬����ΪArial
				outtextxy(WIDTH / 2 - SIZE_REC, HEIGHT / 2 - SIZE_REC, endstr);//���д�����ָ����λ���������
				system("pause");
				gameover = true;
				closegraph();
				exit(0);
			}
			else
			{
				if (!isstart)
				{
					isstart = true;
					generateBombs(m, bombcount, x, y);
				}
				uncoverCell(m, x, y);
				printMap(m);
			}
		}
	}
}

//����Ҽ������¼�
void mouseRightClick(vector<vector<MAP>>& m, ExMessage& msg, bool& gameover, bool& isstart)
{
	int x = msg.x / SIZE_REC;
	int y = msg.y / SIZE_REC;
	if (x >= 0 && x < SIZE && y >= 0 && y < SIZE)
	{
		if (m[y][x].state == COVERED && bomb_show > 0)
		{
			if (!isstart)
			{
				isstart = true;
				generateBombs(m, bombcount, x, y);
			}
			m[y][x].state = FLAGGED;
			bomb_show--;
		}
		else if (m[y][x].state == FLAGGED && bomb_show < bombcount)
		{
			m[y][x].state = COVERED;
			bomb_show++;
		}
		printMap(m);
		if (bomb_show == 0 && isWin(m))
		{
			gameover = true;
		}
	}
}

//������˫���¼�
void mouseDoubleClick(vector<vector<MAP>>& m, ExMessage& msg, bool& gameover)
{
	int x = msg.x / SIZE_REC;
	int y = msg.y / SIZE_REC;
	if (x >= 0 && x < SIZE && y >= 0 && y < SIZE)
	{
		if (m[y][x].state == UNCOVERED)
		{
			int flagcount = 0;
			for (int dx = -1;dx <= 1;++dx)
			{
				for (int dy = -1;dy <= 1;++dy)
				{
					int nx = x + dx;
					int ny = y + dy;
					if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && m[ny][nx].state == FLAGGED)
					{
						flagcount++;
					}

				}
			}
			if (flagcount == m[y][x].countbomb)
			{
				for (int dx = -1;dx <= 1;++dx)
				{
					for (int dy = -1;dy <= 1;++dy)
					{
						int nx = x + dx;
						int ny = y + dy;
						if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && m[ny][nx].state == COVERED)
						{
							if (m[ny][nx].isbomb)
							{
								uncoverCell(m, nx, ny);
								printMap(m);
								settextcolor(RED);
								settextstyle(50, 0, _T("Arial"));//��������߶�Ϊ20���������Ӧ��_T֧��Unicode���룬����ΪArial
								TCHAR endstr[20];//TCHAR��һ���꣬����Unicode�汾��Windows�б�����Ϊwchar_t���ڷ�Unicode�汾�б�����Ϊchar����ʹ�ó����ܹ����ݱ��뻷���Զ�ѡ����ʵ��ַ����͡�
								_stprintf_s(endstr, _T("��"));//���д���ʹ��sprintf_s�������ַ���"Game Over!"ת��Ϊ�ַ��������洢��endstr�����С�_stprintf_s��sprintf_s��Unicode�汾��ͬ����Ϊ��֧��Unicode����
								outtextxy(WIDTH / 2 - SIZE_REC, HEIGHT / 2 - SIZE_REC, endstr);//���д�����ָ����λ���������
								Sleep(1000);
								printBombs(m);
								settextcolor(RED);
								settextstyle(50, 0, _T("Arial"));//��������߶�Ϊ20���������Ӧ��_T֧��Unicode���룬����ΪArial
								outtextxy(WIDTH / 2 - SIZE_REC, HEIGHT / 2 - SIZE_REC, endstr);//���д�����ָ����λ���������
								system("pause");
								gameover = true;
								closegraph();
								exit(0);
							}
							else
							{
								uncoverCell(m, nx, ny);
								printMap(m);
							}
						}
					}
				}
			}
		}
	}
}

using namespace std;
int main()
{
	srand(time(NULL));//�����������

	initgraph(WIDTH, HEIGHT, EX_DBLCLKS);//�������ڣ����ΪWIDTH���߶�ΪHEIGHT��EX_DBLCLKS����˫���¼�

	vector<vector<MAP>> Map(SIZE, vector<MAP>(SIZE));//�����׳�

	initMap(Map);//��ʼ���׳�

	printMap(Map);//��ӡ�׳�

	bool gameover = false;
	bool isstart = false;

	while (!gameover)
	{
		ExMessage msg = { 0 };
		peekmessage(&msg, EX_MOUSE);//��ȡ��Ϣ
		if (msg.message == WM_LBUTTONUP)
		{
			mouseClick(Map, msg, gameover, isstart);
		}
		else if (msg.message == WM_RBUTTONUP)
		{
			mouseRightClick(Map, msg, gameover, isstart);
		}
		else if (msg.message == WM_LBUTTONDBLCLK)
		{
			mouseDoubleClick(Map, msg, gameover);
		}
	}

	settextcolor(BLUE);
	settextstyle(50, 0, _T("Arial"));//��������߶�Ϊ20���������Ӧ��_T֧��Unicode���룬����ΪArial
	setbkmode(TRANSPARENT);
	TCHAR endstr[20];//TCHAR��һ���꣬����Unicode�汾��Windows�б�����Ϊwchar_t���ڷ�Unicode�汾�б�����Ϊchar����ʹ�ó����ܹ����ݱ��뻷���Զ�ѡ����ʵ��ַ����͡�
	_stprintf_s(endstr, _T("ɨ�׳ɹ�������"));//���д���ʹ��sprintf_s�������ַ���"Game Over!"ת��Ϊ�ַ��������洢��endstr�����С�_stprintf_s��sprintf_s��Unicode�汾��ͬ����Ϊ��֧��Unicode����
	outtextxy(WIDTH / 2 - 100, HEIGHT / 2, endstr);//���д�����ָ����λ���������

	system("pause");//��ͣ���򣬵ȴ��û�����,��ֹ��ͼ���ڴ򿪼��ر�

	closegraph();
	return 0;
}