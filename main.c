#include "sdl2game.h"

#define HERO          0
#define ENEMY_0       1
#define ENEMY_1       2
#define ENEMY_2       3
#define BULLET_0      4
#define BULLET_1      5
#define BULLET_2      6
#define PROP_0        7
#define PROP_1        8

#define USE_BULLET_0  0
#define USE_BULLET_1  1
#define USE_BULLET_2  2
#define OUT_PROP      3
#define GET_PROP_0    4
#define GET_PROP_1    5
#define ENEMY_DOWN_0  6
#define ENEMY_DOWN_1  7
#define ENEMY_DOWN_2  8
#define BIG_SHIP      9
#define ACHIEVEMENT   10
#define GAME_OVER     11
#define BUTTON        12

#define CRASH         10
#define HIT_0         1
#define HIT_1         2
#define HIT_2         3
#define BEAT_ENEMY_0  100
#define BEAT_ENEMY_1  300
#define BEAT_ENEMY_2  1000

#define FRAMES        12
#define RECORD_FRAMES 100
#define BGSPEED       8
#define MOVE_SPAN     80
#define POWER_POINT   50
#define BULLETS_MAX   50
#define AIRCRAFTS_MAX 8


int keycode=0;
int powerPoint=0;
int frames=FRAMES;
int recordFrames=0;
int record=0,score=0;
int heroBulletType=BULLET_0;
float bgY=0,bgSpeed=BGSPEED;

Sound* sound[15];
Image* heroDown[4];
Image* enemy0Down[4];
Image* enemy1Down[4];
Image* enemy2Down[6];
Image *pause,*resume,*lifePoint;
Image *gameOver,*next;
Image *background,*gameTitle,*restartGame,*exitGame;
Image *hero,*enemy0,*enemy1,*enemy2;
Image *bullet0,*bullet1,*bullet2;
Image *prop0,*prop1;
Image* items[9];

float speed[9]={20,3,2,1,10,10,10,1,1};
int width[9]={50,40,50,82,11,6,6,30,30};
int height[9]={60,50,70,123,11,15,15,50,44};

Bool newRound=FALSE;
Bool mouseMoved=FALSE;
Bool gameStarting=FALSE;
Bool pp0=FALSE;pp1=FALSE;
Bool biu=FALSE,xiu=FALSE;
Bool fk0=FALSE,fk1=FALSE,fk2=FALSE;
Bool onHomePage=TRUE,onPlayPage=FALSE,onPausePage=FALSE,onOverPage=FALSE;

Thread *enemy0Pop=NULL,*enemy1Pop=NULL,*enemy2Pop=NULL;
Thread *heroBulletPop=NULL,*enemyBulletPop=NULL;
Thread *propPop=NULL;

typedef struct item{
    int type;
    int height;
    int width;
    float x;
    float y;
    float ox;
    float oy;
    float speed;
    int frames;
    int hit;
    const struct item* metLast;
}Item;
Item* aircrafts[AIRCRAFTS_MAX];
Item* bullets[BULLETS_MAX];

Item* newItem(int itemType,float initialX,float initialY,Bool moveUpward);
Bool colliding(Item* item1,Item* item2);
void updateAtColliding();
void drawByHit(Item** item);
void drawHero();
void drawOthers();
void drawBullets();
void drawLifePoint();
void propPopFunc();
void enemy0PopFunc();
void enemy1PopFunc();
void enemy2PopFunc();
void heroBulletPopFunc();
void enemyBulletPopFunc();
void freeItem(Item** item);

