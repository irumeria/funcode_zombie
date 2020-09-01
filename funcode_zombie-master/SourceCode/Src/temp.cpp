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

/* ��� */
struct Player{
	float x;
	float y;
	float direction;
	float speedX;
	float speedY;
};


//==============================================================================
//
// ��������

void Move() ;
int newZombie(float ix, float iy);
void move_zombie_classify1(struct Zombie* zombie , char* szName);
void turnYellow( char *szName );
void turnWhite( char *szName );


//==============================================================================
//
// ��������

//��λ��������趨
#define ZOMBIE_MAX 10

// �����ཀྵʬ�ٶ��趨
#define ZOMBIE_TYPE1_SPEED 10.0
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

// ��ʱ�������counterʹ�ã�

// �����������counterʹ�ã�
int zombie_appear_timer; 


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
	printf("initing....");
	// ��ʼ������ֵ / ���� / ��Ǯ
	life = LIFE_INIT;
	money = 0;
	score = 0;

	// ��ʼ�������� �� ��ʱ��
	counter = 0;
	zombie_appear_timer = 1; 

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

	// ��ʼ�����
	player.x = 0;
	player.y = SCREEN_BOTTOM - 10;
	dSetSpritePosition("player",player.x,player.y);

	// ��ʼ��zombie����
	for(i = 0; i < ZOMBIE_MAX; i++) {
		char szName[64];
		sprintf(szName, "zombie_%d", i);
		dCloneSprite("zombie",szName);
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

	// ����Zombie
	if( counter / ( 0.6 * zombie_appear_timer ) > 0 ){
		zombie_appear_timer++;
		newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP); // �ڻ��涥������һ�²����ý�ʬ	
	}


	printf("before move");
	// �����������ִ�����ǵ���Ϊ
	Move();
	printf("after move");
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

	if( KEY_SPACE == iKey && 0 == g_iGameState )
	{
		g_iGameState =	1;
	}

}
//==========================================================================
//
// ���̵���
// ���� iKey������ļ���ֵ�� enum KeyCodes �궨��
void OnKeyUp( const int iKey )
{
	
}
//===========================================================================
//
// �����뾫����ײ
// ���� szSrcName��������ײ�ľ�������
// ���� szTarName������ײ�ľ�������
void OnSpriteColSprite( const char *szSrcName, const char *szTarName )
{
	
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
		switch ( zombie[i].classify ) {
			case 1:
				printf("ID%d:",i);
				move_zombie_classify1( &zombie[i] , szName );
				break;
			default:
				break;
		}
	}
	
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
void move_zombie_classify1(struct Zombie* zombie , char* szName){
	float deltaX = player.x - zombie->x ;
	float deltaY = player.y - zombie->y ;
	float delta = sqrt( (deltaX*deltaX) + (deltaY*deltaY) );
	float speedY = (deltaY / delta) * ZOMBIE_TYPE1_SPEED;
	float speedX = (deltaX / delta) * ZOMBIE_TYPE1_SPEED;
	dSetSpriteLinearVelocityY( szName, speedY );
	dSetSpriteLinearVelocityX( szName, speedX );	
	printf("zombie go to (%.2f, %.2f)",zombie->x,zombie->y);
}
