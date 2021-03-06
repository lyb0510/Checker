#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES	1

#include <bangtal.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <conio.h>
#include <time.h>

SceneID scene1, scene2;
ObjectID startButton, ruleButton, endButton, giveupButton, turnButton, rule;

struct Piece {
	char team;
	// Wor B
	ObjectID object;
	//배치하기 위해 obj선언
	int x;
	int y;
	//위치
	int vector;
	//전진 가능 방향 표시
};

Piece W[12], B[12], m[4];

int pick = 0;
//pick = 선택한 기물 번호
// -12 ~ -1 : B
// 0 : 없음
// 1 ~ 12 : W

int posX[8] = { 330, 410, 490, 570, 650, 730, 810, 890 };
int posY[8] = { 50, 130, 210, 290, 370, 450, 530,610 };
//값으로 부터(-10 ~ +60)이 각 칸의 범위

int check_moved = 1;
// 2 = 적을 끼고 뛰어서 움직이는 이동(2칸)만 가능
// 1 = 이동가능
// 0 = 이동불가
bool turn = true;
//t=B, f=W
bool was_moved = false;
//어떤 기물이 이번턴에 움직였는지를 체크해주는 용도

int win = 9;
// -1 = B승, 0 = 비김, 1 = A승, 9 = 아무것도 아님 

int s_MAP[8][8] = {
	{0, 5, 0, 0, 0, -1, 0, -9},
	{1, 0, 9, 0, 0, 0, -5, 0},
	{0, 6, 0, 0, 0, -2, 0, -10},
	{2, 0, 10, 0, 0, 0, -6, 0},
	{0, 7, 0, 0, 0, -3, 0, -11},
	{3, 0, 11, 0, 0, 0, -7, 0},
	{0, 8, 0, 0, 0, -4, 0, -12},
	{4, 0, 12, 0, 0, 0, -8, 0}
};
int map[8][8];
// BK = 기본 값-10, B= -1~-12, 빈칸 = 0, W= 1~12, WK =기본값 +10

int cntW = 12;
int cntWK = 0;
int cntB = 12;
int cntBK = 0;

void changeTurn() {
	if (turn) {
		hideObject(turnButton);
		setObjectImage(turnButton, "Images/turnB.png");
		showObject(turnButton);
	}
	else {
		hideObject(turnButton);
		setObjectImage(turnButton, "Images/turnW.png");
		showObject(turnButton);
	}
}

void check_end() {
	if (cntW == 0) {
		win = -1;
	}
	else if (cntB == 0) {
		win = 1;
	}
	else if (cntW ==1 && cntWK == 1 && cntW == cntB && cntBK == cntWK) {
		win = 0;
	}
	switch (win) {
	case 1:
		//W승
		showMessage("게임이 종료되었습니다. 흰색( W ) 승리!");
		enterScene(scene1);
		break;
	case 0:
		//비김
		showMessage("게임이 종료되었습니다. 비겼습니다.");
		enterScene(scene1);
		break;
	case -1:
		//B승
		showMessage("게임이 종료되었습니다. 검은색( B ) 승리!");
		enterScene(scene1);

		break;
	default:
		break;
	}
}

void del_m() {
	for (int i = 0; i < 4; i++) {
		hideObject(m[i].object);
	}
}

void SetGame() {
	cntW = 12;
	cntWK = 0;
	cntB = 12;
	cntBK = 0;

	win = 9;

	del_m();
	check_moved = 1;
	pick = 0;
	turn = true;

	for (int i = 0; i < 12; i++) {
		hideObject(W[i].object);
		hideObject(B[i].object);
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			map[i][j] = s_MAP[i][j];
		}
	}


	hideObject(turnButton);
	setObjectImage(turnButton, "Images/turnB.png");
	showObject(turnButton);

	for (int i = 0; i < 12; i++) {
		B[i].team = 'B';
		B[i].vector = -1;
		B[i].object = createObject("Images/B.png");
		if (8 > i && i > 3) {
			B[i].x = 2 * (i - 4) + 1;
		}
		else {
			B[i].x = 2 * (i % 4);
		}
		B[i].y = (i / 4) + 5;
		locateObject(B[i].object, scene2, posX[B[i].x], posY[B[i].y]);
		showObject(B[i].object);
	}
	for (int i = 0; i < 12; i++) {
		W[i].team = 'W';
		W[i].vector = 1;
		W[i].object = createObject("Images/W.png");
		if (8 > i && i > 3) {
			W[i].x = 2 * (i - 4);
		}
		else {
			W[i].x = 2 * (i % 4) + 1;
		}
		W[i].y = i / 4;
		locateObject(W[i].object, scene2, posX[W[i].x], posY[W[i].y]);
		showObject(W[i].object);
	}
}

