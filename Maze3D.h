#pragma once
/*
	If you want to make a Mod of Maze 3D, you must include 
	this header file. This file defined more things about 
	modify, add game content. If you didn't include this 
	file, you need to define more things to change the game.
*/

/*Change macro define*/
/*MTC=Maze 3 change*/
#define MTC_NOTHING 0x00000000 //Do nothing, you don't need this
#define MTC_MENU 1 //Change Menu UI MTC_MENU(MenuId,ModifyFunc,lParam)
#define MTC_RENDER 2 //Use custom functions to render images
#define MTC_MAZEMAP 3 //Change the Map
#define MTC_LOCATION 4 //Detect Players Location and change it
#define MTC_DISTANCE 5 //Change Distance calculation algorithm
#define MTC_DIRECTION 6 //Detect Direction and change it
#define MTC_RENCONTROL 7 //Use custom functions to draw controls
#define MTC_HOOK 8 //Set Hook to game window

/*Get information of game*/
/*MTG=Maze 3 Get*/
#define MTG_LOCATION 101 //Get Player's Position MTG_LOCATION(&X,&Y,0)
#define MTG_DIRECTION 102 //Get Player's Direction MTG_DIRECTION(&Direction,0,0)
#define MTG_PAGEID 103 //Get Current Page Id MTG_PAGEID(&PageId,0,0)
#define MTG_FPS 104 //Get Current FPS MTG_FPS(&FPS,0,0)
#define MTG_WNDRECT 105 //Get Rect of Window MTG_WNDRECT(IsClient,&WndRect,0)
#define MTG_PRECISION 106 //Get Precision of game MTG_PRECISION(&Precision,0,0)
#define MTG_SPEED 107 //Get Speed of game MTG_SPEED(&Speed,0,0)
#define MTG_VISIBILITY 108 //Get Visibility of game MTG_VISIBILITY(&VISIBILITY,0,0)

#define MTS_LOCATION 201 //Set Player's Position MTS_LOCATION(&X,&Z,0)
#define MTS_DIRECTION 202 //Set Player's Direction MTS_DIRECTION(&Direction,0,0) 
#define MTS_FPSLIMIT 203 //Set FPS Limit MTS_FPSLIMIT(&FPSLimit,0,0)

//Pages definition
#define IDP_MAINMENU 301 //Main Menu
#define IDP_MAZE 302 //Maze
#define IDP_WIN 303 //Win Menu
#define IDP_ESCAPE 304 //Escape Menu
#define IDP_OPTESCAPE 305 //Option Menu in Escape Menu

typedef LPSTR *LPLPSTR;
typedef VOID (*MENUMODPROC)(HWND,UINT,LPARAM,BOOL); //MenuModProc(hwnd,PageId,lParam,beQuit)
typedef VOID (*RENDERPROC)(HDC,double,double,double,LPARAM); //RenderProc(hdc,X,Y,Direction,lParam)
typedef VOID (*MODMAZEPROC)(LPLPSTR,double,double,double,LPARAM); //ModMazeProc(Map,X,Y,Direction,lParam)

#define MI_NAME 0x00000001 /*Name of this mod(Must include)*/
#define MI_WRITER 0x00000002 /*Include Writer of this mod*/
#define MI_DETAIL 0x00000004 /*Include Detail text of this mod*/
#define MI_ICON 0x00000008 /*Use hIcon to be icon of this mod*/
#define MI_BMICON 0x0000000F /*Use bmIcon to be icon of this mod*/
#define MI_UIBYSELF 0x00000010 /*Draw Mod Information UI by this mod*/
#define MI_VERSION 0x00000020 /*Include Version in This Struct*/
struct MODINFO{ //Information of mod
	DWORD dwFlags/*Flags of Mod*/;
	LPCSTR Name /*Name of Mod*/,Writer /*Writer of Mod*/,Detail /*Detail of Mod*/,Version /*Version of Mod*/;
	HBITMAP bmIcon; //Icon of Mod(HBITMAP Format)
	HICON hIcon; //Icon of Mod(HICON Format)
};