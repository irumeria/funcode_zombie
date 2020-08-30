/////////////////////////////////////////////////////////////////////////////////
//
//
//
//
/////////////////////////////////////////////////////////////////////////////////
#include "Stdio.h"
#include "stdlib.h"
#include "Math.h"
#include "CommonAPI.h"
#include "LessonX.h"
#include "string.h"
////////////////////////////////////////////////////////////////////////////////
//
//
int	g_iGameState = 0;		// ��Ϸ״̬��0 -- ��Ϸ�����ȴ���ʼ״̬��1 -- ���¿ո����ʼ����ʼ����Ϸ��2 -- ��Ϸ������
//
void GameInit();
void GameRun( float fDeltaTime );
void GameEnd();


//==============================================================================
//
// ����ṹ��

/*
��ʬ�����ࣺ classify Ĭ��Ϊ1 ��������ģ�� / �ж���ʽ / �ٶ�
*/
struct Zombie{
	int classify;
	float x;
	float y;
	float birthday; // �������shoot_flag���ȫ�ֵ�counter��ɶ�ʱ��Ϊ
	int eatable_flag; // ���˾������̵�һ���̶�֮����
	int get_shot_flag; // ������ʱΪtrue
	int walk_flag; // ���ƽ�ʬ��ʱ��·������
	int life;
	boolean active;
};

// �ӵ�
struct Bullet{
	int speed;
    float x;
    float y;
	int direction;
	int color; // Ĭ��Ϊ1
	int speedX;
	int speedY;
	boolean active;
};

/* ��� */
struct Player{
	float x;
	float y;
	float direction;
	float speedLeft;  	// �����ٶ�
	float speedRight;  	// ��
	float speedTop;  	// ��
	float speedBottom ;  	// ��
};


//==============================================================================
//
// ��������

void Move() ;
int newZombie(float ix, float iy);
void move_zombie_classify1(struct Zombie* zombie , char* szName);
void turnYellow( char *szName );
void turnWhite( char *szName );
void Disappear( struct Zombie* zombie );
int newBullet(float ix, float iy, float idirection, float ispeed);
void bulletMove( struct Bullet* bullet, char* szName);
void playerShot();
void playerMove();

//==============================================================================
//
// ��������

//��λ��������趨
#define ZOMBIE_MAX 50
#define BULLET_MAX 30

// �ӵ��ٶ��趨
#define BULLET_TYPE1_SPEED 100.0
#define BULLET_TYPE2_SPEED 200.0
#define BULLET_TYPE3_SPEED 50.0

// �����ཀྵʬ�ٶ��趨
#define ZOMBIE_TYPE1_SPEED 8.0
#define ZOMBIE_TYPE2_SPEED 30.0
#define ZOMBIE_TYPE3_SPEED 25.0
#define ZOMBIE_TYPE4_SPEED 25.0

// ��ҳ�ʼ����ֵ�趨
#define LIFE_INIT 5

// �����ཀྵʬ��ʼ����ֵ�趨
#define LIFE_OF_TYPE1 5
#define LIFE_OF_TYPE2 10
#define LIFE_OF_TYPE3 8
#define LIFE_OF_TYPE4 3

#define PI 3.1415926535

//==============================================================================
//
// ����ȫ�ֱ���

boolean GodMode = false;	//�޵�ģʽ�������ã�

struct Bullet bullet[BULLET_MAX];	//Ԥ����Bullet��
struct Zombie zombie[ZOMBIE_MAX];	//Ԥ����Zombie��
struct Player player;

int life; // ����ֵ

float SCREEN_LEFT;    // ��Ļ�߽�ֵ
float SCREEN_RIGHT;    
float SCREEN_TOP;    
float SCREEN_BOTTOM;  

int counter; // ȫ�ּ�ʱ��

int score; // ����
int money; // ��Ǯ

// ������
boolean shooting_flag; // ��������ӵ����


// ʱ������ʱ��
float zombie_appear_timer;
float bullet_appear_timer;

//==============================================================================


//==============================================================================
//
// ����ĳ�������Ϊ��GameMainLoop����Ϊ��ѭ��������������ÿ֡ˢ����Ļͼ��֮�󣬶��ᱻ����һ�Ρ�


