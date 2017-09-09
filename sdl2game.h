
/**
 * Copyright by lixinhui from wanke001.com
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_thread.h>
#include <iconv.h>

#define Bool  unsigned char     
#define TRUE  1
#define FALSE 0
#define MAX_ANIM 200            
#define max(a,b) (a>b)?(a):(b)
#define min(a,b) (a>b)?(b):(a)
#define map(val, min1, max1, min2, max2) val*(max2-min2)/(max1-min1)
#define Color SDL_Color
#define ThreadFunction SDL_ThreadFunction
#define Thread         SDL_Thread
#define Mutex          SDL_mutex
#define rgb(r,g,b) {r, g, b, 0xff}
#define rgba(r,g,b,a) {r, g, b, a}


typedef SDL_Surface Bitmap;  
typedef SDL_Texture Image;   
typedef Mix_Chunk   Sound;   

typedef struct uSDL_Animation {
	Image** keyFrames;
	int frameNum;     
	float cx;         
	float cy;
	float duration;   
	float frameTime;  
	float t0;         
	int stopped;      
	int looping;      

	void (*draw)(struct uSDL_Animation* anim, float stateTime);

} Animation;	      


Uint16 _screenW = 400;         
Uint16 _screenH = 300;
SDL_Window* _window = NULL;    
SDL_Renderer* _canvas = NULL;  
Image* _bgColor = NULL;        
Image* _bgImage = NULL;        
Bool _clearBg = TRUE;          
Mix_Music* _bgMusic = NULL;    
SDL_Cursor* _cursor = NULL;	  

TTF_Font* _font = NULL;
char* _fontName = "font/DroidSansFallback.ttf";
int _fontSize = 16;
float _duration = 0.0;         
Animation* _animations[MAX_ANIM]; 
int _animCount = 0;

Bool mousePressed = FALSE;  
Bool mouseDragged = FALSE;  
Uint8 mouseButton;          
Sint32 mouseX = 0;          
Sint32 mouseY = 0;
Sint32 pmouseX = -1;        
Sint32 pmouseY = -1;
Bool keyPressed = FALSE;    
SDL_Keycode key;	 	    

#define KEY_A SDLK_a        
#define KEY_B SDLK_b
#define KEY_C SDLK_c
#define KEY_D SDLK_d
#define KEY_E SDLK_e
#define KEY_F SDLK_f
#define KEY_G SDLK_g
#define KEY_H SDLK_h
#define KEY_I SDLK_i
#define KEY_J SDLK_j
#define KEY_K SDLK_k
#define KEY_L SDLK_l
#define KEY_M SDLK_m
#define KEY_N SDLK_n
#define KEY_O SDLK_o
#define KEY_P SDLK_p
#define KEY_Q SDLK_q
#define KEY_R SDLK_r
#define KEY_S SDLK_s
#define KEY_T SDLK_t
#define KEY_U SDLK_u
#define KEY_V SDLK_v
#define KEY_W SDLK_w
#define KEY_X SDLK_x
#define KEY_Y SDLK_y
#define KEY_Z SDLK_z
#define KEY_SPACE  SDLK_SPACE  
#define KEY_ENTER  SDLK_RETURN 
#define KEY_RETURN SDLK_RETURN
#define KEY_TAB    SDLK_TAB    
#define KEY_ESC    SDLK_ESCAPE 
#define KEY_0 SDLK_0           
#define KEY_1 SDLK_1
#define KEY_2 SDLK_2
#define KEY_3 SDLK_3
#define KEY_4 SDLK_4
#define KEY_5 SDLK_5
#define KEY_6 SDLK_6
#define KEY_7 SDLK_7
#define KEY_8 SDLK_8
#define KEY_9 SDLK_9
#define KEY_UP    SDLK_UP      
#define KEY_DOWN  SDLK_DOWN    
#define KEY_LEFT  SDLK_LEFT    
#define KEY_RIGHT SDLK_RIGHT   

#define MOUSE_LEFT   SDL_BUTTON_LEFT   
#define MOUSE_MIDDLE SDL_BUTTON_MIDDLE 
#define MOUSE_RIGHT  SDL_BUTTON_RIGHT  



// -------- program framework --------
extern void setup();
extern void draw(float stateTime);
extern void mousePress();
extern void mouseMove();
extern void mouseRelease();
extern void keyDown();
extern void keyUp();
extern void close();



// -------- for developers --------
extern Animation* loadanim(float duration, float cx, float cy, char* frameFile, int row, int col, int looping);
extern Animation* loadanim2(float duration, float cx, float cy, char* frameFiles[], int frameNum, int looping);
extern void addanims(float duration, float cx, float cy, char* frameFile, int row, int col);
extern void addanims2(float duration, float cx, float cy, char* frameFiles[], int frameNum);
extern Sound* loadsound(char* fileName);
extern Image* loadimage(char *fileName);
extern Image* loadimagea(char *fileName, Uint8 alphaR, Uint8 alphaG, Uint8 alphaB);
extern Bitmap* loadbitmap(char *fileName);
extern Image* createimagearea(Bitmap* img, int x, int y, int w, int h);
extern Image* createimagescaled(Bitmap* img, int destW, int destH);
extern Bitmap* createbitmapscaled(Bitmap* img, int destW, int destH);
extern void sliceimage(Bitmap* img, int row, int col, Image* imgs[]);
extern void unloadimage(Image* img);
extern void unloadbitmap(Bitmap* bmp);
extern void unloadsound(Sound* sound);
extern void unloadanimation(Animation* anim);
extern void size(Uint16 width, Uint16 height);
extern void title(char *s);
extern void bgcolor(Uint8 r, Uint8 g, Uint8 b);
extern void bgimage(char *fileName);
extern void nobackground();
extern void bgmusic(char *fileName);
extern void cursor(char *fileName);
extern void cursora(char *fileName, Uint8 alphaR, Uint8 alphaG, Uint8 alphaB);
extern void textsize(int size);
extern void color(Uint8 r, Uint8 g, Uint8 b);
extern void colora(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
extern void point(int x, int y);
extern void line(int x0, int y0, int x1, int y1);
extern void rect(int x, int y, int w, int h);
extern void coloredrect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b);
extern void fillrect(int x, int y, int w, int h);
extern void coloredfillrect(int x, int y, int w, int h, Uint8 r0, Uint8 g0, Uint8 b0, Uint8 r1, Uint8 g1, Uint8 b1);
extern void circle(int x0, int y0, double r);
extern void semicircle(int x0, int y0, double r0,Uint8 r, Uint8 g, Uint8 b);
extern void fillsemicircle(int x0, int y0, int r);
extern void coloredcircle(int x0, int y0, double rr, Uint8 r, Uint8 g, Uint8 b);
extern void fillcircle(int x0, int y0, int r);
extern void text(char *s, int x, int y, Uint8 r, Uint8 g, Uint8 b);
extern void texta(char *s, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
extern void image(Image* img, int x, int y);
extern void imagea(Image* img, int x, int y, Uint8 alpha);
extern void imagezoomed(Image* img, int x, int y, int w, int h);
extern void imagerotated(Image* img, int x, int y, double angle);
extern void drawanim(Animation* anim, float stateTime);
extern void playsound(Sound* sound);
extern void playtimedsound(Sound* sound, int time);
extern Bool inbound( int x0, int y0, int x, int y,int w, int h);
extern Thread* thread(ThreadFunction func, void* data);
extern int waitfor(Thread* thread);
extern void delay(Uint32 ms);
extern Uint32 timetick();



Thread* thread(ThreadFunction func, void* data) {
	SDL_Thread* thread = SDL_CreateThread( func, "uThread", data );
	return thread;
}

int waitfor(Thread* thread) {
	int val;
	SDL_WaitThread( thread, &val );
	return val;
}

void delay(Uint32 ms) {
	SDL_Delay(ms);
}

Uint32 timetick() {
	return SDL_GetTicks();
}

Mutex* threadmutex() {
	return SDL_CreateMutex();
}

void lock(Mutex* mutex) {
	SDL_LockMutex(mutex);
}

void unlock(Mutex* mutex) {
    SDL_UnlockMutex(mutex);
}

void freemutex(Mutex* mutex) {
	SDL_DestroyMutex(mutex);
}

Bool inbound( int x0, int y0, int x, int y,int w, int h) {
	if (x0 >= x && x0 <= x + w && y0 >= y && y0 <= y + h) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

char* utf8(char* src) {
	int inlen = strlen(src);
  int outlen = inlen * 4;
  char *buf = (char*)malloc(outlen);
	
	char *inbuf = src;
	char *outbuf = buf;
	iconv_t cd; 
	char **pin = &inbuf;
	char **pout = &outbuf;
	
	cd = iconv_open("utf-8","gbk");
	memset(outbuf,0,outlen); 
	iconv(cd,pin,&inlen,pout,&outlen); 
	iconv_close(cd); 
	
	return buf;
}

Animation* loadanim(float duration, float cx, float cy, char* frameFile, int row, int col, int looping) {
	Animation *anim;
	anim = (Animation*) malloc(sizeof(Animation));

	int frameNum = row * col;

	anim->duration = duration;
	anim->cx = cx;
	anim->cy = cy;
	anim->frameNum = frameNum;
	anim->frameTime = duration / frameNum;
	anim->looping = looping;
	anim->stopped = FALSE;
	anim->t0 = -1;
	anim->draw = drawanim;


	int i;
	anim->keyFrames = (Image**) malloc(frameNum * sizeof(Image*));

	SDL_Surface *surface = IMG_Load(frameFile);
	sliceimage(surface, row, col, anim->keyFrames);
	SDL_FreeSurface(surface);

	return anim;
}

Animation* loadanim2(float duration, float cx, float cy, char* frameFiles[], int frameNum, int looping) {
	Animation *anim;
	anim = (Animation*) malloc(sizeof(Animation));

	anim->duration = duration;
	anim->cx = cx;
	anim->cy = cy;
	anim->frameNum = frameNum;
	anim->frameTime = duration / frameNum;
	anim->looping = looping;
	anim->stopped = FALSE;
	anim->t0 = -1;
	anim->draw = drawanim;


	int i;
	anim->keyFrames = (Image**) malloc(frameNum * sizeof(Image*));
	for (i=0; i<frameNum; i++) {
		SDL_Surface *surface = IMG_Load(frameFiles[i]);
		Image *texture = SDL_CreateTextureFromSurface(_canvas, surface);
		SDL_FreeSurface(surface);
		anim->keyFrames[i] = texture;
	}

	return anim;
}

void drawanim(Animation* anim, float stateTime) {
	if (anim->t0 == -1) {
		anim->t0 = stateTime;
	}

	int frameNumber = (int)((stateTime - anim->t0) / anim->frameTime);

	if (!anim->looping) {
		frameNumber = min(anim->frameNum - 1, frameNumber);
		if (stateTime - anim->t0 >= anim->duration) {
			anim->stopped = TRUE;
		}
	} else {
		frameNumber = frameNumber % anim->frameNum;
	}

	Image* img = anim->keyFrames[frameNumber];
	int w, h;
	SDL_QueryTexture(img, NULL, NULL, &w, &h);
	SDL_Rect destR = {anim->cx - w/2, anim->cy - h/2, w, h};
	SDL_RenderCopy(_canvas, img, NULL, &destR);
}

void addanims(float duration, float cx, float cy, char* frameFile, int row, int col) {
	if (_animCount < MAX_ANIM - 1) {
		Animation *anim = loadanim(duration, cx, cy, frameFile, row, col, FALSE);
		_animations[_animCount] = anim;
		_animCount++;
	}
}

void addanims2(float duration, float cx, float cy, char* frameFiles[], int frameNum) {
	if (_animCount < MAX_ANIM - 1) {
		Animation *anim = loadanim2(duration, cx, cy, frameFiles, frameNum, FALSE);
		_animations[_animCount] = anim;
		_animCount++;
	}
}

void bgcolor(Uint8 r, Uint8 g, Uint8 b) {
	SDL_Surface *surface = SDL_CreateRGBSurface(0, _screenW, _screenH, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r, g, b));
	SDL_Texture *texture = SDL_CreateTextureFromSurface(_canvas, surface);
	SDL_FreeSurface(surface);

	if (_bgColor != NULL) {
		SDL_DestroyTexture(_bgColor);
		_bgColor = NULL;
	}
	_bgColor = texture;
}

void bgimage(char *fileName) {
	SDL_Surface *surface = IMG_Load(fileName);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(_canvas, surface);
	SDL_FreeSurface(surface);

	if (_bgImage != NULL) {
		SDL_DestroyTexture(_bgImage);
		_bgImage = NULL;
	}
	_bgImage = texture;
}

void nobackground() {
	_clearBg = FALSE;
}

void bgmusic(char *fileName) {
	if (_bgMusic != NULL ) {
		Mix_HaltMusic();
		Mix_FreeMusic( _bgMusic );
		_bgMusic = NULL;
	}
	_bgMusic = Mix_LoadMUS( fileName );
	Mix_PlayMusic( _bgMusic, -1 );
}

void cursor(char *fileName) {
	if (fileName == NULL) {
		return;
	}
	if (_cursor != NULL) {
		SDL_FreeCursor(_cursor);
	}
	
	SDL_Surface* surface = IMG_Load(fileName);
	_cursor = SDL_CreateColorCursor(surface, 0, 0);
	SDL_FreeSurface(surface);
	SDL_SetCursor(_cursor);
}

void cursora(char *fileName, Uint8 alphaR, Uint8 alphaG, Uint8 alphaB) {
	if (fileName == NULL) {
		return;
	}
	if (_cursor != NULL) {
		SDL_FreeCursor(_cursor);
	}

	SDL_Surface* surface = IMG_Load(fileName);
	SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, alphaR, alphaG, alphaB));
	_cursor = SDL_CreateColorCursor(surface, 0, 0);
	SDL_FreeSurface(surface);
	SDL_SetCursor(_cursor);
}

void size(Uint16 width, Uint16 height) {
	_screenW = width;
	_screenH = height;
	SDL_SetWindowSize(_window, width, height);
}

void title(char *s) {
	char *buf = utf8(s);
	SDL_SetWindowTitle(_window, buf);
	free(buf);
}

Sound* loadsound(char* fileName) {
	return Mix_LoadWAV( fileName );
}

Image* loadimage(char *fileName) {
	SDL_Surface *surface = IMG_Load(fileName);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(_canvas, surface);
	SDL_FreeSurface(surface);

	return texture;
}

Image* loadimagea(char *fileName, Uint8 alphaR, Uint8 alphaG, Uint8 alphaB) {
	SDL_Surface *surface = IMG_Load(fileName);

	SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, alphaR, alphaG, alphaB));
	SDL_Texture *texture = SDL_CreateTextureFromSurface(_canvas, surface);
	SDL_FreeSurface(surface);
	return texture;
}

Bitmap* loadbitmap(char *fileName) {
	return IMG_Load(fileName);
}

void image(Image* img, int x, int y) {
	SDL_Rect destR = {x, y, 0, 0};
	SDL_QueryTexture(img, NULL, NULL, &destR.w, &destR.h);
	SDL_RenderCopy(_canvas, img, NULL, &destR);
}

void imagea(Image* img, int x, int y, Uint8 alpha) {
	Uint8 a;
	SDL_GetTextureAlphaMod(img, &a);
	//
	SDL_Rect destR = {x, y, 0, 0};
	SDL_QueryTexture(img, NULL, NULL, &destR.w, &destR.h);
	SDL_SetTextureAlphaMod(img, alpha);
	SDL_RenderCopy(_canvas, img, NULL, &destR);
	//
	SDL_SetTextureAlphaMod(img, a);
}

void imagezoomed(Image* img, int x, int y, int w, int h) {
	SDL_Rect destR = {x, y, w, h};
	SDL_RenderCopy(_canvas, img, NULL, &destR);
}

void imagerotated(Image* img, int x, int y, double angle) {
	SDL_Rect destR = {x, y, 0, 0};
	SDL_QueryTexture(img, NULL, NULL, &destR.w, &destR.h);
	SDL_RenderCopyEx(_canvas, img, NULL, &destR, angle, NULL, SDL_FLIP_NONE);
}

Image* createimagearea(Bitmap* img, int x, int y, int w, int h) {
	SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
	SDL_Rect srcR = {x, y, w, h};
	SDL_BlitSurface(img, &srcR, surface, NULL);

	SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));
	SDL_Texture *texture = SDL_CreateTextureFromSurface(_canvas, surface);
	SDL_FreeSurface(surface);
	return texture;
}

Image* createimagescaled(Bitmap* img, int destW, int destH) {
	SDL_Surface *surface = SDL_CreateRGBSurface(0, destW, destH, 32, 0, 0, 0, 0);
	SDL_BlitScaled(img, NULL, surface, NULL);

	SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));
	SDL_Texture *texture = SDL_CreateTextureFromSurface(_canvas, surface);
	SDL_FreeSurface(surface);
	return texture;
}

Bitmap* createbitmapscaled(Bitmap* img, int destW, int destH) {
	SDL_Surface *surface = SDL_CreateRGBSurface(0, destW, destH, 32, 0, 0, 0, 0);
	SDL_BlitScaled(img, NULL, surface, NULL);
	return surface;
}

void sliceimage(Bitmap* img, int row, int col, Image* imgs[]) {
	int width, height;
	width = img->w;
	height = img->h;

	float w = width / (float)col;
	float h = height / (float)row;

	int i,j;
	for (i=0; i<row; i++) {
		for (j=0; j<col; j++) {
			SDL_Surface *surface = SDL_CreateRGBSurface(0, (int)w, (int)h, 32, 0, 0, 0, 0);
			SDL_Rect srcR;
			srcR.x = (int)(j * w);
			srcR.y = (int)(i * h);
			srcR.w = (int)w;
			srcR.h = (int)h;
			SDL_BlitSurface(img, &srcR, surface, NULL);

			SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));
			SDL_Texture *texture = SDL_CreateTextureFromSurface(_canvas, surface);
			SDL_FreeSurface(surface);
			imgs[ i*col + j ] = texture;
		}
	}
}

void color(Uint8 r, Uint8 g, Uint8 b) {
	SDL_SetRenderDrawColor(_canvas, r, g, b, 0xff);
}

void colora(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_SetRenderDrawColor(_canvas, r, g, b, a);
}

void point(int x, int y) {
	SDL_RenderDrawPoint(_canvas, x, y);
}

void line(int x0, int y0, int x1, int y1) {
	SDL_RenderDrawLine(_canvas, x0, y0, x1, y1);
}

void rect(int x, int y, int w, int h) {
	SDL_Rect rect = {x, y, w, h};
	SDL_RenderDrawRect(_canvas, &rect);
}

void coloredrect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b) {
	SDL_Rect rect = {x, y, w, h};
	SDL_SetRenderDrawColor(_canvas, r, g, b, 0xff);
	SDL_RenderDrawRect(_canvas, &rect);
}

void fillrect(int x, int y, int w, int h) {
	SDL_Rect rect = {x, y, w, h};
	SDL_RenderFillRect(_canvas, &rect);
}

void coloredfillrect(int x, int y, int w, int h, Uint8 r0, Uint8 g0, Uint8 b0, Uint8 r1, Uint8 g1, Uint8 b1) {
	SDL_Rect rect = {x, y, w, h};
	SDL_SetRenderDrawColor(_canvas, r0, g0, b0, 0xff);
	SDL_RenderFillRect(_canvas, &rect);
	SDL_SetRenderDrawColor(_canvas, r1, g1, b1, 0xff);
	SDL_RenderDrawRect(_canvas, &rect);
}

void circle(int x0, int y0, double r) {
	int x,y,d;
	x=0;
	y=(int)r;
	d=(int)(3-2*r);
	while(x < y) {
		point((x0+x),(y0+y));
		point((x0+y),(y0+x));
		point((x0+y),(y0-x));
		point((x0+x),(y0-y));
		point((x0-x),(y0-y));
		point((x0-y),(y0-x));
		point((x0-y),(y0+x));
		point((x0-x),(y0+y));

		if(d < 0) {
			d+=4*x+6;
		} else {
			d+=4*(x-y)+10;
			y--;
		}
		x++;
	}
	if(x==y) {
		point((x0+x),(y0+y));
		point((x0+y),(y0+x));
		point((x0+y),(y0-x));
		point((x0+x),(y0-y));
		point((x0-x),(y0-y));
		point((x0-y),(y0-x));
		point((x0-y),(y0+x));
		point((x0-x),(y0+y));
	}
}
void semicircle(int x0, int y0, double r0,Uint8 r, Uint8 g, Uint8 b) {
    color(r,g,b);
	int x,y,d;
	x=0;
	y=(int)r0;
	d=(int)(3-2*r0);
	while(x < y) {
		point((x0+y),(y0-x));
		point((x0+x),(y0-y));
		point((x0-x),(y0-y));
		point((x0-y),(y0-x));

		if(d < 0) {
			d+=4*x+6;
		} else {
			d+=4*(x-y)+10;
			y--;
		}
		x++;
	}
	if(x==y) {
		point((x0+y),(y0-x));
		point((x0+x),(y0-y));
		point((x0-x),(y0-y));
		point((x0-y),(y0-x));
	}
}

void coloredcircle(int x0, int y0, double rr, Uint8 r, Uint8 g, Uint8 b) {
	color(r, g, b);
	circle(x0, y0, rr);
}

void fillcircle(int x0, int y0, int r) {
	int tx = 0, ty = r, d = 3 - 2 * r;
	int x = x0, y = y0;

	while(tx < ty) {
		SDL_RenderDrawLine(_canvas, x - ty, y - tx, x + ty, y - tx);
		if(tx != 0) {
			SDL_RenderDrawLine(_canvas, x - ty, y + tx, x + ty, y + tx);
		}

		if(d < 0) {  
			d += 4 * tx + 6;
		} else {     
			SDL_RenderDrawLine(_canvas, x - tx, y - ty, x + tx, y - ty);
			SDL_RenderDrawLine(_canvas, x - tx, y + ty, x + tx, y + ty);
			d += 4 * (tx - ty) + 10;
			ty--;
		}
		tx++;
	}

	if(tx == ty) { 
		SDL_RenderDrawLine(_canvas, x - ty, y - tx, x + ty, y - tx);
		SDL_RenderDrawLine(_canvas, x - ty, y + tx, x + ty, y + tx);
	}
}

void fillsemicircle(int x0, int y0, int r) {
	int tx = 0, ty = r, d = 3 - 2 * r;
	int x = x0, y = y0;

	while(tx < ty) {

		SDL_RenderDrawLine(_canvas, x - ty, y - tx, x + ty, y - tx);
		if(tx != 0) { 
		}

		if(d < 0) {  
			d += 4 * tx + 6;
		} else {    
			SDL_RenderDrawLine(_canvas, x - tx, y - ty, x + tx, y - ty);
			d += 4 * (tx - ty) + 10;
			ty--;
		}
		tx++;
	}

	if(tx == ty) {  
		SDL_RenderDrawLine(_canvas, x - ty, y - tx, x + ty, y - tx);
		SDL_RenderDrawLine(_canvas, x - ty, y + tx, x + ty, y + tx);
	}
}
void textsize(int size) {
	if ( _fontSize != size ) {
		if ( _font != NULL ) {
			TTF_CloseFont( _font );
			_font = NULL;
		}
		_fontSize = size;
		_font = TTF_OpenFont(_fontName, _fontSize);
	}
}

void text(char *s, int x, int y, Uint8 r, Uint8 g, Uint8 b) {
	SDL_Color color = {r, g, b, 0xff};
	SDL_Rect pos = {x, y, 0, 0};
	char* buf = utf8(s);
	SDL_Surface *surface = TTF_RenderUTF8_Blended(_font, buf, color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(_canvas, surface);
	free(buf);

	SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
	pos.x = pos.x - pos.w / 2;
	pos.y = pos.y - pos.h;
	SDL_RenderCopy(_canvas, texture, NULL, &pos);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void texta(char *s, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_Color color = {r, g, b, a};
	SDL_Rect pos = {x, y, 0, 0};
	char* buf = utf8(s);
	SDL_Surface *surface = TTF_RenderUTF8_Blended(_font, utf8(s), color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(_canvas, surface);
	free(buf);

	SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
	pos.x = pos.x - pos.w / 2;
	pos.y = pos.y - pos.h;
	SDL_RenderCopy(_canvas, texture, NULL, &pos);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void playsound(Sound* sound) {
	Mix_PlayChannel( -1, sound, 0 );
}

void playtimedsound(Sound* sound,int time) {
	Mix_PlayChannelTimed( -1, sound, 0, time);
}

void unloadimage(Image* img) {
	SDL_DestroyTexture(img);
}

void unloadbitmap(Bitmap* bmp) {
	SDL_FreeSurface(bmp);
}

void unloadsound(Sound* sound) {
	Mix_FreeChunk( sound );
}

void unloadanimation(Animation* anim) {
	int i;
	for (i=0; i<anim->frameNum; i++) {
		SDL_DestroyTexture( anim->keyFrames[i] );
		anim->keyFrames[i] = NULL;
	}
	free(anim->keyFrames);
	free(anim);
}

void screenshot(char* file,int x ,int y,int w ,int h) {
	SDL_Rect rect;
	Uint32 format;
	int bpp;
	Uint32 rmask,gmask,bmask,amask;
	char*pixels;
	int pitch;
	SDL_Surface* surface;
	rect.x=x;
	rect.y=y;
	rect.w=w;
	rect.h=h;
	format = SDL_GetWindowPixelFormat(_window);
	SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask,&bmask,&amask);
	pitch = SDL_BYTESPERPIXEL(format) * rect.w;
	pixels = (char*)malloc(pitch * rect.h * sizeof (char));
	SDL_RenderReadPixels(_canvas, &rect, format, (void*)pixels, pitch);

	surface = SDL_CreateRGBSurfaceFrom((void*)pixels,rect.w,rect.h,bpp,pitch,rmask,gmask,bmask,amask);

	SDL_SaveBMP(surface, file);

	SDL_FreeSurface(surface);
	free(pixels);
}

Color getpixel(int x, int y) {
	SDL_Rect rect;
	Uint32 format;
	int bpp;
	Uint32 rmask,gmask,bmask,amask;
	char*pixels;
	int pitch;
	SDL_Surface* surface;
	Uint32 pixel;

	rect.x=0;
	rect.y=0;
	SDL_GetWindowSize(_window, &rect.w, &rect.h);
	format = SDL_GetWindowPixelFormat(_window);
	SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask,&bmask,&amask);
	pitch = SDL_BYTESPERPIXEL(format)*(rect.w);
	pixels = (char*)malloc(pitch * rect.h * sizeof (char));

	SDL_RenderReadPixels(_canvas, &rect, format, (void*)pixels, pitch);

	surface = SDL_CreateRGBSurfaceFrom((void*)pixels, rect.w, rect.h, bpp, pitch, rmask, gmask, bmask, amask);

	bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
		case 1:
			pixel = *p;
			break;
		case 2:
			pixel = *(Uint16 *)p;
			break;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				pixel = p[0] << 16 | p[1] << 8 | p[2];
			else
				pixel = p[0] | p[1] << 8 | p[2] << 16;
			break;
		case 4:
			pixel = *(Uint32 *)p;
			break;
		default:
			pixel = 0;       
	}

	Uint8 r,g,b,a;
	SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

	SDL_FreeSurface(surface);
	free(pixels);

	SDL_Color color = {r, g, b, a};

	return color;
}

void internalUpdateanims() {
	int n = 0, i;
	while (n < _animCount) {
		Animation* anim = _animations[n];
		if (anim->stopped) {
			for (i=n; i<_animCount-1; i++) {
				_animations[i] = _animations[i+1];
				_animations[i+1] = NULL;
			}
			unloadanimation(anim);
			_animCount--;
		} else {
			n++;
		}
	}
}

void internalDrawanims(float stateTime) {
	int i;
	for (i=0; i<_animCount; i++) {
		_animations[i]->draw(_animations[i], stateTime);
	}
}

void internalRender(float stateTime) {
	if (_clearBg) {
		SDL_SetRenderDrawColor(_canvas, 0xf5, 0xf5, 0xf5, 0xff);
		SDL_RenderClear(_canvas);

		if (_bgImage != NULL) {
			SDL_RenderCopy(_canvas, _bgImage, NULL, NULL);
		} else if (_bgColor != NULL) {
			SDL_RenderCopy(_canvas, _bgColor, NULL, NULL);
		}
	}
	SDL_SetRenderDrawColor(_canvas, 0x00, 0x00, 0x00, 0xff);

	draw(stateTime);

	internalUpdateanims();
	internalDrawanims(stateTime);
}

int main(int argc, char* args[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(0);
	Mix_Init(0);
	TTF_Init();

	_window = SDL_CreateWindow("MyTest", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
	                           _screenW, _screenH, SDL_WINDOW_OPENGL|SDL_WINDOW_HIDDEN);
	_canvas = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	_font = TTF_OpenFont(_fontName, _fontSize);
	Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );

	int i;
	for (i=0; i<MAX_ANIM; i++) {
		_animations[i] = NULL;
	}


	SDL_ShowWindow(_window);

	float deltaTime, delta;
	unsigned int tick0, tick1;
	SDL_Event event;
	Bool quit = FALSE;


	tick0 = SDL_GetTicks();
	setup();
	while( !quit ) {

		if ( SDL_PollEvent( &event ) ) {
			switch (event.type) {
				case SDL_QUIT :
					quit = TRUE;
					break;
				case SDL_KEYDOWN :
					key = event.key.keysym.sym;
					keyPressed = TRUE;
					keyDown();
					if ( key == SDLK_ESCAPE ) {
						quit = TRUE;
					}
					break;
				case SDL_KEYUP:
					keyPressed = FALSE;
					keyUp();
					break;
				case SDL_MOUSEBUTTONDOWN :
					mouseButton = event.button.button;
					pmouseX = mouseX;
					pmouseY = mouseY;
					mouseX = event.button.x;
					mouseY = event.button.y;
					mousePressed = TRUE;
					mouseDragged = FALSE;
					mousePress();
					break;
				case SDL_MOUSEMOTION :
					pmouseX = mouseX;
					pmouseY = mouseY;
					mouseX = event.motion.x;
					mouseY = event.motion.y;
					if (event.motion.state & SDL_BUTTON_LMASK || event.motion.state & SDL_BUTTON_RMASK) {
						mouseDragged = TRUE;
					}
					mouseMove();
					break;
				case SDL_MOUSEBUTTONUP :
					pmouseX = mouseX;
					pmouseY = mouseY;
					mouseX = event.button.x;
					mouseY = event.button.y;
					mousePressed = FALSE;
					mouseDragged = FALSE;
					mouseRelease();
					break;
				default:
					break;
			}
		}


		internalRender(_duration);

		tick1 = SDL_GetTicks();
		delta = tick1 - tick0;
		tick0 = SDL_GetTicks();
		deltaTime = delta / 1000.0;

		if (delta < 16.7) { 
			SDL_Delay(17 - delta);
			deltaTime = 0.016667;
		}
		_duration = _duration + deltaTime;

		float fps = 1.0 / deltaTime + 0.5;
		char info[20];
		sprintf(info, "Ö¡ÂÊFPS:%d", (int)fps);

		SDL_RenderPresent(_canvas);
	}


	close();

	if (_bgColor != NULL ) SDL_DestroyTexture(_bgColor);
	if (_bgImage != NULL ) SDL_DestroyTexture(_bgImage);
	if (_cursor != NULL) SDL_FreeCursor(_cursor);
	if (_font != NULL) TTF_CloseFont( _font );
	if (_bgMusic != NULL ) {
		Mix_HaltMusic();
		Mix_FreeMusic( _bgMusic );
	}

	for (i=0; i<_animCount; i++) {
		Animation* anim = _animations[i];
		unloadanimation(anim);
		_animations[i] = NULL;
	}

	SDL_DestroyRenderer(_canvas);
	SDL_DestroyWindow(_window);

	IMG_Quit();
	Mix_Quit();
	TTF_Quit();
	SDL_Quit();

	return 0;
}


