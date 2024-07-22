/*
* rushing kids by ������ ����ͮ
* ��Ϸ����easyxͼ�ο�
* 1.������Ϸ����
* 2.�����Ϸ����
*	a.3����Ϸ������ͬ�ٶ��ƶ�
*	b.ѭ����������ʵ��
* 3.ʵ����Ϸ����
*	a.���ر�����Դ
*	b.��Ⱦ(����֪ʶ:���꣩
*		�������⣺����ͼƬ��png��ʽͼƬ���ֺ�ɫ
* 4.ʵ����ҵı���
* 5.ʵ����ҵ���Ծ
* 6.ʵ�����С�ڹ�
* 7.�����ϰ���ṹ������³�ʼ��
* 9.��װ�����ϰ������ʾ
* 10.ʵ������¶׼���
* 11.ʵ�֡����ӡ��ϰ���
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
#include<algorithm>//�㷨��
#include<math.h>
#include<stack>
#include<queue>
#include <tchar.h>
using namespace std;

#define WIN_SCORE 50
#define WIN_WIDTH 1012
#define WIN_HEIGHT 396
#define OBSTACLE_COUNT 10
//����ͼƬ
IMAGE imgBgs[3];//����ͼƬ����
int bgX[3];//����ͼƬ��x����
int bgSpeed[3] = { 1,2,4 };//����ͼƬ�ƶ��ٶ� 
IMAGE imgHeros[12];
int heroX;//��ҵ�X����
int heroY;//��ҵ�Y����
int heroIndex;//��ҵ�ǰ���ܵ�ͼƬ֡���
bool heroJump;//��ʾ���������Ծ
int jumpHeightMax;//��Ծ���߶�
int heroJumpOff;//�����Ծƫ����
int update;//��ʾ�Ƿ���Ҫ����ˢ�»���
//IMAGE imgTortoise;//С�ڹ�
//int torToiseX;//С�ڹ��ˮƽ����
//int torToiseY;//С�ڹ����ֱ����
//bool tortoiseExist;//��ǰ�����Ƿ���С�ڹ�

int heroBlood;
int score;
typedef enum {
	TORTOISE,//�ڹ� 0
	LION,//ʨ�� 1
	HOOK1,//����
	HOOK2,
	HOOK3,
	HOOK4,
	OBSTACLE_TYPE_COUNT// 6
}obstacle_type;
int cmd;

vector<vector<IMAGE>> obstacleImgs;//�ɱ�� IMAGE obstacleImgs[OBSTACLE_TYPE_COUNT][Img_Types];

typedef struct obstacle {
	int type;//�ϰ�������� 
	int imgIndex;//��ǰ��ʾ���ϰ����ͼƬ���
	int x, y;//�ϰ��������
	int speed;//�ϰ�����ƶ��ٶ�
	int power;//�ϰ�����˺�ֵ
	bool exist;//�ϰ����Ƿ����
	bool hited;//��ʾ�Ƿ��Ѿ�������ײ
	bool passed;//��ʾ�Ƿ��Ѿ���ͨ��
}obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];

int lastObsIndex;

IMAGE imgHeroDown[2];

bool heroDown;//��ʾ��������¶�

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


//������
int main(void) {
	init();
	//displayHelpImage();//�������Դ���
	//��ʾ��ʼ����
	while (1)
	{
		BeginBatchDraw();
		//FlushBatchDraw();
	//mu:
		cmd = startUI();
		switch (cmd)
		{
		case 1:
			PLAY();//������Ϸ
			break;
		case 2:
			displayHelpImage();//��ʾ����
			break;
		case 3:
			//���а�
			RankingList();
			break;
		case 0:
			exit(0);
		}
		EndBatchDraw();
	}

	//Sleep(30)  ����
	system("pause");
	return 0;
}


void init() {
	//������Ϸ����
	initgraph(WIN_WIDTH,WIN_HEIGHT);

	//���ر�����Դ
	char name[64];
	for (int i = 0; i < 3; i++) {
		//"res/bg001.png" "res/bg002.png" "res/bg003.png" 
		sprintf(name, "res/bg%03d.png", i + 1);
		loadimage(&imgBgs[i],name);

		bgX[i] = 0;
	}
	//�������ͼƬ�ز�
	for (int i = 0; i < 12; i++) {
		//"res/hero1.png"..."res/hero12.png"
		sprintf(name, "res/hero%d.png", i + 1);
		loadimage(&imgHeros[i], name);
	}
	//������ҳ�ʼλ��
	heroX = WIN_WIDTH * 0.5 - imgHeros[0].getwidth() * 0.5;
	heroY = 345-imgHeros[0].getheight();
	heroIndex = 0;

	heroJump = false;
	jumpHeightMax = 345 - imgHeros[0].getheight() - 120;
	heroJumpOff = -4;

	update = true;
	//����С�ڹ��ز�
	/*loadimage(&imgTortoise, "res/t1.png");
	tortoiseExist = false;
	torToiseY = 345 - imgTortoise.getheight()+5;*/
	IMAGE imgTort;
	loadimage(&imgTort, "res/t1.png");
	vector<IMAGE> imgTortArray;
	imgTortArray.push_back(imgTort);
	obstacleImgs.push_back(imgTortArray);
	//����ʨ���ز�
	IMAGE imgLion;
	vector<IMAGE> imgLionArray;
	for (int i = 0; i < 6; i++) {
		sprintf(name, "res/p%d.png", i + 1);
		loadimage(&imgLion, name);
		imgLionArray.push_back(imgLion);
		}
	obstacleImgs.push_back(imgLionArray);
	//��ʼ���ϰ������
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		obstacles[i].exist = false;
	}

	//�����¶��ز�
	loadimage(&imgHeroDown[0], "res/d1.png");
	loadimage(&imgHeroDown[1], "res/d2.png");
	heroDown = false;

	//��������ͼƬ
	IMAGE imgH;
	
	for (int i = 0; i < 4; i++) {
		vector<IMAGE> imgHookArray;
		sprintf(name,"res/h%d.png", i + 1);
		loadimage(&imgH, name,63,260,true);//����������Ϊ�ȱ�������
		imgHookArray.push_back(imgH);
		obstacleImgs.push_back(imgHookArray);
	}
	
	heroBlood = 100;

	//Ԥ������Ч
	preLoadSound("res/hit.mp3");

	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);

	lastObsIndex = -1;
	score = 0;

	//��������ͼƬ
	for (int i = 0; i < 10; i++) {
		sprintf(name, "res/sz/%d.png", i);
		loadimage(&imgSZ[i], name);
	}
}