//==============================================================================
//
// ��Ϸ��ѭ�����˺���������ͣ�ĵ��ã�����ÿˢ��һ����Ļ���˺�����������һ��
// ���Դ�����Ϸ�Ŀ�ʼ�������С������ȸ���״̬. 
// ��������fDeltaTime : �ϴε��ñ��������˴ε��ñ�������ʱ��������λ����
void GameMainLoop( float	fDeltaTime )
{
	switch( g_iGameState )
	{
		// ��ʼ����Ϸ�������һ���������
	case 1:
		{
			GameInit();
			g_iGameState	=	2; // ��ʼ��֮�󣬽���Ϸ״̬����Ϊ������
		}
		break;

		// ��Ϸ�����У����������Ϸ�߼�
	case 2:
		{
			// TODO �޸Ĵ˴���Ϸѭ�������������ȷ��Ϸ�߼�
			if( true )
			{
				GameRun( fDeltaTime );
			}
			else
			{
				// ��Ϸ������������Ϸ���㺯����������Ϸ״̬�޸�Ϊ����״̬
				g_iGameState	=	0;
				GameEnd();
			}
		}
		break;

		// ��Ϸ����/�ȴ����ո����ʼ
	case 0:
	default:
		break;
	};
}

//==============================================================================
//
// ÿ�ֿ�ʼǰ���г�ʼ���������һ���������
void GameInit()
{
	// ��ʼ������ֵ / ���� / ��Ǯ
	life = LIFE_INIT;
	money = 0;
	score = 0;

	// ��ʼ��������
	shooting_flag = false;

	// ��ʼ�������� �� ��ʱ��
	counter = 0;
	zombie_appear_timer = 0.6;
	bullet_appear_timer = 0.5;

	// �õ����ڱ߽�
	SCREEN_LEFT = dGetScreenLeft();
	SCREEN_RIGHT = dGetScreenRight(); 
	SCREEN_TOP = dGetScreenTop();
	SCREEN_BOTTOM = dGetScreenBottom();

	// ѭ�����Ʊ���	
	int i;

	// ��ʼ��zombie
	for(i = 0; i < ZOMBIE_MAX; i++) {
		zombie[i].classify = 1;
		zombie[i].x = 0;
		zombie[i].y = 0;
		zombie[i].life = 0;
		zombie[i].birthday = 0;
		zombie[i].eatable_flag = false;
		zombie[i].active = false;
		zombie[i].eatable_flag = 0;
		zombie[i].get_shot_flag = false;
		zombie[i].walk_flag = 0;
	}

	// ��ʼ���ӵ�
	for(i = 0; i < BULLET_MAX; i++){ 
		bullet[i].speed = 0;
		bullet[i].x = 0.0;
		bullet[i].y = 0.0;
		bullet[i].color = 1;
		bullet[i].direction = 0;
		bullet[i].active = false;
	}

	// ��ʼ�����
	player.x = 0;
	player.y = SCREEN_BOTTOM - 10;
	player.direction = 90;
	player.speedLeft = 0.f;  	// �����ٶ�
	player.speedRight = 0.f;  	// ��
	player.speedTop = 0.f;  	// ��
	player.speedBottom = 0.f;  	// ��
	dSetSpritePosition("player",player.x,player.y);

	// ��ʼ��zombie����
	for(i = 0; i < ZOMBIE_MAX; i++) {
		char szName[64];
		sprintf(szName, "zombie_%d", i);
		dCloneSprite("zombie",szName);
		dSetSpriteVisible(szName,0); // �������ǲ��ɼ�
	}

	// ��ʼ��bullet����
	for(i = 0; i < BULLET_MAX; i++){
		char szName[64];
		sprintf(szName,"bullet_%d",i);
		dCloneSprite("bullet",szName);
		dSetSpriteVisible(szName,0); // �������ǲ��ɼ�
	}
}
//==============================================================================
//
// ÿ����Ϸ������
void GameRun( float fDeltaTime )
{
	// ��ʱ
	counter += fDeltaTime;	
	zombie_appear_timer -= fDeltaTime;
	bullet_appear_timer -= fDeltaTime;

	// ����ȵļ������Zombie
	if( zombie_appear_timer < 0 ){
		zombie_appear_timer = 0.6;
		newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP); // �ڻ��涥������һ�²����ý�ʬ	
	}

	// ����ȵļ������Bullet
	if ( shooting_flag == true){
		if( bullet_appear_timer < 0){
			playerShot();
			shooting_flag = false;
		}
		else{
			shooting_flag = false;
		}
	}

	// �����������ִ�����ǵ���Ϊ
	Move();

}
//====================================================== ========================
//
// ������Ϸ����
void GameEnd()
{
}
//==========================================================================
//
// ����ƶ�
// ���� fMouseX, fMouseY��Ϊ��굱ǰ����
void OnMouseMove( const float fMouseX, const float fMouseY )
{
	
}
//==========================================================================
//
// �����
// ���� iMouseType����갴��ֵ���� enum MouseTypes ����
// ���� fMouseX, fMouseY��Ϊ��굱ǰ����
void OnMouseClick( const int iMouseType, const float fMouseX, const float fMouseY )
{
	
}
//==========================================================================
//
// ��굯��
// ���� iMouseType����갴��ֵ���� enum MouseTypes ����
// ���� fMouseX, fMouseY��Ϊ��굱ǰ����
void OnMouseUp( const int iMouseType, const float fMouseX, const float fMouseY )
{
	
}
//==========================================================================
//
// ���̰���
// ���� iKey�������µļ���ֵ�� enum KeyCodes �궨��
// ���� iAltPress, iShiftPress��iCtrlPress�������ϵĹ��ܼ�Alt��Ctrl��Shift��ǰ�Ƿ�Ҳ���ڰ���״̬(0δ���£�1����)
void OnKeyDown( const int iKey, const bool bAltPress, const bool bShiftPress, const bool bCtrlPress )
{	
	//  �ո��������Ϸ
	if( KEY_SPACE == iKey && 0 == g_iGameState )
	{
		g_iGameState =	1;
	}

	// ���������ƶ�
	switch(iKey)
	{
		case KEY_UP:		
			player.speedTop = -10.f;
			break;
		case KEY_LEFT:
			player.speedLeft = -15.f;		
			break;
		case KEY_DOWN:	
			player.speedBottom = 10.f;
			break;
		case KEY_RIGHT:
			player.speedRight = 15.f;		
			break;
		default:
			break;
	}
	dSetSpriteLinearVelocity("player", player.speedLeft + player.speedRight, player.speedTop + player.speedBottom);

}
//==========================================================================
//
// ���̵���
// ���� iKey������ļ���ֵ�� enum KeyCodes �궨��
void OnKeyUp( const int iKey )
{
	if(iKey == KEY_SPACE)
	{
		shooting_flag = true;
	}

	switch(iKey)
	{
		case KEY_UP:		
			player.speedTop = 0.f;
			break;
		case KEY_LEFT:
			player.speedLeft = 0.f;		
			break;
		case KEY_DOWN:	
			player.speedBottom = 0.f;
			break;
		case KEY_RIGHT:
			player.speedRight = 0.f;		
			break;		
	}
	dSetSpriteLinearVelocity("player", player.speedLeft + player.speedRight, player.speedTop + player.speedBottom);
}
//===========================================================================
//
// �����뾫����ײ
// ���� szSrcName��������ײ�ľ�������
// ���� szTarName������ײ�ľ�������
void OnSpriteColSprite( const char *szSrcName, const char *szTarName )
{
	int i; // i,jΪѭ�����Ʊ���
	int j;
	for(i = 0;i < BULLET_MAX;i++){
		if(!bullet[i].active){
			continue;
		}
		char szName_bullet[64];
		sprintf(szName_bullet,"bullet_%d",i);
		if(!strcmp(szSrcName, szName_bullet)){
			bullet->active = false;
			dSetSpriteVisible(szName_bullet,0);
		}
		for(j = 0;j < ZOMBIE_MAX;j++){
			if(!zombie[i].active){
				continue;
			}
			char szName_zombie[64];
			sprintf(szName_zombie,"zombie_%d",j);
			if(!strcmp(szTarName, szName_zombie)){
				zombie[i].life--;
				zombie[i].get_shot_flag = true;
			}
		}
	}
	
	
}
//===========================================================================
//
// ����������߽���ײ
// ���� szName����ײ���߽�ľ�������
// ���� iColSide����ײ���ı߽� 0 ��ߣ�1 �ұߣ�2 �ϱߣ�3 �±�
void OnSpriteColWorldLimit( const char *szName, const int iColSide )
{
	
}