void movepos(int vec, int x, int y) {
	int i = 0;
	if (vec == 1 || vec == 0) { //W 일반기물 이동 가능 위치 파악 and 왕 계급의 절반 위치 파악
		if ((x < 7 && y < 7) && map[x + 1][y + 1] == 0 && check_moved != 2) {
			m[i].x = x + 1;
			m[i].y = y + 1;
			m[i].object = createObject("Images/posible.png");
			locateObject(m[i].object, scene2, posX[m[i].x], posY[m[i].y]);
			showObject(m[i].object);
			i++;
		}
		if ((x > 0 && y < 7) && map[x - 1][y + 1] == 0 && check_moved != 2) {
			m[i].x = x - 1;
			m[i].y = y + 1;
			m[i].object = createObject("Images/posible.png");
			locateObject(m[i].object, scene2, posX[m[i].x], posY[m[i].y]);
			showObject(m[i].object);
			i++;
		}
		if ((x < 6 && y < 6) && map[x + 2][y + 2] == 0 && map[x + 1][y + 1] * map[x][y] < 0) {
			m[i].x = x + 2;
			m[i].y = y + 2;
			m[i].object = createObject("Images/posible.png");
			locateObject(m[i].object, scene2, posX[m[i].x], posY[m[i].y]);
			showObject(m[i].object);
			i++;
		}
		if ((x > 1 && y < 6) && map[x - 2][y + 2] == 0 && map[x - 1][y + 1] * map[x][y] < 0 ) {
			m[i].x = x - 2;
			m[i].y = y + 2;
			m[i].object = createObject("Images/posible.png");
			locateObject(m[i].object, scene2, posX[m[i].x], posY[m[i].y]);
			showObject(m[i].object);
			i++;
		}
	}
	if (vec == -1 || vec == 0) { //B 일반기물 이동 가능 위치 파악 and 왕 계급의 절반 위치 파악
		if ((x < 7 && y > 0) && map[x + 1][y - 1] == 0 && check_moved != 2) {
			m[i].x = x + 1;
			m[i].y = y - 1;
			m[i].object = createObject("Images/posible.png");
			locateObject(m[i].object, scene2, posX[m[i].x], posY[m[i].y]);
			showObject(m[i].object);
			i++;
		}
		if ((x > 0 && y > 0) && map[x - 1][y - 1] == 0 && check_moved != 2) {
			m[i].x = x - 1;
			m[i].y = y - 1;
			m[i].object = createObject("Images/posible.png");
			locateObject(m[i].object, scene2, posX[m[i].x], posY[m[i].y]);
			showObject(m[i].object);
			i++;
		}
		if ((x < 6 && y > 1) && map[x + 2][y - 2] == 0 && map[x + 1][y - 1] * map[x][y] < 0) {
			m[i].x = x + 2;
			m[i].y = y - 2;
			m[i].object = createObject("Images/posible.png");
			locateObject(m[i].object, scene2, posX[m[i].x], posY[m[i].y]);
			showObject(m[i].object);
			i++;
		}
		if ((x > 1 && y > 1) && map[x - 2][y - 2] == 0 && map[x - 1][y - 1] * map[x][y] < 0) {
			m[i].x = x - 2;
			m[i].y = y - 2;
			m[i].object = createObject("Images/posible.png");
			locateObject(m[i].object, scene2, posX[m[i].x], posY[m[i].y]);
			showObject(m[i].object);
			i++;
		}
	}
	if (was_moved && i == 0) {
		turn = !turn;
		changeTurn();
		check_moved = 1;
	}
}

