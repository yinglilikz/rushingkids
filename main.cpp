/*
* rushing kids by 陈仲文 陈弈彤
* 游戏基于easyx图形库
* 1.创建游戏背景
* 2.设计游戏背景
*	a.3重游戏背景不同速度移动
*	b.循环滚动背景实现
* 3.实现游戏背景
*	a.加载背景资源
*	b.渲染(背景知识:坐标）
*		遇到问题：背景图片的png格式图片出现黑色
* 4.实现玩家的奔跑
* 5.实现玩家的跳跃
* 6.实现随机小乌龟
* 7.创建障碍物结构体后重新初始化
* 9.封装后多个障碍物的显示
* 10.实现玩家下蹲技能
* 11.实现“柱子”障碍物
* 
*/
#include<stdio.h>
#include<graphics.h>
#include<conio.h>
#include<vector>
#include"tools.h"
#include<iostream>
#include<time.h>
#include<fstream>
#include<sstream>
#include<algorithm>//算法库
#include<math.h>
#include<stack>
#include<queue>
#include <tchar.h>
using namespace std;

#define WIN_SCORE 50
#define WIN_WIDTH 1012
#define WIN_HEIGHT 396
#define OBSTACLE_COUNT 10
//背景图片
IMAGE imgBgs[3];//三张图片背景
int bgX[3];//背景图片的x坐标
int bgSpeed[3] = { 1,2,4 };//背景图片移动速度 
IMAGE imgHeros[12];
int heroX;//玩家的X坐标
int heroY;//玩家的Y坐标
int heroIndex;//玩家当前奔跑的图片帧序号
bool heroJump;//表示玩家正在跳跃
int jumpHeightMax;//跳跃最大高度
int heroJumpOff;//玩家跳跃偏移量
int update;//表示是否需要马上刷新画面
//IMAGE imgTortoise;//小乌龟
//int torToiseX;//小乌龟的水平坐标
//int torToiseY;//小乌龟的竖直坐标
//bool tortoiseExist;//当前窗口是否有小乌龟

int heroBlood;
int score;
typedef enum {
	TORTOISE,//乌龟 0
	LION,//狮子 1
	HOOK1,//柱子
	HOOK2,
	HOOK3,
	HOOK4,
	OBSTACLE_TYPE_COUNT// 6
}obstacle_type;
int cmd;

vector<vector<IMAGE>> obstacleImgs;//可变的 IMAGE obstacleImgs[OBSTACLE_TYPE_COUNT][Img_Types];

typedef struct obstacle {
	int type;//障碍物的类型 
	int imgIndex;//当前显示的障碍物的图片序号
	int x, y;//障碍物的坐标
	int speed;//障碍物的移动速度
	int power;//障碍物的伤害值
	bool exist;//障碍物是否存在
	bool hited;//表示是否已经发生碰撞
	bool passed;//表示是否已经被通过
}obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];

int lastObsIndex;

IMAGE imgHeroDown[2];

bool heroDown;//表示玩家正在下蹲

IMAGE imgSZ[10];

void init();
void createObstacle();
void checkHit();
void fly();
void updateBg();
void jump();
void down();
void keyEvent();
void updateEnemy();
void updateHero();
void updateBloodBar();
struct HERO;
bool cmp(const HERO& a, const HERO& b);
int startUI();
void readFile(const char* file, vector<HERO>& h);
void saveFile(const char* file, vector<HERO>& h);
void RankingList();
void ShowSaveDialog();
void PLAY();
void displayHelpImage();
void RankingList();
void checkOver();
void checkScore();
void updateScore();
void checkWin();
void displayHelpImage();


//主函数
int main(void) {
	init();
	//displayHelpImage();//单独测试代码
	//显示初始画面
	while (1)
	{
		BeginBatchDraw();
		//FlushBatchDraw();
	//mu:
		cmd = startUI();
		switch (cmd)
		{
		case 1:
			PLAY();//进入游戏
			break;
		case 2:
			displayHelpImage();//显示帮助
			break;
		case 3:
			//排行榜
			RankingList();
			break;
		case 0:
			exit(0);
		}
		EndBatchDraw();
	}

	//Sleep(30)  休眠
	system("pause");
	return 0;
}