//===========================================================================
//
// ȫ�ֺ���

void Move() {	

	printf("moving");
	int i; // ѭ�����Ʊ���

	//�����ͷַ�Zombie��Ϊ
	for(i = 0;i <ZOMBIE_MAX;i++){

		// ͬ��λ������
		char szName[64];
		sprintf(szName,"zombie_%d",i);
		zombie[i].x = dGetSpritePositionX(szName);
		zombie[i].y = dGetSpritePositionY(szName);

		if( !zombie[i].active ){
			continue;
		}
		// �����е�Zombie�Ի�ɫ
		if( zombie[i].get_shot_flag ){
			zombie[i].get_shot_flag = false;
			turnYellow(szName);
		}else{
			turnWhite(szName);
		}
		switch ( zombie[i].classify ) {
			case 1:
				printf("ID%d:",i);
				move_zombie_classify1( &zombie[i] , szName );
				break;
			default:
				break;
		}
	}
	// bullet ��Ϊ
	for(i = 0;i <BULLET_MAX;i++){
		char szName[64];
		sprintf(szName,"bullet_%d",i);
		bulletMove( &bullet[i] ,szName);
	}
	// player ��Ϊ
	playerMove();
	
	
}

//===========================================================================
// ���� Zombie �ĺ���

/**
 * Zombie�ļ���
 * ���� ix(float)�����ɴ�x����
 * ���� iy(float)�����ɴ�y����
 * ���� �ӵ�ID(int)��(���û�п�ȱ��-1)
 */
