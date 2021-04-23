﻿#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0] = rand() % 7;
	nextBlock[1] = rand() % 7;
	nextBlock[2] = rand() % 7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2, WIDTH + 10);
	printw("NEXT BLOCK");
	DrawBox(3, WIDTH + 10, 4, 8);

	move(9, WIDTH + 10);
	DrawBox(9, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15, WIDTH + 10);
	printw("SCORE");
	DrawBox(16, WIDTH + 10, 1, 8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag==1) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for (i = 0; i < 4; i++) {
		move(4 + i, WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[1]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for (i = 0; i < 4; i++) {
		move(10 + i, WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0) {
				move(i + y + 1, j + x + 1);
				attron(A_REVERSE);
				printw("%c", tile);
				attroff(A_REVERSE);
			}
		}
	}
	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				if (blockY + i >= HEIGHT || blockX + j >= WIDTH) {
					return 0;
				}
				if (blockX + j < 0) {
					return 0;
				}
				if (f[blockY + i][blockX + j] == 1) {
					return 0;
				}
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX) {
	int i, j;
	DrawField();
	switch (command) {
		case KEY_UP :
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (blockRotate == 0) {
						blockRotate = 3;
					}
					else {
						blockRotate--;
					}
					if (block[currentBlock][blockRotate][i][j] == 1) {
						f[blockY + i][blockX + j] = 0;
					}
				}
			}
			break;
		case KEY_DOWN :
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (block[currentBlock][blockRotate][i][j] == 1) {
						f[blockY - 1 + i][blockX + j] = 0;
					}
				}
			}
			break;
		case KEY_RIGHT :
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (block[currentBlock][blockRotate][i][j] == 1) {
						f[blockY + i][blockX - 1 + j] = 0;
					}
				}
			}
			break;
		case KEY_LEFT:
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (block[currentBlock][blockRotate][i][j] == 1) {
						f[blockY + i][blockX + 1 + j] = 0;
					}
				}
			}
			break;
		case 'q' :
		case 'Q' :
			command = QUIT;
			break;
		default :
			command = NOTHING;
			break;
	}
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	//DrawBlock(blockY, blockX, currentBlock, blockRotate, ' ');
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 
}

void BlockDown(int sig) {
	int flag;
	if ((flag = CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX))) {
		blockY++;
	}
	else {
		if (blockY == -1) {
			gameOver = 1;
		}
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);

		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % 7;
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;
	}
	timed_out = 0;
	DrawNextBlock(nextBlock);
	PrintScore(score);
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	//DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
	//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;
	int touched = 0, sc = 0;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				f[blockY + i][blockX + j] = 1;
				if (blockY + 1 + i == HEIGHT && f[blockY + i][blockX + j] == 1) {
					touched++;
				}
			}
		}
	}
	//Block이 추가된 영역의 필드값을 바꾼다.

	sc = touched * 10;

	return sc;
}

int DeleteLine(char f[HEIGHT][WIDTH]) {
	int addscore;
	int cnt = 0;
	int flag;
	int i, j, k;

	for (i = 0; i < HEIGHT; i++) {
		flag = 0;
		for (j = 0; j < WIDTH; j++) {
			if (f[i][j] == 0)
				flag = 1;
		}
		if (flag == 0) {
			for (j = i; j > 0; j--) {
				for (k = 0; k < WIDTH; k++) {
					f[j][k] = f[j - 1][k];
				}
			}
			cnt++;
		}
	}
	addscore = (cnt * cnt) * 100;

	return addscore;
	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	int flag = 0;
	int shadow = 0;

	while ((flag = CheckToMove(field, blockID, blockRotate, y + shadow + 1, blockX))) {
		shadow++;
	}
	DrawBlock(y + shadow, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	DrawShadow(y, x, blockID, blockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

void createRankList(){
	// user code
}

void rank(){
	int command;
	int x, y;

	clear();
	printw("list ranks from X to Y\n");
	printw("list ranks by a specific name\n");
	printw("delete a specific rank\n");

	command = wgetch(stdscr);
	switch (command) {
	case '1': 
		printw("X : ");
		scanw("%d", &x);
	default: break;
	}
	getch();
	refresh();
}

void writeRankFile(){
	// user code
}

void newRank(int score){
	// user code
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
