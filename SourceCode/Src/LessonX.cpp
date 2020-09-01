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
	float speedX;
	float speedY;
	float direction;
	float birthday; // ���ȫ�ֵ�counter��ɶ�ʱ��Ϊ
	float get_shot_timer; // ������ʱΪ¼�����״̬��ʱ��
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
	int id;
	int life;
	int direction;
	float speedLeft;  	// �����ٶ�
	float speedRight;  	// ��
	float speedTop;  	// ��
	float speedBottom ;  	// ��
	boolean shooting_flag; // ��������ӵ����
};

/* ǽ�� */
struct Wall{
	float x;
	float y;
	int classify;
	int life;
	boolean active;
	float get_shot_timer; // ������ʱΪ¼�����״̬��ʱ��
};

//==============================================================================
//
// ��������

void Move() ;
int newZombie(float ix, float iy);
void move_zombie_classify1(struct Zombie* zombie , char* szName);
void turnYellow( char *szName );
void turnWhite( char *szName );
void zombieDisappear( struct Zombie* zombie ,char* szName);
int newBullet(float ix, float iy, float idirection, float ispeed);
void bulletMove( struct Bullet* bullet, char* szName);
void playerShot( struct Player* player );
void playerMove( struct Player* player, char* szName);
int newWall(float ix, float iy, int classify);
void wallMove(struct Wall* wall, char* szName);
void bulletDisappear( struct Bullet* bullet,char* szName);

//==============================================================================
//
// ��������

//��λ��������趨
#define ZOMBIE_MAX 150
#define BULLET_MAX 30
#define WALL_MAX 150

// �ӵ��ٶ��趨
#define BULLET_TYPE1_SPEED 150.0
#define BULLET_TYPE2_SPEED 300.0
#define BULLET_TYPE3_SPEED 75.0

// �����ཀྵʬ�ٶ��趨
#define ZOMBIE_TYPE1_SPEED 5.0
#define ZOMBIE_TYPE2_SPEED 30.0
#define ZOMBIE_TYPE3_SPEED 25.0
#define ZOMBIE_TYPE4_SPEED 25.0

// ������ǽ�ڵ�����ֵ�趨
#define WALL_TYPE1_LIFE 8
#define WALL_TYPE2_LIFE 20

// ��ҳ�ʼ����ֵ�趨
#define LIFE_INIT 5

// �����ཀྵʬ��ʼ����ֵ�趨
#define ZOMBIE_TYPE1_LIFE 3
#define ZOMBIE_TYPE2_LIFE 10
#define ZOMBIE_TYPE3_LIFE 8
#define ZOMBIE_TYPE4_LIFE 2

#define PI 3.1415926535

// ��ʱ���
#define ZOMBIE_APPEAR_TIME 1.8
#define BULLET_APPEAR_TIME 0.6
#define ZOMBIE_HITTEN_TIME 0.3
#define WALL_HITTEN_TIME 0.3

// ������
#define GAP_BETWEEN_PLAYER_AND_BULLET 3

// ��������
#define PLAYER_NUMBER 2
//==============================================================================
//
// ����ȫ�ֱ���

boolean GodMode = false;	//�޵�ģʽ�������ã�

struct Bullet bullet[BULLET_MAX];	//Ԥ����Bullet��
struct Zombie zombie[ZOMBIE_MAX];	//Ԥ����Zombie��
struct Wall wall[WALL_MAX];
struct Player player[PLAYER_NUMBER];

float SCREEN_LEFT;    // ��Ļ�߽�ֵ
float SCREEN_RIGHT;    
float SCREEN_TOP;    
float SCREEN_BOTTOM;  

int counter; // ȫ�ּ�ʱ��

int score; // ����
int money; // ��Ǯ

// ������
/**
 * 1: ��ʼҳ��
 * 2������ģʽ
 * 3����սģʽ
 * 4���޾�ģʽ
 * 5����Ϸ�淨
 */
int game_scene_flag;
int current_scene_flag = 1;

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
	case 0: g_iGameState = 1;
	default:
		break;
	};
}