int newZombie(float ix, float iy){
	for (int i = 0; i < ZOMBIE_MAX; i++) {
		if ((zombie[i].active) == false) {
			// �ڲ�
			zombie[i].x = ix;
			zombie[i].y = iy;
			zombie[i].classify = 1; // ��ɿ��ǰ���Ȱ����н�ʬ��������1
			zombie[i].life = LIFE_OF_TYPE1;	
			zombie[i].active = true;
			zombie[i].birthday = counter;

			// �ⲿ��ʾ
			char szName[64];
			sprintf(szName,"zombie_%d",i);
			dSetSpritePositionX(szName,zombie[i].x);
			dSetSpritePositionY(szName,zombie[i].y);
			dSetSpriteVisible(szName,1); 
			printf("new zombie_%d in (zombie[i].x, zombie[i].y)\n",i);
			return i;
		}
	}
	return -1;
}

/**
 * Zombie����Ϊ1
 * ��ʬ������ҵķ�����
 */
void Disappear( struct Zombie* zombie , char* szName ){
	zombie->active = false;
	dSetSpriteVisible(szName,0);
}


/**
 * Zombie����Ϊ1
 * ��ʬ������ҵķ�����
 */
void move_zombie_classify1(struct Zombie* zombie , char* szName){
	float deltaX = player.x - zombie->x ;
	float deltaY = player.y - zombie->y ;
	float delta = sqrt( (deltaX*deltaX) + (deltaY*deltaY) );
	float speedY = (deltaY / delta) * ZOMBIE_TYPE1_SPEED;
	float speedX = (deltaX / delta) * ZOMBIE_TYPE1_SPEED;
	dSetSpriteLinearVelocityY( szName, speedY );
	dSetSpriteLinearVelocityX( szName, speedX );	
	printf("zombie go to (%.2f, %.2f)",zombie->x,zombie->y);

	// �ж��Ƿ�����
	if( zombie->life <= 0){
		Disappear(zombie,szName);
	}
}

//===========================================================================

// ���� �ӵ� �ĺ���

/**
 *  �����ܷ���bullet�ĺ���
 * ������������������boss�ļ��ܰ�
 * */
