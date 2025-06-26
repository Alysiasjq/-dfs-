#include<iostream>
#include<easyx.h>
#include<vector>
#include<ctime>
#include<cstdlib>
#include<windows.h>
#include<tchar.h>

using namespace std;
#define SIZE 9//定义雷场的大小

#define SIZE_REC 20//定义每个格子的大小

#define WIDTH (SIZE+2)*SIZE_REC//定义游戏窗口的宽度
#define HEIGHT SIZE*SIZE_REC//定义游戏窗口的高度

#define RATE 0.13//定义难度系数

int bombcount = SIZE * SIZE * RATE;//定义雷的数量
int bomb_show = bombcount;//屏幕中显示的雷的数量

//雷场中每个元素的框的显示状态
enum CellState {
	COVERED,
	UNCOVERED,
	FLAGGED
};

//雷场中的每个元素的状态
typedef struct map
{
	CellState state;//框的显示状态
	bool isbomb;//是否是雷
	int countbomb;//周围有几个雷
}MAP;

//初始化雷场
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

//右键标记显示小红旗
void mark(vector<vector<MAP>>& m, int x, int y)
{
	setfillcolor(LIGHTGRAY);
	fillrectangle(x * SIZE_REC, y * SIZE_REC, (x + 1) * SIZE_REC, (y + 1) * SIZE_REC);//绘制小矩形（4个参数，分别是左上角的点和右下角的点）

	//定义三角形的三个顶点
	POINT pts[] = { {(x + 0.5) * SIZE_REC,y * SIZE_REC},
		{(x + 0.5) * SIZE_REC,(y + 0.5) * SIZE_REC},
		{(x + 1) * SIZE_REC,(y + 0.25) * SIZE_REC} };
	setfillcolor(RED);//设置填充颜色为红色
	fillpolygon(pts, 3);//绘制三角形
	line((x + 0.5) * SIZE_REC, (y + 0.5) * SIZE_REC, (x + 0.5) * SIZE_REC, (y + 1) * SIZE_REC);//绘制旗杆
}

