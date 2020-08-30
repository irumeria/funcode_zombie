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
int	g_iGameState = 0;		// 游戏状态，0 -- 游戏结束等待开始状态；1 -- 按下空格键开始，初始化游戏；2 -- 游戏进行中
//
void GameInit();
void GameRun( float fDeltaTime );
void GameEnd();


//==============================================================================
//
// 定义结构体

/*
僵尸的种类： classify 默认为1 决定它的模样 / 行动方式 / 速度
*/
struct Zombie{
	int classify;
	float x;
	float y;
	float birthday; // 与下面的shoot_flag配合全局的counter完成定时行为
	int eatable_flag; // 与人距离缩短到一定程度之后增
	int get_shot_flag; // 被击中时为true
	int walk_flag; // 控制僵尸定时走路的旗子
	int life;
	boolean active;
};

/* 玩家 */
struct Player{
	float x;
	float y;
	float direction;
	float speedX;
	float speedY;
};


//==============================================================================
//
// 声明函数

void Move() ;
int newZombie(float ix, float iy);
void move_zombie_classify1(struct Zombie* zombie , char* szName);
void rePaint();
void turnYellow( char *szName );
void turnWhite( char *szName );


//==============================================================================
//
// 声明常量

//单位最大数量设定
#define ZOMBIE_MAX 10

// 各种类僵尸速度设定
#define ZOMBIE_TYPE1_SPEED 10.0
#define ZOMBIE_TYPE2_SPEED 30.0
#define ZOMBIE_TYPE3_SPEED 25.0
#define ZOMBIE_TYPE4_SPEED 25.0

// 玩家初始生命值设定
#define LIFE_INIT 5

// 各种类僵尸初始生命值设定
#define LIFE_OF_TYPE1 5
#define LIFE_OF_TYPE2 10
#define LIFE_OF_TYPE3 8
#define LIFE_OF_TYPE4 3

#define PI 3.1415926535

//==============================================================================
//
// 声明全局变量

boolean GodMode = false;	//无敌模式（调试用）

struct Zombie zombie[ZOMBIE_MAX];	//预生成Zombie池
struct Player player;

int life; // 生命值

float SCREEN_LEFT;    // 屏幕边界值
float SCREEN_RIGHT;    
float SCREEN_TOP;    
float SCREEN_BOTTOM;  

int counter; // 全局计时器

int score; // 分数
int money; // 金钱

// 控制旗
boolean shooting_flag; // 控制玩家子弹输出

// 计时器（配合counter使用）

// 计数器（配合counter使用）
int zombie_appear_timer; 


//==============================================================================


//==============================================================================
//
// 大体的程序流程为：GameMainLoop函数为主循环函数，在引擎每帧刷新屏幕图像之后，都会被调用一次。


//==============================================================================
//
// 游戏主循环，此函数将被不停的调用，引擎每刷新一次屏幕，此函数即被调用一次
// 用以处理游戏的开始、进行中、结束等各种状态. 
// 函数参数fDeltaTime : 上次调用本函数到此次调用本函数的时间间隔，单位：秒
void GameMainLoop( float	fDeltaTime )
{
	switch( g_iGameState )
	{
		// 初始化游戏，清空上一局相关数据
	case 1:
		{
			GameInit();
			g_iGameState	=	2; // 初始化之后，将游戏状态设置为进行中
		}
		break;

		// 游戏进行中，处理各种游戏逻辑
	case 2:
		{
			// TODO 修改此处游戏循环条件，完成正确游戏逻辑
			if( true )
			{
				GameRun( fDeltaTime );
			}
			else
			{
				// 游戏结束。调用游戏结算函数，并把游戏状态修改为结束状态
				g_iGameState	=	0;
				GameEnd();
			}
		}
		break;

		// 游戏结束/等待按空格键开始
	case 0:
	default:
		break;
	};
}