void FireRound(int x, int y){
	for (int i = 0; i < 360; i += 30 )
	{
		newBullet(x, y, i, 3);
	}
}

 /**
 * �ӵ�����
 * ���� ix(float)�����ɴ�x����
 * ���� iy(float)�����ɴ�y����
 * ���� idirection(float)���ƶ��ķ���
 * ���� ispeed(float)���ƶ����ٶ�
 * ���� �ӵ�ID(float)��(���û�п�ȱ��-1)
*/
int newBullet(float ix, float iy, float idirection, float ispeed) {
	for (int i = 0; i < BULLET_MAX; i++) {
		if ((bullet[i].active) == false) {
			bullet[i].x = ix;
			bullet[i].y = iy;
			bullet[i].direction = idirection;
			bullet[i].speed = ispeed;
			bullet[i].active = true;

			//��ߴ����ٶȣ��Ӽ�����ת��ֱ������
			double radian;
			radian = bullet[i].direction*2*PI/360;	//����ת����
			bullet[i].speedX = bullet[i].speed * cos(radian);
			bullet[i].speedY = -bullet[i].speed * sin(radian); // ����ϵת����ϵ

			char szName[64];
			sprintf(szName,"bullet_%d",i);
			dSetSpritePosition(szName,bullet[i].x,bullet[i].y);
			dSetSpriteLinearVelocity(
				szName,
				bullet[i].speedX,
				bullet[i].speedY
			);
			dSetSpriteVisible(szName,1);
			return i;
		}
	}
	return -1;
}

void bulletMove( struct Bullet* bullet ,char* szName){
	// ͬ��λ��
	bullet->x = dGetSpritePositionX(szName);
	bullet->y = dGetSpritePositionY(szName);

	// �ӵ���������ʧ
	if ( (bullet->x < SCREEN_LEFT)||(SCREEN_RIGHT < bullet->x)||(bullet->y < SCREEN_TOP)||(SCREEN_BOTTOM < bullet->y) ) {
		bullet->active = false;
		dSetSpriteVisible(szName,0);
	}
}

//===========================================================================
//
// ���������Ϊ�ĺ���

/* ��ҷ����ӵ� */
void playerShot() {	
	if(life > 0){
		newBullet(player.x, player.y,player.direction,BULLET_TYPE1_SPEED);
	}	
}

/* �����Ϊ*/
void playerMove(){
	// ͬ��λ��
	player.x = dGetSpritePositionX("player");
	player.y = dGetSpritePositionY("player");

	// �����ӵ����䷽�����ĳ��� 
	// Ŀǰֻ�������ĸ�����֮������
	if(abs((int)(player.speedBottom + player.speedTop)) > abs((int)(player.speedLeft + player.speedRight))){
		if( (player.speedBottom + player.speedTop) > 0){
			// ��������
			if(player.direction == 270){
				return;
			}
			player.direction = 270;
			dAnimateSpritePlayAnimation( "player", "player_testAnimation", 0);
		}else{
			// ��������
			if(player.direction == 90){
				return;
			}
			player.direction = 90;
			dAnimateSpritePlayAnimation( "player", "player_testAnimation3", 0);
		}
	}else if(abs((int)(player.speedBottom + player.speedTop)) < abs((int)(player.speedLeft + player.speedRight))){
		if((player.speedLeft + player.speedRight) > 0){
			// ��������
			if(player.direction == 0){
				return;
			}
			player.direction = 0;
			dAnimateSpritePlayAnimation("player","player_testAnimation2",0);
		}else{
			// ��������
			if(player.direction == 180){
				return;
			}
			player.direction = 180;
			dAnimateSpritePlayAnimation("player","player_testAnimation1",0);
		}
	}
}

//===========================================================================
//
// API ��ʹ�ü򻯺���

/**
 * �������ɻ�ɫ��
 */
void turnYellow( char *szName ){
	dSetSpriteColorBlue( szName, 0 );
	dSetSpriteColorRed( szName, 255 );
	dSetSpriteColorGreen( szName, 255 );
}

/**
 * ������������ɫ��
 */
void turnWhite( char *szName ){
	dSetSpriteColorBlue( szName, 255 );
	dSetSpriteColorRed( szName, 255 );
	dSetSpriteColorGreen( szName, 255 );
}