void init() {
	//创建游戏窗口
	initgraph(WIN_WIDTH,WIN_HEIGHT);

	//加载背景资源
	char name[64];
	for (int i = 0; i < 3; i++) {
		//"res/bg001.png" "res/bg002.png" "res/bg003.png" 
		sprintf(name, "res/bg%03d.png", i + 1);
		loadimage(&imgBgs[i],name);

		bgX[i] = 0;
	}
	//加载玩家图片素材
	for (int i = 0; i < 12; i++) {
		//"res/hero1.png"..."res/hero12.png"
		sprintf(name, "res/hero%d.png", i + 1);
		loadimage(&imgHeros[i], name);
	}
	//设置玩家初始位置
	heroX = WIN_WIDTH * 0.5 - imgHeros[0].getwidth() * 0.5;
	heroY = 345-imgHeros[0].getheight();
	heroIndex = 0;

	heroJump = false;
	jumpHeightMax = 345 - imgHeros[0].getheight() - 120;
	heroJumpOff = -4;

	update = true;
	//加载小乌龟素材
	/*loadimage(&imgTortoise, "res/t1.png");
	tortoiseExist = false;
	torToiseY = 345 - imgTortoise.getheight()+5;*/
	IMAGE imgTort;
	loadimage(&imgTort, "res/t1.png");
	vector<IMAGE> imgTortArray;
	imgTortArray.push_back(imgTort);
	obstacleImgs.push_back(imgTortArray);
	//加载狮子素材
	IMAGE imgLion;
	vector<IMAGE> imgLionArray;
	for (int i = 0; i < 6; i++) {
		sprintf(name, "res/p%d.png", i + 1);
		loadimage(&imgLion, name);
		imgLionArray.push_back(imgLion);
		}
	obstacleImgs.push_back(imgLionArray);
	//初始化障碍物池子
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		obstacles[i].exist = false;
	}

	//加载下蹲素材
	loadimage(&imgHeroDown[0], "res/d1.png");
	loadimage(&imgHeroDown[1], "res/d2.png");
	heroDown = false;

	//加载柱子图片
	IMAGE imgH;
	
	for (int i = 0; i < 4; i++) {
		vector<IMAGE> imgHookArray;
		sprintf(name,"res/h%d.png", i + 1);
		loadimage(&imgH, name,63,260,true);//后三个参数为等比例缩放
		imgHookArray.push_back(imgH);
		obstacleImgs.push_back(imgHookArray);
	}
	
	heroBlood = 100;

	//预加载音效
	preLoadSound("res/hit.mp3");

	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);

	lastObsIndex = -1;
	score = 0;

	//加载数字图片
	for (int i = 0; i < 10; i++) {
		sprintf(name, "res/sz/%d.png", i);
		loadimage(&imgSZ[i], name);
	}
}

//生成障碍物
void createObstacle() {
	int i;
	for (i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist==false) {
			break;
		}
	}
	if (i >= OBSTACLE_COUNT) {
		return;
	}
	obstacles[i].exist = true;
	obstacles[i].hited = false;
	obstacles[i].imgIndex = 0;
	//obstacles[i].type = (obstacle_type)(rand() % OBSTACLE_TYPE_COUNT);
	// 随机选择障碍物类型
	obstacles[i].type = (obstacle_type)(rand() % 3);

	// 如果上一个障碍物是HOOK1到HOOK4，并且位于屏幕右侧（WIN_WIDTH - 500），则新生成的障碍物类型为TORTOISE
	if (lastObsIndex >= 0 &&
			obstacles[lastObsIndex].type >= HOOK1 &&
			obstacles[lastObsIndex].type <= HOOK4 &&
			obstacles[i].type == LION &&
			obstacles[lastObsIndex].x > (WIN_WIDTH - 500)) {
		obstacles[i].type = TORTOISE;
	}
	lastObsIndex = i;
	if (obstacles[i].type == HOOK1)
	{
		obstacles[i].type += rand() % 4;//0-3
	}
	obstacles[i].x = WIN_WIDTH;
	obstacles[i].y = 345+5 - obstacleImgs[obstacles[i].type][0].getheight();
	if (obstacles[i].type == TORTOISE) {
		obstacles[i].speed = 0;
		obstacles[i].power = 10;
	}
	else if (obstacles[i].type == LION) {
		obstacles[i].speed = 4;
		obstacles[i].power = 20;
	}
	else if (obstacles[i].type >= HOOK1 && obstacles[i].type <= HOOK4)
	{
		obstacles[i].speed = 0;
		obstacles[i].power = 30;
		// 钩子类型的障碍物，纵坐标为0
		obstacles[i].y = 0;
	}
	// 标记障碍物尚未通过
	obstacles[i].passed = false;
}

