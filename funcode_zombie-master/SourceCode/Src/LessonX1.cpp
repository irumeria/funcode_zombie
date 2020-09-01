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

// 子弹
struct Bullet{
	int speed;
    float x;
    float y;
	int direction;
	int color; // 默认为1
	int speedX;
	int speedY;
	boolean active;
};


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


/**
 * 粒子效果
 * 击中目标和爆炸的时候触发 
*/
struct Particle{
	float x;
	float y;
	float speed;
	boolean active;
};

//==============================================================================
//
// 声明函数

void Move() ;
int getZombieSpeed(int classify);
int newZombie(float ix, float iy);
void move_zombie_classify1(struct Zombie* zombie);
void move_zombie_classify2( struct Zombie* zombie );
void Disappear( struct Zombie* zombie );
void FireRound(int x, int y);
int newBullet(float ix, float iy, float idirection, float ispeed);
void bulletMove( struct Bullet* bullet);
int newParticle(float ix, float iy, float direction, int ispeed);
void playerShot() ;
void rePaint();
void turnYellow( char *szName );
void turnWhite( char *szName );

//==============================================================================
//
// 声明常量

//单位最大数量设定
#define ZOMBIE_MAX 50
#define PARTICLE_MAX 400
#define BULLET_MAX 30

// 子弹速度设定
#define BULLET_TYPE1_SPEED 100.0
#define BULLET_TYPE2_SPEED 200.0
#define BULLET_TYPE3_SPEED 50.0

// Zombie速度设定
#define ZOMBIE_TYPE1_SPEED 20.0
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

struct Bullet bullet[BULLET_MAX];	//预生成Bullet池
struct Zombie zombie[ZOMBIE_MAX];	//预生成Zombie池
struct Particle particle[PARTICLE_MAX];	//预生成粒子池
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
boolean gameover_flag; // 控制游戏结束
boolean talking_flag; // 控制游戏进入对话模式
boolean shopping_flag; // 控制游戏进入商城模式
boolean shooting_flag; // 控制玩家子弹输出

// 计时器（配合counter使用）
int zombie_appear_counter; 
int shooting_counter;