// 初始化
void setup() {
    size(480,680);
    title("飞机大战");
    bgmusic("sound/bgmusic.mp3");
    background=loadimage("images/bg0.png");
    gameTitle=loadimage("images/title.png");
    restartGame=loadimage("images/restart.png");
    exitGame=loadimage("images/exit.png");

    items[0]=hero=loadimage("images/hero.png");
    items[1]=enemy0=loadimage("images/enemy0.png");
    items[2]=enemy1=loadimage("images/enemy1.png");
    items[3]=enemy2=loadimage("images/enemy2.png");
    items[4]=bullet0=loadimage("images/bullet0.png");
    items[5]=bullet1=loadimage("images/bullet1.png");
    items[6]=bullet2=loadimage("images/bullet2.png");
    items[7]=prop0=loadimage("images/prop0.png");
    items[8]=prop1=loadimage("images/prop1.png");

    heroDown[0]=loadimage("images/hero_down1.png");
    heroDown[1]=loadimage("images/hero_down2.png");
    heroDown[2]=loadimage("images/hero_down3.png");
    heroDown[3]=loadimage("images/hero_down4.png");

    enemy0Down[0]=loadimage("images/enemy0_down1.png");
    enemy0Down[1]=loadimage("images/enemy0_down2.png");
    enemy0Down[2]=loadimage("images/enemy0_down3.png");
    enemy0Down[3]=loadimage("images/enemy0_down4.png");

    enemy1Down[0]=loadimage("images/enemy1_down1.png");
    enemy1Down[1]=loadimage("images/enemy1_down2.png");
    enemy1Down[2]=loadimage("images/enemy1_down3.png");
    enemy1Down[3]=loadimage("images/enemy1_down4.png");

    enemy2Down[0]=loadimage("images/enemy2_down1.png");
    enemy2Down[1]=loadimage("images/enemy2_down2.png");
    enemy2Down[2]=loadimage("images/enemy2_down3.png");
    enemy2Down[3]=loadimage("images/enemy2_down4.png");
    enemy2Down[4]=loadimage("images/enemy2_down5.png");
    enemy2Down[5]=loadimage("images/enemy2_down6.png");

    pause=loadimage("images/pause.png");
    resume=loadimage("images/resume.png");
    lifePoint=loadimage("images/life_point.png");

    gameOver=loadimage("images/game_over.png");
    next=loadimage("images/next.png");

    sound[0]=loadsound("sound/use_bullet0.mp3");
    sound[1]=loadsound("sound/use_bullet1.mp3");
    sound[2]=loadsound("sound/use_bullet2.mp3");
    sound[3]=loadsound("sound/out_prop.mp3");
    sound[4]=loadsound("sound/get_prop0.mp3");
    sound[5]=loadsound("sound/get_prop1.mp3");
    sound[6]=loadsound("sound/enemy0_down.mp3");
    sound[7]=loadsound("sound/enemy1_down.mp3");
    sound[8]=loadsound("sound/enemy2_down.mp3");
    sound[9]=loadsound("sound/big_ship.mp3");
    sound[10]=loadsound("sound/achievement.mp3");
    sound[11]=loadsound("sound/game_over.mp3");
    sound[12]=loadsound("sound/button.mp3");

    if(!enemy0Pop) thread(enemy0PopFunc,(void*)NULL);
    if(!enemy1Pop) thread(enemy1PopFunc,(void*)NULL);
    if(!enemy2Pop) thread(enemy2PopFunc,(void*)NULL);
    if(!propPop) thread(propPopFunc,(void*)NULL);
    if(!heroBulletPop) thread(heroBulletPopFunc,(void*)NULL);
    if(!enemyBulletPop) thread(enemyBulletPopFunc,(void*)NULL);

    aircrafts[HERO]=newItem(HERO,400,550,FALSE);
}

