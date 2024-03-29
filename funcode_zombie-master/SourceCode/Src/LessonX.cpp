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
int	g_iGameState = 0;		// 游戏状态，0 -- 游戏结束等待开始状态；1 -- 按下空格键开始，初始化游戏；2 -- 游戏进行中
//
int g_iGameScene = 0;      //游戏场景，0--游戏开始界面；1--游戏正式开始；2--游戏玩法介绍 
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
	float birthday; // 配合全局的counter完成定时行为
	float get_shot_timer; // 被击中时为录入击中状态的时间
	int life;
	boolean active;
};

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

/* 玩家 */
struct Player{
	float x;
	float y;
	float direction;
	float speedLeft;  	// 左方向速度
	float speedRight;  	// 右
	float speedTop;  	// 上
	float speedBottom ;  	// 下
};

/* 墙壁 */
struct Wall{
	float x;
	float y;
	int classify;
	int life;
	boolean active;
};

//==============================================================================
//
// 声明函数

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
int newWall(float ix, float iy, int classify);
void wallMove(struct Wall* wall, char* szName);

//==============================================================================
//
// 声明常量

//单位最大数量设定
#define ZOMBIE_MAX 150
#define BULLET_MAX 30
#define WALL_MAX 150

// 子弹速度设定
#define BULLET_TYPE1_SPEED 150.0
#define BULLET_TYPE2_SPEED 300.0
#define BULLET_TYPE3_SPEED 75.0

// 各种类僵尸速度设定
#define ZOMBIE_TYPE1_SPEED 5.0
#define ZOMBIE_TYPE2_SPEED 30.0
#define ZOMBIE_TYPE3_SPEED 25.0
#define ZOMBIE_TYPE4_SPEED 25.0

// 各种类墙壁的生命值设定
#define WALL_TYPE1_LIFE 8
#define WALL_TYPE2_LIFE 20

// 玩家初始生命值设定
#define LIFE_INIT 5

// 各种类僵尸初始生命值设定
#define ZOMBIE_TYPE1_LIFE 3
#define ZOMBIE_TYPE2_LIFE 10
#define ZOMBIE_TYPE3_LIFE 8
#define ZOMBIE_TYPE4_LIFE 2

#define PI 3.1415926535

// 计时间隔
#define ZOMBIE_APPEAR_TIME 1.8
#define BULLET_APPEAR_TIME 0.6
#define ZOMBIE_HITTEN_TIME 0.3
//==============================================================================
//
// 声明全局变量

boolean GodMode = false;	//无敌模式（调试用）

struct Bullet bullet[BULLET_MAX];	//预生成Bullet池
struct Zombie zombie[ZOMBIE_MAX];	//预生成Zombie池
struct Wall wall[WALL_MAX];
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


// 时间间隔计时器
float zombie_appear_timer;
float bullet_appear_timer;

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
	switch( g_iGameScene)
	{case 1:
		{
			GameInit();
			g_iGameScene	=	1; // 初始化之后进入场景1 ，开始游戏
		}
		break;
	}

}