//�����ϰ���
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
	// ���ѡ���ϰ�������
	obstacles[i].type = (obstacle_type)(rand() % 3);

	// �����һ���ϰ�����HOOK1��HOOK4������λ����Ļ�ҲࣨWIN_WIDTH - 500�����������ɵ��ϰ�������ΪTORTOISE
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
		// �������͵��ϰ��������Ϊ0
		obstacles[i].y = 0;
	}
	// ����ϰ�����δͨ��
	obstacles[i].passed = false;
}

//�����ײ
void checkHit() {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist&&obstacles[i].hited==false) {
			int a1x, a1y, a2x, a2y;
			int off = 30;

			// ����Ӣ�۵�״̬ѡ��ͬ�����귶Χ
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

			// �ϰ�������귶Χ
			IMAGE img = obstacleImgs[obstacles[i].type][obstacles[i].imgIndex];
			int b1x = obstacles[i].x + off;
			int b1y = obstacles[i].y + off;
			int b2x = obstacles[i].x 
				+ img.getwidth()-off;
			int b2y = obstacles[i].y
				+ img.getheight() - 10;

			// �����ײ
			if (rectIntersect(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y)) {
				heroBlood -= obstacles[i].power;
				printf("Ѫ��ʣ��%d\n",heroBlood);
				playSound("res/hit.mp3");
				obstacles[i].hited = true;
			}



		}
	}
}

//���ر���ѭ���仯+���֡ͼƬ�仯+��Ծ�ж�+����С�ڹ�
//������Ϸ��ɫ���к��ϰ�������
void fly() {
	// ��������
	for (int i = 0; i < 3; i++) {
		bgX[i] -= bgSpeed[i];
		if (bgX[i] < -WIN_WIDTH) {
			bgX[i] = 0;
		}
	}
	

	//ʵ����Ծ
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
		//����Ծ�Ҳ��¶�
		heroIndex = (heroIndex + 1) % 12;//���֡ͼƬ�仯
	}
	//�����ϰ���
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

	//�����ϰ�������
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

	//��Һ��ϰ���ġ���ײ��⡱����
	checkHit();
}