// 界面绘图
void draw(float stateTime) {
    int i;
    srand((unsigned)timetick());
    if(!onOverPage){
        if(bgY>=0) imagezoomed(background,0,-680+bgY,480,680);
        imagezoomed(background,0,bgY,480,680);
        bgY=(int)(bgY+bgSpeed) % 680;
    }

    if(onHomePage){
        score=0;
        heroBulletType=BULLET_0;
        for(i=1;i<AIRCRAFTS_MAX;i++) freeItem(&aircrafts[i]);
        for(i=0;i<BULLETS_MAX;i++) freeItem(&bullets[i]);
        if(!gameStarting){
            aircrafts[HERO]=newItem(HERO,400,550,FALSE);
            image(gameTitle,25,200);
            image(restartGame,184,400);
            image(exitGame,185,500);
            textsize(16);
            text("@Ray",426,670,0,0,0);
        }else{
            if(aircrafts[HERO]->x>=190) aircrafts[HERO]->x-=5;
            if(aircrafts[HERO]->y>=335) aircrafts[HERO]->y-=5;
            if(mouseMoved||(aircrafts[HERO]->x<=((480-width[HERO])/2)&&aircrafts[HERO]->y<=400)){
                mouseMoved=FALSE;
                gameStarting=FALSE;
                onHomePage=FALSE;
                onPlayPage=TRUE;
            }
        }
        drawHero();
    }

    if(onPlayPage){
        score++;
        char info[20];
        bgSpeed=BGSPEED;
        updateAtColliding();
        drawHero();
        drawOthers();
        drawBullets();
        image(pause,20,20);
        drawLifePoint();
        itoa(score,info,10);
        textsize(20);
        text(info,420,60,0,0,0);
        if(score>=record&&record!=0&&newRound) {
                newRound=FALSE;
                recordFrames=RECORD_FRAMES;
                playsound(sound[ACHIEVEMENT]);
        }
        if(recordFrames!=0){
                text("新纪录！",350,60,0,0,0);
                recordFrames--;
        }
    }

    if(onPausePage){
        char info[20];
        bgSpeed=0;
        for(i=0;i<AIRCRAFTS_MAX;i++)
            if(aircrafts[i]&&aircrafts[i]->hit!=0) aircrafts[i]->frames--;
        drawHero();
        drawOthers();
        drawBullets();
        image(resume,20,20);
        image(next,390,522);
        image(restartGame,335,572);
        image(exitGame,335,622);
        drawLifePoint();
        textsize(20);
        itoa(score,info,10);
        text(info,420,60,0,0,0);
        if(recordFrames!=0) text("新纪录！",350,60,0,0,0);
    }

    if(onOverPage){
        char info[20];
        record=record>score? record:score;
        imagezoomed(gameOver,0,0,480,680);
        imagezoomed(next,210,600,60,30);
        textsize(50);
        itoa(record,info,10);
        text(info,240,310,0,0,0);
        textsize(40);
        itoa(score,info,10);
        text(info,240,550,0,0,0);
    }
}

// 鼠标事件处理
void mousePress() {
    int i;
    if(onHomePage){
        if(inbound(mouseX,mouseY,185,500,185+110,500+28)) {
            playsound(sound[BUTTON]);
            exit(0);
        }
        if(inbound(mouseX,mouseY,185,400,185+110,400+28)) {
            gameStarting=TRUE;
            newRound=TRUE;
            playsound(sound[BUTTON]);
        }
    }
    if((onPausePage||onPlayPage)&&
       inbound(mouseX,mouseY,20,20,20+42,20+45)){
        onPausePage=!onPausePage;
        onPlayPage=!onPlayPage;
        playsound(sound[BUTTON]);
    }
    if(onPausePage){
        if(inbound(mouseX,mouseY,390,522,390+55,522+25)){
            onPausePage=FALSE;
            onPlayPage=TRUE;
            playsound(sound[BUTTON]);
        }
        if(inbound(mouseX,mouseY,335,572,335+110,572+28)){
            onPausePage=FALSE;
            onPlayPage=TRUE;
            score=0;
            newRound=TRUE;
            heroBulletType=BULLET_0;
            aircrafts[HERO]->x=((480-width[HERO])/2);aircrafts[HERO]->y=400;
            for(i=1;i<AIRCRAFTS_MAX;i++) freeItem(&aircrafts[i]);
            for(i=0;i<BULLETS_MAX;i++) freeItem(&bullets[i]);
            playsound(sound[BUTTON]);
        }
        if(inbound(mouseX,mouseY,335,622,335+110,622+28)){
            playsound(sound[BUTTON]);
            exit(0);
        }
    }
    if(onOverPage){
        if(inbound(mouseX,mouseY,210,600,210+60,600+30)){
            onOverPage=FALSE;
            onHomePage=TRUE;
            playsound(sound[BUTTON]);
        }
    }
}

void mouseMove() {
    if(gameStarting) mouseMoved=TRUE;
    if(onHomePage){
        if(inbound(mouseX,mouseY,185,500,185+110,500+28)||
           inbound(mouseX,mouseY,185,400,185+110,400+28))
            cursor("images/pointer.png");
        else cursor("images/arrow.png");
    }
    if(onPlayPage){
        if(inbound(mouseX,mouseY,20,20,20+42,20+45))
            cursor("images/pointer.png");
        else cursor("images/crossing.png");
        if(inbound(mouseX,mouseY,0,150,480,680)){
            aircrafts[HERO]->x=mouseX-(aircrafts[HERO]->width-16)/2;
            aircrafts[HERO]->y=mouseY-aircrafts[HERO]->height/3;
        }
    }
    if(onPausePage){
        if(inbound(mouseX,mouseY,20,20,20+42,20+45)||
           inbound(mouseX,mouseY,390,522,390+55,522+25)||
           inbound(mouseX,mouseY,335,572,335+110,572+28)||
           inbound(mouseX,mouseY,335,622,335+110,622+28))
            cursor("images/pointer.png");
        else cursor("images/arrow.png");
    }
    if(onOverPage){
        if(inbound(mouseX,mouseY,210,600,210+60,600+30))
            cursor("images/pointer.png");
        else cursor("images/arrow.png");
    }
}

