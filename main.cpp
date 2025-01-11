//Include Library
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <math.h>
#include "Maze3D.h"
#include "ModServer.h"
//No opengl, only wingdi32

#define DRAWBYSELF
#ifdef DRAWBYSELF
#undef SS_CENTER
#define SS_CENTER (SS_OWNERDRAW|0x00000001L)
#endif

//Value Type definition
typedef double FLOAT2;
typedef HBRUSH *LPHBRUSH;
#define INVALID_LOCATION_CALCULATE ((FLOAT)-1)
//Timer definition
#define IDT_REFRESH 101 //Maze Renderer
#define IDT_CHANGEVIEW 102 //For Test(Repealed)
#define IDT_KEYBOARDDETECT 103 //Check your Keyboard in Maze Game 
#define IDT_CLIP 104 //Limit Cursor area
#define IDT_MENUKEYDET 105 //Check your Keyboard in Escape Menu
//Controls definition
#define IDC_STARTGAME 201 //In Main Menu 
#define IDC_EDIT 202
#define IDC_EXIT 203
#define IDC_TITLE 204
#define IDC_OPTION 205

#define IDC_WINTEXT 206 //In Win Menu
#define IDC_WINEXIT 207
#define IDC_WINSUPPORT 208
#define IDC_WINICON 209

#define IDC_BACKGAME 210 //In Escape Menu
#define IDC_EXITGAME 211
#define IDC_ESCOPTION 212

#define IDC_ESCPRECISION 213 //Precision Track Bar //In Option Menu(in Escape Menu) 
#define IDC_ESCSPEED 214 //Speed Track Bar 
#define IDC_ESCVISIBILITY 215 //Visibility Track Bar
#define IDC_ESCPRETIP 216 //Precision Label
#define IDC_ESCPRETIPLOW 217
#define IDC_ESCPRETIPHIGH 218
#define IDC_ESCSPEEDTIP 219 //Speed Label
#define IDC_ESCSPEEDTIPLOW 220
#define IDC_ESCSPEEDTIPHIGH 221
#define IDC_ESCVISTIP 222 //Visibility Label
#define IDC_ESCVISTIPLOW 223 //
#define IDC_ESCVISTIPHIGH 224 //Static of "High" for visibility
#define IDC_ESCPREDEF 225 //Button to set Precision default
#define IDC_ESCSPEEDDEF 226 //Button to set Speed default
#define IDC_ESCVISDEF 227 //Button to set Visibility default
#define IDC_ESCOK 228 //Button to apply and quit from options
#define IDC_ESCCANCEL 229 //Button to quit from options
#define IDC_ESCAPPLY 230 //Button to apply options

//Message definition
#define WM_EX_KEY (WM_USER+100)
#define WM_EX_WIN (WM_USER+101)
#define key_press(key) (GetAsyncKeyState(key)&0x8000) //Define macro for detect keyboard
typedef struct LOCATION{
	FLOAT2 X,Y,fac;
}*LPLOCATION; //Location For Renderer, Players
LOCATION Player; //Player's Location
HDC hdc,MainHdc; //Create canvas for Maze Renderer
DWORD LastX;//Last X Pointer of Cursor, use in WM_MOUSEMOVE message
HFONT hFont=CreateFont(20,0,0,0,0,0,0,0,0,0,0,0,0,"Consolas");//Default Font
HBRUSH hWallBrush=CreateSolidBrush(RGB(0,255,0));/*Color of Wall,Default=RGB(0,255,0)*/
HBRUSH hFloorBrush=CreateSolidBrush(RGB(128,128,128));/*Color of Floor,Default=RGB(128,128,128)*/
HBRUSH hEdgeBrush=CreateSolidBrush(RGB(0,0,0));/*Color of Edge,Default=RGB(0,0,0)*/
HBRUSH hCursorBrush=CreateSolidBrush(RGB(255,255,255));/*Color of Cursor(Center Cross),Default=RGB(255,255,255)*/
HBRUSH hDestinationBrush=CreateSolidBrush(RGB(255,0,0));/*Color of Destination,Default=RGB(255,0,0)*/
HBRUSH hStartBrush=CreateSolidBrush(RGB(0,0,255));/*Color of Start Point,Default=RGB(0,0,255)*/
DWORD PageId=IDP_MAINMENU /*Page Id of game*/,OpQ2Page=IDP_MAINMENU;
INT DestinationX,DestinationY /*Destination X,Y position*/,
StartX,StartY /*Starting Point X,Y position*/,
MazeCx=20,MazeCy=15 /*Width, Height of Maze*/,
ModCnt=0 /*Mods Count*/;
FLOAT2 Precision=0.02/*Precision Accuracy, High=Low accuracy,Low=High accuracy,Default=0.02*/,
Speed=0.02/*Speed(DONT TAKE IT HIGH FORERVER!),High=High Speed,Low=Low Speed,Default=0.02*/,
Visibility=12.0/*Visibility,High=Visibility Low,Low=Visibility High*/;
const DWORD RefreshInterval=1000/50;//Default Refresh Interval
SERVER MainModServer /*Mod Server, manage all the mods*/;
HMODULE hMods[114514] /*Handle of Mods*/;
MODINFO ModInfos[114514] /*Information of Mods*/;
HWND MainWnd; /* A 'HANDLE', hence the H, or a pointer to our window */

COLORREF StartFlag[256][256];
COLORREF EndFlag[256][256];

char Maze[20][25]={
	"WWWWWWWWWWWWWWWWWWWWW",
	" WWWWWWWWWWWWWWWWWWWW",
	" WFFFFFFFFFFFFFFWDWFW",
	" WFWWWWWWWWWWWWWWFFFW",
	" WFFFFFFFFFFFFFFWFWWW",
	" WWFWWWWWWFWWWWWFFFFW",
	" WFFWFFFFFFFWWWWFWWWW",
	" WFFWFFFFFWFFFFFFFFFW",
	" WWWWWWWFWWWFWWWWWWWW",
	" WFFFFFFFWFWFFFFFFFWW",
	" WFWWWFWFFFFWWWWWWWWW",
	" WFWWWFFFWWFFFFFFFFFW",
	" WFWWWWWWWFFWWWWWWFFW",
	" WFWWFFFFWWWWWWWWWWFW",
	" WFFFFWWFFFFFFFFFFFSW",
	" WWWWWWWWWWWWWWWWWWWW"
};

/*
	W=Wall,
	F=Floor,
	S=Start Point,
	D=Destination
*/

/*char Maze[20][25]={
	"WWWWWWWWWWWWWWWWWWWWW",
	" WWWWWWWWWWWWWWWWWWWW",
	" WDFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFFW",
	" WFFFFFFFFFFFFFFFFFSW",
	" WWWWWWWWWWWWWWWWWWWW"
};*/

LRESULT CALLBACK EWProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		case WM_DESTROY:{
			ShowWindow(hwnd,SW_HIDE);
			break;
		}
		default: return DefWindowProc(hwnd,Message,wParam,lParam);
	}
	return 0;
}

