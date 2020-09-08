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
	float speedX;
	float speedY;
	int direction;
	float fire_timer;
	float get_shot_timer; // 被击中时为录入击中状态的时间
	int life;
	boolean active;
};

// 子弹
struct Bullet{
	int classify;
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
	int id;
	int life;
	int direction;
	float speed;
	float speedX;  	// X方向速度
	float speedY;   // Y方向速度
	float speedTop;
	float speedLeft;
	float speedRight;
	float speedBottom;
	boolean shooting_flag; // 控制玩家子弹输出
	float bitten_timer; // 人被咬了之后开始计时
	Zombie* bit_zombie; // 咬人的僵尸
};

/* 墙壁 */
struct Wall{
	float x;
	float y;
	int classify;
	int life;
	boolean active;
	float get_shot_timer; // 被击中时为录入击中状态的时间
};

/* 钱币 */
struct Money{
	int x;
	int y;
	boolean active;
	boolean get_flag;// 与下面的get_timer共同完成钱币计数特效
	float get_timer;
};

/* 剧情节点 */
struct Plot{
	int id; 
	char* speaker; // 说话者名字
	char* content; // 说话的内容
	int picture1; // 对应的立绘
	int picture2;
	boolean end_flag;
};

//==============================================================================
//
// 声明函数

void GameInit();
void GameRun( float fDeltaTime );
void GameEnd();
void Move();
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
int newMoney( float ix,float iy );
void getMoney( struct Money* money,char* szName );
void moneyMove( struct Money* money,char* szName );

//==============================================================================
//
// 声明常量

//单位最大数量设定
#define ZOMBIE_MAX 150
#define BULLET_MAX 50
#define WALL_MAX 150
#define MONEY_MAX 30

// 玩家最大速度设定
#define PLAYER_SPEED 10

// 子弹速度设定
#define BULLET_TYPE1_SPEED 150.0
#define BULLET_TYPE2_SPEED 300.0

// 僵尸子弹速度设定
#define BULLET_TYPE1_SPEED_ZOMBIE 20
#define BULLET_TYPE2_SPEED_ZOMBIE 15

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
#define ZOMBIE_TYPE1_LIFE 2
#define ZOMBIE_TYPE2_LIFE 6
#define ZOMBIE_TYPE3_LIFE 10

// 僵尸被杀死后money的掉落率 0~10
#define MONEY_APPEAR_RATE 4

// 钱币计数特效的持续时间
#define MONEY_FLY_TIME 0.4

// 圆周率
#define PI 3.1415926535

// 计时间隔
#define ZOMBIE_APPEAR_TIME 2.8
#define BULLET_APPEAR_TIME 0.6
#define ZOMBIE_HITTEN_TIME 0.3
#define WALL_HITTEN_TIME 0.3
#define PLAYER_BITTEN_TIME 0.1
#define ZOMBIE_FIRE_TIME 2

// 发射间距
#define GAP_BETWEEN_PLAYER_AND_BULLET 1

// 游玩人数
#define PLAYER_NUMBER 2

// 人物名字
#define BOY "罗吉"
#define GIRL "爱斯卡"

// 人物表情对应的帧数
#define BOY_NORMAL 0
#define BOY_NERVOUS 1
#define BOY_BAD 2
#define BOY_SCARED 3
#define BOY_CRY 4
#define GIRL_NORMAL 0
#define GIRL_CURIOUS 1
#define GIRL_HAPPY 2

// 游戏中的各种状态
#define MAIN_GAME 1
#define TALKING 2
#define SHOPPING 3

//==============================================================================
//
// 声明全局变量

boolean GodMode = false;	//无敌模式（调试用）

int bgmID; // 现在播放着的bgm的id

struct Bullet bullet[BULLET_MAX];	//预生成Bullet池
struct Zombie zombie[ZOMBIE_MAX];	//预生成Zombie池
struct Wall wall[WALL_MAX];  //预生成Wall池
struct Money money[MONEY_MAX];
struct Player player[PLAYER_NUMBER];  

struct Plot plot[100]; // 生成剧情链条
int current_plot;
int currnet_plot_ensure;

float SCREEN_LEFT;    // 屏幕边界值
float SCREEN_RIGHT;    
float SCREEN_TOP;    
float SCREEN_BOTTOM;  

float counter; // 全局计时器

int myMoney; // 金钱


// 控制旗

/**
 * 场景控制旗
 * 1: 开始页面
 * 2：剧情模式第一关
 * 3：第二关
 * 4：第三关
 * 5：游戏玩法
 */
int game_scene_flag = 1;
int current_scene_flag = 0;

/**
 * 状态控制旗
 * MAIN_GAME 1 游戏中状态
 * TALKING 2 剧情对话播放状态
 * SHOPPING 3 自动售货机购物中状态
 */
int game_mode_flag;

boolean click_flag = false; // 检测鼠标点击


boolean boss_flag; // 出现boss
// 时间间隔计时器
float zombie_appear_timer;
float bullet_appear_timer;

//==============================================================================
//
// 大体的程序流程为：GameMainLoop函数为主循环函数，在引擎每帧刷新屏幕图像之后，都会被调用一次。