//检测碰撞
void checkHit() {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist&&obstacles[i].hited==false) {
			int a1x, a1y, a2x, a2y;
			int off = 30;

			// 根据英雄的状态选择不同的坐标范围
			if (!heroDown) {
				a1x = heroX+off;
				a1y = heroY+off;
				a2x = heroX + imgHeros[heroIndex].getwidth()-off;
				a2y = heroY + imgHeros[heroIndex].getheight();

			}
			else {
				a1x = heroX + off;
				a1y = 345-imgHeroDown[heroIndex].getheight();
				a2x = heroX + imgHeroDown[heroIndex].getwidth()-off;
				a2y = 345;
			}

			// 障碍物的坐标范围
			IMAGE img = obstacleImgs[obstacles[i].type][obstacles[i].imgIndex];
			int b1x = obstacles[i].x + off;
			int b1y = obstacles[i].y + off;
			int b2x = obstacles[i].x 
				+ img.getwidth()-off;
			int b2y = obstacles[i].y
				+ img.getheight() - 10;

			// 检测碰撞
			if (rectIntersect(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y)) {
				heroBlood -= obstacles[i].power;
				printf("血量剩余%d\n",heroBlood);
				playSound("res/hit.mp3");
				obstacles[i].hited = true;
			}



		}
	}
}

//三重背景循环变化+玩家帧图片变化+跳跃判定+创建小乌龟
//控制游戏角色飞行和障碍物生成
void fly() {
	// 背景滚动
	for (int i = 0; i < 3; i++) {
		bgX[i] -= bgSpeed[i];
		if (bgX[i] < -WIN_WIDTH) {
			bgX[i] = 0;
		}
	}
	

	//实现跳跃
	if (heroJump) {
		if (heroY < jumpHeightMax) {
			heroJumpOff = 7;
		}
		heroY += heroJumpOff;
		if (heroY > 345 - imgHeros[0].getheight()) {
			heroJump = false;
			heroJumpOff = -7;
		}
	}
	else if (heroDown) 
	{
		static int count = 0;
		int delays[2] = { 7,27 };
		count++;
		if (count >= delays[heroIndex]) {
			count = 0;
			heroIndex++;
			if (heroIndex >= 2) {
				heroIndex = 0;
				heroDown = false;
			}
		}
	}
	else 
	{
		//不跳跃且不下蹲
		heroIndex = (heroIndex + 1) % 12;//玩家帧图片变化
	}
	//创建障碍物
	static int frameCount = 0;
	static int enemyFre = 50;
	frameCount++;
	if (frameCount > enemyFre) {
		frameCount = 0;
		enemyFre=50+rand() % 50;//50-99
		createObstacle();
	}
	/*if (tortoiseExist) {
		torToiseX -= bgSpeed[2];
		if (torToiseX < -imgTortoise.getwidth()) {
			tortoiseExist = false;
		}
	}*/

	//更新障碍物坐标
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist) {
			obstacles[i].x -= obstacles[i].speed+bgSpeed[2];
			if (obstacles[i].x < -obstacleImgs[obstacles[i].type][0].getwidth()*2) {
				obstacles[i].exist = false;
			}
			int len=obstacleImgs[obstacles[i].type].size();
			obstacles[i].imgIndex = (obstacles[i].imgIndex + 1) % len;
		}
	}

	//玩家和障碍物的“碰撞检测”处理
	checkHit();
}