/*
The GetTriangleEdge3Length function just like this:
  M
  MM Calculate the length of this line
4 M M a^2+b^2=c^2 (a ^ b is the power b of a)
  MMMM
   4
*/
FLOAT2 GetTriangleEdge3Length(FLOAT2 Edge1,FLOAT2 Edge2){
	return pow(Edge1*Edge1+Edge2*Edge2,0.5);
}
/*
The ToPositiveAngle function just like this:
Input: -270.0
Output: 90.0 
Keep the angle within the range of 0~360
*/
FLOAT2 ToPositiveAngle(FLOAT2 Angle){
	while(Angle<0) Angle+=360.0;
	while(Angle>=360) Angle-=360.0;
	return Angle;
}
/*Floating point version of absolute value*/
FLOAT2 Float2Abs(FLOAT2 Num){return ((Num>0)?Num:-Num);}
/*
Calulate Distance to wall:
We use a simple method.
If the maze is like this:
MMMMMMMMMMMMMMMMM
M               M
M P             M
M               M
M               M
M               M
MMMMMMMMMMMMMMMMM
P=Player
M=Wall
Player's XY=3,3
Player's Facing Angle=315

Distance=GetTriangleEdgeLength(1,1)=pow(1*1+1*1,0.5)=sqrt(2)

This function calculates distance by simulating the emission of light in the eye.
*/
FLOAT2 GetDistanceToWall(LPLOCATION lpLocation,LPHBRUSH ShouldBeBrush,BOOL UseBrush){
	//if(Maze[int(lpLocation->Y)][int(lpLocation->X)]=='W') return INVALID_LOCATION_CALCULATE;
	//Location X Angle: 0~0.01
	//Location Y Angle: 0.01-X
	FLOAT2 fl=fmod(lpLocation->fac,90.0);
	FLOAT2 X,Y;
	if(lpLocation->fac<90){//Angle: 0~89
		X=fl/90*Precision;
		Y=-((90.0-fl)/90.0*Precision);
	}
	else if(lpLocation->fac<180){//Angle: 90~179
		X=(90.0-fl)/90*Precision;
		Y=fl/90.0*Precision;
	}
	else if(lpLocation->fac<270){//Angle: 180~269
		X=-(fl/90*Precision);
		Y=(90.0-fl)/90.0*Precision;
	}
	else{//Angle: 270~359
		X=-((90.0-fl)/90*Precision);
		Y=-(fl/90.0*Precision);
	}
	FLOAT2 WallX=lpLocation->X,WallY=lpLocation->Y;//Player's position
	for(;WallX>=0&&WallY>=0&&
		Maze[int(round(WallY))][int(round(WallX))]!='W'&&/*Wall*/
		Maze[int(round(WallY))][int(round(WallX))]!='S'&&/*Start Point*/ 
		Maze[int(round(WallY))][int(round(WallX))]!='D'/*Destination*/;WallY+=Y,WallX+=X);//Try to walk
	FLOAT2 Result=Float2Abs(GetTriangleEdge3Length(Float2Abs(WallX-lpLocation->X),Float2Abs(WallY-lpLocation->Y)));//Calculate Distance
	if(UseBrush==TRUE){//Is Drawing
		/*LOCATION Tmp,Tmp2;
		Tmp.X=lpLocation->X;
		Tmp.Y=lpLocation->Y;
		Tmp.fac=lpLocation->fac+0.5;
		Tmp2.X=lpLocation->X;
		Tmp2.Y=lpLocation->Y;
		Tmp2.fac=lpLocation->fac-0.5;
		FLOAT2 Left=GetDistanceToWall(&Tmp2,NULL,FALSE),Right=GetDistanceToWall(&Tmp,NULL,FALSE);
		if((Left<Result-0.005&&Right<Result-0.005)||(Left<=Result-0.005&&Right<Result-0.005)||
		(Left<Result-0.005&&Right<=Result-0.005)||(Left>Result+0.005&&Right>Result+0.005)||
		(Left>=Result+0.005&&Right>Result+0.005)||(Left>Result+0.005&&Right>=Result+0.005)){
			*ShouldBeBrush=hEdgeBrush;
		}
		else *ShouldBeBrush=hWallBrush;*/
		if(WallX-(int)WallX<0.05||WallY-(int)WallY<0.05) *ShouldBeBrush=hEdgeBrush;//Draw Edge
		else{
			switch(Maze[int(round(WallY))][int(round(WallX))]){
				case 'S':{
					if(Maze[(int)round(lpLocation->Y)][(int)round(lpLocation->X)]!='S') *ShouldBeBrush=hStartBrush;//Draw Start Point
					else *ShouldBeBrush=hWallBrush;//Draw Wall
					break;
				}
				case 'D':{
					if(Maze[(int)round(lpLocation->Y)][(int)round(lpLocation->X)]!='D') *ShouldBeBrush=hDestinationBrush;//Draw Destination
					else *ShouldBeBrush=hWallBrush;//Draw Wall
					break;
				}
				default:*ShouldBeBrush=hWallBrush;//Draw Wall
			}
		}
	}
	return Result;
}
/*
Try to Walk with facing angle.
Principle: Same as GetDistanceToWall
*/
void WalkFront(LPLOCATION lpLocation){
	FLOAT2 fl=fmod(lpLocation->fac,90.0),X,Y;
	if(lpLocation->fac<90){//Angle: 0~89
		X=fl/90*Speed;
		Y=-((90.0-fl)/90.0*Speed);
	}
	else if(lpLocation->fac<180){//Angle: 90~179
		X=(90.0-fl)/90*Speed;
		Y=fl/90.0*Speed;
	}
	else if(lpLocation->fac<270){//Angle: 180~269
		X=-(fl/90*Speed);
		Y=(90.0-fl)/90.0*Speed;
	}
	else{//Angle: 270~359
		X=-((90.0-fl)/90*Speed);
		Y=-(fl/90.0*Speed);
	}
	X*=10;//Make the move distance big
	Y*=10;
	lpLocation->X+=X;
	lpLocation->Y+=Y;//Set location
	while(Maze[(int)round(lpLocation->Y)][(int)round(lpLocation->X)]=='W'){
		lpLocation->X-=X*0.01;
		lpLocation->Y-=Y*0.01;
	}
	return;
}
/*We also using the Pythagorean theorem.*/
FLOAT2 GetDistanceToDestination(LPLOCATION lpLocation){
	FLOAT2 Result=Float2Abs(GetTriangleEdge3Length(Float2Abs(DestinationX-lpLocation->X),Float2Abs(DestinationY-lpLocation->Y)));
	return Result;
}
/*Counting the renderer time for modifying correctly FPS*/
DWORD WINAPI Counter(LPVOID lparam){
	LPDWORD Return=(LPDWORD)lparam;
	DWORD milliSecond=0;
	while(*Return==FALSE){//Check variable
		Sleep(1);
		milliSecond++;
	}
	*Return=milliSecond;
	return 0;
}

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		case WM_CREATE:{//Init the controls,canvas
			PageId=IDP_MAINMENU;
			MainHdc=GetDC(hwnd);
			hdc=CreateCompatibleDC(MainHdc);
			RECT rect;GetClientRect(hwnd,&rect);
			FillRect(MainHdc,&rect,hCursorBrush);
   			SelectObject(hdc, CreateCompatibleBitmap(MainHdc, 900, 480));
   			CreateWindowEx(NULL,"BUTTON","Start Game",WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|WS_TABSTOP,0,0,200,100,hwnd,(HMENU)IDC_STARTGAME,NULL,NULL);
   			CreateWindowEx(NULL,"BUTTON","Edit Maze Map",WS_CHILD|WS_VISIBLE|WS_TABSTOP,0,0,200,100,hwnd,(HMENU)IDC_EDIT,NULL,NULL);
   			CreateWindowEx(NULL,"BUTTON","Exit",WS_CHILD|WS_VISIBLE|WS_TABSTOP,0,0,200,100,hwnd,(HMENU)IDC_EXIT,NULL,NULL);
   			CreateWindowEx(NULL,"BUTTON","Options...",WS_CHILD|WS_VISIBLE|WS_TABSTOP,0,0,200,100,hwnd,(HMENU)IDC_OPTION,NULL,NULL);
   			CreateWindowEx(NULL,"STATIC","Maze 3D",WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,200,100,hwnd,(HMENU)IDC_TITLE,NULL,NULL);
			//SetTimer(hwnd,IDT_REFRESH,RefreshInterval,NULL);//Graphical Loop 
			
			//SetTimer(hwnd,IDT_CHANGEVIEW,100,NULL);
			//SetTimer(hwnd,IDT_KEYBOARDDETECT,50,NULL);
			//SetTimer(hwnd,IDT_CLIP,50,NULL);
			PostMessage(hwnd,WM_SIZE,NULL,NULL);
			break;
		}
		
		case WM_MOVE:
		case WM_SIZE:{//Change the size, position of controls in different Pages
			RECT rect;GetClientRect(hwnd,&rect);
			switch(PageId){
				case IDP_MAINMENU:{//Main Menu
					SetWindowPos(GetDlgItem(hwnd,IDC_STARTGAME),NULL,rect.right*0.2,rect.bottom*0.3,rect.right*0.6,rect.bottom*0.08,SWP_NOZORDER);//Start Game
					SetWindowPos(GetDlgItem(hwnd,IDC_EDIT),NULL,rect.right*0.2,rect.bottom*0.4,rect.right*0.6,rect.bottom*0.08,SWP_NOZORDER);//Edit Maze Map
					SetWindowPos(GetDlgItem(hwnd,IDC_EXIT),NULL,rect.right*0.51,rect.bottom*0.6,rect.right*0.29,rect.bottom*0.08,SWP_NOZORDER);//Exit This game 
					SetWindowPos(GetDlgItem(hwnd,IDC_OPTION),NULL,rect.right*0.2,rect.bottom*0.6,rect.right*0.29,rect.bottom*0.08,SWP_NOZORDER);//Options
					SetWindowPos(GetDlgItem(hwnd,IDC_TITLE),NULL,rect.right*0.1,rect.bottom*0.1,rect.right*0.8,rect.bottom*0.1,SWP_NOZORDER);//Maze 3D
					break;
				}
				case IDP_WIN:{//Win Menu
					SetWindowPos(GetDlgItem(hwnd,IDC_WINEXIT),NULL,rect.right*0.51,rect.bottom*0.8,rect.right*0.29,rect.bottom*0.08,SWP_NOZORDER);//Exit To Main Menu
					SetWindowPos(GetDlgItem(hwnd,IDC_WINSUPPORT),NULL,rect.right*0.2,rect.bottom*0.8,rect.right*0.29,rect.bottom*0.08,SWP_NOZORDER);//Support button
					SetWindowPos(GetDlgItem(hwnd,IDC_WINTEXT),NULL,rect.right*0.1,rect.bottom*0.1,rect.right*0.8,rect.bottom*0.1,SWP_NOZORDER);//Text when you were won
					SetWindowPos(GetDlgItem(hwnd,IDC_WINICON),NULL,rect.right*0.1,rect.bottom*0.25,rect.right*0.8,rect.bottom*0.5,SWP_NOZORDER);
					break;
				}
				case IDP_ESCAPE:{//Escape Menu
					SetWindowPos(GetDlgItem(hwnd,IDC_BACKGAME),NULL,rect.right*0.2,rect.bottom*0.3,rect.right*0.6,rect.bottom*0.08,SWP_NOZORDER);//Back to game
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCOPTION),NULL,rect.right*0.2,rect.bottom*0.6,rect.right*0.29,rect.bottom*0.08,SWP_NOZORDER);//Options
					SetWindowPos(GetDlgItem(hwnd,IDC_EXITGAME),NULL,rect.right*0.51,rect.bottom*0.6,rect.right*0.29,rect.bottom*0.08,SWP_NOZORDER);//Exit to Main Menu
					break;
				}
				case IDP_OPTESCAPE:{
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCPRECISION),NULL,rect.right*0.2,rect.bottom*0.05,rect.right*0.4,rect.bottom*0.1,SWP_NOZORDER);//Precision
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCPRETIP),NULL,rect.right*0.01,rect.bottom*0.05,rect.right*0.17,rect.bottom*0.1,SWP_NOZORDER);
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCPRETIPLOW),NULL,rect.right*0.49,rect.bottom*0.15,rect.right*0.2,rect.bottom*0.1,SWP_NOZORDER);
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCPRETIPHIGH),NULL,rect.right*0.15,rect.bottom*0.15,rect.right*0.2,rect.bottom*0.1,SWP_NOZORDER);
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCPREDEF),NULL,rect.right*0.7,rect.bottom*0.05,rect.right*0.2,rect.bottom*0.1,SWP_NOZORDER);//Set to default
					
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCSPEED),NULL,rect.right*0.2,rect.bottom*0.3,rect.right*0.4,rect.bottom*0.1,SWP_NOZORDER);//Speed
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCSPEEDTIP),NULL,rect.right*0.01,rect.bottom*0.3,rect.right*0.17,rect.bottom*0.1,SWP_NOZORDER);
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCSPEEDTIPLOW),NULL,rect.right*0.15,rect.bottom*0.4,rect.right*0.2,rect.bottom*0.1,SWP_NOZORDER);
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCSPEEDTIPHIGH),NULL,rect.right*0.49,rect.bottom*0.4,rect.right*0.2,rect.bottom*0.1,SWP_NOZORDER);
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCSPEEDDEF),NULL,rect.right*0.7,rect.bottom*0.3,rect.right*0.2,rect.bottom*0.1,SWP_NOZORDER);//Set to default
					
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCVISIBILITY),NULL,rect.right*0.2,rect.bottom*0.55,rect.right*0.4,rect.bottom*0.1,SWP_NOZORDER);//Visibility
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCVISTIP),NULL,rect.right*0.01,rect.bottom*0.55,rect.right*0.17,rect.bottom*0.09,SWP_NOZORDER);
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCVISTIPLOW),NULL,rect.right*0.49,rect.bottom*0.65,rect.right*0.2,rect.bottom*0.1,SWP_NOZORDER);
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCVISTIPHIGH),NULL,rect.right*0.15,rect.bottom*0.65,rect.right*0.2,rect.bottom*0.1,SWP_NOZORDER);
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCVISDEF),NULL,rect.right*0.7,rect.bottom*0.55,rect.right*0.2,rect.bottom*0.1,SWP_NOZORDER);//Set to default
					
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCOK),NULL,rect.right*0.31,rect.bottom*0.8,rect.right*0.18,rect.bottom*0.1,SWP_NOZORDER);//OK
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCCANCEL),NULL,rect.right*0.51,rect.bottom*0.8,rect.right*0.18,rect.bottom*0.1,SWP_NOZORDER);//Cancel
					SetWindowPos(GetDlgItem(hwnd,IDC_ESCAPPLY),NULL,rect.right*0.71,rect.bottom*0.8,rect.right*0.18,rect.bottom*0.1,SWP_NOZORDER);//Apply
					break;
				}
			}
			for(int i=IDC_STARTGAME;i<=299;i++){
				HWND hControl=GetDlgItem(hwnd,i);
				if(hControl!=NULL){
					RECT rect;
					GetClientRect(hControl,&rect);
					SendMessage(hControl,WM_SETFONT,(WPARAM)CreateFont(rect.bottom-5,0,0,0,0,0,0,0,0,0,0,0,0,"Consolas"),NULL);
				}
			}
			break;
		}
		
		case WM_TIMER:{
			switch(wParam){
				case IDT_REFRESH:{//Refersh Timer, For rendering picture
					if(PageId!=IDP_MAZE) break;
					KillTimer(hwnd,IDT_REFRESH);
					DWORD Return=FALSE;//Init counter variable
					CreateThread(NULL,NULL,Counter,(LPVOID)&Return,NULL,NULL);//Create the counter Thread
					RECT ResetRect,MainWnd;ResetRect.left=0;ResetRect.top=0;ResetRect.bottom=480;ResetRect.right=900;
					//Use default color to fill canvas
					GetClientRect(hwnd,&MainWnd);
					FillRect(hdc,&ResetRect,hFloorBrush);
					LOCATION loc;
					loc.X=Player.X;
					loc.Y=Player.Y;
					RECT Fill;FLOAT2 Result;int posX=1;
					HBRUSH Tmp;
					if(Maze[(int)round(Player.Y)][(int)round(Player.X)]!='S'){
						for(FLOAT2 i=Player.fac-45.0;i<=Player.fac+45.0;i+=0.1){//Range for drawing
							loc.fac=ToPositiveAngle(i);
							Result=GetDistanceToWall(&loc,&Tmp,TRUE);
							if((480-Result*Visibility)>=Result*Visibility*2){//Can draw
								Fill.top=int(Result*12);
								Fill.bottom=int(480-Result*Visibility);
								Fill.left=posX-1;
								Fill.right=posX;
								FillRect(hdc,&Fill,Tmp);//Fill
							}
							posX++;//Next angle
						}
					}
					else{
						Fill.top=0;
						Fill.bottom=480*0.8;
						Fill.left=0;
						Fill.right=900;
						SetBkMode(hdc,TRANSPARENT);
						SelectObject(hdc,hFont);
						DrawText(hdc,"You are in the start point, try to press 'UP' key to walk!",58,&Fill,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
					}
					Fill.top=100-5;Fill.left=800-5;Fill.bottom=100+5;Fill.right=800+5;
					FillRect(hdc,&Fill,hFloorBrush);
					for(int i=-5;i<=5;i++){
						for(int j=-5;j<=5;j++){
							Fill.top=99+i;Fill.bottom=100+i;Fill.left=799+j;Fill.right=800+j;
							if(i==0&&j==0) SetPixel(hdc,Fill.left,Fill.top,RGB(255,255,255));
							else if((int)Player.Y+i>=1&&(int)Player.X+j>=1&&(int)Player.Y+i<=MazeCy&&(int)Player.X+j<=MazeCx){
								switch(Maze[(int)((int)Player.Y+i)][(int)((int)Player.X+j)]){
									case 'D':{
										SetPixel(hdc,Fill.left,Fill.top,RGB(255,0,0));
										//FillRect(hdc,&Fill,hDestinationBrush);
										break;
									}
									case 'S':{
										SetPixel(hdc,Fill.left,Fill.top,RGB(0,0,255));
										//FillRect(hdc,&Fill,hStartBrush);
										break;
									}
									case 'F':{
										SetPixel(hdc,Fill.left,Fill.top,RGB(128,128,128));
										//FillRect(hdc,&Fill,hFloorBrush);
										break;
									}
									default:{
										SetPixel(hdc,Fill.left,Fill.top,RGB(0,255,0));
										//FillRect(hdc,&Fill,hWallBrush);
										break;
									}
								}
							}
							else{
								SetPixel(hdc,Fill.left,Fill.top,RGB(0,0,0));
							}
						}
					}
					StretchBlt(hdc,650,10,200,200,hdc,800-5,100-5,10,10,SRCCOPY);
					ResetRect.top=480*0.47;//Draw the vertical part of the cursor
					ResetRect.bottom=480*0.53;
					ResetRect.left=900*0.5-1;
					ResetRect.right=900*0.5+1;
					FillRect(hdc,&ResetRect,hCursorBrush);
					
					ResetRect.top=480*0.5-1;//Draw the horizontal part of the cursor
					ResetRect.bottom=480*0.5+1;
					ResetRect.left=900*0.5-480*0.03;
					ResetRect.right=900*0.5+480*0.03;
					FillRect(hdc,&ResetRect,hCursorBrush);
					
					char *Information=(char*)calloc(1145,sizeof(char));//Create a memory space for Information
					SelectObject(hdc,hFont);//Select font for Drawing Text
					Return=TRUE;//Stop Counter Thread
					Sleep(1);
					sprintf(Information,
						"FPS: %.1lf\r\nXYZ: %lf / Unused / %lf\r\nFacing: %lf\r\nDistance to Wall: %lf\r\nDistance to Destination: %lf",//Format
						1000/((Return+5)*1.0),//FPS
						Player.X,Player.Y,//XYZ
						Player.fac,//Facing
						GetDistanceToWall(&Player,NULL,FALSE),//Distance To Wall
						GetDistanceToDestination(&Player)//Distance To Destincation
					);
					RECT rect;rect.left=10;rect.top=10;rect.right=rect.left+500;rect.bottom=rect.top+200;
					SetBkMode(hdc,TRANSPARENT);//Set text background
					DrawText(hdc,Information,strlen(Information),&rect,NULL);//Draw Text
					free(Information);//Release memory
					StretchBlt(MainHdc,0,0,MainWnd.right,MainWnd.bottom,hdc,0,0,900,480,SRCCOPY);//Copy picture from buffer to main window
					if(GetDistanceToDestination(&Player)<=0.5){//If you are in the destination
						SendMessage(hwnd,WM_KILLFOCUS,NULL,NULL);
						PageId=IDP_WIN;
						PostMessage(hwnd,WM_EX_WIN,NULL,NULL);//Tell the program you are win
						break;
					}
					SetTimer(hwnd,IDT_REFRESH,5/*Return+5*/,NULL);
					//BitBlt(MainHdc,0,0,900,480,hdc,0,0,SRCCOPY); 
					break;
				}
				case IDT_KEYBOARDDETECT:{//Detect keyboard in Maze Game
					if(key_press(VK_UP)||key_press('W')) SendMessage(hwnd,WM_EX_KEY,VK_UP,NULL);//Up
					if(key_press(VK_DOWN)||key_press('S')) SendMessage(hwnd,WM_EX_KEY,VK_DOWN,NULL);//Down
					if(key_press(VK_LEFT)||key_press('A')) SendMessage(hwnd,WM_EX_KEY,VK_LEFT,NULL);//Left
					if(key_press(VK_RIGHT)||key_press('D')) SendMessage(hwnd,WM_EX_KEY,VK_RIGHT,NULL);//Right
					if(key_press(VK_ESCAPE)){//Escape
						while(key_press(VK_ESCAPE)) Sleep(10);
						KillTimer(hwnd,IDT_KEYBOARDDETECT);//Stop this timer
						PostMessage(hwnd,WM_EX_KEY,VK_ESCAPE,NULL);
					}
					break;
				}
				case IDT_CLIP:{//Limit cursor area
					RECT rect,LT;
					GetClientRect(hwnd,&rect);
					GetWindowRect(hwnd,&LT);//limit in main window
					rect.left=LT.left;
					rect.top=LT.top;
					rect.right+=LT.left;
					rect.bottom+=LT.top;
					ClipCursor(&rect);
					break;
				}
				case IDT_MENUKEYDET:{//Detect keyboard in Escape Menu
					KillTimer(hwnd,IDT_MENUKEYDET);
					if(PageId!=IDP_ESCAPE) break;
					if(key_press(VK_ESCAPE)){//User pressed Escape key
						while(key_press(VK_ESCAPE)) Sleep(10);
						PostMessage(hwnd,WM_EX_KEY,VK_ESCAPE,NULL);
						break; 
					}
					SetTimer(hwnd,IDT_MENUKEYDET,50,NULL);
					break;
				}
			}
			break;
		}
		case WM_COMMAND:{
			switch(LOWORD(wParam)){
				case IDC_STARTGAME:{//Start Game
					SetTimer(hwnd,IDT_REFRESH,RefreshInterval,NULL);//Graphical Loop
					for(int i=200;i<=300;i++) if(GetDlgItem(hwnd,i)) DestroyWindow(GetDlgItem(hwnd,i));//Remove Controls
					Player.fac=0;
					PageId=IDP_MAZE;
					for(int i=1;i<=MazeCy;i++){//Find the destination and starting point
						for(int j=1;j<=MazeCx;j++){
							if(Maze[i][j]=='D') DestinationX=j,DestinationY=i;
							if(Maze[i][j]=='S') StartX=j,StartY=i;
						}
					}
					Player.X=StartX;//Set Default Position
					Player.Y=StartY;
					PostMessage(hwnd,WM_SETFOCUS,NULL,NULL);//Start Game
					break;
				}
				case IDC_OPTION:{
					OpQ2Page=IDP_MAINMENU;//Set Return to Page
					if(GetDlgItem(hwnd,IDC_STARTGAME)) DestroyWindow(GetDlgItem(hwnd,IDC_STARTGAME));//Remove 5 controls
					if(GetDlgItem(hwnd,IDC_EDIT)) DestroyWindow(GetDlgItem(hwnd,IDC_EDIT));
					if(GetDlgItem(hwnd,IDC_EXIT)) DestroyWindow(GetDlgItem(hwnd,IDC_EXIT));
					if(GetDlgItem(hwnd,IDC_TITLE)) DestroyWindow(GetDlgItem(hwnd,IDC_TITLE));
					if(GetDlgItem(hwnd,IDC_OPTION)) DestroyWindow(GetDlgItem(hwnd,IDC_OPTION));
					PostMessage(hwnd,WM_COMMAND,IDC_ESCOPTION,NULL);//Come to options page
					break;
				}
				case IDC_EXIT:{//Exit from this game
					PostMessage(hwnd,WM_DESTROY,NULL,NULL);
					break;
				}
				case IDC_WINSUPPORT:{//Support button in Win Menu
					WinExec("cmd.exe /C start https://space.bilibili.com/1439352366/",SW_HIDE);
					break;
				} 
				case IDC_WINEXIT:{
					DestroyWindow(GetDlgItem(hwnd,IDC_WINSUPPORT));//Destroy controls to come back to main menu
					DestroyWindow(GetDlgItem(hwnd,IDC_WINEXIT));
					DestroyWindow(GetDlgItem(hwnd,IDC_WINTEXT));
					SendMessage(hwnd,WM_CREATE,NULL,NULL);
					break;
				}
				case IDC_BACKGAME:{//Back to game in Escape Menu
					KillTimer(hwnd,IDT_MENUKEYDET);
					SendMessage(hwnd,WM_EX_KEY,VK_ESCAPE,NULL);
					break;
				}
				case IDC_ESCOPTION:{
					if(GetDlgItem(hwnd,IDC_BACKGAME)) DestroyWindow(GetDlgItem(hwnd,IDC_BACKGAME));//Remove 3 Controls
					if(GetDlgItem(hwnd,IDC_ESCOPTION)) DestroyWindow(GetDlgItem(hwnd,IDC_ESCOPTION));
					if(GetDlgItem(hwnd,IDC_EXITGAME)) DestroyWindow(GetDlgItem(hwnd,IDC_EXITGAME));
					if(PageId==IDP_ESCAPE) OpQ2Page=IDP_ESCAPE;//Set Return to Page
					PageId=IDP_OPTESCAPE;//Change to Option Menu
					CreateWindowEx(NULL,TRACKBAR_CLASS,NULL,WS_CHILD|WS_VISIBLE|TBS_AUTOTICKS,0,0,0,0,hwnd,(HMENU)IDC_ESCPRECISION,NULL,NULL);//Precision
					SendDlgItemMessage(hwnd,IDC_ESCPRECISION,TBM_SETRANGE,TRUE,MAKELPARAM(1,500));//0.001~0.5
					CreateWindowEx(NULL,"STATIC","Precision",WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,0,0,hwnd,(HMENU)IDC_ESCPRETIP,NULL,NULL);//Precision Tip
					CreateWindowEx(NULL,"STATIC","Low",WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,0,0,hwnd,(HMENU)IDC_ESCPRETIPLOW,NULL,NULL);//Precision Tip Low
					CreateWindowEx(NULL,"STATIC","High",WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,0,0,hwnd,(HMENU)IDC_ESCPRETIPHIGH,NULL,NULL);//Precision Tip High
					CreateWindowEx(NULL,"BUTTON","Set Default",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)IDC_ESCPREDEF,NULL,NULL);//Precision Default
					
					SendMessage(GetDlgItem(hwnd,IDC_ESCPRECISION),TBM_SETPOS,TRUE,Precision*1000);//Set Default for Precision
					
					CreateWindowEx(NULL,TRACKBAR_CLASS,NULL,WS_CHILD|WS_VISIBLE|TBS_AUTOTICKS,0,0,0,0,hwnd,(HMENU)IDC_ESCSPEED,NULL,NULL);//Speed
					SendDlgItemMessage(hwnd,IDC_ESCSPEED,TBM_SETRANGE,TRUE,MAKELPARAM(1,200));//0.001~0.2
					CreateWindowEx(NULL,"STATIC","Speed",WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,0,0,hwnd,(HMENU)IDC_ESCSPEEDTIP,NULL,NULL);//Speed Tip
					CreateWindowEx(NULL,"STATIC","Low",WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,0,0,hwnd,(HMENU)IDC_ESCSPEEDTIPLOW,NULL,NULL);//Speed Tip Low
					CreateWindowEx(NULL,"STATIC","High",WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,0,0,hwnd,(HMENU)IDC_ESCSPEEDTIPHIGH,NULL,NULL);//Speed Tip High
					CreateWindowEx(NULL,"BUTTON","Set Default",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)IDC_ESCSPEEDDEF,NULL,NULL);//Precision Default
					
					SendMessage(GetDlgItem(hwnd,IDC_ESCSPEED),TBM_SETPOS,TRUE,Speed*1000);//Set Default for Speed
					
					CreateWindowEx(NULL,TRACKBAR_CLASS,NULL,WS_CHILD|WS_VISIBLE|TBS_AUTOTICKS,0,0,0,0,hwnd,(HMENU)IDC_ESCVISIBILITY,NULL,NULL);//Visibility
					SendDlgItemMessage(hwnd,IDC_ESCVISIBILITY,TBM_SETRANGE,TRUE,MAKELPARAM(10,500));//1.0~50.0
					CreateWindowEx(NULL,"STATIC","Visibility",WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,0,0,hwnd,(HMENU)IDC_ESCVISTIP,NULL,NULL);//Visibility Tip
					CreateWindowEx(NULL,"STATIC","Low",WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,0,0,hwnd,(HMENU)IDC_ESCVISTIPLOW,NULL,NULL);//Visibility Tip Low
					CreateWindowEx(NULL,"STATIC","High",WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,0,0,hwnd,(HMENU)IDC_ESCVISTIPHIGH,NULL,NULL);//Visibility Tip High
					CreateWindowEx(NULL,"BUTTON","Set Default",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)IDC_ESCVISDEF,NULL,NULL);//Precision Default
					
					CreateWindowEx(NULL,"BUTTON","OK",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)IDC_ESCOK,NULL,NULL);//Precision Default
					CreateWindowEx(NULL,"BUTTON","Cancel",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)IDC_ESCCANCEL,NULL,NULL);//Precision Default
					CreateWindowEx(NULL,"BUTTON","Apply",WS_CHILD|WS_VISIBLE|WS_DISABLED,0,0,0,0,hwnd,(HMENU)IDC_ESCAPPLY,NULL,NULL);//Precision Default
					
					SendMessage(GetDlgItem(hwnd,IDC_ESCVISIBILITY),TBM_SETPOS,TRUE,Visibility*10);//Set Default for Visibility
					PostMessage(hwnd,WM_SIZE,NULL,NULL);
					break;
				}
				case IDC_EXITGAME:{//Exit to Main Window
					DestroyWindow(GetDlgItem(hwnd,IDC_BACKGAME));//Remove 3 buttons
					DestroyWindow(GetDlgItem(hwnd,IDC_ESCOPTION));
					DestroyWindow(GetDlgItem(hwnd,IDC_EXITGAME));
					SendMessage(hwnd,WM_CREATE,NULL,NULL);
					break;
				}
				case IDC_ESCPREDEF:{
					EnableWindow(GetDlgItem(hwnd,IDC_ESCAPPLY),TRUE);
					SendMessage(GetDlgItem(hwnd,IDC_ESCPRECISION),TBM_SETPOS,TRUE,0.02*1000);//Set Default for Precision
					break;
				}
				case IDC_ESCSPEEDDEF:{
					EnableWindow(GetDlgItem(hwnd,IDC_ESCAPPLY),TRUE);
					SendMessage(GetDlgItem(hwnd,IDC_ESCSPEED),TBM_SETPOS,TRUE,0.02*1000);//Set Default for Speed
					break;
				}
				case IDC_ESCVISDEF:{
					EnableWindow(GetDlgItem(hwnd,IDC_ESCAPPLY),TRUE);
					SendMessage(GetDlgItem(hwnd,IDC_ESCVISIBILITY),TBM_SETPOS,TRUE,12*10);//Set Default for Visibility
					break;
				}
				case IDC_ESCOK:{
					PostMessage(hwnd,WM_COMMAND,IDC_ESCAPPLY,NULL);//Save and quit
					PostMessage(hwnd,WM_COMMAND,IDC_ESCCANCEL,NULL);
					break;
				}
				case IDC_ESCCANCEL:{//Quit without save
					for(int i=IDC_ESCPRECISION;i<=IDC_ESCAPPLY;i++) DestroyWindow(GetDlgItem(hwnd,i));//Remove some controls
					if(OpQ2Page==IDP_ESCAPE){
						PageId=IDP_MAZE;
						PostMessage(hwnd,WM_EX_KEY,VK_ESCAPE,NULL);//Come back to Escape Menu
					}
					else SendMessage(hwnd,WM_CREATE,NULL,NULL);//Come back to Main Menu
					break;
				}
				case IDC_ESCAPPLY:{//Save without quit
					Precision=SendDlgItemMessage(hwnd,IDC_ESCPRECISION,TBM_GETPOS,NULL,NULL)*1.0/1000;//Save Precision
					Speed=SendDlgItemMessage(hwnd,IDC_ESCSPEED,TBM_GETPOS,NULL,NULL)*1.0/1000;//Save Speed
					Visibility=SendDlgItemMessage(hwnd,IDC_ESCVISIBILITY,TBM_GETPOS,NULL,NULL)*1.0/10;//Save Visibility
					EnableWindow(GetDlgItem(hwnd,IDC_ESCAPPLY),FALSE);//Disable 'Apply' button
					break;
				}
			}
			break;
		}
		case WM_HSCROLL:{//Enable 'Apply' button
			switch(PageId){
				case IDP_OPTESCAPE:{
					EnableWindow(GetDlgItem(hwnd,IDC_ESCAPPLY),TRUE);
					break;
				}
			}
			break;
		}
		case WM_NEXTDLGCTL:{
			if(LOWORD(lParam)==FALSE){
				SetFocus((HWND)wParam);
			}
			else{
				SetFocus(GetDlgItem(hwnd,wParam));
			}
			break;
		}
		case WM_GETDLGCODE:{
			return DLGC_WANTTAB;
			break;
		}
		case WM_SETFOCUS:{//Message for set Timer
			switch(PageId){
				case IDP_MAZE:{
					//SetTimer(hwnd,IDT_CLIP,50,NULL);
					SetTimer(hwnd,IDT_KEYBOARDDETECT,50,NULL);
					break;
				}
				case IDP_MAINMENU:{
					SetFocus(GetNextDlgTabItem(hwnd, (HWND)wParam, FALSE));
					break;
				}
			}
			break;
		}
		case WM_KILLFOCUS:{//Message for stop Timer
			switch(PageId){
				case IDP_MAZE:{
					//KillTimer(hwnd,IDT_CLIP);
					KillTimer(hwnd,IDT_KEYBOARDDETECT);
					break;
				}
			}
			break;
		}
		case WM_MOUSEMOVE:{//Message tell program the mouse moved
			switch(PageId){
				case IDP_MAZE:{//If mouse move in maze
					//ShowCursor(FALSE);
					if(LOWORD(lParam)!=LastX){
						Player.fac=ToPositiveAngle(Player.fac-LastX*0.5+LOWORD(lParam)*0.5);//Chage Angle
						LastX=LOWORD(lParam);//Set Last Position of cursor
						RECT rect,LT;//Get size,position of window
						GetClientRect(hwnd,&rect);
						GetWindowRect(hwnd,&LT);
						rect.left=LT.left;
						rect.top=LT.top;
						rect.right+=LT.left;
						rect.bottom+=LT.top;
						if(LastX<10){//If the cursor are in left
							LastX=rect.right-rect.left-10;
							SetCursorPos(rect.right-10,(rect.bottom-rect.top)/2+rect.top);//Take it to right
							break;
						}
						if(rect.right-LT.left-10<=LastX){//If the cursor are in right
							LastX=20;
							SetCursorPos(LT.left+20,(rect.bottom-rect.top)/2+rect.top);//Take it to left
							break;
						}
					}
					break;
				}
				case IDP_ESCAPE:{//Set Last Potition of cursor
					LastX=LOWORD(lParam);//Make the picture will not rotate
					break;
				}
			}
			break;
		}
		case WM_EX_KEY:{
			switch(wParam){
				case VK_UP:{//Walk to front
					LOCATION loc;
					loc.X=Player.X;
					loc.Y=Player.Y;
					loc.fac=ToPositiveAngle(Player.fac);
					WalkFront(&loc);//Move
					if(Maze[int(round(Player.Y))][int(round(loc.X))]!='W') Player.X=loc.X;
					if(Maze[int(round(loc.Y))][int(round(Player.X))]!='W') Player.Y=loc.Y;
					break;
				}
				case VK_DOWN:{//Walk to back
					LOCATION loc;
					loc.X=Player.X;
					loc.Y=Player.Y;
					loc.fac=ToPositiveAngle(Player.fac+180);
					WalkFront(&loc);//Move
					if(Maze[int(round(Player.Y))][int(round(loc.X))]!='W') Player.X=loc.X;
					if(Maze[int(round(loc.Y))][int(round(Player.X))]!='W') Player.Y=loc.Y;
					break;
				}
				case VK_LEFT:{//Walk to left
					LOCATION loc;
					loc.X=Player.X;
					loc.Y=Player.Y;
					loc.fac=ToPositiveAngle(Player.fac-90);
					WalkFront(&loc);//Move
					if(Maze[int(round(Player.Y))][int(round(loc.X))]!='W') Player.X=loc.X;
					if(Maze[int(round(loc.Y))][int(round(Player.X))]!='W') Player.Y=loc.Y;
					break;
				}
				case VK_RIGHT:{//Walk to right
					LOCATION loc;
					loc.X=Player.X;
					loc.Y=Player.Y;
					loc.fac=ToPositiveAngle(Player.fac+90);
					WalkFront(&loc);//Move 
					if(Maze[int(round(Player.Y))][int(round(loc.X))]!='W') Player.X=loc.X;
					if(Maze[int(round(loc.Y))][int(round(Player.X))]!='W') Player.Y=loc.Y;
					break;
				}
				case VK_ESCAPE:{//Escape Menu
					if(PageId==IDP_MAZE){//If it's in Maze Game
						SendMessage(hwnd,WM_KILLFOCUS,NULL,NULL);
						PageId=IDP_ESCAPE;
						KillTimer(hwnd,IDT_REFRESH);//Stop Renderner
						CreateWindowEx(NULL,"BUTTON","Back to game...",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)IDC_BACKGAME,NULL,NULL);//Create Buttons
						CreateWindowEx(NULL,"BUTTON","Options...",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)IDC_ESCOPTION,NULL,NULL);
						CreateWindowEx(NULL,"BUTTON","Exit to Main Menu...",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)IDC_EXITGAME,NULL,NULL);
						SetTimer(hwnd,IDT_MENUKEYDET,50,NULL);//Turn to Escape Menu
						PostMessage(hwnd,WM_SIZE,NULL,NULL);
						RECT MainWnd;
						GetClientRect(hwnd,&MainWnd);
					}
					else if(PageId==IDP_ESCAPE){//If it's in Escape menu
						DestroyWindow(GetDlgItem(hwnd,IDC_BACKGAME));//Destroy 3 buttons in Sscape menu
						DestroyWindow(GetDlgItem(hwnd,IDC_EXITGAME));
						DestroyWindow(GetDlgItem(hwnd,IDC_ESCOPTION));
						PageId=IDP_MAZE;//Turn to maze game
						SetTimer(hwnd,IDT_REFRESH,RefreshInterval,NULL);
						PostMessage(hwnd,WM_SETFOCUS,NULL,NULL);
					}
					break;
				}
			}
			break;
		}
		case WM_EX_WIN:{//Win Menu
			//Create controls
			CreateWindowEx(NULL,"STATIC","Congratulations! You won just now!",WS_CHILD|WS_VISIBLE|SS_CENTER,0,0,0,0,hwnd,(HMENU)IDC_WINTEXT,NULL,NULL);
			/*CreateWindowEx(NULL,"STATIC",NULL,WS_CHILD|WS_VISIBLE|SS_ICON,0,0,0,0,hwnd,(HMENU)IDC_WINICON,NULL,NULL);
			SendDlgItemMessage(hwnd,IDC_WINICON,STM_SETICON,(WPARAM)LoadIcon(0,IDI_INFORMATION),NULL); */
			CreateWindowEx(NULL,"BUTTON","Exit",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)IDC_WINEXIT,NULL,NULL);
			CreateWindowEx(NULL,"BUTTON","Support Me",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)IDC_WINSUPPORT,NULL,NULL);
			SendMessage(hwnd,WM_SIZE,NULL,NULL);//Setting control's position
			SendMessage(hwnd,WM_PAINT,NULL,NULL);//Redraw window
			break;
		}
		
		case WM_CTLCOLORSTATIC:{//Color for Static
			static HBRUSH hbrBackground;
			SetBkColor((HDC)wParam,RGB(255,255,255));
			RECT rect;
			GetClientRect((HWND)lParam,&rect);
			SelectObject((HDC)wParam,CreateFontA((int)rect.bottom*0.8,0,0,0,0,0,0,0,0,0,0,0,0,"Consolas"));
			if(hbrBackground==NULL) hbrBackground=CreateSolidBrush(RGB(255,255,255));//Back=RGB(255,255,255)
			return (HRESULT)hbrBackground;
			break;
		}
		
		case WM_ACTIVATE:{//When Windows activated thie window
			if(wParam!=WA_INACTIVE) PostMessage(hwnd,WM_SETFOCUS,NULL,NULL);
			else if(PageId==IDP_MAZE){
				PostMessage(hwnd,WM_KILLFOCUS,NULL,NULL);
				PostMessage(hwnd,WM_EX_KEY,VK_ESCAPE,NULL);
			}
			break;
		}
		
		case WM_DRAWITEM:{//Draw Static Controls
			LPDRAWITEMSTRUCT lpDis=(LPDRAWITEMSTRUCT)lParam;
			switch(lpDis->CtlType){
				case ODT_STATIC:{
					if(lpDis->itemAction&ODA_DRAWENTIRE||lpDis->itemAction&ODA_FOCUS||lpDis->itemAction&ODA_SELECT){
						RECT rect;HDC hStatic=GetDC(lpDis->hwndItem); 
						char Text[1145];
						GetWindowText(lpDis->hwndItem,Text,1140);
						GetClientRect(lpDis->hwndItem,&rect);
						FillRect(hStatic,&rect,(HBRUSH)SendMessage(GetParent(lpDis->hwndItem),WM_CTLCOLORSTATIC,(WPARAM)hStatic,(LPARAM)lpDis->hwndItem));
						DrawText(hStatic,Text,strlen(Text),&rect,DT_CENTER|DT_VCENTER);
					}
					break;
				}
			}
			break;
		}
		
		/* Upon destruction, tell the main thread to stop */
		case WM_DESTROY: {//Close Game
			PostQuitMessage(0);
			break;
		}
		
		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