void mouseRelease() {
}

// 键盘事件处理函数
void keyDown() {
    int i;
    if(onHomePage){
        if(key==KEY_ENTER) {
                gameStarting=TRUE;
                onPlayPage=TRUE;
                newRound=TRUE;
                playsound(sound[BUTTON]);
        }
    }
    if(onPlayPage){
        aircrafts[HERO]->ox=aircrafts[HERO]->x;
        aircrafts[HERO]->oy=aircrafts[HERO]->y;
        if(key==KEY_LEFT) keycode=1;
        if(key==KEY_RIGHT) keycode=2;
        if(key==KEY_UP) keycode=3;
        if(key==KEY_DOWN)keycode=4;
    }
    if((onPausePage||onPlayPage)&&key==KEY_SPACE){
        onPausePage=!onPausePage;
        onPlayPage=!onPlayPage;
        playsound(sound[BUTTON]);
    }
    if(onPausePage){
        if(key==KEY_ENTER){
            onPausePage=FALSE;
            onPlayPage=TRUE;
            playsound(sound[BUTTON]);
        }
        if(key==KEY_TAB){
            onPausePage=FALSE;
            onPlayPage=TRUE;
            score=0;
            newRound=TRUE;
            heroBulletType=BULLET_0;
            aircrafts[HERO]->x=((480-width[HERO])/2);aircrafts[HERO]->y=400;
            for(i=1;i<AIRCRAFTS_MAX;i++) freeItem(&aircrafts[i]);
            for(i=0;i<BULLETS_MAX;i++) freeItem(&bullets[i]);
            playsound(sound[BUTTON]);
        }
    }
    if(onOverPage&&key==KEY_ENTER){
        onOverPage=FALSE;
        onHomePage=TRUE;
        playsound(sound[BUTTON]);
    }
}

void keyUp() {
}

// 程序结束时回收游戏资源
void close() {
    unloadimage(background);
    unloadimage(title);
    unloadimage(restartGame);
    unloadimage(exitGame);

    unloadimage(hero);
    unloadimage(enemy0);
    unloadimage(enemy1);
    unloadimage(enemy2);

    unloadimage(pause);
    unloadimage(resume);
    unloadimage(lifePoint);

    if(enemy0Pop) waitfor(enemy0Pop);
    if(enemy1Pop) waitfor(enemy1Pop);
    if(enemy2Pop) waitfor(enemy2Pop);
    if(heroBulletPop) waitfor(heroBulletPop);
    if(enemyBulletPop) waitfor(enemyBulletPop);
    if(propPop) waitfor(propPop);
}

Item* newItem(int itemType,float initialX,float initialY,Bool moveUpward){
    Item* item=(Item*)malloc(sizeof(Item));
    item->x=initialX;
    item->y=initialY;
    item->ox=initialX;
    item->oy=initialY;
    item->width=width[itemType];
    item->height=height[itemType];
    item->type=itemType;
    item->hit=0;
    item->frames=0;
    item->metLast=NULL;
    if(moveUpward) item->speed=-speed[itemType];
    else item->speed=speed[itemType];
    return item;
}

Bool colliding(Item* item1,Item* item2){
    float item1X=item1->x+(item1->width/2);
    float item1Y=item1->y+(item1->height/2);
    float item2X=item2->x+(item2->width/2);
    float item2Y=item2->y+(item2->height/2);
    float spanX=(item1->width+item2->width)/2;
    float spanY=(item1->height+item2->height)/2;
    return abs(item1X-item2X)<spanX&&abs(item1Y-item2Y)<spanY;
}