//渲染游戏背景
void updateBg() {
	putimagePNG2(bgX[0], 0, &imgBgs[0]);//前两个参数为图片左上角的坐标
	putimagePNG2(bgX[1], 119, &imgBgs[1]);
	putimagePNG2(bgX[2], 330, &imgBgs[2]);

}

//判断跳没跳
void jump() {
	heroJump = true;
	update = true;
}

//判断蹲没蹲
void down() {
	update = true;
	heroDown = true;
	heroIndex = 0;
}
//处理用户按键的输入
void keyEvent() {
	char ch;
	//不能用scanf，会阻塞程序执行（会等待玩家输入指令）
	if (_kbhit()) { //有按键按下则kbhit()返回 true
		ch=_getch();     //getch不需要按下回车即可直接读取字符
		if (ch == ' ') {
			jump();
		}
		else if (ch == 80) {
			//80为下的ascall码
			down();
		}
	}
}

//更新游戏中障碍物的位置和渲染
void updateEnemy() {
	//渲染小乌龟
	/*if (tortoiseexist) {
			putimagepng2(tortoisex, tortoisey,win_width,&imgtortoise);
		}*/
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist) {
			putimagePNG2(obstacles[i].x, obstacles[i].y,WIN_WIDTH,&obstacleImgs[obstacles[i].type][obstacles[i].imgIndex]);
		}
	}
}

//如果蹲了则要更新主角图像
void updateHero() {
	if (!heroDown){
		putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);
	}
	else {
		int y = 345 - imgHeroDown[heroIndex].getheight();
		putimagePNG2(heroX, y, &imgHeroDown[heroIndex]);
	}
}

//更新主角血量
void updateBloodBar() {
	drawBloodBar(10, 10, 200, 10, 2, BLUE, DARKGRAY, RED, heroBlood / 100.0);
}


struct HERO {
	HERO(string name,int st)
		:name(name)
		,score(st)
	{}
	
	string name;
	int score;
};

//重载比较运算符
bool cmp(const HERO& a, const HERO& b) {
	return a.score > b.score;
}

//从文件中读取数据并填充到存储HERO对象的向量
void readFile(const char*file, vector<HERO>&h)
{
	ifstream ifs(file);
	if (!ifs)
	{
		cerr << "open fail" << endl;
		//return;
	}
	string line;
	string n;
	int st;
	while (getline(ifs, line))//一行一行读取
	{
		istringstream iss(line);
		iss >> n >> st;//类似sprintf函数
		h.push_back(HERO(n, st));
	}
	ifs.close();

}

//将HERO对象的数据写入文件的函数saveFile
void saveFile(const char* file, vector<HERO>& h)
{
	ofstream ofs(file);
	if (!ofs)
	{
		cerr << "open fail" << endl;
		return;
	}
	for (auto& x : h)
	{
		ofs << x.name<<"\t" << x.score <<"\t" << endl;
	}
	ofs.close();
}


//可视化对话框
void ShowSaveDialog() {
	char names[256] = { 0 };
	
	InputBox(names, 39, "请输入您的名字：");
	string name(names);
	//保存数据
	//先读取数据，在更新数据
	vector<HERO>_hero;
	readFile("save.txt",_hero);
	//然后比对，看是否存在，若存在则更新
	for (auto& x : _hero)
	{
		if (x.name == name)
		{
			x.score = score;
			//然后再保存新数据
			saveFile("save.txt", _hero);
		}
	}
	_hero.push_back(HERO(name, score));
	//然后再保存新数据
	saveFile("save.txt", _hero);
	int choice = MessageBox(GetHWnd(), _T("保存成功"), _T("提示"), MB_OK);
	cleardevice();
	IMAGE  imgBg;
	loadimage(&imgBg, "res/main.png");
	putimage(0, 0, WIN_WIDTH, WIN_HEIGHT, &imgBg, 0, 0, SRCCOPY);
	while (1)
	{
		// 检测是否按下 ESC 键
		if (_kbhit()) {//判断键盘是否就绪
			int key = _getch();
			if (key == 27) {  // 27 是 ESC 键的 ASCII 码
				break;
			}
		}

		BeginBatchDraw();
		FlushBatchDraw();
		// 读取数据
		vector<HERO> v;
		readFile("save.txt", v);
		sort(v.begin(), v.end(), cmp);
		// 输出数据到屏幕
		settextstyle(20, 10, _T("宋体"));  // 设置字体样式
		setbkmode(TRANSPARENT);  // 设置文字背景透明
		settextcolor(BLACK);
		int y = 180;
		char buf[526] = { 0 };
		int cnt = 0;
		for (const HERO& hero : v) {
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "用户名 ：%s 成绩：%s", hero.name.c_str(), to_string(hero.score).c_str());
			outtextxy(260, y, buf);
			y += 30;
			++cnt;
			if (cnt == 6) {
				break;
			}
		}
		EndBatchDraw();
	}
	cmd = startUI();
}