//��Ⱦ��Ϸ����
void updateBg() {
	putimagePNG2(bgX[0], 0, &imgBgs[0]);//ǰ��������ΪͼƬ���Ͻǵ�����
	putimagePNG2(bgX[1], 119, &imgBgs[1]);
	putimagePNG2(bgX[2], 330, &imgBgs[2]);

}

//�ж���û��
void jump() {
	heroJump = true;
	update = true;
}

//�ж϶�û��
void down() {
	update = true;
	heroDown = true;
	heroIndex = 0;
}
//�����û�����������
void keyEvent() {
	char ch;
	//������scanf������������ִ�У���ȴ��������ָ�
	if (_kbhit()) { //�а���������kbhit()���� true
		ch=_getch();     //getch����Ҫ���»س�����ֱ�Ӷ�ȡ�ַ�
		if (ch == ' ') {
			jump();
		}
		else if (ch == 80) {
			//80Ϊ�µ�ascall��
			down();
		}
	}
}

//������Ϸ���ϰ����λ�ú���Ⱦ
void updateEnemy() {
	//��ȾС�ڹ�
	/*if (tortoiseexist) {
			putimagepng2(tortoisex, tortoisey,win_width,&imgtortoise);
		}*/
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist) {
			putimagePNG2(obstacles[i].x, obstacles[i].y,WIN_WIDTH,&obstacleImgs[obstacles[i].type][obstacles[i].imgIndex]);
		}
	}
}

//���������Ҫ��������ͼ��
void updateHero() {
	if (!heroDown){
		putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);
	}
	else {
		int y = 345 - imgHeroDown[heroIndex].getheight();
		putimagePNG2(heroX, y, &imgHeroDown[heroIndex]);
	}
}

//��������Ѫ��
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

//���رȽ������
bool cmp(const HERO& a, const HERO& b) {
	return a.score > b.score;
}

//���ļ��ж�ȡ���ݲ���䵽�洢HERO���������
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
	while (getline(ifs, line))//һ��һ�ж�ȡ
	{
		istringstream iss(line);
		iss >> n >> st;//����sprintf����
		h.push_back(HERO(n, st));
	}
	ifs.close();

}

//��HERO���������д���ļ��ĺ���saveFile
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