//==============================================================================
//
// ÿ�ֿ�ʼǰ���г�ʼ���������һ���������
void GameInit()
{
	// �Ȳ�������
	dPlaySound("bgm_test1.ogg", 1, 1.0 );

	// ��ʼ������ֵ / ���� / ��Ǯ
	money = 0;
	score = 0;

	// ��ʼ��������
	game_scene_flag = 1;
	current_scene_flag = 1;

	// ��ʼ�������� �� ��ʱ��
	counter = 0;
	zombie_appear_timer = ZOMBIE_APPEAR_TIME;
	bullet_appear_timer = BULLET_APPEAR_TIME;

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
		zombie[i].direction = 0;
		zombie[i].speedX = 0;
		zombie[i].speedY = 0;
		zombie[i].active = false;
		zombie[i].get_shot_timer = 0;
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

	// ��ʼ��ǽ��
	for(i = 0;i < WALL_MAX;i++){
		wall[i].x = 0.0;
		wall[i].y = 0.0;
		wall[i].life = 0;
		wall[i].classify = 1;
		wall[i].active = false;
		wall[i].get_shot_timer = 0;
	}

	// ��ʼ�����
	for(i = 0;i < PLAYER_NUMBER;i++){
		player[i].life = LIFE_INIT;
		player[i].x = pow(-1,i)*SCREEN_RIGHT/2;
		player[i].y = SCREEN_BOTTOM - 10;
		player[i].id = i;
		player[i].direction = 90;
		player[i].speedLeft = 0.f;  	// �����ٶ�
		player[i].speedRight = 0.f;  	// ��
		player[i].speedTop = 0.f;  	// ��
		player[i].speedBottom = 0.f;  	// ��
		player[i].shooting_flag = false;	
	}
	

	// �����ַ�����ַ��������Դ�˷�
	char szName[64];

	// ��ʼ��zombie����
	for(i = 0; i < ZOMBIE_MAX; i++) {		
		sprintf(szName, "zombie_%d", i);
		dCloneSprite("zombie",szName);
		dSetSpriteVisible(szName,0); // �������ǲ��ɼ�
	}

	// ��ʼ��bullet����
	for(i = 0; i < BULLET_MAX; i++){
		sprintf(szName,"bullet_%d",i);
		dCloneSprite("bullet",szName);
		dSetSpriteVisible(szName,0); // �������ǲ��ɼ�
	}

	// ��ʼ��wall����
	for(i = 0;i < WALL_MAX;i++){
		sprintf(szName,"wall_%d",i);
		dCloneSprite("wall",szName);
		dSetSpriteVisible(szName,0);
	}

	// ��ʼ��player����
	for(i = 0;i < PLAYER_NUMBER;i++){
		sprintf(szName,"player_%d",i);
		dSetSpritePosition( szName,player[i].x,player[i].y);
	}
	
	// ���������ͼ�ϵ�ǽ��
	// for(i = 0;i < 30;i++){
	// 	float random1 = rand()%10/10.0*SCREEN_RIGHT*2 - SCREEN_RIGHT;
	// 	float random2 = rand()%10/10.0*SCREEN_BOTTOM*2 - SCREEN_BOTTOM;
	// 	newWall(random1,random2,1); // ��ɿ��ǰ�Ȱ�����ǽ�ڵ�������1
	// }
}
//==============================================================================
//
// ÿ����Ϸ������
void GameRun( float fDeltaTime )
{

	// �ж���Ϸ���ĸ�������
	if( current_scene_flag == game_scene_flag){		
	}else{
		switch( game_scene_flag ){
			case 1: dLoadMap("start.t2d");break;
			case 2: dLoadMap("baseView.t2d");break;
		}
		current_scene_flag = game_scene_flag;
	}
	int i; // ѭ�����Ʊ���

	// ��ʱ
	counter += fDeltaTime;	
	zombie_appear_timer -= fDeltaTime;
	bullet_appear_timer -= fDeltaTime;
	for( i = 0;i < ZOMBIE_MAX;i++){
		zombie[i].get_shot_timer -= fDeltaTime;
	}
	for( i = 0;i < WALL_MAX;i++){
		wall[i].get_shot_timer -= fDeltaTime;
	}

	// ����ȵļ������Zombie
	if( zombie_appear_timer < 0 ){
		zombie_appear_timer = ZOMBIE_APPEAR_TIME;
		newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP); // �ڻ��涥������һ�²����ý�ʬ	
	}

	// ����ȵļ������Bullet
	// ������ 1. �ո������ 2. 0.5����ӵ�װ��ʱ�� �ſ��Է����ӵ�
	for( i = 0;i < PLAYER_NUMBER;i++){

		if ( player[i].shooting_flag == true){
		if( bullet_appear_timer < 0){
			playerShot( &player[i] );
			player[i].shooting_flag = false;
			bullet_appear_timer = BULLET_APPEAR_TIME;
		}
		else{
			player[i].shooting_flag = false;
		}
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
	if( iMouseType == 0 ){
		float left = dGetSpritePositionX("start") - dGetSpriteWidth("start")/2;
		float right = dGetSpritePositionX("start") + dGetSpriteWidth("start")/2;
		float top = dGetSpritePositionY("start") - dGetSpriteHeight("start")/2;
		float bottom = dGetSpritePositionY("start") - dGetSpriteHeight("start")/2;
		if( (fMouseX > left)&&(fMouseX < right)&&(fMouseY > top)&&(fMouseY < bottom)){
			game_scene_flag = 2;
		}
	}
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
	// if( KEY_SPACE == iKey && 0 == g_iGameState )
	// {
	// 	g_iGameState =	1;
		
	// }

	// ���������ƶ�
	switch(iKey)
	{
		case KEY_UP:		
			player[0].speedTop = -8.f;
			break;
		case KEY_LEFT:
			player[0].speedLeft = -10.f;		
			break;
		case KEY_DOWN:	
			player[0].speedBottom = 8.f;
			break;
		case KEY_RIGHT:
			player[0].speedRight = 10.f;		
			break;
		default:
			break;
	}
	dSetSpriteLinearVelocity("player_0", player[0].speedLeft + player[0].speedRight, player[0].speedTop + player[0].speedBottom);

	// ���������ƶ�
	switch(iKey)
	{
		case KEY_W:		
			player[1].speedTop = -8.f;
			break;
		case KEY_A:
			player[1].speedLeft = -10.f;		
			break;
		case KEY_S:	
			player[1].speedBottom = 8.f;
			break;
		case KEY_D:
			player[1].speedRight = 10.f;		
			break;
		default:
			break;
	}
	dSetSpriteLinearVelocity("player_1", player[1].speedLeft + player[1].speedRight, player[1].speedTop + player[1].speedBottom);

}
//==========================================================================
//
// ���̵���
// ���� iKey������ļ���ֵ�� enum KeyCodes �궨��
void OnKeyUp( const int iKey )
{
	if(iKey == KEY_SPACE)
	{
		player[0].shooting_flag = true;
	}

	if(iKey == KEY_G)
	{
		player[1].shooting_flag = true;
	}

	switch(iKey)
	{
		case KEY_UP:		
			player[0].speedTop = 0.f;
			break;
		case KEY_LEFT:
			player[0].speedLeft = 0.f;		
			break;
		case KEY_DOWN:	
			player[0].speedBottom = 0.f;
			break;
		case KEY_RIGHT:
			player[0].speedRight = 0.f;		
			break;		
	}
	dSetSpriteLinearVelocity("player_0", player[0].speedLeft + player[0].speedRight, player[0].speedTop + player[0].speedBottom);

	switch(iKey)
	{
		case KEY_W:		
			player[1].speedTop = 0.f;
			break;
		case KEY_A:
			player[1].speedLeft = 0.f;		
			break;
		case KEY_S:	
			player[1].speedBottom = 0.f;
			break;
		case KEY_D:
			player[1].speedRight = 0.f;		
			break;		
	}
	dSetSpriteLinearVelocity("player_1", player[1].speedLeft + player[1].speedRight, player[1].speedTop + player[1].speedBottom);
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
	char szName_bullet[64];
	char szName_zombie[64];
	char szName_wall[64];
	boolean activeflag = false;
	for(i = 0;i < BULLET_MAX;i++){
		if(!bullet[i].active){
			continue;
		}		
		sprintf(szName_bullet,"bullet_%d",i);
		if( !strcmp(szSrcName, szName_bullet) ){
			for(j = 0;j < ZOMBIE_MAX;j++){
				if(!zombie[j].active){
					continue;
				}	
				sprintf(szName_zombie,"zombie_%d",j);
				if( !strcmp(szTarName, szName_zombie) ){
					zombie[j].life--;
					zombie[j].get_shot_timer = ZOMBIE_HITTEN_TIME;
					activeflag = true;
				}
			}
			for(j = 0;j < WALL_MAX;j++){
				if(!wall[j].active){
					continue;
				}
				sprintf(szName_wall,"wall_%d",j);
				if( !strcmp(szTarName,szName_wall) ){
					wall[j].life--;
					wall[j].get_shot_timer = WALL_HITTEN_TIME;
					activeflag = true;
				}
			}
			if( activeflag == true ){
				bulletDisappear(&bullet[i],szName_bullet);
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
	char szName[64];
	int i; // ѭ�����Ʊ���

	//�����ͷַ�Zombie��Ϊ
	for(i = 0;i <ZOMBIE_MAX;i++){
		if( !zombie[i].active ){
			continue;
		}
		sprintf(szName,"zombie_%d",i);
		// �����е�Zombie�Ի�ɫ
		if( zombie[i].get_shot_timer > 0 ){
			turnYellow(szName);
		}else{
			turnWhite(szName);
		}
		switch ( zombie[i].classify ) {
			case 1:
				move_zombie_classify1( &zombie[i] , szName );
				break;
			default:
				break;
		}
	}
	// bullet ��Ϊ
	for(i = 0;i < BULLET_MAX;i++){
		if( !bullet[i].active ){
			continue;
		}
		sprintf(szName,"bullet_%d",i);
		bulletMove( &bullet[i] ,szName );
	}
	// wall ��Ϊ
	for(i = 0;i < WALL_MAX;i++){
		if( !wall[i].active ){
			continue;
		}
		// �����е�Wall�Ի�ɫ
		if( wall[i].get_shot_timer > 0 ){
			turnYellow(szName);
		}else{
			turnWhite(szName);
		}
		sprintf(szName,"wall_%d",i);
		wallMove( &wall[i],szName );
	}
	
	// player ��Ϊ
	for(i = 0;i <PLAYER_NUMBER;i++){
		sprintf(szName,"player_%d",i);
		playerMove( &player[i],szName );
	}
	
	
}

//===========================================================================
// ���� Zombie �ĺ���

/**
 * Zombie�ļ���
 * ���� ix(float)�����ɴ�x����
 * ���� iy(float)�����ɴ�y����
 * ���� ��ʬID(int)��(���û�п�ȱ��-1)
 */
int newZombie(float ix, float iy){
	for (int i = 0; i < ZOMBIE_MAX; i++) {
		if (zombie[i].active == false) {
			// �ڲ�
			zombie[i].x = ix;
			zombie[i].y = iy;
			zombie[i].classify = 1; // ��ɿ��ǰ���Ȱ����н�ʬ��������1
			zombie[i].life = ZOMBIE_TYPE1_LIFE;	
			zombie[i].direction = 270;
 			zombie[i].active = true;
			zombie[i].birthday = counter;

			// �ⲿ��ʾ
			char szName[64];
			sprintf(szName,"zombie_%d",i);
			dSetSpritePositionX(szName,zombie[i].x);
			dSetSpritePositionY(szName,zombie[i].y);
			dSetSpriteVisible(szName,1); 
			return i;
		}
	}
	return -1;
}

/**
 * Zombie�ҵ�
 * ��ʧ��
 */
void zombieDisappear( struct Zombie* zombie , char* szName ){
	zombie->active = false;
	dSetSpriteVisible(szName,0);
}


/**
 * Zombie����Ϊ1
 * ��ʬ������ҵķ�����
 */
void move_zombie_classify1(struct Zombie* zombie , char* szName){

	// ͬ��λ��
	zombie->x = dGetSpritePositionX(szName);
	zombie->y = dGetSpritePositionY(szName);
	// zombie->direction = zombie->x

	float deltaX[2];
	float deltaY[2];
	float delta[2];
	float speedY;
	float speedX;
	// �����˷�����
	for(int i = 0;i < PLAYER_NUMBER;i++){
		deltaX[i] = player[i].x - zombie->x;
		deltaY[i] = player[i].y - zombie->y;
		delta[i] = sqrt( (deltaX[i]*deltaX[i]) + (deltaY[i]*deltaY[i]) );
	}
	if(delta[0] <= delta[1]){
		speedY = (deltaY[0] / delta[0]) * ZOMBIE_TYPE1_SPEED;
		speedX = (deltaX[0] / delta[0]) * ZOMBIE_TYPE1_SPEED;	
	}else{
		speedY = (deltaY[1] / delta[1]) * ZOMBIE_TYPE1_SPEED;
		speedX = (deltaX[1] / delta[1]) * ZOMBIE_TYPE1_SPEED;	
	}

	// �ж�һ���Ƿ�ײǽ
	// int i;
	// char szName_wall[64];
	// for(i = 0;i < WALL_MAX;i++){
	// 	if( !wall[i].active ){
	// 		continue;
	// 	}
	// 	sprintf(szName_wall,"wall_%d",i);
	// 	float gapX = dGetSpriteWidth(szName_wall)/2 + dGetSpriteWidth(szName)/2;
	// 	float gapY = dGetSpriteHeight(szName_wall)/2 + dGetSpriteHeight(szName)/2;
	// 	if((zombie->y - wall[i].y) < gapY){
	// 		speedY = 0;
	// 		// if( wall[i].get_shot_timer < 0){
	// 		// 	wall[i].life--;
	// 		// 	wall[i].get_shot_timer = WALL_HITTEN_TIME;
	// 		// }			
	// 	}else if((zombie->x - wall[i].x) < gapX){
	// 		speedX = 0;
	// 		// if( wall[i].get_shot_timer < 0){
	// 		// 	wall[i].life--;
	// 		// 	wall[i].get_shot_timer = WALL_HITTEN_TIME;
	// 		// }	
	// 	}
	// }	
	zombie->speedX = speedX;
	zombie->speedY = speedY;
	dSetSpriteLinearVelocityY( szName, speedY );
	dSetSpriteLinearVelocityX( szName, speedX );

	// �ж��Ƿ�����
	if( zombie->life <= 0){
		zombieDisappear(zombie,szName);
	}

	if(abs((int)speedY) > abs((int)speedX)){
		if( speedY > 0){
			// ��������
			if(zombie->direction == 270){
				return;
			}
			zombie->direction = 270;
			dAnimateSpritePlayAnimation( szName, "zombie1Animation3", 0);
		}else{
			// ��������
			if(zombie->direction == 90){
				return;
			}
			zombie->direction = 90;
			dAnimateSpritePlayAnimation( szName, "zombie1Animation2", 0);
		}
	}else if(abs((int)speedX) > abs((int)speedY)){
		if( speedX > 0 ){
			// ��������
			if(zombie->direction == 0){
				return;
			}
			zombie->direction = 0;
			dAnimateSpritePlayAnimation( szName,"zombie1Animation",0);
		}else{
			// ��������
			if(zombie->direction == 180){
				return;
			}
			zombie->direction = 180;
			dAnimateSpritePlayAnimation( szName,"zombie1Animation1",0);
		}
	}

		

	
}

//===========================================================================
//
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

			// �Ӽ�����ת��ֱ������
			double radian;
			radian = bullet[i].direction*2*PI/360;	// ����ת����
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
			dSetSpriteRotation( szName, 90 - bullet[i].direction );
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

/**
 * Bullet��ʧ
 */
void bulletDisappear( struct Bullet* bullet , char* szName ){
	bullet->active = false;
	dSetSpriteVisible(szName,0);
}
//===========================================================================
//
// ���� ǽ�� �ĺ���

/**
 * Wall�ļ���
 * ���� ix(float)�����ɴ�x����
 * ���� iy(float)�����ɴ�y����
 * ���� classify(int): ����ǽ�ڵ�����
 * ���� ǽ��ID(int)��(���û�п�ȱ��-1)
 */
int newWall(float ix, float iy, int classify){
	char szName[64];
	for (int i = 0; i < WALL_MAX; i++) {
		if (wall[i].active == false) {
			// �ڲ�
			wall[i].x = ix;
			wall[i].y = iy;
			wall[i].classify = classify; 
			wall[i].life = WALL_TYPE1_LIFE;	// ��ɿ��ǰ���Ƚ�����ǽ�ڵ�������1
 			wall[i].active = true;

			// �ⲿ��ʾ		
			sprintf(szName,"wall_%d",i);
			dSetSpritePositionX(szName,wall[i].x);
			dSetSpritePositionY(szName,wall[i].y);
			dSetSpriteVisible(szName,1); 
			return i;
		}
	}
	return -1;
}

/* ǽ�ڵ���Ϊ */
void wallMove(struct Wall* wall, char* szName){

	if( wall->life <= 0){
		wall->active = false;
		dSetSpriteVisible(szName,0);
	}
}

//===========================================================================
//
// ���������Ϊ�ĺ���

/* ��ҷ����ӵ� */
void playerShot(Player* player) {	
	if( player->life > 0){
		int shootX;
		int shootY;
		char szName[64];
		sprintf(szName,"player_%d",player->id);
		switch ( player->direction ){
			case 0: // ���ҿ�ǹ
				shootX = player->x + dGetSpriteWidth( szName )/2 + GAP_BETWEEN_PLAYER_AND_BULLET; 
				shootY = player->y;
				break; 
			case 45: // �����Ϸ���ǹ
				shootX = player->x + dGetSpriteWidth( szName )/2 + GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y + dGetSpriteHeight( szName )/2 + GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 90: // ���Ͽ�ǹ
				shootX = player->x;
				shootY = player->y - dGetSpriteHeight( szName )/2 - GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 135: // �����Ϸ���ǹ
				shootX = player->x - dGetSpriteWidth( szName )/2 - GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y - dGetSpriteHeight( szName )/2 - GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 180: // ����ǹ
				shootX = player->x - dGetSpriteHeight( szName )/2 - GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y;
				break;
			case 225: // �����·���ǹ
				shootX = player->x - dGetSpriteWidth( szName )/2 - GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y + dGetSpriteHeight( szName )/2 + GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 270: // ���¿�ǹ
				shootX = player->x;
				shootY = player->y + dGetSpriteHeight( szName )/2 + GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 315: // �����·���ǹ
				shootX = player->x + dGetSpriteWidth( szName )/2 + GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y + dGetSpriteHeight( szName )/2 + GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
		}
		dPlaySound("gun_type1.wav", 0, 1 );	
		newBullet(
			shootX, 
			shootY,
			player->direction,
			BULLET_TYPE1_SPEED
		);
	}	
}

/* �����Ϊ */
void playerMove( struct Player* player, char* szName){
	// ͬ��λ��
	player->x = dGetSpritePositionX(szName);
	player->y = dGetSpritePositionY(szName);

	// ������ҵĲ�ͬ��ȡ����Ĳ�ͬ 
	char left[64];
	char right[64];
	char bottom[64];
	char top[64];
	if(player->id == 0){
		sprintf(left,"player%dAnimation1",player->id+1);
		sprintf(right,"player%dAnimation2",player->id+1);
		sprintf(bottom,"player%dAnimation3",player->id+1);
		sprintf(top,"player%dAnimation",player->id+1);
	}else if(player->id == 1){
		sprintf(left,"player%dAnimation1",player->id+1);
		sprintf(right,"player%dAnimation",player->id+1);
		sprintf(bottom,"player%dAnimation3",player->id+1);
		sprintf(top,"player%dAnimation2",player->id+1);
	}
	// �����ӵ����䷽�����ĳ��� 
	// Ŀǰֻ�������ĸ�����֮������
	if(abs((int)(player->speedBottom + player->speedTop)) > abs((int)(player->speedLeft + player->speedRight))){
		if( (player->speedBottom + player->speedTop) > 0){
			// ��������
			if(player->direction == 270){
				return;
			}
			player->direction = 270;
			dAnimateSpritePlayAnimation( szName, bottom, 0);
		}else{
			// ��������
			if(player->direction == 90){
				return;
			}
			player->direction = 90;
			dAnimateSpritePlayAnimation( szName,top, 0);
		}
	}else if(abs((int)(player->speedBottom + player->speedTop)) < abs((int)(player->speedLeft + player->speedRight))){
		if((player->speedLeft + player->speedRight) > 0){
			// ��������
			if(player->direction == 0){
				return;
			}
			player->direction = 0;
			dAnimateSpritePlayAnimation(szName,right,0);
		}else{
			// ��������
			if(player->direction == 180){
				return;
			}
			player->direction = 180;
			dAnimateSpritePlayAnimation(szName,left,0);
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