//检查血量
void checkOver() {
	if (heroBlood <= 0) 
	{
		loadimage(0, "res/over.png");
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		//结束游戏
		int choice = MessageBox(GetHWnd(), _T("是否保存战绩？"), _T("提示"), MB_YESNO);
		if (choice == IDYES) {
			ShowSaveDialog();
		}
		else
		{
			exit(0);//退出
		}
		//system("pause");
		Sleep(5);
		init();
	}
	//暂停功能
	else
	{
		//这个更好用
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {

			int choice = MessageBox(GetHWnd(), _T("是否继续？"), _T("提示"), MB_YESNO);
			if (choice == IDNO) {
				//返回菜单
				init();
				cmd = startUI();
				while (1)
				{
					cmd = startUI();
					switch (cmd)
					{
					case 1:
						PLAY();//进入游戏
						break;
					case 2:
						displayHelpImage();//显示帮助
						break;
					case 3:
						//排行榜
						RankingList();
						break;
					case 0:
						exit(0);
					}
				}

			}
		}
		//// 检测是否按下 ESC 键
		//if (_kbhit()) {//判断键盘是否就绪
		//	int key = _getch();
		//	if (key == 27) {  // 27 是 ESC 键的 ASCII 码
		//		// 弹出提示框询问用户是否保存游戏状态或退出
		//	
		//		int choice = MessageBox(GetHWnd(), _T("是否继续？"), _T("提示"), MB_YESNO);
		//		if (choice == IDNO) {
		//			//返回菜单
		//			cmd = startUI();
		//		}
		//	}
		//}
	}
	//暂停之后继续玩，冲币复活，或者直接开下一局
	/*heroBlood = 100;
	score = 0;
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);*/

	
}

//检查得分
void checkScore() {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist&&
				obstacles[i].passed==false&&
				obstacles[i].hited==false&&
				obstacles[i].x+obstacleImgs[obstacles[i].type][0].getwidth()<heroX) {
			score++;
			obstacles[i].passed = true;
			printf("score: %d\n", score);
		}
	}
}

//更新得分显示
void updateScore() {
	//50->"50"
	char str[8];
	sprintf(str, "%d", score);

	int x = 20;
	int y = 25;
	for (int i = 0; str[i]; i++) {
		int sz = str[i] - '0';
		putimagePNG(x, y, &imgSZ[sz]);
		x += imgSZ[sz].getwidth() + 5;
	}
}

//检查游戏胜利条件
void checkWin() {
	if (score >= WIN_SCORE) {
		FlushBatchDraw();
		mciSendString("play res/win.mp3", 0, 0, 0);
		Sleep(2000);
		loadimage(0, "res/win.png");
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");
		init();
	}
}

//显示帮助图片
void displayHelpImage()
{
	while (1)
	{
		// 检测是否按下 ESC 键
		if (_kbhit()) {//判断键盘是否就绪
			int key = _getch();
			if (key == 27) {  // 27 是 ESC 键的 ASCII 码
				break;
			}
		}
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			break;
		}
		BeginBatchDraw();
		FlushBatchDraw();
		cleardevice();
		IMAGE help;
		loadimage(&help, "res/bzbg.png");
		putimage(150, 0, &help);
		Sleep(10);
		//FlushBatchDraw();
		EndBatchDraw();
	}
	
}