//���ӻ��Ի���
void ShowSaveDialog() {
	char names[256] = { 0 };
	
	InputBox(names, 39, "�������������֣�");
	string name(names);
	//��������
	//�ȶ�ȡ���ݣ��ڸ�������
	vector<HERO>_hero;
	readFile("save.txt",_hero);
	//Ȼ��ȶԣ����Ƿ���ڣ������������
	for (auto& x : _hero)
	{
		if (x.name == name)
		{
			x.score = score;
			//Ȼ���ٱ���������
			saveFile("save.txt", _hero);
		}
	}
	_hero.push_back(HERO(name, score));
	//Ȼ���ٱ���������
	saveFile("save.txt", _hero);
	int choice = MessageBox(GetHWnd(), _T("����ɹ�"), _T("��ʾ"), MB_OK);
	cleardevice();
	IMAGE  imgBg;
	loadimage(&imgBg, "res/main.png");
	putimage(0, 0, WIN_WIDTH, WIN_HEIGHT, &imgBg, 0, 0, SRCCOPY);
	while (1)
	{
		// ����Ƿ��� ESC ��
		if (_kbhit()) {//�жϼ����Ƿ����
			int key = _getch();
			if (key == 27) {  // 27 �� ESC ���� ASCII ��
				break;
			}
		}

		BeginBatchDraw();
		FlushBatchDraw();
		// ��ȡ����
		vector<HERO> v;
		readFile("save.txt", v);
		sort(v.begin(), v.end(), cmp);
		// ������ݵ���Ļ
		settextstyle(20, 10, _T("����"));  // ����������ʽ
		setbkmode(TRANSPARENT);  // �������ֱ���͸��
		settextcolor(BLACK);
		int y = 180;
		char buf[526] = { 0 };
		int cnt = 0;
		for (const HERO& hero : v) {
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "�û��� ��%s �ɼ���%s", hero.name.c_str(), to_string(hero.score).c_str());
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

//���Ѫ��
void checkOver() {
	if (heroBlood <= 0) 
	{
		loadimage(0, "res/over.png");
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		//������Ϸ
		int choice = MessageBox(GetHWnd(), _T("�Ƿ񱣴�ս����"), _T("��ʾ"), MB_YESNO);
		if (choice == IDYES) {
			ShowSaveDialog();
		}
		else
		{
			exit(0);//�˳�
		}
		//system("pause");
		Sleep(5);
		init();
	}
	//��ͣ����
	else
	{
		//���������
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {

			int choice = MessageBox(GetHWnd(), _T("�Ƿ������"), _T("��ʾ"), MB_YESNO);
			if (choice == IDNO) {
				//���ز˵�
				init();
				cmd = startUI();
				while (1)
				{
					cmd = startUI();
					switch (cmd)
					{
					case 1:
						PLAY();//������Ϸ
						break;
					case 2:
						displayHelpImage();//��ʾ����
						break;
					case 3:
						//���а�
						RankingList();
						break;
					case 0:
						exit(0);
					}
				}

			}
		}
		//// ����Ƿ��� ESC ��
		//if (_kbhit()) {//�жϼ����Ƿ����
		//	int key = _getch();
		//	if (key == 27) {  // 27 �� ESC ���� ASCII ��
		//		// ������ʾ��ѯ���û��Ƿ񱣴���Ϸ״̬���˳�
		//	
		//		int choice = MessageBox(GetHWnd(), _T("�Ƿ������"), _T("��ʾ"), MB_YESNO);
		//		if (choice == IDNO) {
		//			//���ز˵�
		//			cmd = startUI();
		//		}
		//	}
		//}
	}
	//��֮ͣ������棬��Ҹ������ֱ�ӿ���һ��
	/*heroBlood = 100;
	score = 0;
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);*/

	
}

//���÷�
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

//���µ÷���ʾ
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

//�����Ϸʤ������
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

//��ʾ����ͼƬ
void displayHelpImage()
{
	while (1)
	{
		// ����Ƿ��� ESC ��
		if (_kbhit()) {//�жϼ����Ƿ����
			int key = _getch();
			if (key == 27) {  // 27 �� ESC ���� ASCII ��
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

//ʵ����Ϸ��ʼ����
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
	int flag = 0;//�˵�ѡ��

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
				//��ʼ��Ϸ
				return 1;
			}
			
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 297 && msg.x < 297 + 150 &&
				msg.y>230 && msg.y < 230 + 40) {
				std::cout << "���� displayHelpImage()" << std::endl;
				displayHelpImage();
				//�����ֲ�
				return 2;
			}
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 297 && msg.x < 297 + 150 &&
				msg.y>280 && msg.y < 280 + 40) {
				// �˳�
				return 3;//���а�
				
				
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

//ʵ����Ϸ��ѭ��
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

//��ʾ���а�
void RankingList()
{

	cleardevice();
	IMAGE  imgBg;
	loadimage(&imgBg, "res/main.png");
	putimage(0, 0, WIN_WIDTH, WIN_HEIGHT ,&imgBg, 0, 0, SRCCOPY);
	while (1)
	{
		// ����Ƿ��� ESC ��
		if (_kbhit()) {//�жϼ����Ƿ����
			int key = _getch();
			if (key == 27) {  // 27 �� ESC ���� ASCII ��
				break;
			}
		}
		
		BeginBatchDraw();
		FlushBatchDraw();
		// ��ȡ����
		vector<HERO> v;
		readFile("save.txt", v);
		sort(v.begin(), v.end(),cmp);
		// ������ݵ���Ļ
		settextstyle(20, 10, _T("����"));  // ����������ʽ
		setbkmode(TRANSPARENT);  // �������ֱ���͸��
		settextcolor(BLACK);
		int y = 180;
		char buf[526] = { 0 };
		int cnt = 0;
		for (const HERO& hero : v) {
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "�û��� ��%s �ɼ���%s", hero.name.c_str(), to_string(hero.score).c_str());
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