//==============================================================================
//
// 每局开始前进行初始化，清空上一局相关数据
void GameInit()
{   //正式开始游戏后新页面
	if (g_iGameState==1 )
	{
		dSetSpriteVisible("beijing",false);
	    dSetSpriteVisible("kaishi",false);}
	// 先播个音乐
	dPlaySound("bgm_test1.ogg", 1, 1.0 );

	// 初始化生命值 / 分数 / 金钱
	life = LIFE_INIT;
	money = 0;
	score = 0;

	// 初始化控制旗
	shooting_flag = false;

	// 初始化计数器 与 计时器
	counter = 0;
	zombie_appear_timer = ZOMBIE_APPEAR_TIME;
	bullet_appear_timer = BULLET_APPEAR_TIME;

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
		zombie[i].active = false;
		zombie[i].get_shot_timer = 0;
	}

	// 初始化子弹
	for(i = 0; i < BULLET_MAX; i++){ 
		bullet[i].speed = 0;
		bullet[i].x = 0.0;
		bullet[i].y = 0.0;
		bullet[i].color = 1;
		bullet[i].direction = 0;
		bullet[i].active = false;
	}

	// 初始化墙壁
	for(i = 0;i < WALL_MAX;i++){
		wall[i].x = 0.0;
		wall[i].y = 0.0;
		wall[i].life = 0;
		wall[i].classify = 1;
		wall[i].active = false;
	}

	// 初始化玩家
	player.x = 0;
	player.y = SCREEN_BOTTOM - 10;
	player.direction = 90;
	player.speedLeft = 0.f;  	// 左方向速度
	player.speedRight = 0.f;  	// 右
	player.speedTop = 0.f;  	// 上
	player.speedBottom = 0.f;  	// 下
	dSetSpritePosition("player",player.x,player.y);

	// 共用字符串地址，避免资源浪费
	char szName[64];

	// 初始化zombie精灵
	for(i = 0; i < ZOMBIE_MAX; i++) {		
		sprintf(szName, "zombie_%d", i);
		dCloneSprite("zombie",szName);
		dSetSpriteVisible(szName,0); // 设置它们不可见
	}

	// 初始化bullet精灵
	for(i = 0; i < BULLET_MAX; i++){
		sprintf(szName,"bullet_%d",i);
		dCloneSprite("bullet",szName);
		dSetSpriteVisible(szName,0); // 设置它们不可见
	}

	// 初始化wall精灵
	for(i = 0;i < WALL_MAX;i++){
		sprintf(szName,"wall_%d",i);
		dCloneSprite("wall",szName);
		dSetSpriteVisible(szName,0);
	}

	// 随机创建地图上的墙壁
	// for(i = 0;i < 10;i++){
	// 	float random1 = rand()%10/10.0*SCREEN_RIGHT*2 - SCREEN_RIGHT;
	// 	float random2 = rand()%10/10.0*SCREEN_BOTTOM*2 - SCREEN_BOTTOM;
	// 	newWall(random1,random2,1); // 完成框架前先把所有墙壁当作类型1
	// }
}
//==============================================================================
//
// 每局游戏进行中
void GameRun( float fDeltaTime )
{   
	int i; // 循环控制变量
	//检测游戏应该进入 哪个页面
	int g_iGameScene=0;
	char szName_kaishi.t2d;
	dLoadMap( const char *szName_kaishi.t2d );

	if（g_iGameScene == 0 )
	{
	dLoadMap( kaishi.t2d );}
	else if(g_iGameScene == 1)
	{
	dLoadMap( baseView.t2d);}
	else if (g_iGameScene == 2)
	{
	dLoadMap(shengcun.t2d);}
	
	// 计时
	counter += fDeltaTime;	
	zombie_appear_timer -= fDeltaTime;
	bullet_appear_timer -= fDeltaTime;
	for( i = 0;i < ZOMBIE_MAX;i++){
		zombie[i].get_shot_timer -= fDeltaTime;
	}

	// 以相等的间隔激活Zombie
	if( zombie_appear_timer < 0 ){
		zombie_appear_timer = ZOMBIE_APPEAR_TIME;
		newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP); // 在画面顶部生成一下测试用僵尸	
	}

	// 以相等的间隔发射Bullet
	// 需满足 1. 空格键弹起 2. 0.5秒的子弹装填时间 才可以发射子弹
	if ( shooting_flag == true){
		if( bullet_appear_timer < 0){
			playerShot();
			shooting_flag = false;
			bullet_appear_timer = BULLET_APPEAR_TIME;
		}
		else{
			shooting_flag = false;
		}
	}

	// 各个对象各自执行它们的行为
	Move();

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
	float fstartPosX=0.f,fstartPosY=0.f;
	if(iMouseType==0)
	{
	fstartPosX=dGetSpritePositionX("kaishi");
	fstartPosY=dGetSpritePositionY("kaishi");
	}
	if(fstartPosY=fMouseY)
	{g_iGameState=1;
	}
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
	//  空格键进入游戏
	if( KEY_SPACE == iKey && 0 == g_iGameState )
	{
		g_iGameState =	1;
	}

	// 控制主角移动
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
// 键盘弹起
// 参数 iKey：弹起的键，值见 enum KeyCodes 宏定义
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
// 精灵与精灵碰撞
// 参数 szSrcName：发起碰撞的精灵名字
// 参数 szTarName：被碰撞的精灵名字
void OnSpriteColSprite( const char *szSrcName, const char *szTarName )
{
	int i; // i,j为循环控制变量
	int j;
	char szName_bullet[64];
	char szName_zombie[64];
	char szName_wall[64];
	for(i = 0;i < BULLET_MAX;i++){
		if(!bullet[i].active){
			continue;
		}		
		sprintf(szName_bullet,"bullet_%d",i);
		if( !strcmp(szSrcName, szName_bullet) ){
			bullet->active = false;
			dSetSpriteVisible(szName_bullet,0);
		}
		for(j = 0;j < ZOMBIE_MAX;j++){
			if(!zombie[j].active){
				continue;
			}	
 			sprintf(szName_zombie,"zombie_%d",j);
			if( !strcmp(szTarName, szName_zombie) ){
				zombie[j].life--;
				zombie[j].get_shot_timer = ZOMBIE_HITTEN_TIME;
			}
		}
		for(j = 0;j < WALL_MAX;j++){
			if(!wall[j].active){
				continue;
			}
			sprintf(szName_wall,"wall_%d",j);
			if( !strcmp(szTarName,szName_wall) ){
				wall[j].life--;
				// wall[j].get_shot_timer = WALL_HITTEN_TIME;
			}
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

void Move() {	
	char szName[64];
	int i; // 循环控制变量

	//按类型分发Zombie行为
	for(i = 0;i <ZOMBIE_MAX;i++){
		if( !zombie[i].active ){
			continue;
		}
		sprintf(szName,"zombie_%d",i);
		// 被击中的Zombie显黄色
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
	// bullet 行为
	for(i = 0;i < BULLET_MAX;i++){
		if( !bullet[i].active ){
			continue;
		}
		sprintf(szName,"bullet_%d",i);
		bulletMove( &bullet[i] ,szName );
	}
	// wall 行为
	for(i = 0;i < WALL_MAX;i++){
		if( !wall[i].active ){
			continue;
		}
		sprintf(szName,"wall_%d",i);
		wallMove( &wall[i],szName);
	}
	
	// player 行为
	playerMove();
	
}

//===========================================================================
// 关于 Zombie 的函数

/**
 * Zombie的激活
 * 参数 ix(float)：生成处x坐标
 * 参数 iy(float)：生成处y坐标
 * 返回 僵尸ID(int)：(如果没有空缺：-1)
 */
int newZombie(float ix, float iy){
	for (int i = 0; i < ZOMBIE_MAX; i++) {
		if (zombie[i].active == false) {
			// 内部
			zombie[i].x = ix;
			zombie[i].y = iy;
			zombie[i].classify = 1; // 完成框架前，先把所有僵尸当作种类1
			zombie[i].life = ZOMBIE_TYPE1_LIFE;	
 			zombie[i].active = true;
			zombie[i].birthday = counter;

			// 外部显示
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
 * Zombie挂掉
 * 消失了
 */
void Disappear( struct Zombie* zombie , char* szName ){
	zombie->active = false;
	dSetSpriteVisible(szName,0);
}


/**
 * Zombie的行为1
 * 僵尸朝着玩家的方向走
 */
void move_zombie_classify1(struct Zombie* zombie , char* szName){

	// 同步位置
	zombie->x = dGetSpritePositionX(szName);
	zombie->y = dGetSpritePositionY(szName);

	// 朝着人方向走
	float deltaX = player.x - zombie->x ;
	float deltaY = player.y - zombie->y ;
	float delta = sqrt( (deltaX*deltaX) + (deltaY*deltaY) );
	float speedY = (deltaY / delta) * ZOMBIE_TYPE1_SPEED;
	float speedX = (deltaX / delta) * ZOMBIE_TYPE1_SPEED;
	dSetSpriteLinearVelocityY( szName, speedY );
	dSetSpriteLinearVelocityX( szName, speedX );	

	// 判断是否死亡
	if( zombie->life <= 0){
		Disappear(zombie,szName);
	}
}

//===========================================================================
//
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

			// 从极坐标转到直角坐标
			double radian;
			radian = bullet[i].direction*2*PI/360;	// 度数转弧度
			bullet[i].speedX = bullet[i].speed * cos(radian);
			bullet[i].speedY = -bullet[i].speed * sin(radian); // 右手系转左手系

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
	// 同步位置
	bullet->x = dGetSpritePositionX(szName);
	bullet->y = dGetSpritePositionY(szName);

	// 子弹出界则消失
	if ( (bullet->x < SCREEN_LEFT)||(SCREEN_RIGHT < bullet->x)||(bullet->y < SCREEN_TOP)||(SCREEN_BOTTOM < bullet->y) ) {
		bullet->active = false;
		dSetSpriteVisible(szName,0);
	}
}
//===========================================================================
//
// 关于 墙壁 的函数

/**
 * Wall的激活
 * 参数 ix(float)：生成处x坐标
 * 参数 iy(float)：生成处y坐标
 * 参数 classify(int): 生成墙壁的种类
 * 返回 墙壁ID(int)：(如果没有空缺：-1)
 */
int newWall(float ix, float iy, int classify){
	char szName[64];
	for (int i = 0; i < WALL_MAX; i++) {
		if (wall[i].active == false) {
			// 内部
			wall[i].x = ix;
			wall[i].y = iy;
			wall[i].classify = classify; 
			wall[i].life = ZOMBIE_TYPE1_LIFE;	// 完成框架前，先将所有墙壁当作种类1
 			wall[i].active = true;

			// 外部显示		
			sprintf(szName,"wall_%d",i);
			dSetSpritePositionX(szName,wall[i].x);
			dSetSpritePositionY(szName,wall[i].y);
			dSetSpriteVisible(szName,1); 
			return i;
		}
	}
	return -1;
}

/* 墙壁的行为 */
void wallMove(struct Wall* wall, char* szName){
	if( wall->life <= 0){
		wall->active = false;
		dSetSpriteVisible(szName,0);
	}
}

//===========================================================================
//
// 关于玩家行为的函数

/* 玩家发射子弹 */
void playerShot() {	
	if(life > 0){
		dPlaySound("gun_type1.wav", 0, 1 );
		newBullet(player.x, player.y,player.direction,BULLET_TYPE1_SPEED);
	}	
}

/* 玩家行为*/
void playerMove(){
	// 同步位置
	player.x = dGetSpritePositionX("player");
	player.y = dGetSpritePositionY("player");

	// 设置子弹发射方向、脸的朝向 
	// 目前只设置了四个朝向，之后增加
	if(abs((int)(player.speedBottom + player.speedTop)) > abs((int)(player.speedLeft + player.speedRight))){
		if( (player.speedBottom + player.speedTop) > 0){
			// 方向向下
			if(player.direction == 270){
				return;
			}
			player.direction = 270;
			dAnimateSpritePlayAnimation( "player", "player_testAnimation", 0);
		}else{
			// 方向向上
			if(player.direction == 90){
				return;
			}
			player.direction = 90;
			dAnimateSpritePlayAnimation( "player", "player_testAnimation3", 0);
		}
	}else if(abs((int)(player.speedBottom + player.speedTop)) < abs((int)(player.speedLeft + player.speedRight))){
		if((player.speedLeft + player.speedRight) > 0){
			// 方向向右
			if(player.direction == 0){
				return;
			}
			player.direction = 0;
			dAnimateSpritePlayAnimation("player","player_testAnimation2",0);
		}else{
			// 方向向左
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