// 计数器（配合counter使用）
int zombie_appear_timer; 
int shooting_timer;


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

	// 初始化控制旗
	gameover_flag = false;
	talking_flag = false;
	shopping_flag = false;
	shooting_flag = false;

	// 初始化计数器 与 计时器
	zombie_appear_counter = 0;
	shooting_counter = 0;
	shooting_timer = 0;
	counter = 0;

	// 拿到窗口边界
	SCREEN_LEFT = dGetScreenLeft();
	SCREEN_RIGHT = dGetScreenRight(); 
	SCREEN_TOP = dGetScreenTop();
	SCREEN_BOTTOM = dGetScreenBottom();

	// 循环控制变量	
	int i;
	// 初始化子弹
	for(i = 0; i < BULLET_MAX; i++){ 
		bullet[i].speed = 0;
		bullet[i].x = 0.0;
		bullet[i].y = 0.0;
		bullet[i].color = 1;
		bullet[i].direction = 0;
		bullet[i].active = false;
	}

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
	
	// 初始化爆炸效果
	for(i = 0; i < PARTICLE_MAX; i++) {
		particle[i].speed = 0;
		particle[i].x = 0.0;
		particle[i].y = 0.0;
		particle[i].active = false;
	}

	// 初始化玩家
	player.x = 0;
	player.y = SCREEN_BOTTOM-10;

	// 初始化zombie精灵
	for(i = 0; i < ZOMBIE_MAX; i++) {
		char szName[64];
		sprintf(szName, "zombie_%d", i);
		dCloneSprite("zombie",szName);
		dSetSpriteVisible(szName,0); // 设置它们不可见
	}

	// 初始化bullet精灵
	for(i = 0; i < BULLET_MAX; i++){
		char szName[64];
		sprintf(szName,"bullet_%d",i);
		dCloneSprite("bullet",szName);
		dSetSpriteVisible(szName,0); // 设置它们不可见
	}

}
//==============================================================================
//
// 每局游戏进行中
void GameRun( float fDeltaTime )
{	// 计时
	counter += fDeltaTime;

	// 激活Zombie
	if( counter / ( 0.6 * zombie_appear_timer ) > 0 ){
		zombie_appear_timer++;
		newZombie((SCREEN_RIGHT - SCREEN_LEFT)*rand(), SCREEN_TOP);
	}

	//以相等的间隔发射子弹
	if (( shooting_flag == true )&&( (counter - shooting_counter) / ( 0.03*shooting_timer ) > 0 )){
		playerShot();
	}

	// Zombie / Bullet 各自执行它们的行为
	Move();

	// 重绘游戏主画面
	rePaint();

}
//==============================================================================
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
	if(iKey == KEY_SPACE)
	{
		shooting_flag = true;
	}

	switch(iKey)
	{
		case KEY_UP:		
			player.speedY = -10.f;
			break;
		case KEY_LEFT:
			player.speedX = -15.f;		
			break;
		case KEY_DOWN:	
			player.speedY = 10.f;
			break;
		case KEY_RIGHT:
			player.speedX = 15.f;		
			break;
		default:
			break;
	}


}
//==========================================================================
//
// 键盘弹起
// 参数 iKey：弹起的键，值见 enum KeyCodes 宏定义
void OnKeyUp( const int iKey )
{
	if(iKey == KEY_SPACE)
	{
		shooting_flag = false;
	}
	
}
//===========================================================================
//
// 精灵与精灵碰撞
// 参数 szSrcName：发起碰撞的精灵名字
// 参数 szTarName：被碰撞的精灵名字
void OnSpriteColSprite( const char *szSrcName, const char *szTarName )
{
	int i; // 循环控制变量
	char szName[64];
	for( i = 0;i < ZOMBIE_MAX;i++){
		sprintf(szName,"zombie_%d",i);
		if( (szName == szSrcName) && (szTarName == "player")){
			// 僵尸定时发起攻击
			// zombie->eatable_flag
		}
	}
	
	
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

/**
 * 各个对象执行它们分别的行为
 */
void Move() {	//counter到达一定数值之后执行

	int i; // 循环控制变量

	//按类型分发Zombie行为
	for(i = 0;i <ZOMBIE_MAX;i++){
		if( !zombie[i].active ){
			continue;
		}
		switch ( zombie[i].classify ) {
			case 1:
			move_zombie_classify1( &zombie[i] );
			break;
			case 2:
			move_zombie_classify2( &zombie[i] );
			break;
			default:break;
		}
	}
	// 调用bullet行为
	for(i = 0;i < BULLET_MAX;i++){
		if( !bullet[i].active ){
			continue;
		}
		bulletMove( &bullet[i] );
	}
	
}

//===========================================================================
// 关于 Zombie 的函数

/* 获取不同种类Zombie的速度 */
int getZombieSpeed(int classify){
	float ret;
	switch( classify )
	{
		case 1: ret = ZOMBIE_TYPE1_SPEED;break;
		case 2: ret = ZOMBIE_TYPE2_SPEED;break;
		case 3: ret = ZOMBIE_TYPE3_SPEED;break;
		case 4: ret = ZOMBIE_TYPE4_SPEED;break;
	}
	return ret;
}

/**
 * Zombie的激活
 * 参数 ix(float)：生成处x坐标
 * 参数 iy(float)：生成处y坐标
 * 返回 子弹ID(int)：(如果没有空缺：-1)
 */
int newZombie(float ix, float iy){
	for (int i = 0; i < ZOMBIE_MAX; i++) {
		if ((zombie[i].active) == false) {
			zombie[i].x = ix;
			zombie[i].y = iy;
			zombie[i].classify = 1;
			zombie[i].life = LIFE_OF_TYPE1;	// 完成框架前，先把所有僵尸当作种类1
			zombie[i].active = true;
			zombie[i].birthday = counter;
			return i;
		}
	}
	return -1;
}

/**
 * Zombie的行为1
 * 朝着玩家的方向走
 */
void move_zombie_classify1(struct Zombie* zombie){	
	if( ((counter - zombie->birthday) / (zombie->walk_flag * 0.5)) > 0)
	{
		zombie->walk_flag++;
		float deltaX = zombie->x - player.x;
		float deltaY = - (zombie->y - player.y); // funcode里的是左手系，需要将y轴反转使之成为右手系
		float delta = sqrt( (deltaX*deltaX) + (deltaY*deltaY) );
		zombie->y = zombie->y + (deltaY / delta) * ZOMBIE_TYPE1_SPEED;
		zombie->x = zombie->x + (deltaX / delta) * ZOMBIE_TYPE1_SPEED;
	}	
}

/**
 * Zombie的行为2
 * 未启用
 */
void move_zombie_classify2( struct Zombie* zombie ){  // 待完成
	
	zombie->y = zombie->y + 1;
	//左右摆动
	zombie->x += sin(zombie->y / 20);
	//从画面消失
	if (((SCREEN_BOTTOM+20) < zombie->y)) {
		zombie->active = false;
	}
	//间隔一定时间发动对player的攻击
	// if (( zombie->eatable_flag )&&((counter - zombie->birthday) / ( 0.08*zombie->eaten_flag ) > 0)) {
	// 	zombie->eatable_flag = zombie->eatable_flag + 1;
	// 	FireRound(zombie->x, zombie->y);
	// }
}

/**
 *  Zombie 挂掉时调用
 */
void Disappear( struct Zombie* zombie ){	//不同Zombie不同得分
	switch (zombie->classify) {
		case 1: score += 10;
		break;
		case 2: score += 20;
		break;
		default:break;
	}

	//挂掉时候的粒子效果
	// newParticle(zombie->x, zombie->y, 45, 2);
	// newParticle(zombie->x, zombie->y, 135, 2);
	// newParticle(zombie->x, zombie->y, 225, 2);
	// newParticle(zombie->x, zombie->y, 315, 2);

	zombie->active = false;
	zombie->eatable_flag = 0;
}

//===========================================================================

// 关于 子弹 的函数

/**
 *  向四周发射bullet的函数
 * 这可以做成人物或者是boss的技能吧
 * */
void FireRound(int x, int y){
	for (int i = 0; i < 360; i += 30 )
	{
		newBullet(x, y, i, 3);
	}
}

 /**
 * 子弹激活
 * 参数 ix(float)：生成处x坐标
 * 参数 iy(float)：生成处y坐标
 * 参数 idirection(float)：移动的方向
 * 参数 ispeed(float)：移动的速度
 * 返回 子弹ID(float)：(如果没有空缺：-1)
*/
int newBullet(float ix, float iy, float idirection, float ispeed) {
	for (int i = 0; i < BULLET_MAX; i++) {
		if ((bullet[i].active) == false) {
			bullet[i].x = ix;
			bullet[i].y = iy;
			bullet[i].direction = idirection;
			bullet[i].speed = ispeed;
			bullet[i].active = true;

			//提高处理速度，从极坐标转到直角坐标
			double radian;
			radian = bullet[i].direction*2*PI/360;	//度数转弧度
			bullet[i].speedX = bullet[i].speed * cos(radian);
			bullet[i].speedY = bullet[i].speed * sin(radian);
			return i;
		}
	}
	return -1;
}

void bulletMove( struct Bullet* bullet){

	bullet->x += bullet->speedX;
	bullet->y += bullet->speedY;
		
	if ( (bullet->x < SCREEN_LEFT)||(SCREEN_RIGHT < bullet->x)||(bullet->y < SCREEN_TOP)||(SCREEN_BOTTOM < bullet->y) ) {
		bullet->active = false;
	}
}

//===========================================================================
//
//关于爆炸粒子的函数

/**
 * 粒子的激活
 * 参数 ix(float)：生成处x坐标
 * 参数 iy(float)：生成处y坐标
 * 参数 int direction(float)：粒子飞散的方向
 * 参数 int ispeed(int)：粒子飞散的初始速度
 * 返回 粒子对象的ID(int)：(如果没有空缺：-1)
 */
int newParticle(float ix, float iy, float direction, int ispeed){
	for (int i = 0; i < PARTICLE_MAX; i++) {
			if ((particle[i].active) == false){
				particle[i].x = ix;
				particle[i].y = iy;
				particle[i].speed = ispeed;
				particle[i].active = true;
				return i;
			}
		}
	return -1;
}

//===========================================================================
//
// 关于玩家行为的函数

/* 玩家发射子弹 */
void playerShot() {	
	if(life > 0){
		newBullet(player.x, player.y,player.direction,BULLET_TYPE1_SPEED);
	}	
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

		if( !zombie[i].active ){
			dSetSpriteVisible( szName, 0);
			continue; // 能绘制在屏幕上的Zombie必须是处于active状态的
		}
		
		// 被玩家子弹击中的Zombie变成黄色的显示状态，不然为原色
		if ( zombie[i].get_shot_flag ) {
           turnYellow( szName ); 
        } else {
            switch ( zombie[i].classify ) {
            	case 1:
                	turnWhite(szName);
                	break;            
                case 2:
					//	待完成
                	break;
                default:
					turnWhite(szName);
            }
        }
		dSetSpritePosition( szName,zombie[i].x,zombie[i].y );
		dSetSpriteVisible( szName, 1);
	}

	/**
	 * 重绘Bullet
	 * 将Bullet结构体中的数据映射到界面上
	 */
	for(i = 0;i < BULLET_MAX;i++){
		// 拿到界面上的bullet精灵
		char szName[64];
		sprintf(szName, "bullet_%d", i);

		if( !bullet[i].active ){
			dSetSpriteVisible( szName, 0);
			continue; 
		}

		dSetSpritePosition( szName,bullet[i].x,bullet[i].y );
		dSetSpriteVisible( szName, 1);

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