int timer = 0;

//实现游戏开始界面
int startUI() {
	IMAGE  imgBg, imgMenu1, imgMenu2, imgMenu3,imgMenu4;
	//	IMAGE  imgBg,imgMunu1, imgMunu2, imgMunu3;
//	loadimage(&imgBg, "res/main.png");
//	loadimage(&imgMunu1, "res/hh1.png");
//	loadimage(&imgMunu2, "res/hh2.png");
//	loadimage(&imgMunu3, "res/hh3.png");
//	int flag = 0;
//	
	loadimage(&imgBg, "res/main.png");
	loadimage(&imgMenu1, "res/hh1.png");
	loadimage(&imgMenu2, "res/hh2.png");
	loadimage(&imgMenu3, "res/hh7.png");
	loadimage(&imgMenu4, "res/hh3.png");
	int flag = 0;//菜单选项

	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		putimagePNG(297, 180, &imgMenu1);
		putimagePNG(297, 230, &imgMenu2);
		putimagePNG(297, 280, &imgMenu3);
		putimagePNG(297, 330, &imgMenu4);
		ExMessage msg;

		if (peekmessage(&msg)) {
			std::cout << msg.x << "\t" << msg.y << endl;
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 297 && msg.x < 297 + 150 &&
				msg.y>180 && msg.y < 180 + 40) {
				//开始游戏
				return 1;
			}
			
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 297 && msg.x < 297 + 150 &&
				msg.y>230 && msg.y < 230 + 40) {
				std::cout << "调用 displayHelpImage()" << std::endl;
				displayHelpImage();
				//帮助手册
				return 2;
			}
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 297 && msg.x < 297 + 150 &&
				msg.y>280 && msg.y < 280 + 40) {
				// 退出
				return 3;//排行榜
				
				
			}
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 297 && msg.x < 297 + 150 &&
				msg.y>330 && msg.y < 330 + 40) {
				
				return 0;
				/*flag = 1;
				exit(0);*/
			}
		}
		EndBatchDraw();

	}

}

//实现游戏主循环
void PLAY()
{
	while (1)
	{
		BeginBatchDraw();
		keyEvent();
		timer += getDelay();
		if (timer > 30) {
			timer = 0;
			update = true;
		}
		if (update) {
			update = false;
			BeginBatchDraw();

			updateBg();
			//putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);
			updateHero();

			updateEnemy();

			updateBloodBar();

			updateScore();

			checkWin();

			EndBatchDraw();

			checkOver();

			checkScore();

			fly();
		}
		EndBatchDraw();
	}
}

//显示排行榜
void RankingList()
{

	cleardevice();
	IMAGE  imgBg;
	loadimage(&imgBg, "res/main.png");
	putimage(0, 0, WIN_WIDTH, WIN_HEIGHT ,&imgBg, 0, 0, SRCCOPY);
	while (1)
	{
		// 检测是否按下 ESC 键
		if (_kbhit()) {//判断键盘是否就绪
			int key = _getch();
			if (key == 27) {  // 27 是 ESC 键的 ASCII 码
				break;
			}
		}
		
		BeginBatchDraw();
		FlushBatchDraw();
		// 读取数据
		vector<HERO> v;
		readFile("save.txt", v);
		sort(v.begin(), v.end(),cmp);
		// 输出数据到屏幕
		settextstyle(20, 10, _T("宋体"));  // 设置字体样式
		setbkmode(TRANSPARENT);  // 设置文字背景透明
		settextcolor(BLACK);
		int y = 180;
		char buf[526] = { 0 };
		int cnt = 0;
		for (const HERO& hero : v) {
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "用户名 ：%s 成绩：%s", hero.name.c_str(), to_string(hero.score).c_str());
			outtextxy(260, y, buf);
			y += 30;
			++cnt;
			if (cnt == 6) {
				break;
			}
		}
		EndBatchDraw();
	}
	

}

