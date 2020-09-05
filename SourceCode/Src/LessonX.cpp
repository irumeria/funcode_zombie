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
	int direction;
	float fire_timer;
	float get_shot_timer; // ������ʱΪ¼�����״̬��ʱ��
	int life;
	boolean active;
};

// �ӵ�
struct Bullet{
	int classify;
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
	float speed;
	float speedX;  	// X�����ٶ�
	float speedY;   // Y�����ٶ�
	float speedTop;
	float speedLeft;
	float speedRight;
	float speedBottom;
	boolean shooting_flag; // ��������ӵ����
	float bitten_timer; // �˱�ҧ��֮��ʼ��ʱ
	Zombie* bit_zombie; // ҧ�˵Ľ�ʬ
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

/* ����ڵ� */
struct Plot{
	int id; 
	char* speaker; // ˵��������
	char* content; // ˵��������
	int picture1; // ��Ӧ������
	int picture2;
	boolean end_flag;
};

//==============================================================================
//
// ��������

void Move() ;
int newZombie(float ix, float iy,int classify);
void zombieMove(struct Zombie* zombie , char* szName);
void turnYellow( char *szName );
void turnWhite( char *szName );
void turnGray( char *szName );
void turnRed( char *szName );
void zombieDisappear( struct Zombie* zombie ,char* szName);
int newBullet(float ix, float iy, float idirection, float ispeed, int classify);
void bulletMove( struct Bullet* bullet, char* szName);
void playerShot( struct Player* player );
void playerMove( struct Player* player, char* szName);
int newWall(float ix, float iy, int classify);
void wallMove(struct Wall* wall, char* szName);
void bulletDisappear( struct Bullet* bullet,char* szName);
void loadPlots();
void playPlots( float fDeltaTime );
void FireAim(float x,float y,int direction,char* user);
void FireRound(float x, float y,char* user);

//==============================================================================
//
// ��������

//��λ��������趨
#define ZOMBIE_MAX 150
#define BULLET_MAX 30
#define WALL_MAX 150

// �������ٶ��趨
#define PLAYER_SPEED 10
// �ӵ��ٶ��趨
#define BULLET_TYPE1_SPEED 150.0
#define BULLET_TYPE2_SPEED 300.0

// ��ʬ�ӵ��ٶ��趨
#define BULLET_TYPE1_SPEED_ZOMBIE 30
#define BULLET_TYPE2_SPEED_ZOMBIE 80

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
#define ZOMBIE_APPEAR_TIME 2.8
#define BULLET_APPEAR_TIME 0.6
#define ZOMBIE_HITTEN_TIME 0.3
#define WALL_HITTEN_TIME 0.3
#define PLAYER_BITTEN_TIME 0.2
#define ZOMBIE_FIRE_TIME 2

// ������
#define GAP_BETWEEN_PLAYER_AND_BULLET 1

// ��������
#define PLAYER_NUMBER 2

// ��������
#define BOY "�޼�"
#define GIRL "��˹��"

// ��������Ӧ��֡��
#define BOY_NORMAL 0
#define BOY_NERVOUS 1
#define BOY_BAD 2
#define BOY_SCARED 3
#define BOY_CRY 4
#define GIRL_NORMAL 0
#define GIRL_CURIOUS 1
#define GIRL_HAPPY 2

// ��Ϸ�еĸ���״̬
#define MAIN_GAME 1
#define TALKING 2
#define SHOPPING 3

//==============================================================================
//
// ����ȫ�ֱ���

boolean GodMode = false;	//�޵�ģʽ�������ã�

struct Bullet bullet[BULLET_MAX];	//Ԥ����Bullet��
struct Zombie zombie[ZOMBIE_MAX];	//Ԥ����Zombie��
struct Wall wall[WALL_MAX];  //Ԥ����Wall��
struct Player player[PLAYER_NUMBER];  

struct Plot plot[100]; // ���ɾ�������
int current_plot;
int currnet_plot_ensure;

float SCREEN_LEFT;    // ��Ļ�߽�ֵ
float SCREEN_RIGHT;    
float SCREEN_TOP;    
float SCREEN_BOTTOM;  

float counter; // ȫ�ּ�ʱ��

int score; // ����
int money; // ��Ǯ


// ������

/**
 * ����������
 * 1: ��ʼҳ��
 * 2������ģʽ
 * 3����սģʽ
 * 4���޾�ģʽ
 * 5����Ϸ�淨
 */
int game_scene_flag = 1;
int current_scene_flag = 0;

/**
 * ״̬������
 * MAIN_GAME 1 ��Ϸ��״̬
 * TALKING 2 ����Ի�����״̬
 * SHOPPING 3 �Զ��ۻ���������״̬
 */
int game_mode_flag;

boolean click_flag = false; // ��������


boolean boss_flag; // ����boss
// ʱ������ʱ��
float zombie_appear_timer;
float bullet_appear_timer;

//==============================================================================
//
// ����ĳ�������Ϊ��GameMainLoop����Ϊ��ѭ��������������ÿ֡ˢ����Ļͼ��֮�󣬶��ᱻ����һ�Ρ�


//==============================================================================
//
// ��Ϸ��ѭ�����˺���������ͣ�ĵ��ã�����ÿˢ��һ����Ļ���˺�����������һ��
// ���Դ�����Ϸ�Ŀ�ʼ�������С������ȸ���״̬. 
// ��������fDeltaTime : �ϴε��ñ��������˴ε��ñ�������ʱ��������λ����
void GameMainLoop( float fDeltaTime )
{
	

	switch( g_iGameState )
	{
		// ��ʼ����Ϸ�������һ���������
	case 1:
		{		
			g_iGameState = 2; // ��ʼ��֮�󣬽���Ϸ״̬����Ϊ������
		}
		break;

		// ��Ϸ�����У����������Ϸ�߼�
	case 2:
		{
			// TODO �޸Ĵ˴���Ϸѭ�������������ȷ��Ϸ�߼�
			if( true )
			{
				// �ж���Ϸ���ĸ�������
				if( current_scene_flag == game_scene_flag ){							
				}else{
					switch( game_scene_flag ){
						case 1: dLoadMap("start.t2d");break;
						case 2: dLoadMap("baseView.t2d"); GameInit();loadPlots(); break;
					}
					current_scene_flag = game_scene_flag;
				}
				if( game_scene_flag == 2){
					if(game_mode_flag == MAIN_GAME){
						GameRun( fDeltaTime );
					}
					if(game_mode_flag == TALKING){
						playPlots( fDeltaTime );
					}				
				}
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
	boss_flag = false;
	// ��ʼ�������� �� ��ʱ��
	counter = 10;
	zombie_appear_timer = ZOMBIE_APPEAR_TIME;
	bullet_appear_timer = BULLET_APPEAR_TIME;

	// �õ����ڱ߽�
	SCREEN_LEFT = dGetScreenLeft();
	SCREEN_RIGHT = dGetScreenRight(); 
	SCREEN_TOP = dGetScreenTop();
	SCREEN_BOTTOM = dGetScreenBottom();

	// �Ƚ������ģʽ
	game_mode_flag = TALKING;
	current_plot = 0;
	currnet_plot_ensure = -1;

	// ѭ�����Ʊ���	
	int i;

	// ��ʼ��zombie
	for(i = 0; i < ZOMBIE_MAX; i++) {
		zombie[i].classify = 1;
		zombie[i].x = 0;
		zombie[i].y = 0;
		zombie[i].life = 0;
		zombie[i].direction = 0;
		zombie[i].speedX = 0;
		zombie[i].speedY = 0;
		zombie[i].active = false;
		zombie[i].get_shot_timer = 0;
		zombie[i].fire_timer = ZOMBIE_FIRE_TIME;
	}

	// ��ʼ���ӵ�
	for(i = 0; i < BULLET_MAX; i++){ 
		bullet[i].classify = 1;
		bullet[i].speed = 0;
		bullet[i].x = 0.0;
		bullet[i].y = 0.0;
		bullet[i].color = 1;
		bullet[i].direction = 0;
		bullet[i].active = false;
	}

	// ��ʼ����ʬ���ӵ�
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
		player[i].x = -pow(-1,i)*SCREEN_RIGHT/2;
		player[i].y = SCREEN_BOTTOM - 10;
		player[i].id = i;
		player[i].direction = 90;
		player[i].speed = PLAYER_SPEED;
		player[i].speedX = 0.f;  	// X�����ٶ�
		player[i].speedY = 0.f; // Y�����ٶ�
		player[i].speedRight = 0.f;  	
		player[i].speedLeft = 0.f;  	
		player[i].speedTop = 0.f;  
		player[i].speedBottom = 0.f;  
		player[i].shooting_flag = false;	
		player[i].bitten_timer = 0;
		player[i].bit_zombie = NULL;
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
		dSetSpritePosition( szName,player[i].x,player[i].y );
	}
	
	// ���������ͼ�ϵ�ǽ��
	for(i = 0;i < 10;i++){
		float random1 = rand()%10/10.0*SCREEN_RIGHT*2 - SCREEN_RIGHT;
		float random2 = rand()%10/10.0*SCREEN_BOTTOM*2 - SCREEN_BOTTOM;
		newWall(random1,random2,1); // ��ɿ��ǰ�Ȱ�����ǽ�ڵ�������1
	}
}
//==============================================================================
//
// ÿ����Ϸ������
void GameRun( float fDeltaTime )
{	
	int i; // ѭ�����Ʊ���

	// ��ʱ
	counter -= fDeltaTime;	
	zombie_appear_timer -= fDeltaTime;
	bullet_appear_timer -= fDeltaTime;
	for( i = 0;i < ZOMBIE_MAX;i++){
		zombie[i].get_shot_timer -= fDeltaTime;
		if(zombie[i].classify == 2){
			zombie[i].fire_timer -= fDeltaTime;
		}
		
		
	}
	for( i = 0;i < WALL_MAX;i++){
		wall[i].get_shot_timer -= fDeltaTime;
	}
	for( i = 0;i < PLAYER_NUMBER;i++){
		player[i].bitten_timer -= fDeltaTime;
	}

	// ����ȵļ������Zombie
	if( counter >= 0){
		if( zombie_appear_timer < 0 ){
			zombie_appear_timer = ZOMBIE_APPEAR_TIME;
			newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP,1); // �ڻ��涥������һ�²����ý�ʬ	
		}
	}
	// ����boss
	if( (counter < 0) && (boss_flag == false) ){
		boss_flag = true;
		newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP,2);
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
	dSetSpritePosition("cursor",fMouseX,fMouseY);
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
	if( iMouseType == 0 ){
		click_flag = true;
	}
}
//==========================================================================
//
// ���̰���
// ���� iKey�������µļ���ֵ�� enum KeyCodes �궨��
// ���� iAltPress, iShiftPress��iCtrlPress�������ϵĹ��ܼ�Alt��Ctrl��Shift��ǰ�Ƿ�Ҳ���ڰ���״̬(0δ���£�1����)
void OnKeyDown( const int iKey, const bool bAltPress, const bool bShiftPress, const bool bCtrlPress )
{	

	// ���������ƶ�
	if( player[0].bitten_timer <= 0){
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
		float totalSpeed = abs((int)(player[0].speedLeft + player[0].speedRight ))+ abs((int)(player[0].speedTop + player[0].speedBottom ));
		player[0].speedX = player[0].speed*(player[0].speedLeft + player[0].speedRight)/totalSpeed;
		player[0].speedY = player[0].speed*(player[0].speedTop + player[0].speedBottom)/totalSpeed;
		dSetSpriteLinearVelocity(
			"player_0", 
			player[0].speedX, 
			player[0].speedY
		);
	}
	

	// ���������ƶ�
	if( player[1].bitten_timer <= 0 ){
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
		float totalSpeed = abs((int)(player[1].speedLeft + player[1].speedRight ))+ abs((int)(player[1].speedTop + player[1].speedBottom ));
		player[1].speedX = player[1].speed*(player[1].speedLeft + player[1].speedRight)/totalSpeed;
		player[1].speedY = player[1].speed*(player[1].speedTop + player[1].speedBottom)/totalSpeed;
		dSetSpriteLinearVelocity(
			"player_1", 
			player[1].speedX, 
			player[1].speedY
		);
	}
	

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

	if( player[0].bitten_timer <= 0 ){
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
		float totalSpeed = abs((int)(player[0].speedLeft + player[0].speedRight ))+ abs((int)(player[0].speedTop + player[0].speedBottom ));
		player[0].speedX = player[0].speed*(player[0].speedLeft + player[0].speedRight)/totalSpeed;
		player[0].speedY = player[0].speed*(player[0].speedTop + player[0].speedBottom)/totalSpeed;
		dSetSpriteLinearVelocity(
			"player_0", 
			player[0].speedX, 
			player[0].speedY
		);
	}
	
	if( player[1].bitten_timer <= 0 ){
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
		float totalSpeed = abs((int)(player[1].speedLeft + player[1].speedRight ))+ abs((int)(player[1].speedTop+ player[1].speedBottom ));
		player[1].speedX = player[1].speed*(player[1].speedLeft + player[1].speedRight)/totalSpeed;
		player[1].speedY = player[1].speed*(player[1].speedTop + player[1].speedBottom)/totalSpeed;
		dSetSpriteLinearVelocity(
			"player_1", 
			player[1].speedX, 
			player[1].speedY
		);
	}
	
}
//===========================================================================
//
// �����뾫����ײ
// ���� szSrcName��������ײ�ľ�������
// ���� szTarName������ײ�ľ�������
void OnSpriteColSprite( const char *szSrcName, const char *szTarName )
{
	// �ȼ����꾫����ײ�¼�
	// ����¼�1���ӳ���1�л�������2����ʼ����ģʽ
	if( click_flag ){
		click_flag = false;
		if( !strcmp(szSrcName, "cursor") && !strcmp(szTarName, "start") ){
			game_scene_flag = 2;
		}
	}

	int i; // i,jΪѭ�����Ʊ���
	int j;
	char szName_bullet[64];
	char szName_zombie[64];
	char szName_wall[64];
	char szName_player[2];
	boolean activeflag = false;

	for(i = 0;i < BULLET_MAX;i++){
		if(!bullet[i].active){
			continue;
		}	
		sprintf(szName_bullet,"bullet_%d",i);
		if( !strcmp(szSrcName, szName_bullet) ){
			// ���������ӵ�
			if( bullet[i].classify == 1 ){
				for(j = 0;j < ZOMBIE_MAX;j++){
					if(!zombie[j].active){
						continue;
					}	
					sprintf(szName_zombie,"zombie_%d",j);
					if( !strcmp(szTarName, szName_zombie) ){
						COL_RESPONSE_RIGID;
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
			}else if( bullet[i].classify == 2){ // ��ʬ����ĵ�Ļ
				for(j = 0;j < PLAYER_NUMBER;j++){
					if( !player[j].life > 0 ){
						continue;
					}
					sprintf(szName_player,"player_%d",j);
					if( !strcmp(szTarName, szName_player) ){
						if( player[j].bitten_timer <= 0 ){
							player[j].bitten_timer = PLAYER_BITTEN_TIME;
							activeflag = true;
						}
					}
				}	
				if( activeflag == true ){
					bulletDisappear(&bullet[i],szName_bullet);
				}
			}
			
		}
	}
	for( i = 0;i < ZOMBIE_MAX ;i++){
		if( !zombie[i].active ){
			continue;
		}
		sprintf(szName_zombie,"zombie_%d",i);
		if( !strcmp(szSrcName, szName_zombie) ){
			for(j = 0;j < PLAYER_NUMBER;j++){
				sprintf(szName_player,"player_%d",j);
				if( !strcmp(szTarName, szName_player) ){
					if( player[j].bitten_timer <= 0 ){
						player[j].bitten_timer = PLAYER_BITTEN_TIME;
						player[j].bit_zombie = zombie;
					}
					dSetSpriteLinearVelocityY( szName_zombie, 0 );
					dSetSpriteLinearVelocityX( szName_zombie, 0 );
				}
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
		zombieMove( &zombie[i],szName );
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
 * ���� classify(int)�����ɽ�ʬ������
 * ���� ��ʬID(int)��(���û�п�ȱ��-1)
 */
int newZombie(float ix, float iy,int classify){
	for (int i = 0; i < ZOMBIE_MAX; i++) {
		if (zombie[i].active == false) {
			// �ڲ�
			zombie[i].x = ix;
			zombie[i].y = iy;
			zombie[i].classify = classify; 
			switch(classify){
				case 1:zombie[i].life = ZOMBIE_TYPE1_LIFE;break;
				case 2:zombie[i].life = ZOMBIE_TYPE2_LIFE; break;
			}		
			zombie[i].direction = 270;
 			zombie[i].active = true;
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

	// ����һ̲Ѫ��
	char bloodClone[128];
	sprintf(bloodClone,"blood_clone_%d", rand()%1000);
	dCloneSprite("blood",bloodClone);
	dSetSpritePosition(bloodClone,zombie->x,zombie->y);
	dSetSpriteLifeTime(bloodClone,5);

	// ��ʬ��ʧ
	zombie->active = false;
	dSetSpriteVisible(szName,0);
}


/**
 * Zombie����Ϊ
 * ��ʬ������ҵķ�����
 */
void zombieMove(struct Zombie* zombie , char* szName){

	// ͬ��λ��
	zombie->x = dGetSpritePositionX(szName);
	zombie->y = dGetSpritePositionY(szName);

	float deltaX[2];
	float deltaY[2];
	float delta[2];
	float speedY;
	float speedX;

	// ���������������
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
	
	// �Ͻ�ʬ�������������˷��䵯Ļ
	if( zombie->classify == 2){
		if( zombie->fire_timer < 0){
			zombie->fire_timer = ZOMBIE_FIRE_TIME;
			FireAim(zombie->x,zombie->y,zombie->direction,"zombie");
		}
	}

	// �����˷�����
	zombie->speedX = speedX;
	zombie->speedY = speedY;
	dSetSpriteLinearVelocityY( szName, speedY );
	dSetSpriteLinearVelocityX( szName, speedX );

	// �ж��Ƿ�����
	if( zombie->life <= 0){
		zombieDisappear(zombie,szName);
	}

	// ���ݽ�ʬ�������趨��ʬ����ʾͼƬ
	char left[64];
	char right[64];
	char bottom[64];
	char top[64];
	char top_left[64];
	char top_right[64];
	char bottom_left[64];
	char bottom_right[64];
	switch( zombie->classify ){
		case 1:{
			sprintf(left,"zombie%dAnimation",zombie->classify);
			sprintf(right,"zombie%dAnimation",zombie->classify);
			sprintf(top,"zombie%dAnimation2",zombie->classify);
			sprintf(bottom,"zombie%dAnimation3",zombie->classify);
			sprintf(top_left,"zombie%d_1Animation",zombie->classify);
			sprintf(top_right,"zombie%d_1Animation1",zombie->classify);
			sprintf(bottom_left,"zombie%d_2Animation1",zombie->classify);
			sprintf(bottom_right,"zombie%d_2Animation",zombie->classify);
			break;
		}case 2:{
			sprintf(left,"zombie%d_4Animation",zombie->classify);
			sprintf(right,"zombie%d_4Animation1",zombie->classify);
			sprintf(top,"zombie%d_3Animation",zombie->classify);
			sprintf(bottom,"zombie%d_3Animation1",zombie->classify);
			sprintf(top_left,"zombie%d_1Animation",zombie->classify);
			sprintf(top_right,"zombie%d_1Animation1",zombie->classify);
			sprintf(bottom_left,"zombie%d_2Animation",zombie->classify);
			sprintf(bottom_right,"zombie%d_2Animation1",zombie->classify);
			break;
		}
	}

	// �����ٶ�תͷ������direction
	if( (speedX == 0) && (speedY == 0) ){
		return;
	}	
	// �õ��ٶ�����������ϵ�еĽǶ�ֵ
	float shita = atan2(-speedY,speedX)*180/PI;// yȡ���ţ�����ϵת����ϵ
	if( shita < -170 ){ // ��������
		if(zombie->direction == 180){
			return;
		}
		zombie->direction = 180;
		dAnimateSpritePlayAnimation( szName,left,0);
	}else if( shita < -100 ){ // ���������½�
		if(zombie->direction == 225){
			return;
		}
		zombie->direction = 225;
		dAnimateSpritePlayAnimation(szName,bottom_left,0);
	}else if( shita < -80 ){ // ��������
		if(zombie->direction == 270){
			return;
		}
		zombie->direction = 270;
		dAnimateSpritePlayAnimation( szName, bottom, 0);
	}else if( shita < -10 ){ // ���������½�
		if(zombie->direction == 315){
			return;
		}
		zombie->direction = 315;
		dAnimateSpritePlayAnimation(szName,bottom_right,0);
	}else if( shita < 10 ){ // ��������
		if(zombie->direction == 0){
			return;
		}
		zombie->direction = 0;
		dAnimateSpritePlayAnimation( szName,right,0);
	}else if( shita < 80 ){ // ���������Ͻ�
		if(zombie->direction == 45){
			return;
		}
		zombie->direction = 45;
		dAnimateSpritePlayAnimation(szName,top_right,0);
	}else if( shita < 110 ){ // ��������
		if(zombie->direction == 90){
			return;
		}
		zombie->direction = 90;
		dAnimateSpritePlayAnimation( szName, top, 0);
	}else if( shita < 170 ){ // ���������Ͻ�
		if(zombie->direction == 135){
			return;
		}
		zombie->direction = 135;
		dAnimateSpritePlayAnimation(szName,top_left,0);
	}else{ // ��������
		if(zombie->direction == 180){
			return;
		}
		zombie->direction = 180;
		dAnimateSpritePlayAnimation( szName,left,0);
	};
}

//===========================================================================
//
// ���� �ӵ� �ĺ���

/**
 *  �����ܷ���bullet�ĺ���
 * ������������������boss�ļ��ܰ�
 * */
void FireRound(float x, float y,char* user){
	for (int i = 0; i < 360; i += 30 )
	{
		if(user == "zombie"){
			newBullet(x, y, i, BULLET_TYPE1_SPEED_ZOMBIE,2);
		}
		
	}
}

void FireAim(float x,float y,int direction,char* user){
	if(user == "zombie"){
		newBullet(x, y, direction, BULLET_TYPE1_SPEED_ZOMBIE,2);
		newBullet(x, y, direction+20, BULLET_TYPE1_SPEED_ZOMBIE,2);
		newBullet(x, y, direction-20, BULLET_TYPE1_SPEED_ZOMBIE,2);
	}
}
 /**
 * �ӵ�����
 * ���� ix(float)�����ɴ�x����
 * ���� iy(float)�����ɴ�y����
 * ���� idirection(float)���ƶ��ķ���
 * ���� ispeed(float)���ƶ����ٶ�
 * ���� classify(float)���ӵ�������
 * ���� �ӵ�ID(float)��(���û�п�ȱ��-1)
*/
int newBullet(float ix, float iy, float idirection, float ispeed,int classify) {
	for (int i = 0; i < BULLET_MAX; i++) {
		if ((bullet[i].active) == false) {
			bullet[i].x = ix;
			bullet[i].y = iy;
			bullet[i].direction = idirection;
			bullet[i].speed = ispeed;
			bullet[i].classify = classify;
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
			if( classify == 2 ){
				dSetStaticSpriteImage("bullet","particles1ImageMap", 1 );
			}else if( classify == 1 ){
				dSetStaticSpriteImage("bullet","bulletImageMap", 1 );
			}
			dSetSpriteRotation( szName, 90 - bullet[i].direction );
			dSetSpriteVisible(szName,1);
			return i;
		}
	}
	return -1;
}

/**
 * �ӵ���Ϊ
 */
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
				shootX = player->x + GAP_BETWEEN_PLAYER_AND_BULLET; 
				shootY = player->y;
				break; 
			case 45: // �����Ϸ���ǹ
				shootX = player->x + GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y + GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 90: // ���Ͽ�ǹ
				shootX = player->x;
				shootY = player->y - GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 135: // �����Ϸ���ǹ
				shootX = player->x - GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y - GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 180: // ����ǹ
				shootX = player->x - GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y;
				break;
			case 225: // �����·���ǹ
				shootX = player->x - GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y + GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 270: // ���¿�ǹ
				shootX = player->x;
				shootY = player->y + GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 315: // �����·���ǹ
				shootX = player->x + GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y + GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
		}
		dPlaySound("gun_type1.wav", 0, 1 );	
		newBullet(
			shootX, 
			shootY,
			player->direction,
			BULLET_TYPE1_SPEED,
			1
		);
	}	
}

/* �����Ϊ */
void playerMove( struct Player* player, char* szName){
	// ͬ��λ��
	player->x = dGetSpritePositionX(szName);
	player->y = dGetSpritePositionY(szName);

	// ����һ����ҳ��������
	if(dGetSpritePositionY( szName ) < SCREEN_TOP){
		dSetSpritePositionY( szName,SCREEN_TOP);
	}else if(dGetSpritePositionY( szName ) > SCREEN_BOTTOM){
		dSetSpritePositionY(szName,SCREEN_BOTTOM);
	}else if(dGetSpritePositionX( szName ) < SCREEN_LEFT){
		dSetSpritePositionX(szName,SCREEN_LEFT);
	}else if(dGetSpritePositionX(szName) > SCREEN_RIGHT){
		dSetSpritePositionX(szName,SCREEN_RIGHT);
	}

	// �������ܹ���֮������׶���ҵ���Ϊ
	if( player->bitten_timer > PLAYER_BITTEN_TIME/2 ){
		turnRed( szName );
	}else if( (player->bitten_timer > 0) && (player->bitten_timer < PLAYER_BITTEN_TIME/2) ){
		if( player->bit_zombie != NULL){
			player->bit_zombie = NULL;
			dSetSpriteLinearVelocityY( szName, 0 );
			dSetSpriteLinearVelocityX( szName, 0 );
		}	
		turnRed( szName );
	}else{
		turnWhite( szName );
	}
	// �ж��Ƿ������˹���
	// �ܵ��˹�����ֱ��ȡ��֮����ж�
	if( player->bit_zombie != NULL){
		float deltaX = player->bit_zombie->x - player->x;
		float deltaY = player->bit_zombie->y - player->y;
		float delta = sqrt( (deltaX*deltaX) + (deltaY*deltaY) );
		float speedY = -(deltaY / delta) * 60;
		float speedX = -(deltaX / delta) * 60;
		dSetSpriteLinearVelocityX( szName,speedX );
		dSetSpriteLinearVelocityY( szName,speedY );
		return;
	}

	// ������ҵĲ�ͬ��ȡ����Ĳ�ͬ 
	char left[64];
	char right[64];
	char bottom[64];
	char top[64];
	char top_left[64];
	char top_right[64];
	char bottom_left[64];
	char bottom_right[64];
	if(player->id == 0){
		sprintf(left,"player%dAnimation1",player->id+1);
		sprintf(right,"player%dAnimation2",player->id+1);
		sprintf(bottom,"player%dAnimation3",player->id+1);
		sprintf(top,"player%dAnimation",player->id+1);
		sprintf(top_left,"player%d_2Animation1",player->id+1);
		sprintf(top_right,"player%d_1Animation1",player->id+1);
		sprintf(bottom_left,"player%d_1Animation",player->id+1);
		sprintf(bottom_right,"player%d_2Animation",player->id+1);
	}else if(player->id == 1){
		sprintf(left,"player%dAnimation1",player->id+1);
		sprintf(right,"player%dAnimation",player->id+1);
		sprintf(bottom,"player%dAnimation3",player->id+1);
		sprintf(top,"player%dAnimation2",player->id+1);
		sprintf(top_left,"player%d_1Animation1",player->id+1);
		sprintf(top_right,"player%d_2Animation",player->id+1);
		sprintf(bottom_left,"player%d_2Animation1",player->id+1);
		sprintf(bottom_right,"player%d_1Animation",player->id+1);
	}

	// �����ӵ����䷽�����ĳ��� 
	if( (player->speedX == 0) && (player->speedY == 0) ){
		return;
	}
	// �õ��ٶ�����������ϵ�еĽǶ�ֵ
	float shita = atan2(-player->speedY,player->speedX)*180/PI;// yȡ���ţ�����ϵת����ϵ
	if( shita < -170 ){ // ��������
		if(player->direction == 180){
			return;
		}
		player->direction = 180;
		dAnimateSpritePlayAnimation( szName,left,0);
	}else if( shita < -100 ){ // ���������½�
		if(player->direction == 225){
			return;
		}
		player->direction = 225;
		dAnimateSpritePlayAnimation(szName,bottom_left,0);
	}else if( shita < -80 ){ // ��������
		if(player->direction == 270){
			return;
		}
		player->direction = 270;
		dAnimateSpritePlayAnimation( szName, bottom, 0);
	}else if( shita < -10 ){ // ���������½�
		if(player->direction == 315){
			return;
		}
		player->direction = 315;
		dAnimateSpritePlayAnimation(szName,bottom_right,0);
	}else if( shita < 10 ){ // ��������
		if(player->direction == 0){
			return;
		}
		player->direction = 0;
		dAnimateSpritePlayAnimation( szName,right,0);
	}else if( shita < 80 ){ // ���������Ͻ�
		if(player->direction == 45){
			return;
		}
		player->direction = 45;
		dAnimateSpritePlayAnimation(szName,top_right,0);
	}else if( shita < 110 ){ // ��������
		if(player->direction == 90){
			return;
		}
		player->direction = 90;
		dAnimateSpritePlayAnimation( szName, top, 0);
	}else if( shita < 170 ){ // ���������Ͻ�
		if(player->direction == 135){
			return;
		}
		player->direction = 135;
		dAnimateSpritePlayAnimation(szName,top_left,0);
	}else{ // ��������
		if(player->direction == 180){
			return;
		}
		player->direction = 180;
		dAnimateSpritePlayAnimation( szName,left,0);
	};

}
//===========================================================================
//
// ������йصĺ���

/* ������� */
void loadPlots(){
	plot[0].id = 0;
	plot[0].speaker = BOY;
	plot[0].content = "û�뵽��Ȼ�����͵�δ��ȥ��";
	plot[0].picture1 = BOY_NERVOUS;
	plot[0].picture2 = GIRL_NORMAL;
	plot[0].end_flag = false;

	plot[1].id = 1;
	plot[1].speaker = GIRL;
	plot[1].content = "������������δ����Ҫ��ȥ��Զ��ʱ����û���̲趼û�ú���";
	plot[1].picture1 = BOY_NERVOUS;
	plot[1].picture2 = GIRL_HAPPY;
	plot[1].end_flag = false;

	plot[2].id = 2;
	plot[2].speaker = BOY;
	plot[2].content = "�G�㻹��������̲衭";
	plot[2].picture1 = BOY_NERVOUS;
	plot[2].picture2 = GIRL_HAPPY;
	plot[2].end_flag = false;

	plot[3].id = 3;
	plot[3].speaker = BOY;
	plot[3].content = "�G�������̲�!";
	plot[3].picture1 = BOY_SCARED;
	plot[3].picture2 = GIRL_HAPPY;
	plot[3].end_flag = false;
	
	plot[4].id = 4;
	plot[4].speaker = GIRL;
	plot[4].content = "�ղ��Զ�������������������";
	plot[4].picture1 = BOY_SCARED;
	plot[4].picture2 = GIRL_CURIOUS;
	plot[4].end_flag = false;

	plot[5].id = 5;
	plot[5].speaker = BOY;
	plot[5].content = "������ȫû��������˵������δ��������ô��ȫ��������Ӱ��";
	plot[5].picture1 = BOY_NERVOUS;
	plot[5].picture2 = GIRL_CURIOUS;
	plot[5].end_flag = false;

	plot[6].id = 6;
	plot[6].speaker = GIRL;
	plot[6].content = "��ʬ���в���";
	plot[6].picture1 = BOY_NORMAL;
	plot[6].picture2 = GIRL_HAPPY;
	plot[6].end_flag = false;

	plot[7].id = 7;
	plot[7].speaker = BOY;
	plot[7].content = "��ĚG";
	plot[7].picture1 = BOY_CRY;
	plot[7].picture2 = GIRL_HAPPY;
	plot[7].end_flag = false;

	plot[8].id = 8;
	plot[8].speaker = GIRL;
	plot[8].content = "�������";
	plot[8].picture1 = BOY_NERVOUS;
	plot[8].picture2 = GIRL_NORMAL;
	plot[8].end_flag = false;

	plot[9].id = 9;
	plot[9].speaker = BOY;
	plot[9].content = "���ǡ���ǹ���������İ�";
	plot[9].picture1 = BOY_SCARED;
	plot[9].picture2 = GIRL_NORMAL;
	plot[9].end_flag = false;

	plot[10].id = 10;
	plot[10].speaker = GIRL;
	plot[10].content = "Ҳ���Զ�������������";
	plot[10].picture1 = BOY_SCARED;
	plot[10].picture2 = GIRL_HAPPY;
	plot[10].end_flag = true;

	plot[11].id = 11;
	plot[11].speaker = GIRL;
	plot[11].content = "���Ǳ���ص���ȥ��ֹ��һ�еķ���";
	plot[11].picture1 = BOY_NERVOUS;
	plot[11].picture2 = GIRL_HAPPY;
	plot[11].end_flag = false;

	plot[12].id = 12;
	plot[12].speaker = BOY;
	plot[12].content = "�������ˣ������ľ���չ��";
	plot[12].picture1 = BOY_NORMAL;
	plot[12].picture2 = GIRL_NORMAL;
	plot[12].end_flag = false;

	plot[13].id = 13;
	plot[13].speaker = GIRL;
	plot[13].content = "���ǵò�ʿ���һ�������������ĺ���";
	plot[13].picture1 = BOY_NORMAL;
	plot[13].picture2 = GIRL_NORMAL;
	plot[13].end_flag = false;

	plot[14].id = 14;
	plot[14].speaker = BOY;
	plot[14].content = "Ŷ������ȡ�����ǵ��о����ϣ����������͵�����ʱ��";
	plot[14].picture1 = BOY_NERVOUS;
	plot[14].picture2 = GIRL_CURIOUS;
	plot[14].end_flag = false;

	plot[15].id = 15;
	plot[15].speaker = BOY;
	plot[15].content = "����Ϊ���о���ʬ������";
	plot[15].picture1 = BOY_NERVOUS;
	plot[15].picture2 = GIRL_CURIOUS;
	plot[15].end_flag = false;

	plot[16].id = 16;
	plot[16].speaker = GIRL;
	plot[16].content = "û����������Ҫ���ľ��Ǵ�ʱ��֮�Ż�ȥ10��ǰ���о���";
	plot[16].picture1 = BOY_NORMAL;
	plot[16].picture2 = GIRL_NORMAL;
	plot[16].end_flag = false;

	plot[17].id = 17;
	plot[17].speaker = GIRL;
	plot[17].content = "��ֹ�����̲���ú�";
	plot[17].picture1 = BOY_NORMAL;
	plot[17].picture2 = GIRL_HAPPY;
	plot[17].end_flag = false;

	plot[18].id = 18;
	plot[18].speaker = BOY;
	plot[18].content = "�G������ô�������̲�";
	plot[18].picture1 = BOY_SCARED;
	plot[18].picture2 = GIRL_HAPPY;
	plot[18].end_flag = true;

	plot[19].id = 19;
	plot[19].speaker = BOY;
	plot[19].content = "�о��������Ѿ������˽�ʬ����������һ����";
	plot[19].picture1 = BOY_NERVOUS;
	plot[19].picture2 = GIRL_NORMAL;
	plot[19].end_flag;

	plot[20].id = 20;
	plot[20].speaker = GIRL;
	plot[20].content = "��ʵ��ʬ��û�г��о���ѽ��Ӧ��˵���øոպð�";
	plot[19].picture1 = BOY_NERVOUS;
	plot[19].picture2 = GIRL_NORMAL;
	plot[19].end_flag = false;
	
	plot[21].id = 21;
	plot[21].speaker = BOY;
	plot[21].content = "��ô˵��ֻ�ý���о�������������";
	plot[21].picture1 = BOY_NORMAL;
	plot[21].picture2 = GIRL_HAPPY;
	plot[21].end_flag = true;
}

/**
 * չʾ���� ��GameRunͬ���ĺ���
 */
void playPlots( float fDeltaTime ){

	// ���ڵ��Ƿ���ת���������չʾ����
	if( current_plot != currnet_plot_ensure){

		currnet_plot_ensure = current_plot;

		// ������ʾ����ľ���ɼ�
		dSetSpriteVisible("boy",1);
		dSetSpriteVisible("girl",1);
		dSetSpriteVisible("panel",1);
		dSetSpriteVisible("speaker",1);
		dSetSpriteVisible("content",1);

		// ����click_flag ������־����������
		click_flag = false;

		if( !strcmp(plot[current_plot].speaker,BOY) ){
			turnWhite( "boy" );
			turnGray( "girl" );
		}
		if( !strcmp(plot[current_plot].speaker,GIRL) ){
			turnWhite( "girl" );
			turnGray( "boy" );
		}
		dSetStaticSpriteFrame( "boy", plot[current_plot].picture1 );
		dSetStaticSpriteFrame( "girl", plot[current_plot].picture2 );
		dSetTextString( "speaker", plot[current_plot].speaker );
		dSetTextString( "content", plot[current_plot].content );
	}

	// ����Ƿ�����Ļ�����������ת�ڵ�
	if( click_flag == true){
		click_flag = false;
		// ��⵱ǰ�ڵ��Ƿ�Ϊ��ξ�������һ���ڵ�
		if( plot[current_plot].end_flag ){
			current_plot++;
			// ��һ���˳���Ч
			dCloneSprite("boy","boy_show");
			dCloneSprite("girl","girl_show");
			dCloneSprite("panel","panel_show");
			dSetSpriteLinearVelocityX("boy_show",-90);
			dSetSpriteLinearVelocityX("girl_show",90);
			dSetSpriteLinearVelocityY("panel_show",80);
			dSetSpriteLifeTime("boy_show",3);
			dSetSpriteLifeTime("girl_show",3);
			dSetSpriteLifeTime("panel_show",3);

			// ���Ž�������ʾ����ľ��鲻�ɼ�
			dSetSpriteVisible("boy",0);
			dSetSpriteVisible("girl",0);
			dSetSpriteVisible("panel",0);
			dSetSpriteVisible("speaker",0);
			dSetSpriteVisible("content",0);
			game_mode_flag = MAIN_GAME;
		}else{
			current_plot++;
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
 * �������ɺ�ɫ��
 */
void turnRed( char *szName ){
	dSetSpriteColorBlue( szName, 0 );
	dSetSpriteColorRed( szName, 255 );
	dSetSpriteColorGreen( szName, 0 );
}


/**
 * ������������ɫ��
 */
void turnWhite( char *szName ){
	dSetSpriteColorBlue( szName, 255 );
	dSetSpriteColorRed( szName, 255 );
	dSetSpriteColorGreen( szName, 255 );
}
/**
 * �������ɻ�ɫ��
 */
void turnGray( char *szName ){
	dSetSpriteColorBlue( szName, 150 );
	dSetSpriteColorRed( szName, 150 );
	dSetSpriteColorGreen( szName, 150 );
}