//==============================================================================
//
// 每局开始前进行初始化，清空上一局相关数据
void GameInit()
{
	// 初始化生命值 / 分数 / 金钱
	life = LIFE_INIT;
	money = 0;
	score = 0;

	// 初始化计数器 与 计时器
	counter = 0;
	zombie_appear_timer = 1; 

	// 拿到窗口边界
	SCREEN_LEFT = dGetScreenLeft();
	SCREEN_RIGHT = dGetScreenRight(); 
	SCREEN_TOP = dGetScreenTop();
	SCREEN_BOTTOM = dGetScreenBottom();

	// 循环控制变量	
	int i;

	// 初始化zombie
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

	// 初始化玩家
	player.x = 0;
	player.y = SCREEN_BOTTOM - 10;
	dSetSpritePosition("player",player.x,player.y);

	// 初始化zombie精灵
	for(i = 0; i < ZOMBIE_MAX; i++) {
		char szName[64];
		sprintf(szName, "zombie_%d", i);
		dCloneSprite("zombie",szName);
		dSetSpriteVisible(szName,0); // 设置它们不可见
	}
}
//==============================================================================
//
// 每局游戏进行中
void GameRun( float fDeltaTime )
{
	// 计时
	counter += fDeltaTime;

	// 激活Zombie
	if( counter / ( 0.6 * zombie_appear_timer ) > 0 ){
		zombie_appear_timer++;
		newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP); // 在画面顶部生成一下测试用僵尸	
	}


	printf("before move");
	// 各个对象各自执行它们的行为
	Move();
	printf("after move");
	// 重绘游戏主画面
	rePaint();
}
//====================================================== ========================
//
// 本局游戏结束
void GameEnd()
{
}
//==========================================================================
//
// 鼠标移动
// 参数 fMouseX, fMouseY：为鼠标当前坐标
void OnMouseMove( const float fMouseX, const float fMouseY )
{
	
}
//==========================================================================
//
// 鼠标点击
// 参数 iMouseType：鼠标按键值，见 enum MouseTypes 定义
// 参数 fMouseX, fMouseY：为鼠标当前坐标
void OnMouseClick( const int iMouseType, const float fMouseX, const float fMouseY )
{
	
}
//==========================================================================
//
// 鼠标弹起
// 参数 iMouseType：鼠标按键值，见 enum MouseTypes 定义
// 参数 fMouseX, fMouseY：为鼠标当前坐标
void OnMouseUp( const int iMouseType, const float fMouseX, const float fMouseY )
{
	
}
//==========================================================================
//
// 键盘按下
// 参数 iKey：被按下的键，值见 enum KeyCodes 宏定义
// 参数 iAltPress, iShiftPress，iCtrlPress：键盘上的功能键Alt，Ctrl，Shift当前是否也处于按下状态(0未按下，1按下)
void OnKeyDown( const int iKey, const bool bAltPress, const bool bShiftPress, const bool bCtrlPress )
{	

	if( KEY_SPACE == iKey && 0 == g_iGameState )
	{
		g_iGameState =	1;
	}

}
//==========================================================================
//
// 键盘弹起
// 参数 iKey：弹起的键，值见 enum KeyCodes 宏定义
void OnKeyUp( const int iKey )
{
	
}
//===========================================================================
//
// 精灵与精灵碰撞
// 参数 szSrcName：发起碰撞的精灵名字
// 参数 szTarName：被碰撞的精灵名字
void OnSpriteColSprite( const char *szSrcName, const char *szTarName )
{
	
}
//===========================================================================
//
// 精灵与世界边界碰撞
// 参数 szName：碰撞到边界的精灵名字
// 参数 iColSide：碰撞到的边界 0 左边，1 右边，2 上边，3 下边
void OnSpriteColWorldLimit( const char *szName, const int iColSide )
{
	
}

//===========================================================================
//
// 全局函数

void Move() {	

	printf("moving");
	int i; // 循环控制变量

	//按类型分发Zombie行为
	for(i = 0;i <ZOMBIE_MAX;i++){

		// 同步位置数据
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
// 关于 Zombie 的函数

/**
 * Zombie的激活
 * 参数 ix(float)：生成处x坐标
 * 参数 iy(float)：生成处y坐标
 * 返回 子弹ID(int)：(如果没有空缺：-1)
 */
int newZombie(float ix, float iy){
	for (int i = 0; i < ZOMBIE_MAX; i++) {
		if ((zombie[i].active) == false) {
			// 内部
			zombie[i].x = ix;
			zombie[i].y = iy;
			zombie[i].classify = 1; // 完成框架前，先把所有僵尸当作种类1
			zombie[i].life = LIFE_OF_TYPE1;	
			zombie[i].active = true;
			zombie[i].birthday = counter;

			// 外部显示
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
 * Zombie的行为1
 * 僵尸朝着玩家的方向走
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

//===========================================================================
//
// 全局重绘函数

void rePaint(){

	int i; // 循环控制变量

	/**
	 * 重绘Zombie 
	 * 将Zombie结构体中的数据映射到界面上
	 */
	for(i = 0;i <= ZOMBIE_MAX;i++){
		
		// 拿到界面上的Zombie精灵
		char szName[64];
		sprintf(szName, "zombie_%d", i);
	}

	/**
	 * 重绘Player
	 * 将player结构体中的数据映射到界面上
	 */
	// dSetSpritePosition( "player",player.x,player.y );

}

//===========================================================================
//
// API 的使用简化函数

/**
 * 将精灵变成黄色调
 */
void turnYellow( char *szName ){
	dSetSpriteColorBlue( szName, 0 );
	dSetSpriteColorRed( szName, 255 );
	dSetSpriteColorGreen( szName, 255 );
}

/**
 * 将精灵变成正常色调
 */
void turnWhite( char *szName ){
	dSetSpriteColorBlue( szName, 255 );
	dSetSpriteColorRed( szName, 255 );
	dSetSpriteColorGreen( szName, 255 );
}