void move(Piece m) {
	was_moved = false;
	if (pick > 0) {
		if (abs(W[pick - 1].x - m.x) == 2) {
			check_moved = 2;
			was_moved = true;
			int del = map[(W[pick - 1].x + m.x) / 2][(W[pick - 1].y + m.y) / 2];
			map[(W[pick - 1].x + m.x) / 2][(W[pick - 1].y + m.y) / 2] = 0;
			hideObject(B[-del - 1].object);
			cntB--;
		}	
		else if (abs(W[pick - 1].x - m.x) == 1) {
			turn = !turn;
			changeTurn();
		}// 움직임 제어(룰에 따라)

		int temp = map[W[pick - 1].x][W[pick - 1].y];
		map[W[pick - 1].x][W[pick - 1].y] = map[m.x][m.y];
		map[m.x][m.y] = temp;
		// map에 저장된 값 교체
		W[pick - 1].x = m.x;
		W[pick - 1].y = m.y;
		locateObject(W[pick - 1].object, scene2, posX[W[pick - 1].x], posY[W[pick - 1].y]);
		//화면에 출력되는 이미지 위치 수정

		if (m.y == 7) {
			W[pick - 1].vector = 0;
			setObjectImage(W[pick - 1].object, "Images/WK.png");
			cntWK++;
		}//마지막줄 도달시 승급

		if (check_moved == 2) {
			movepos(W[pick - 1].vector, W[pick - 1].x, W[pick - 1].y);
		}
	}
	else if (pick < 0) {
		if (abs(B[-pick - 1].x - m.x) == 2) {
			check_moved = 2;
			was_moved = true;
			int del = map[(B[-pick - 1].x + m.x) / 2][(B[-pick - 1].y + m.y) / 2];
			map[(B[-pick - 1].x + m.x) / 2][(B[-pick - 1].y + m.y) / 2] = 0;
			hideObject(W[del - 1].object);
			cntW--;
		}
		else if (abs(B[-pick - 1].x - m.x) == 1) {
			turn = !turn;
			changeTurn();
		}// 움직임 제어(룰에 따라)

		int temp = map[B[-pick - 1].x][B[-pick - 1].y];
		map[B[-pick - 1].x][B[-pick - 1].y] = map[m.x][m.y];
		map[m.x][m.y] = temp;
		// map에 저장된 값 교체
		B[-pick - 1].x = m.x;
		B[-pick - 1].y = m.y;
		locateObject(B[-pick - 1].object, scene2, posX[B[-pick - 1].x], posY[B[-pick - 1].y]);
		//화면에 출력되는 이미지 위치 수정 

		if (m.y == 0) {
			B[-pick - 1].vector = 0;
			setObjectImage(B[-pick - 1].object, "Images/BK.png");
			cntBK++;
		}//마지막줄 도달시 승급

		if (check_moved == 2) {
			movepos(B[-pick - 1].vector, B[-pick - 1].x, B[-pick - 1].y);
		}
	}
	check_end();
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action) {
	if (object == startButton) {
		enterScene(scene2);
		SetGame();
	}
	else if (object == ruleButton) {
		showObject(rule);
	}
	else if (object == rule) {
		hideObject(rule);
	}
	else if (object == endButton) {
		endGame();
	}
	else if (object == giveupButton) {
		if (turn) {
			win = 1;
		}
		else {
			win = -1;
		}
		check_end();
		enterScene(scene1);
	}
	for (int i = 0; i < 12; i++) {
		if (!turn && object == W[i].object) {
			pick = i+1;
			del_m();
			movepos(W[i].vector, W[i].x, W[i].y);
		}
		else if (turn && object == B[i].object) {
			pick = -i-1;
			del_m();
			movepos(B[i].vector, B[i].x, B[i].y);
		}
	}
	for (int i = 0; i < 4; i++) {
		if (object == m[i].object) {
			del_m();
			move(m[i]);
		}
	}
}

int main() {
	setMouseCallback(mouseCallback);

	scene1 = createScene("Lobby", "Images/Lobby.png");
	scene2 = createScene("Play!", "Images/Board.png");

	startButton = createObject("Images/start.png");
	locateObject(startButton, scene1, 540, 300);
	scaleObject(startButton, 2.0f);
	showObject(startButton);

	ruleButton = createObject("Images/showrule.png");
	locateObject(ruleButton, scene1, 540, 200);
	scaleObject(ruleButton, 2.0f);
	showObject(ruleButton);

	endButton = createObject("Images/end.png");
	locateObject(endButton, scene1, 540, 100);
	scaleObject(endButton, 2.0f);
	showObject(endButton);

	rule = createObject("Images/rule.png");
	locateObject(rule, scene1, 0, 0);

	giveupButton = createObject("Images/giveup.png");
	locateObject(giveupButton, scene2, 1000, 100);
	scaleObject(giveupButton, 1.5f);
	showObject(giveupButton);

	turnButton = createObject("Images/turnB.png");
	locateObject(turnButton, scene2, 1000, 550);
	scaleObject(turnButton, 1.5f);
	startGame(scene1);
}