//打印雷场
void printMap(vector<vector<MAP>>& m)
{
	//清理上一次打印的东西
	clearrectangle(SIZE * SIZE_REC, 0, WIDTH, SIZE_REC);

	//打印雷的数量
	settextcolor(YELLOW);
	settextstyle(SIZE_REC, 0, _T("Arial"));//设置字体高度为20，宽度自适应，_T支持Unicode编码，字体为Arial
	TCHAR endstr[20];//TCHAR是一个宏，它在Unicode版本的Windows中被定义为wchar_t，在非Unicode版本中被定义为char。这使得程序能够根据编译环境自动选择合适的字符类型。
	_stprintf_s(endstr, _T("%d"), bomb_show);//这行代码使用sprintf_s函数将整数bomb_show转换为字符串，并存储在endstr数组中。_stprintf_s是sprintf_s的Unicode版本，同样是为了支持Unicode编码
	outtextxy(SIZE * SIZE_REC, 0, endstr);//这行代码在指定的位置输出文字

	//打印雷场
	for (int i = 0;i < SIZE;i++)
	{
		for (int j = 0;j < SIZE;j++)
		{
			if (m[i][j].state == COVERED)
			{
				setfillcolor(LIGHTGRAY);
				fillrectangle(j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC);//绘制小矩形
			}
			else if (m[i][j].state == UNCOVERED)
			{
				if (m[i][j].isbomb)
				{
					setfillcolor(RED);
					fillrectangle(j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC);//绘制小矩形
					settextcolor(BLACK);
					RECT rc = { j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC };
					setbkmode(TRANSPARENT);
					drawtext('X', &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//绘制X,DT_CENTER: 这个标志表示文本将在矩形区域内水平居中绘制。DT_VCENTER: 这个标志表示文本将在矩形区域内垂直居中绘制。DT_SINGLELINE : 这个标志表示文本将作为单行处理，不处理换行符。这意味着即使文本内容包含换行符，它也会被忽略，文本会一直绘制在同一行上，直到达到矩形的右边界为止。
				}
				else
				{
					setfillcolor(DARKGRAY);
					fillrectangle(j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC);//绘制小矩形
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
						drawtext(c, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//绘制数字		
					}
				}
			}
			else if (m[i][j].state == FLAGGED)
			{
				mark(m, j, i);//绘制小红旗
			}
		}
	}
}

//生成地雷
void generateBombs(vector<vector<MAP>>& m, int bombcount, int x, int y)//保证用户第一个点击到的位置不是雷
{
	int count = 0;
	while (count < bombcount)
	{
		int bomb_x = rand() % SIZE;//取模是为了保证取到0-8之间的随机数
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

//游戏失败打印完整地雷信息
void printBombs(vector<vector<MAP>>& m)
{
	for (int i = 0;i < SIZE;i++)
	{
		for (int j = 0;j < SIZE;j++)
		{
			if (m[i][j].isbomb)
			{
				setfillcolor(RED);
				fillrectangle(j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC);//绘制小矩形
				settextcolor(BLACK);
				settextstyle(SIZE_REC, 0, _T("Arial"));
				RECT rc = { j * SIZE_REC, i * SIZE_REC, (j + 1) * SIZE_REC, (i + 1) * SIZE_REC };
				setbkmode(TRANSPARENT);
				drawtext('X', &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
		}
	}
}

//将覆盖修改为未覆盖
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
}//对点击覆盖的格子后的搜索（dfs）
void uncoverCell(vector<vector<MAP>>& m, int x, int y)
{
	dfs(m, y, x);
}

//游戏是否获胜
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

//鼠标左键单击事件
void mouseClick(vector<vector<MAP>>& m, ExMessage& msg, bool& gameover, bool& isstart)
{
	//获取鼠标点击位置
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
				settextstyle(50, 0, _T("Arial"));//设置字体高度为20，宽度自适应，_T支持Unicode编码，字体为Arial
				TCHAR endstr[20];//TCHAR是一个宏，它在Unicode版本的Windows中被定义为wchar_t，在非Unicode版本中被定义为char。这使得程序能够根据编译环境自动选择合适的字符类型。
				_stprintf_s(endstr, _T("菜"));//这行代码使用sprintf_s函数将字符串"Game Over!"转换为字符串，并存储在endstr数组中。_stprintf_s是sprintf_s的Unicode版本，同样是为了支持Unicode编码
				outtextxy(WIDTH / 2 - SIZE_REC, HEIGHT / 2 - SIZE_REC, endstr);//这行代码在指定的位置输出文字
				Sleep(1000);
				printBombs(m);
				settextcolor(RED);
				settextstyle(50, 0, _T("Arial"));//设置字体高度为20，宽度自适应，_T支持Unicode编码，字体为Arial
				outtextxy(WIDTH / 2 - SIZE_REC, HEIGHT / 2 - SIZE_REC, endstr);//这行代码在指定的位置输出文字
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

//鼠标右键单击事件
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

//鼠标左键双击事件
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
								settextstyle(50, 0, _T("Arial"));//设置字体高度为20，宽度自适应，_T支持Unicode编码，字体为Arial
								TCHAR endstr[20];//TCHAR是一个宏，它在Unicode版本的Windows中被定义为wchar_t，在非Unicode版本中被定义为char。这使得程序能够根据编译环境自动选择合适的字符类型。
								_stprintf_s(endstr, _T("菜"));//这行代码使用sprintf_s函数将字符串"Game Over!"转换为字符串，并存储在endstr数组中。_stprintf_s是sprintf_s的Unicode版本，同样是为了支持Unicode编码
								outtextxy(WIDTH / 2 - SIZE_REC, HEIGHT / 2 - SIZE_REC, endstr);//这行代码在指定的位置输出文字
								Sleep(1000);
								printBombs(m);
								settextcolor(RED);
								settextstyle(50, 0, _T("Arial"));//设置字体高度为20，宽度自适应，_T支持Unicode编码，字体为Arial
								outtextxy(WIDTH / 2 - SIZE_REC, HEIGHT / 2 - SIZE_REC, endstr);//这行代码在指定的位置输出文字
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
	srand(time(NULL));//设置随机种子

	initgraph(WIDTH, HEIGHT, EX_DBLCLKS);//创建窗口，宽度为WIDTH，高度为HEIGHT，EX_DBLCLKS接受双击事件

	vector<vector<MAP>> Map(SIZE, vector<MAP>(SIZE));//定义雷场

	initMap(Map);//初始化雷场

	printMap(Map);//打印雷场

	bool gameover = false;
	bool isstart = false;

	while (!gameover)
	{
		ExMessage msg = { 0 };
		peekmessage(&msg, EX_MOUSE);//获取消息
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
	settextstyle(50, 0, _T("Arial"));//设置字体高度为20，宽度自适应，_T支持Unicode编码，字体为Arial
	setbkmode(TRANSPARENT);
	TCHAR endstr[20];//TCHAR是一个宏，它在Unicode版本的Windows中被定义为wchar_t，在非Unicode版本中被定义为char。这使得程序能够根据编译环境自动选择合适的字符类型。
	_stprintf_s(endstr, _T("扫雷成功！！！"));//这行代码使用sprintf_s函数将字符串"Game Over!"转换为字符串，并存储在endstr数组中。_stprintf_s是sprintf_s的Unicode版本，同样是为了支持Unicode编码
	outtextxy(WIDTH / 2 - 100, HEIGHT / 2, endstr);//这行代码在指定的位置输出文字

	system("pause");//暂停程序，等待用户操作,防止绘图窗口打开即关闭

	closegraph();
	return 0;
}