void updateAtColliding(){
    int i,j;
    for(i=0;i<=0;i++){
        if(aircrafts[i]){
            for(j=i+1;j<AIRCRAFTS_MAX;j++){
                if(aircrafts[j]){
                    if(colliding(aircrafts[i],aircrafts[j])){
                        if(i==HERO&&aircrafts[j]->type==PROP_0){
                            heroBulletType=BULLET_1;
                            powerPoint=POWER_POINT;
                            playsound(sound[GET_PROP_0]);
                            freeItem(&aircrafts[j]);
                            continue;
                        }
                        if(i==HERO&&aircrafts[j]->type==PROP_1){
                            heroBulletType=BULLET_2;
                            powerPoint=POWER_POINT;
                            playsound(sound[GET_PROP_1]);
                            freeItem(&aircrafts[j]);
                            continue;
                        }
                        if(aircrafts[i]->metLast!=aircrafts[j]){
                            aircrafts[i]->metLast=aircrafts[j];
                            aircrafts[i]->hit+=CRASH;
                            aircrafts[i]->frames=0;
                        }
                        if(aircrafts[j]->metLast!=aircrafts[i]){
                            aircrafts[j]->metLast=aircrafts[i];
                            aircrafts[j]->hit+=CRASH;
                            aircrafts[j]->frames=0;
                        }
                    }
                }
            }
        }
    }
    for(i=0;i<BULLETS_MAX;i++){
        if(bullets[i]){
            for(j=0;j<AIRCRAFTS_MAX;j++){
                if(aircrafts[j]){
                    if(colliding(bullets[i],aircrafts[j])){
                        if(aircrafts[j]->type==PROP_0||aircrafts[j]->type==PROP_1) continue;
                        aircrafts[j]->frames=0;
                        if(bullets[i]->type==BULLET_0) aircrafts[j]->hit+=HIT_0;
                        if(bullets[i]->type==BULLET_1) aircrafts[j]->hit+=HIT_1;
                        if(bullets[i]->type==BULLET_2) aircrafts[j]->hit+=HIT_2;
                        freeItem(&bullets[i]);
                        break;
                    }
                }
            }
        }
    }
}