DWORD WINAPI MsgThread(LPVOID lparam){
	LPSERVER lpServ=(LPSERVER)lparam;
	while(1){
		while(lpServ->IsEmpty(lpServ->MsgQueue)) Sleep(1);
		while(lpServ->IsEmpty(lpServ->MsgQueue)==FALSE){
			LPMESSAGE lpMsg=lpServ->Front(lpServ->MsgQueue);
			switch(lpMsg->Msg){
				case MTG_DIRECTION:{
					*((FLOAT2*)lpMsg->lParam1)=Player.fac;
					lpMsg->Result=TRUE;
					break;
				}
				case MTG_LOCATION:{
					*((FLOAT2*)lpMsg->lParam1)=Player.X;
					*((FLOAT2*)lpMsg->lParam2)=Player.Y;
					lpMsg->Result=TRUE;
					break;
				}
				case MTG_PAGEID:{
					*((LPDWORD)lpMsg->lParam1)=PageId;
					lpMsg->Result=TRUE;
					break;
				}
				case MTG_WNDRECT:{
					if(lpMsg->lParam1==TRUE){
						GetClientRect(MainWnd,(LPRECT)lpMsg->lParam2);
					}
					else{
						GetWindowRect(MainWnd,(LPRECT)lpMsg->lParam2);
					}
					*((FLOAT2*)lpMsg->lParam2)=Player.Y;
					lpMsg->Result=TRUE;
					break;
				}
				case MTG_PRECISION:{
					*((LPDWORD)lpMsg->lParam1)=Precision;
					lpMsg->Result=TRUE;
					break;
				}
				case MTG_SPEED:{
					*((LPDWORD)lpMsg->lParam1)=Speed;
					lpMsg->Result=TRUE;
					break;
				}
				case MTG_VISIBILITY:{
					*((LPDWORD)lpMsg->lParam1)=Visibility;
					lpMsg->Result=TRUE;
					break;
				}
				case MTS_DIRECTION:{
					Player.fac=ToPositiveAngle(*((FLOAT2*)lpMsg->lParam1));
					lpMsg->Result=TRUE;
					break;
				}
				case MTS_LOCATION:{
					Player.X=*((FLOAT2*)lpMsg->lParam1);
					Player.Y=*((FLOAT2*)lpMsg->lParam2);
					lpMsg->Result=TRUE;
					break;
				}
			}
			lpServ->Pop(lpServ->MsgQueue);
		}
	}
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	typedef BOOL (WINAPI *SPDA)(VOID);
	SPDA SetProcessDPIAware;
	HMODULE hModule = LoadLibrary("user32.dll");//Load library
	if(hModule){//Normal(How does your Windows work if it doesn't exist?)
		SetProcessDPIAware = (SPDA)GetProcAddress(hModule,"SetProcessDPIAware");//Load Function in library
		if(SetProcessDPIAware) SetProcessDPIAware();//Run function
	}
	if(GetFileAttributes("Mods")!=INVALID_FILE_ATTRIBUTES){
		CreateServer(&MainModServer);
		WIN32_FIND_DATA wfd;
		HANDLE hFindFile=FindFirstFile("Mods\\*.dll",&wfd);
		char PathToMod[114514],PathToHere[114514];
		GetModuleFileName(0,PathToHere,114510);
		for(int i=strlen(PathToHere)-1;i>=0&&PathToHere[i]!='\\';i--) PathToHere[i]='\0';
		do{
			if(strcmp(PathToMod,".")==0&&strlen(PathToMod)==1||strcmp(PathToMod,"..")==0&&strlen(PathToMod)==2) continue;
			sprintf(PathToMod,"%sMods\\%s",PathToHere,wfd.cFileName);
			hMods[ModCnt++]=LoadMod(&MainModServer,PathToMod);
		}while(FindNextFile(hFindFile,&wfd));
	}
	WNDCLASSEXA wc; /* A properties struct of our window */
	MSG msg; /* A temporary location for all messages */

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEXA);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "Maze3D";
	wc.hIcon		 = LoadIcon(hInstance,"A"); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(hInstance,"A"); /* use the name "A" to use the project icon */

	if(!RegisterClassEx(&wc)) {//If the program cannot register the window class
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	wc.lpszClassName = "EmptyWindow";
	wc.lpfnWndProc   = EWProc;

	if(!RegisterClassEx(&wc)) {//If the program cannot register the empty window class
		MessageBox(NULL, "Empty Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	MainWnd = CreateWindowEx(NULL,"Maze3D","Maze 3D",WS_VISIBLE|WS_OVERLAPPEDWINDOW,//Create Window
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		900, /* width */
		500, /* height */
		NULL,NULL,hInstance,NULL);

	if(MainWnd == NULL) {//If the program cannot create the window
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	/*
		This is the heart of our program where all input is processed and 
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while(GetMessage(&msg,NULL,0,0)>0) { /* If no error is received... */
		if(IsDialogMessage(NULL,&msg)==FALSE){
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
	}//Message Loop
	return msg.wParam;//Return value 
}