//==============================================================================
//
// 游戏主循环，此函数将被不停的调用，引擎每刷新一次屏幕，此函数即被调用一次
// 用以处理游戏的开始、进行中、结束等各种状态. 
// 函数参数fDeltaTime : 上次调用本函数到此次调用本函数的时间间隔，单位：秒
void GameMainLoop( float fDeltaTime )
{
	switch( g_iGameState )
	{
		// 初始化游戏，清空上一局相关数据
	case 1:
		{		
			g_iGameState = 2; // 初始化之后，将游戏状态设置为进行中
		}
		break;

		// 游戏进行中，处理各种游戏逻辑
	case 2:
		{
			// TODO 修改此处游戏循环条件，完成正确游戏逻辑
			if( true )
			{
				// 判断游戏在哪个画面中
				if( current_scene_flag == game_scene_flag ){							
				}else{
					switch( game_scene_flag ){
						case 1: dLoadMap("start.t2d");break;
						case 2: dLoadMap("baseView.t2d"); GameInit();loadPlots(); break;
						case 3: GameInit();break;
						case 4: GameInit();break;
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
				if( game_scene_flag == 3){
					if(game_mode_flag == MAIN_GAME){
						GameRun( fDeltaTime );
					}
					if(game_mode_flag == TALKING){
						playPlots( fDeltaTime );
					}				
				}
				if( game_scene_flag == 4){
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
				// 游戏结束。调用游戏结算函数，并把游戏状态修改为结束状态
				g_iGameState = 0;
				GameEnd();
			}
		}
		break;

		// 游戏结束/等待按空格键开始
	case 0: g_iGameState = 1;
	default:
		break;
	};
}

//==============================================================================
//
// 每局开始前进行初始化，清空上一局相关数据
void GameInit()
{
	// 不同关卡的分支操作
	if( game_scene_flag == 2){

		// 先播个音乐
		bgmID = dPlaySound("bgm_test1.ogg", 1, 1.0 );

		// 初始化生命值 / 分数 / 金钱
		myMoney = 0;

		// 初始化计数器 与 计时器
		counter = 20;
		zombie_appear_timer = ZOMBIE_APPEAR_TIME;
		bullet_appear_timer = BULLET_APPEAR_TIME;

		// 去到剧情开始
		current_plot = 0;
		currnet_plot_ensure = -1;

	}else if( game_scene_flag == 3){

		// 先播个音乐
		dStopSound( bgmID );
		bgmID = dPlaySound("bgm_test2.ogg", 1, 1.0 );

		// 初始化计数器 与 计时器
		counter = 20;
		zombie_appear_timer = ZOMBIE_APPEAR_TIME*2;
		bullet_appear_timer = BULLET_APPEAR_TIME;
	}else if( game_scene_flag == 4){

		// 先播个音乐
		dStopSound( bgmID );
		bgmID = dPlaySound("bgm_test3.ogg", 1, 1.0 );

		// 初始化计数器 与 计时器
		// counter = 60;
		zombie_appear_timer = ZOMBIE_APPEAR_TIME*3;
		bullet_appear_timer = BULLET_APPEAR_TIME;
	}
	

	// 初始化控制旗
	boss_flag = false;

	// 拿到窗口边界
	SCREEN_LEFT = dGetScreenLeft();
	SCREEN_RIGHT = dGetScreenRight(); 
	SCREEN_TOP = dGetScreenTop();
	SCREEN_BOTTOM = dGetScreenBottom();

	// 先进入剧情模式
	game_mode_flag = TALKING;
	
	// 循环控制变量	
	int i;

	// 初始化zombie
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

	// 初始化子弹
	for(i = 0; i < BULLET_MAX; i++){ 
		bullet[i].classify = 1;
		bullet[i].speed = 0;
		bullet[i].x = 0.0;
		bullet[i].y = 0.0;
		bullet[i].color = 1;
		bullet[i].direction = 0;
		bullet[i].active = false;
	}

	// 初始化僵尸的子弹
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
		wall[i].get_shot_timer = 0;
	}

	// 初始化钱币
	for(i = 0;i < MONEY_MAX;i++){
		money[i].x = 0;
		money[i].y = 0;
 		money[i].active = false;
		money[i].get_flag = false;
		money[i].get_timer = 0;
	}

	// 初始化玩家
	for(i = 0;i < PLAYER_NUMBER;i++){
		player[i].life = LIFE_INIT;
		player[i].x = -pow(-1,i)*SCREEN_RIGHT/2;
		player[i].y = SCREEN_BOTTOM - 10;
		player[i].id = i;
		player[i].direction = 90;
		player[i].speed = PLAYER_SPEED;
		player[i].speedX = 0.f;  	// X方向速度
		player[i].speedY = 0.f; // Y方向速度
		player[i].speedRight = 0.f;  	
		player[i].speedLeft = 0.f;  	
		player[i].speedTop = 0.f;  
		player[i].speedBottom = 0.f;  
		player[i].shooting_flag = false;	
		player[i].bitten_timer = 0;
		player[i].bit_zombie = NULL;
	}
	

	// 共用字符串地址，避免资源浪费
	char szName[64];

	// 初始化zombie精灵
	for(i = 0; i < ZOMBIE_MAX; i++) {		
		sprintf(szName, "zombie_%d", i);
		if( game_scene_flag == 2){
			dCloneSprite("zombie",szName);
		}	
		dSetSpriteVisible(szName,0); // 设置它们不可见
	}

	// 初始化bullet精灵
	for(i = 0; i < BULLET_MAX; i++){
		sprintf(szName,"bullet_%d",i);
		if( game_scene_flag == 2){
			dCloneSprite("bullet",szName);
		}		
		dSetSpriteVisible(szName,0); // 设置它们不可见
	}

	// 初始化wall精灵
	// 因为墙壁精灵靠的是物理碰撞，无法通过设置active回收，这里手动将它们移开
	for(i = 0;i < WALL_MAX;i++){
		sprintf(szName,"wall_%d",i);
		if( game_scene_flag == 2){
			dCloneSprite("wall",szName);
		}		
		dSetSpritePosition(szName,SCREEN_LEFT-50,SCREEN_TOP-50);
		dSetSpriteVisible(szName,0);
	}

	// 初始化player精灵
	for(i = 0;i < PLAYER_NUMBER;i++){
		sprintf(szName,"player_%d",i);
		dSetSpritePosition( szName,player[i].x,player[i].y );
	}

	// 初始化money精灵
	for(i = 0;i < MONEY_MAX;i++){
		sprintf(szName,"money_%d",i);
		if( game_scene_flag == 2){
			dCloneSprite("money",szName);
		}	
		dSetSpriteVisible(szName,0);
	}
	
	// 随机创建地图上的墙壁
	for(i = 0;i < 10;i++){
		float random1 = rand()%10/10.0*SCREEN_RIGHT*2 - SCREEN_RIGHT;
		float random2 = rand()%10/10.0*SCREEN_BOTTOM*2 - SCREEN_BOTTOM;
		newWall(random1,random2,1); // 完成框架前先把所有墙壁当作类型1
	}
}
//==============================================================================
//
// 每局游戏进行中
void GameRun( float fDeltaTime )
{	
	int i; // 循环控制变量

	// 计时
	counter -= fDeltaTime;	
	zombie_appear_timer -= fDeltaTime;
	bullet_appear_timer -= fDeltaTime;
	for( i = 0;i < ZOMBIE_MAX;i++){
		zombie[i].get_shot_timer -= fDeltaTime;
		if(zombie[i].classify == 2){
			zombie[i].fire_timer -= fDeltaTime;
		}		
		if(zombie[i].classify == 3){
			zombie[i].fire_timer -= fDeltaTime;
		}
	}
	for( i = 0;i < WALL_MAX;i++){
		wall[i].get_shot_timer -= fDeltaTime;
	}
	for( i = 0;i < PLAYER_NUMBER;i++){
		player[i].bitten_timer -= fDeltaTime;
	}
	for( i = 0;i < MONEY_MAX;i++){
		money[i].get_timer -= fDeltaTime;
	}

	// 根据不同的关卡做不一样的事情
	if( game_scene_flag == 2){
		// 以相等的间隔激活Zombie
		if( counter >= 0){
			if( zombie_appear_timer < 0 ){
				zombie_appear_timer = ZOMBIE_APPEAR_TIME;
				newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP,1); // 在画面顶部生成一下测试用僵尸	
			}
		}
		// 激活boss
		if( (counter < 0) && (boss_flag == false) ){
			boss_flag = true;
			newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP,2);
		}
		if( counter < 0 ){
			// 进入下一关
			boolean next_flag = true;
			if( boss_flag == true ){
				for(i = 0;i < ZOMBIE_MAX;i++){
					if( zombie[i].active == true ){
						next_flag = false;
					}		
				}
				if( next_flag == true ){
					game_scene_flag = 3;
					turnYellow("map");
					counter = 20;
				}			
			}
		}
		
	}else if( game_scene_flag == 3 ){
		// 以相等的间隔激活Zombie
		if( counter >= 0){
			if( zombie_appear_timer < 0 ){
				zombie_appear_timer = ZOMBIE_APPEAR_TIME*2;
				newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP,1); // 在画面顶部生成一下测试用僵尸
				newZombie(SCREEN_RIGHT, SCREEN_BOTTOM*2*(rand()%10/10.0) - SCREEN_BOTTOM,1); // 在画面左部生成一下测试用僵尸
				newZombie(SCREEN_LEFT, SCREEN_BOTTOM*2*(rand()%10/10.0) - SCREEN_BOTTOM,1); // 在画面右部生成一下测试用僵尸
			}
		}
		// 激活boss
		if( (counter < 0) && (boss_flag == false) ){
			boss_flag = true;
			newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP,2);
			newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP,2);	
			newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP,2);
		}

		// 进入下一关
		if(counter < 0){
			boolean next_flag = true;
			if( boss_flag == true ){
				for(i = 0;i < ZOMBIE_MAX;i++){
					if( zombie[i].active == true ){
						next_flag = false;
					}		
				}
				if( next_flag == true ){
					game_scene_flag = 4;
					turnRed("map");
				}			
			}
		}
		
	}else if( game_scene_flag == 4 ){
		if( boss_flag == false ){
			boss_flag = true;
			newZombie(SCREEN_RIGHT*2*(rand()%10/10.0) - SCREEN_RIGHT, SCREEN_TOP,3);
		}
		if( zombie_appear_timer < 0 ){
			zombie_appear_timer = ZOMBIE_APPEAR_TIME*3;
			newZombie(SCREEN_RIGHT, SCREEN_BOTTOM*2*(rand()%10/10.0) - SCREEN_BOTTOM,1); // 在画面左部生成一下测试用僵尸
			newZombie(SCREEN_LEFT, SCREEN_BOTTOM*2*(rand()%10/10.0) - SCREEN_BOTTOM,1); // 在画面右部生成一下测试用僵尸
		}
	}
	

	// 以相等的间隔发射Bullet
	// 需满足 1. 空格键弹起 2. 0.5秒的子弹装填时间 才可以发射子弹
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
	// 各个对象各自执行它们的行为
	Move();
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
	dSetSpritePosition("cursor",fMouseX,fMouseY);
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
	if( iMouseType == 0 ){
		click_flag = true;
	}
}
//==========================================================================
//
// 键盘按下
// 参数 iKey：被按下的键，值见 enum KeyCodes 宏定义
// 参数 iAltPress, iShiftPress，iCtrlPress：键盘上的功能键Alt，Ctrl，Shift当前是否也处于按下状态(0未按下，1按下)
void OnKeyDown( const int iKey, const bool bAltPress, const bool bShiftPress, const bool bCtrlPress )
{	
	// 控制主角移动
	if( player[0].bit_zombie == NULL ){
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
	

	// 控制主角移动
	if( player[1].bit_zombie == NULL ){
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
// 键盘弹起
// 参数 iKey：弹起的键，值见 enum KeyCodes 宏定义
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

	if( player[0].bit_zombie == NULL ){
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
	
	if( player[1].bit_zombie == NULL ){
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
// 精灵与精灵碰撞
// 参数 szSrcName：发起碰撞的精灵名字
// 参数 szTarName：被碰撞的精灵名字
void OnSpriteColSprite( const char *szSrcName, const char *szTarName )
{
	// 先检测鼠标精灵碰撞事件
	// 鼠标事件1：从场景1切换至场景2，开始剧情模式
	if( click_flag ){
		click_flag = false;
		if( !strcmp(szSrcName, "cursor") && !strcmp(szTarName, "start") ){
			game_scene_flag = 2;
		}
	}

	int i; // i,j为循环控制变量
	int j;
	char szName_bullet[64];
	char szName_zombie[64];
	char szName_wall[64];
	char szName_player[64];
	char szName_money[64];
	boolean activeflag = false;

	for(i = 0;i < BULLET_MAX;i++){
		if(!bullet[i].active){
			continue;
		}	
		sprintf(szName_bullet,"bullet_%d",i);
		if( !strcmp(szSrcName, szName_bullet) ){
			// 玩家射出的子弹
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
			}else if( (bullet[i].classify == 2)||(bullet[i].classify == 3) ){ // 僵尸射出的弹幕
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
				if( !player[j].life > 0 ){
						continue;
				}
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

	// 玩家和金币碰撞
	// for( i = 0;i < PLAYER_NUMBER ;i++){
	// 	if( player->life < 0 ){
	// 		continue;
	// 	}
	// 	sprintf(szName_player,"player_%d",i);
	// 	if( !strcmp(szSrcName,szName_player) ){
	// 		for(j = 0;j < MONEY_MAX;j++){
	// 			if( !money[j].active ){
	// 					continue;
	// 			}
	// 			sprintf(szName_money,"money_%d",j);
	// 			if( !strcmp(szTarName, szName_money) ){
	// 				getMoney( &money[j],szName_money );
	// 			}
	// 		}
	// 	}

	// }
	
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
		zombieMove( &zombie[i],szName );
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
		// 被击中的Wall显黄色
		if( wall[i].get_shot_timer > 0 ){
			turnYellow(szName);
		}else{
			turnWhite(szName);
		}
		sprintf(szName,"wall_%d",i);
		wallMove( &wall[i],szName );
	}
	
	// player 行为
	for(i = 0;i < PLAYER_NUMBER;i++){
		sprintf(szName,"player_%d",i);
		playerMove( &player[i],szName );
	}
	
	// money 行为
	for(i = 0;i < MONEY_MAX;i++){
		sprintf(szName,"money_%d",i);
		moneyMove( &money[i],szName );
	}
	
	
}

//===========================================================================
// 关于 Zombie 的函数

/**
 * Zombie的激活
 * 参数 ix(float)：生成处x坐标
 * 参数 iy(float)：生成处y坐标
 * 参数 classify(int)：生成僵尸的种类
 * 返回 僵尸ID(int)：(如果没有空缺：-1)
 */
int newZombie(float ix, float iy,int classify){
	for (int i = 0; i < ZOMBIE_MAX; i++) {
		if (zombie[i].active == false) {
			// 内部
			zombie[i].x = ix;
			zombie[i].y = iy;
			zombie[i].classify = classify; 
			switch(classify){
				case 1:zombie[i].life = ZOMBIE_TYPE1_LIFE;break;
				case 2:zombie[i].life = ZOMBIE_TYPE2_LIFE;break;
				case 3:zombie[i].life = ZOMBIE_TYPE3_LIFE;break;
			}		
			zombie[i].direction = 270;
 			zombie[i].active = true;
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
void zombieDisappear( struct Zombie* zombie , char* szName ){

	// 留下一滩血迹
	char bloodClone[128];
	sprintf(bloodClone,"blood_clone_%d", rand()%1000);
	dCloneSprite("blood",bloodClone);
	dSetSpritePosition(bloodClone,zombie->x,zombie->y);
	dSetSpriteLifeTime(bloodClone,5);

	// 僵尸消失
	zombie->active = false;
	dSetSpriteVisible(szName,0);

	// 一定概率产生money
	if( rand()%10 < MONEY_APPEAR_RATE ){
		newMoney(zombie->x,zombie->y);
	}
}


/**
 * Zombie的行为
 * 僵尸朝着玩家的方向走
 */
void zombieMove(struct Zombie* zombie , char* szName){

	// 同步位置
	zombie->x = dGetSpritePositionX(szName);
	zombie->y = dGetSpritePositionY(szName);

	float deltaX[2];
	float deltaY[2];
	float delta[2];
	float speedY;
	float speedX;

	// 锁定离它最近的人
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
	
	// 老僵尸会对离它最近的人发射弹幕
	if( zombie->classify == 2){
		if( zombie->fire_timer < 0){
			zombie->fire_timer = ZOMBIE_FIRE_TIME;
			FireAim(zombie->x,zombie->y,zombie->direction,"zombie");
		}
	}else if( zombie->classify == 3){ // 僵尸BOSS的弹幕
		if( zombie->fire_timer < 0){
			zombie->fire_timer = ZOMBIE_FIRE_TIME;
			FireAim(zombie->x,zombie->y,zombie->direction,"zombie");
			FireAim(zombie->x,zombie->y,zombie->direction,"zombie");
			FireRound(zombie->x,zombie->y,"zombie");
		}
	}

	// 朝着人方向走
	zombie->speedX = speedX;
	zombie->speedY = speedY;
	dSetSpriteLinearVelocityY( szName, speedY );
	dSetSpriteLinearVelocityX( szName, speedX );

	// 判断是否死亡
	if( zombie->life <= 0){
		zombieDisappear(zombie,szName);
	}

	// 根据僵尸的种类设定僵尸的显示图片
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
			sprintf(left,"zombie%dAnimation1",zombie->classify);
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
		}case 3:{
			sprintf(left,"zombie%d_2Animation1",zombie->classify);
			sprintf(right,"zombie%d_2Animation",zombie->classify);
			sprintf(top,"zombie%d_3Animation1",zombie->classify);
			sprintf(bottom,"zombie%d_3Animation",zombie->classify);
			sprintf(top_left,"zombie%d_1Animation3",zombie->classify);
			sprintf(top_right,"zombie%d_1Animation2",zombie->classify);
			sprintf(bottom_left,"zombie%d_1Animation",zombie->classify);
			sprintf(bottom_right,"zombie%d_1Animation1",zombie->classify);
		}
	}

	// 根据速度转头，更换direction
	if( (speedX == 0) && (speedY == 0) ){
		return;
	}	
	// 拿到速度向量在右手系中的角度值
	float shita = atan2(-speedY,speedX)*180/PI;// y取负号：左手系转右手系
	if( shita < -170 ){ // 方向向左
		if(zombie->direction == 180){
			return;
		}
		zombie->direction = 180;
		dAnimateSpritePlayAnimation( szName,left,0);
	}else if( shita < -100 ){ // 方向向左下角
		if(zombie->direction == 225){
			return;
		}
		zombie->direction = 225;
		dAnimateSpritePlayAnimation(szName,bottom_left,0);
	}else if( shita < -80 ){ // 方向向下
		if(zombie->direction == 270){
			return;
		}
		zombie->direction = 270;
		dAnimateSpritePlayAnimation( szName, bottom, 0);
	}else if( shita < -10 ){ // 方向向右下角
		if(zombie->direction == 315){
			return;
		}
		zombie->direction = 315;
		dAnimateSpritePlayAnimation(szName,bottom_right,0);
	}else if( shita < 10 ){ // 方向向右
		if(zombie->direction == 0){
			return;
		}
		zombie->direction = 0;
		dAnimateSpritePlayAnimation( szName,right,0);
	}else if( shita < 80 ){ // 方向向右上角
		if(zombie->direction == 45){
			return;
		}
		zombie->direction = 45;
		dAnimateSpritePlayAnimation(szName,top_right,0);
	}else if( shita < 110 ){ // 方向向上
		if(zombie->direction == 90){
			return;
		}
		zombie->direction = 90;
		dAnimateSpritePlayAnimation( szName, top, 0);
	}else if( shita < 170 ){ // 方向向左上角
		if(zombie->direction == 135){
			return;
		}
		zombie->direction = 135;
		dAnimateSpritePlayAnimation(szName,top_left,0);
	}else{ // 方向向左
		if(zombie->direction == 180){
			return;
		}
		zombie->direction = 180;
		dAnimateSpritePlayAnimation( szName,left,0);
	};
}

//===========================================================================
//
// 关于 子弹 的函数

/**
 *  向四周发射bullet的函数
 * 这可以做成人物或者是boss的技能吧
 * */
void FireRound(float x, float y,char* user){
	for(int i = 0; i < 360; i += 30 )
	{
		if( !strcmp(user,"zombie") ){
			newBullet(x, y, i, BULLET_TYPE2_SPEED_ZOMBIE,3);
		}	
	}
}

/**
 *  向固定方向发射扇形bullet的函数
 *  这可以做成人物或者是boss的技能吧
 * */
void FireAim(float x,float y,int direction,char* user){
	if( !strcmp(user,"zombie") ){
		newBullet(x, y, direction, BULLET_TYPE1_SPEED_ZOMBIE,2);
		newBullet(x, y, direction+20, BULLET_TYPE1_SPEED_ZOMBIE,2);
		newBullet(x, y, direction-20, BULLET_TYPE1_SPEED_ZOMBIE,2);
	}
}
/**
 * 子弹激活
 * 参数 ix(float)：生成处x坐标
 * 参数 iy(float)：生成处y坐标
 * 参数 idirection(float)：移动的方向
 * 参数 ispeed(float)：移动的速度
 * 参数 classify(float)：子弹的种类
 * 返回 子弹ID(float)：(如果没有空缺：-1)
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
			if( classify == 2 ){
				dSetStaticSpriteImage(szName,"particles1ImageMap", 0);
			}else if( classify == 1 ){
				dSetStaticSpriteImage(szName,"bulletImageMap", 0 );
			}else if( classify == 3){
				dSetStaticSpriteImage(szName,"particles1ImageMap",1);
			}
			dSetSpriteRotation( szName, 90 - bullet[i].direction );
			dSetSpriteVisible(szName,1);
			return i;
		}
	}
	return -1;
}

/**
 * 子弹行为
 */
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

/**
 * Bullet消失
 */
void bulletDisappear( struct Bullet* bullet , char* szName ){
	bullet->active = false;
	dSetSpriteVisible(szName,0);
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
			wall[i].life = WALL_TYPE1_LIFE;	// 完成框架前，先将所有墙壁当作种类1
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
		dSetSpritePosition(szName,SCREEN_LEFT-50,SCREEN_TOP-50);
	}
}

//===========================================================================
//
// 关于 钱币 的函数

/**
 * Money的激活
 * 参数 ix(float)：生成处x坐标
 * 参数 iy(float)：生成处y坐标
 * 返回 钱币ID(int)：(如果没有空缺：-1)
 */
int newMoney( float ix,float iy ){
	char szName[64];
	for(int i = 0;i < MONEY_MAX;i++){
		if (money[i].active == false) {
			// 内部
 			wall[i].active = true;
			wall[i].x = ix;
			wall[i].y = iy;
			// 外部显示		
			sprintf(szName,"money_%d",i);
			dSetSpritePositionX(szName,ix);
			dSetSpritePositionY(szName,iy);
			dSetSpriteVisible(szName,1); 
			// dSetSpriteCollisionReceive( szName, 1 );// 设置为可碰撞
			// dSetSpriteCollisionPhysicsReceive( szName, 0 );

			getMoney( money,szName );

			return i;
		}
	}
	return -1;
}

/* 玩家捡到钱币后调用 */
void getMoney( struct Money* money,char* szName ){

	// dSetSpriteCollisionReceive( szName, 0 );// 设置为不可碰撞
	dSpriteMoveTo( // 飞向钱币的计数位置
		szName, 
		dGetSpritePositionX("money"), 
		dGetSpritePositionY("money"), 
		200, 
		1
	);

	money->get_flag = true;
	money->get_timer = MONEY_FLY_TIME;

}
/* 钱币行为，看下有没有特效可以做 */
void moneyMove( struct Money* money,char* szName ){
	// int i;
	// char szName_player[64];
	// 看下自己有没有被捡起来
	// if( !money->get_flag ){
	// 	for( i = 0;i < PLAYER_NUMBER ;i++){
	// 		if( player->life < 0 ){
	// 			continue;
	// 		}
	// 		sprintf(szName_player,"player_%d",i);
	// 		float playerLeft = dGetSpritePositionX(szName_player) - dGetSpriteWidth(szName_player);
	// 		float playerRight = dGetSpritePositionX(szName_player) + dGetSpriteWidth(szName_player);
	// 		float playerTop = dGetSpritePositionY(szName_player) - dGetSpriteHeight(szName_player);
	// 		float playerBottom = dGetSpritePositionY(szName_player) + dGetSpriteHeight(szName_player);
	// 		if( // 判断钱币是否被玩家的坐标范围包围
	// 			(money->x < playerRight)&&
	// 			(money->x > playerLeft)&&
	// 			(money->y > playerTop)&&
	// 			(money->y < playerBottom)
	// 			){
	// 				getMoney( money,szName );
	// 			}
	// 		}

	// 	}
	// 被捡起来之后的特效
	if( money->get_flag ){
		if( money->get_timer > 0){
			// 按照特效剩余时长变换钱币的大小
			dSetSpriteForceScale( szName, money->get_timer/MONEY_FLY_TIME );
		}else{ // 特效执行完毕 钱币+1
			money->get_flag = false;
			dSetSpriteVisible(szName,0);
			dSetSpriteForceScale(szName, 1 );// 回归原来大小
			myMoney++;

			// 修改钱币的显示
			char text[64];
			sprintf( text,"x%d",myMoney );
			dSetTextString( "moneytext", text );
		}
		
	}
}

//===========================================================================
//
// 关于玩家行为的函数

/* 玩家发射子弹 */
void playerShot(Player* player) {	
	if( player->life > 0){
		int shootX;
		int shootY;
		char szName[64];
		sprintf(szName,"player_%d",player->id);
		switch ( player->direction ){
			case 0: // 朝右开枪
				shootX = player->x + GAP_BETWEEN_PLAYER_AND_BULLET; 
				shootY = player->y;
				break; 
			case 45: // 朝右上方开枪
				shootX = player->x + GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y + GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 90: // 朝上开枪
				shootX = player->x;
				shootY = player->y - GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 135: // 朝左上方开枪
				shootX = player->x - GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y - GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 180: // 朝左开枪
				shootX = player->x - GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y;
				break;
			case 225: // 朝左下方开枪
				shootX = player->x - GAP_BETWEEN_PLAYER_AND_BULLET;
				shootY = player->y + GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 270: // 朝下开枪
				shootX = player->x;
				shootY = player->y + GAP_BETWEEN_PLAYER_AND_BULLET;
				break;
			case 315: // 朝右下方开枪
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

/* 玩家行为 */
void playerMove( struct Player* player, char* szName){
	// 同步位置
	player->x = dGetSpritePositionX(szName);
	player->y = dGetSpritePositionY(szName);

	// 处理一下玩家出界的问题
	if(dGetSpritePositionY( szName ) < SCREEN_TOP){
		dSetSpritePositionY( szName,SCREEN_TOP);
	}
	if(dGetSpritePositionY( szName ) > SCREEN_BOTTOM){
		dSetSpritePositionY(szName,SCREEN_BOTTOM);
	}
	if(dGetSpritePositionX( szName ) < SCREEN_LEFT){
		dSetSpritePositionX(szName,SCREEN_LEFT);
	}
	if(dGetSpritePositionX( szName ) > SCREEN_RIGHT){
		dSetSpritePositionX(szName,SCREEN_RIGHT);
	}

	// 设置遭受攻击之后各个阶段玩家的行为
	if( player->bitten_timer > PLAYER_BITTEN_TIME/2 ){
		turnRed( szName );
	}else if( (player->bitten_timer > 0) && (player->bitten_timer < PLAYER_BITTEN_TIME/4) ){
		if( player->bit_zombie != NULL ){
			player->bit_zombie = NULL;
			dSetSpriteLinearVelocityY( szName, 0 );
			dSetSpriteLinearVelocityX( szName, 0 );
		}	
		turnRed( szName );
	}else{
		turnWhite( szName );
	}
	// 判断是否遭受了攻击
	// 受到了攻击则直接取消之后的行动
	if( player->bit_zombie != NULL ){
		float deltaX = player->bit_zombie->x - player->x;
		float deltaY = player->bit_zombie->y - player->y;
		float delta = sqrt( (deltaX*deltaX) + (deltaY*deltaY) );
		float speedY = -(deltaY / delta) * 80;
		float speedX = -(deltaX / delta) * 80;
		dSetSpriteLinearVelocityX( szName,speedX );
		dSetSpriteLinearVelocityY( szName,speedY );
		return;
	}

	// 根据玩家的不同获取朝向的不同 
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

	// 设置子弹发射方向、脸的朝向 
	if( (player->speedX == 0) && (player->speedY == 0) ){
		return;
	}
	// 拿到速度向量在右手系中的角度值
	float shita = atan2(-player->speedY,player->speedX)*180/PI;// y取负号：左手系转右手系
	if( shita < -170 ){ // 方向向左
		if(player->direction == 180){
			return;
		}
		player->direction = 180;
		dAnimateSpritePlayAnimation( szName,left,0);
	}else if( shita < -100 ){ // 方向向左下角
		if(player->direction == 225){
			return;
		}
		player->direction = 225;
		dAnimateSpritePlayAnimation(szName,bottom_left,0);
	}else if( shita < -80 ){ // 方向向下
		if(player->direction == 270){
			return;
		}
		player->direction = 270;
		dAnimateSpritePlayAnimation( szName, bottom, 0);
	}else if( shita < -10 ){ // 方向向右下角
		if(player->direction == 315){
			return;
		}
		player->direction = 315;
		dAnimateSpritePlayAnimation(szName,bottom_right,0);
	}else if( shita < 10 ){ // 方向向右
		if(player->direction == 0){
			return;
		}
		player->direction = 0;
		dAnimateSpritePlayAnimation( szName,right,0);
	}else if( shita < 80 ){ // 方向向右上角
		if(player->direction == 45){
			return;
		}
		player->direction = 45;
		dAnimateSpritePlayAnimation(szName,top_right,0);
	}else if( shita < 110 ){ // 方向向上
		if(player->direction == 90){
			return;
		}
		player->direction = 90;
		dAnimateSpritePlayAnimation( szName, top, 0);
	}else if( shita < 170 ){ // 方向向左上角
		if(player->direction == 135){
			return;
		}
		player->direction = 135;
		dAnimateSpritePlayAnimation(szName,top_left,0);
	}else{ // 方向向左
		if(player->direction == 180){
			return;
		}
		player->direction = 180;
		dAnimateSpritePlayAnimation( szName,left,0);
	};

}
//===========================================================================
//
// 与剧情有关的函数

/* 载入剧情 */
void loadPlots(){
	plot[0].id = 0;
	plot[0].speaker = BOY;
	plot[0].content = "没想到竟然被传送到未来去了";
	plot[0].picture1 = BOY_NERVOUS;
	plot[0].picture2 = GIRL_NORMAL;
	plot[0].end_flag = false;

	plot[1].id = 1;
	plot[1].speaker = GIRL;
	plot[1].content = "还好是来到了未来，要是去到远古时代都没有奶茶都没得喝了";
	plot[1].picture1 = BOY_NERVOUS;
	plot[1].picture2 = GIRL_HAPPY;
	plot[1].end_flag = false;

	plot[2].id = 2;
	plot[2].speaker = BOY;
	plot[2].content = "欸你还有心情喝奶茶…";
	plot[2].picture1 = BOY_NERVOUS;
	plot[2].picture2 = GIRL_HAPPY;
	plot[2].end_flag = false;

	plot[3].id = 3;
	plot[3].speaker = BOY;
	plot[3].content = "欸哪来的奶茶!";
	plot[3].picture1 = BOY_SCARED;
	plot[3].picture2 = GIRL_HAPPY;
	plot[3].end_flag = false;
	
	plot[4].id = 4;
	plot[4].speaker = GIRL;
	plot[4].content = "刚才自动贩卖机里面卖的来着";
	plot[4].picture1 = BOY_SCARED;
	plot[4].picture2 = GIRL_CURIOUS;
	plot[4].end_flag = false;

	plot[5].id = 5;
	plot[5].speaker = BOY;
	plot[5].content = "啊，完全没看见！话说回来，未来世界怎么完全看不见人影啊";
	plot[5].picture1 = BOY_NERVOUS;
	plot[5].picture2 = GIRL_CURIOUS;
	plot[5].end_flag = false;

	plot[6].id = 6;
	plot[6].speaker = GIRL;
	plot[6].content = "僵尸倒有不少";
	plot[6].picture1 = BOY_NORMAL;
	plot[6].picture2 = GIRL_HAPPY;
	plot[6].end_flag = false;

	plot[7].id = 7;
	plot[7].speaker = BOY;
	plot[7].content = "真的欸";
	plot[7].picture1 = BOY_CRY;
	plot[7].picture2 = GIRL_HAPPY;
	plot[7].end_flag = false;

	plot[8].id = 8;
	plot[8].speaker = GIRL;
	plot[8].content = "拿上这个";
	plot[8].picture1 = BOY_NERVOUS;
	plot[8].picture2 = GIRL_NORMAL;
	plot[8].end_flag = false;

	plot[9].id = 9;
	plot[9].speaker = BOY;
	plot[9].content = "这是…手枪，哪里来的啊";
	plot[9].picture1 = BOY_SCARED;
	plot[9].picture2 = GIRL_NORMAL;
	plot[9].end_flag = false;

	plot[10].id = 10;
	plot[10].speaker = GIRL;
	plot[10].content = "也是自动贩卖机里来的";
	plot[10].picture1 = BOY_SCARED;
	plot[10].picture2 = GIRL_HAPPY;
	plot[10].end_flag = true;

	plot[11].id = 11;
	plot[11].speaker = GIRL;
	plot[11].content = "我们必须回到过去阻止这一切的发生";
	plot[11].picture1 = BOY_NERVOUS;
	plot[11].picture2 = GIRL_HAPPY;
	plot[11].end_flag = false;

	plot[12].id = 12;
	plot[12].speaker = BOY;
	plot[12].content = "啊，来了，王道的剧情展开";
	plot[12].picture1 = BOY_NORMAL;
	plot[12].picture2 = GIRL_NORMAL;
	plot[12].end_flag = false;

	plot[13].id = 13;
	plot[13].speaker = GIRL;
	plot[13].content = "还记得博士吗，我怀疑这就是他做的好事";
	plot[13].picture1 = BOY_NORMAL;
	plot[13].picture2 = GIRL_NORMAL;
	plot[13].end_flag = false;

	plot[14].id = 14;
	plot[14].speaker = BOY;
	plot[14].content = "哦，他窃取了我们的研究资料，还把我们送到其他时空";
	plot[14].picture1 = BOY_NERVOUS;
	plot[14].picture2 = GIRL_CURIOUS;
	plot[14].end_flag = false;

	plot[15].id = 15;
	plot[15].speaker = BOY;
	plot[15].content = "就是为了研究僵尸病毒！";
	plot[15].picture1 = BOY_NERVOUS;
	plot[15].picture2 = GIRL_CURIOUS;
	plot[15].end_flag = false;

	plot[16].id = 16;
	plot[16].speaker = GIRL;
	plot[16].content = "没错，我们现在要做的就是打开时空之门回去10年前的研究所";
	plot[16].picture1 = BOY_NORMAL;
	plot[16].picture2 = GIRL_NORMAL;
	plot[16].end_flag = false;

	plot[17].id = 17;
	plot[17].speaker = GIRL;
	plot[17].content = "阻止他…奶茶真好喝";
	plot[17].picture1 = BOY_NORMAL;
	plot[17].picture2 = GIRL_HAPPY;
	plot[17].end_flag = false;

	plot[18].id = 18;
	plot[18].speaker = BOY;
	plot[18].content = "欸…你怎么还喝着奶茶";
	plot[18].picture1 = BOY_SCARED;
	plot[18].picture2 = GIRL_HAPPY;
	plot[18].end_flag = true;

	plot[19].id = 19;
	plot[19].speaker = BOY;
	plot[19].content = "研究所里面已经出现了僵尸，还是晚了一步吗";
	plot[19].picture1 = BOY_NERVOUS;
	plot[19].picture2 = GIRL_NORMAL;
	plot[19].end_flag;

	plot[20].id = 20;
	plot[20].speaker = GIRL;
	plot[20].content = "其实僵尸还没有出研究所呀，应该说来得刚刚好吧";
	plot[19].picture1 = BOY_NERVOUS;
	plot[19].picture2 = GIRL_NORMAL;
	plot[19].end_flag = false;
	
	plot[21].id = 21;
	plot[21].speaker = BOY;
	plot[21].content = "这么说来只用解决研究所的问题是吗";
	plot[21].picture1 = BOY_NORMAL;
	plot[21].picture2 = GIRL_HAPPY;
	plot[21].end_flag = true;
}

/**
 * 展示剧情 与GameRun同级的函数
 */
void playPlots( float fDeltaTime ){

	// 检测节点是否跳转，是则更换展示内容
	if( current_plot != currnet_plot_ensure){

		currnet_plot_ensure = current_plot;

		// 先让显示剧情的精灵可见
		dSetSpriteVisible("boy",1);
		dSetSpriteVisible("girl",1);
		dSetSpriteVisible("panel",1);
		dSetSpriteVisible("speaker",1);
		dSetSpriteVisible("content",1);

		// 设置click_flag 避免出现剧情错误被跳过
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

	// 检测是否点击屏幕，点击了则跳转节点
	if( click_flag == true){
		click_flag = false;
		// 检测当前节点是否为这段剧情的最后一个节点
		if( plot[current_plot].end_flag ){
			current_plot++;
			// 做一个退出特效
			dCloneSprite("boy","boy_show");
			dCloneSprite("girl","girl_show");
			dCloneSprite("panel","panel_show");
			dSetSpriteLinearVelocityX("boy_show",-90);
			dSetSpriteLinearVelocityX("girl_show",90);
			dSetSpriteLinearVelocityY("panel_show",80);
			dSetSpriteLifeTime("boy_show",3);
			dSetSpriteLifeTime("girl_show",3);
			dSetSpriteLifeTime("panel_show",3);

			// 播放结束让显示剧情的精灵不可见
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
 * 将精灵变成红色调
 */
void turnRed( char *szName ){
	dSetSpriteColorBlue( szName, 0 );
	dSetSpriteColorRed( szName, 255 );
	dSetSpriteColorGreen( szName, 0 );
}


/**
 * 将精灵变成正常色调
 */
void turnWhite( char *szName ){
	dSetSpriteColorBlue( szName, 255 );
	dSetSpriteColorRed( szName, 255 );
	dSetSpriteColorGreen( szName, 255 );
}
/**
 * 将精灵变成灰色调
 */
void turnGray( char *szName ){
	dSetSpriteColorBlue( szName, 150 );
	dSetSpriteColorRed( szName, 150 );
	dSetSpriteColorGreen( szName, 150 );
}