void drawByHit(Item** item){
    switch ((*item)->type){
        case HERO:
            if((*item)->hit==0) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                            (*item)->width,(*item)->height);
            if((*item)->hit>=1 && (*item)->hit<=40){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(heroDown[0],(*item)->x,(*item)->y,
                                                         (*item)->width, (*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=41 && (*item)->hit<=70){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(heroDown[1],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=71 && (*item)->hit<=79){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                        (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(heroDown[2],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=80){
                if((*item)->frames<frames/3) imagezoomed(heroDown[3],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames>=frames/3) {
                    onPlayPage=FALSE;
                    onOverPage=TRUE;
                    playtimedsound(sound[GAME_OVER],1400);
                    freeItem(item);
                }else (*item)->frames++;
            }
        break;
        case ENEMY_0:
            if((*item)->hit==0) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                            (*item)->width,(*item)->height);
            if((*item)->hit==1){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(enemy0Down[0],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit==2){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(enemy0Down[1],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit==3){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(enemy0Down[2],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=4){
                if((*item)->frames<frames/3) imagezoomed(enemy0Down[3],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames>=frames/3) {
                    score+=BEAT_ENEMY_0;
                    playsound(sound[ENEMY_DOWN_0]);
                    freeItem(item);
                }else (*item)->frames++;
            }
        break;
        case ENEMY_1:
            if((*item)->hit==0) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                            (*item)->width,(*item)->height);
            if((*item)->hit>=1 && (*item)->hit<=6){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(enemy1Down[0],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=7 && (*item)->hit<=11){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(enemy1Down[1],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=12 && (*item)->hit<=15){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(enemy1Down[2],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=16){
                if((*item)->frames<frames/3) imagezoomed(enemy1Down[3],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames>=frames/3) {
                    score+=BEAT_ENEMY_1;
                    playsound(sound[ENEMY_DOWN_1]);
                    freeItem(item);
                }else (*item)->frames++;
            }
        break;
        case ENEMY_2:
            if((*item)->hit==0) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                            (*item)->width,(*item)->height);
            if((*item)->hit>=1 && (*item)->hit<=25){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(enemy2Down[0],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=26 && (*item)->hit<=45){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(enemy2Down[1],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=46 && (*item)->hit<=60){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(enemy2Down[2],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=61 && (*item)->hit<=70){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(enemy2Down[3],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=71 && (*item)->hit<=75){
                if((*item)->frames>frames/2) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames<=frames/2) imagezoomed(enemy2Down[4],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                (*item)->frames= (*item)->frames==frames ? 0 : (*item)->frames+1;
            }
            if((*item)->hit>=76){
                if((*item)->frames<frames/3) imagezoomed(enemy2Down[5],(*item)->x,(*item)->y,
                                                         (*item)->width,(*item)->height);
                if((*item)->frames>=frames/3) {
                    score+=BEAT_ENEMY_2;
                    playsound(sound[ENEMY_DOWN_2]);
                    freeItem(item);
                }else (*item)->frames++;
            }
        break;
        case PROP_0:
            if((*item)->hit==0) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                            (*item)->width,(*item)->height);
            else freeItem(item);
        break;
        case PROP_1:
            if((*item)->hit==0) imagezoomed(items[(*item)->type],(*item)->x,(*item)->y,
                                            (*item)->width,(*item)->height);
            else freeItem(item);
        break;
        default:
        break;
    }
}

void drawHero(){
    if(!aircrafts[0]) return;
    drawByHit(&aircrafts[0]);
    if(onPausePage) return;
    switch (keycode){
        case 1: if(aircrafts[HERO]->ox-aircrafts[HERO]->x<=MOVE_SPAN){
            if(aircrafts[HERO]->x<=-aircrafts[HERO]->width/2) aircrafts[HERO]->speed=0;
            else aircrafts[HERO]->speed=speed[aircrafts[HERO]->type];
            aircrafts[HERO]->x-=aircrafts[HERO]->speed;
            keycode=0;
        }
        break;
        case 2: if(aircrafts[HERO]->x-aircrafts[HERO]->ox<=MOVE_SPAN){
            if(aircrafts[HERO]->x>=(480-aircrafts[HERO]->width/2)) aircrafts[HERO]->speed=0;
            else aircrafts[HERO]->speed=speed[aircrafts[HERO]->type];
            aircrafts[HERO]->x+=aircrafts[HERO]->speed;
            keycode=0;
        }
        break;
        case 3: if(aircrafts[HERO]->oy-aircrafts[HERO]->y<=MOVE_SPAN){
            if(aircrafts[HERO]->y<=150) aircrafts[HERO]->speed=0;
            else aircrafts[HERO]->speed=speed[aircrafts[HERO]->type];
            aircrafts[HERO]->y-=aircrafts[HERO]->speed;
            keycode=0;
        }
        break;
        case 4: if(aircrafts[HERO]->y-aircrafts[HERO]->oy<=MOVE_SPAN){
            if(aircrafts[HERO]->y>=(680-aircrafts[HERO]->height/2)) aircrafts[HERO]->speed=0;
            else aircrafts[HERO]->speed=speed[aircrafts[HERO]->type];
            aircrafts[HERO]->y+=aircrafts[HERO]->speed;
            keycode=0;
        }
        break;
        default:
        break;
    }
}

void drawOthers(){
    int i;
    for(i=1;i<AIRCRAFTS_MAX;i++) if(aircrafts[i]) drawByHit(&aircrafts[i]);
    if(onPausePage) return;
    for(i=1;i<AIRCRAFTS_MAX;i++) if(aircrafts[i] && aircrafts[i]->y>680) freeItem(&aircrafts[i]);
    if(fk0) {
        for(i=1;i<AIRCRAFTS_MAX;i++){
            if(!aircrafts[i]) {
                float x=(rand() % (395-0+1))+ 0;
                float y=(rand() % (200-100+1))+ 100;
                aircrafts[i]=newItem(ENEMY_0,x,-y,FALSE);
                break;
            }
        }
        fk0=!fk0;
    }
    if(fk1) {
        for(i=1;i<AIRCRAFTS_MAX;i++){
            if(!aircrafts[i]) {
                float x=(rand() % (395-0+1))+ 0;
                float y=(rand() % (200-100+1))+ 100;
                aircrafts[i]=newItem(ENEMY_1,x,-y,FALSE);
                break;
            }
        }
        fk1=!fk1;
    }
    if(fk2) {
        Bool hasEnemy2=FALSE;
        for(i=1;i<AIRCRAFTS_MAX;i++)
            if(aircrafts[i]&&aircrafts[i]->type==ENEMY_2) {
                hasEnemy2=TRUE;
                break;
            }
        if(!hasEnemy2){
            for(i=1;i<AIRCRAFTS_MAX;i++){
                if(!aircrafts[i]) {
                    float x=(rand() % (395-0+1))+ 0;
                    float y=(rand() % (200-100+1))+ 100;
                    aircrafts[i]=newItem(ENEMY_2,x,-y,FALSE);
                    break;
                }
            }
        }
        fk2=!fk2;
    }
    if(pp0) {
        for(i=1;i<AIRCRAFTS_MAX;i++){
            if(!aircrafts[i]) {
                float x=(rand() % (395-0+1))+ 0;
                float y=(rand() % (200-100+1))+ 100;
                aircrafts[i]=newItem(PROP_0,x,-y,FALSE);
                break;
            }
        }
        pp0=!pp0;
    }
    if(pp1) {
        for(i=1;i<AIRCRAFTS_MAX;i++){
            if(!aircrafts[i]) {
                float x=(rand() % (395-0+1))+ 0;
                float y=(rand() % (200-100+1))+ 100;
                aircrafts[i]=newItem(PROP_1,x,-y,FALSE);
                break;
            }
        }
        pp1=!pp1;
    }
    for(i=1;i<AIRCRAFTS_MAX;i++){
        if(aircrafts[i]) {
            aircrafts[i]->y+=speed[aircrafts[i]->type];
            if(aircrafts[i]->type==ENEMY_2){
                aircrafts[i]->y=aircrafts[i]->y>20 ? 20 : aircrafts[i]->y;
                aircrafts[i]->x+=aircrafts[i]->speed;
                if(aircrafts[i]->x>=395) aircrafts[i]->speed=-speed[aircrafts[i]->type];
                if(aircrafts[i]->x<=0) aircrafts[i]->speed=speed[aircrafts[i]->type];
                playsound(sound[BIG_SHIP]);
            }
        }
    }
}

void drawBullets(){
    int i,j;
    for(i=0;i<BULLETS_MAX;i++)
        if(bullets[i])
            imagezoomed(items[bullets[i]->type],bullets[i]->x,bullets[i]->y,bullets[i]->width,bullets[i]->height);
    if(onPausePage) return;
    for(i=0;i<BULLETS_MAX;i++) if(bullets[i] && (bullets[i]->y<0 || bullets[i]->y>680)) freeItem(&bullets[i]);
    if(biu) {
        for(i=0;i<AIRCRAFTS_MAX;i++){
            if(aircrafts[i]&&aircrafts[i]->type==HERO){
                powerPoint--;
                if(powerPoint==0){
                        heroBulletType=BULLET_0;
                        playsound(sound[OUT_PROP]);
                }
                if(heroBulletType==BULLET_0||heroBulletType==BULLET_1){
                    for(j=0;j<BULLETS_MAX;j++){
                        if(!bullets[j]) {
                            float bulletX=aircrafts[HERO]->x+(aircrafts[HERO]->width-width[heroBulletType])/2;
                            float bulletY=aircrafts[HERO]->y-height[heroBulletType]-1;
                            bullets[j]=newItem(heroBulletType,bulletX,bulletY,TRUE);
                            if(heroBulletType==BULLET_0) playtimedsound(sound[USE_BULLET_0],150);
                            if(heroBulletType==BULLET_1) playtimedsound(sound[USE_BULLET_1],200);
                            break;
                        }
                    }
                }else{
                    for(j=0;j<BULLETS_MAX;j++){
                        if(!bullets[j]) {
                            float bulletX=aircrafts[i]->x+(aircrafts[i]->width-2*width[BULLET_2])/6;
                            float bulletY=aircrafts[i]->y-height[heroBulletType]-1;
                            bullets[j]=newItem(BULLET_2,bulletX,bulletY,TRUE);
                            playtimedsound(sound[USE_BULLET_2],250);
                            break;
                        }
                    }
                    for(j=0;j<BULLETS_MAX;j++){
                        if(!bullets[j]) {
                            float bulletX=aircrafts[i]->x+5*(aircrafts[i]->width-2*width[BULLET_2])/6+width[BULLET_2];
                            float bulletY=aircrafts[i]->y-height[heroBulletType]-1;
                            bullets[j]=newItem(BULLET_2,bulletX,bulletY,TRUE);
                            playtimedsound(sound[USE_BULLET_2],250);
                            break;
                        }
                    }
                }
            }
        }
        biu=!biu;
    }
    if(xiu) {
        for(i=0;i<AIRCRAFTS_MAX;i++){
            if(aircrafts[i]){
               if(aircrafts[i]->type==ENEMY_0){
                    for(j=0;j<BULLETS_MAX;j++){
                        if(!bullets[j]) {
                            float bulletX=aircrafts[i]->x+(aircrafts[i]->width-width[BULLET_0])/2;
                            float bulletY=aircrafts[i]->y+height[aircrafts[i]->type]+1;
                            bullets[j]=newItem(BULLET_0,bulletX,bulletY,FALSE);
                            playtimedsound(sound[USE_BULLET_0],150);
                            break;
                        }
                    }
                }
                if(aircrafts[i]->type==ENEMY_1){
                    for(j=0;j<BULLETS_MAX;j++){
                        if(!bullets[j]) {
                            float bulletX=aircrafts[i]->x+(aircrafts[i]->width-width[BULLET_1])/2;
                            float bulletY=aircrafts[i]->y+height[aircrafts[i]->type]+1;
                            bullets[j]=newItem(BULLET_1,bulletX,bulletY,FALSE);
                            playtimedsound(sound[USE_BULLET_1],150);
                            break;
                        }
                    }
                }
                if(aircrafts[i]->type==ENEMY_2){
                    for(j=0;j<BULLETS_MAX;j++){
                        if(!bullets[j]) {
                            float bulletX=aircrafts[i]->x+(aircrafts[i]->width-2*width[BULLET_2])/6;
                            float bulletY=aircrafts[i]->y+height[aircrafts[i]->type]+1;
                            bullets[j]=newItem(BULLET_2,bulletX,bulletY,FALSE);
                            playtimedsound(sound[USE_BULLET_2],150);
                            break;
                        }
                    }
                    for(j=0;j<BULLETS_MAX;j++){
                        if(!bullets[j]) {
                            float bulletX=aircrafts[i]->x+5*(aircrafts[i]->width-2*width[BULLET_2])/6+width[BULLET_2];
                            float bulletY=aircrafts[i]->y+height[aircrafts[i]->type]+1;
                            bullets[j]=newItem(BULLET_2,bulletX,bulletY,FALSE);
                            playtimedsound(sound[USE_BULLET_2],150);
                            break;
                        }
                    }
                }
            }
        }
        xiu=!xiu;
    }
    for(i=0;i<BULLETS_MAX;i++) if(bullets[i]) bullets[i]->y+=bullets[i]->speed;
}

void drawLifePoint(){
    int i,pointNum;
    if(!aircrafts[HERO]) return;
    int point=80-aircrafts[HERO]->hit;
    if(point<=0)             pointNum=0;
    if(point>=1 &&point<=10) pointNum=1;
    if(point>=11&&point<=20) pointNum=2;
    if(point>=21&&point<=30) pointNum=3;
    if(point>=31&&point<=40) pointNum=4;
    if(point>=41&&point<=50) pointNum=5;
    if(point>=51&&point<=60) pointNum=6;
    if(point>=61&&point<=70) pointNum=7;
    if(point>=71&&point<=80) pointNum=8;
    for(i=0;i<pointNum;i++){
        int x=20+i*20;
        imagezoomed(lifePoint,x,630,10,20);
    }
}

void propPopFunc(){
    while(1){
        int t;
        t=(rand() % (15000-10000))+ 10000;
        delay(t);
        pp0=!pp0;
        t=(rand() % (15000-10000))+ 10000;
        delay(t);
        pp1=!pp1;
    }
}

void enemy0PopFunc(){
    while(1){
        int t=(rand() % (4000-2000))+ 2000;
        delay(t);
        fk0=!fk0;
    }
}

void enemy1PopFunc(){
    while(1){
        int t=(rand() % (6000-3000))+ 3000;
        delay(t);
        fk1=!fk1;
    }
}

void enemy2PopFunc(){
    while(1){
        int t=(rand() % (16000-120000))+ 12000;
        delay(t);
        fk2=!fk2;
    }
}

void heroBulletPopFunc(){
    while(1){
        biu=!biu;
        if(heroBulletType==BULLET_0)delay(150);
        if(heroBulletType==BULLET_1)delay(200);
        if(heroBulletType==BULLET_2)delay(250);
    }
}

void enemyBulletPopFunc(){
    while(1){
        xiu=!xiu;
        delay(150);
    }
}

void freeItem(Item** item){
    free(*item);
    *item=NULL;
}