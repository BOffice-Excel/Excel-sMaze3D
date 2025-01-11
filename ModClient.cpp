/* Replace "dll.h" with the name of your header */
#include "ModClient.h"
#include "ModServer.h"
#include "Maze3D.h"
#include <windows.h>

LPSERVER TargetServer;

extern "C" DLLIMPORT BOOL SetPlayerPosition(double X,double Y){
	return (BOOL)TargetServer->SendMsg(TargetServer->MsgQueue,MTS_LOCATION,(LPARAM)&X,(LPARAM)&Y,NULL);
} 

extern "C" DLLIMPORT VOID GetPlayerPosition(double *lpX,double *lpY){
	TargetServer->SendMsg(TargetServer->MsgQueue,MTG_LOCATION,(LPARAM)lpX,(LPARAM)lpY,NULL);
} 

extern "C" DLLIMPORT BOOL SetPlayerDirection(double Direction){
	return (BOOL)TargetServer->SendMsg(TargetServer->MsgQueue,MTS_DIRECTION,(LPARAM)&Direction,NULL,NULL);
} 

extern "C" DLLIMPORT VOID GetPlayerDirection(double *lpDirection){
	TargetServer->SendMsg(TargetServer->MsgQueue,MTG_LOCATION,(LPARAM)lpDirection,NULL,NULL);
} 

extern "C" DLLIMPORT VOID GetPlayerFPS(double *lpFPS){
	TargetServer->SendMsg(TargetServer->MsgQueue,MTG_FPS,(LPARAM)lpFPS,NULL,NULL);
} 

extern "C" DLLIMPORT VOID GetPlayerPageId(LPUINT lpPageId){
	TargetServer->SendMsg(TargetServer->MsgQueue,MTG_PAGEID,(LPARAM)lpPageId,NULL,NULL);
} 

extern "C" DLLIMPORT BOOL GetWindowClientRect(LPRECT lpRect){
	return (BOOL)TargetServer->SendMsg(TargetServer->MsgQueue,MTG_WNDRECT,(LPARAM)lpRect,NULL,NULL);
} 

extern "C" DLLIMPORT BOOL RegisterModifyMenuFunc(UINT PageId,MENUMODPROC MenuModProc,LPARAM lParam){
	return (BOOL)TargetServer->SendMsg(TargetServer->MsgQueue,MTC_MENU,PageId,(LPARAM)MenuModProc,lParam);
}

extern "C" DLLIMPORT VOID SetFPSLimit(double FPS){
	TargetServer->SendMsg(TargetServer->MsgQueue,MTS_FPSLIMIT,(LPARAM)&FPS,0,0);
}

extern "C" DLLIMPORT BOOL RegisterRenderFunc(RENDERPROC RenderProc,LPARAM lParam){
	return (BOOL)TargetServer->SendMsg(TargetServer->MsgQueue,MTC_RENDER,(LPARAM)RenderProc,lParam,0);
}

extern "C" DLLIMPORT BOOL ModifyMazeMapNow(MODMAZEPROC ModMazeProc,LPARAM lParam){
	return (BOOL)TargetServer->SendMsg(TargetServer->MsgQueue,MTC_MAZEMAP,(LPARAM)ModMazeProc,lParam,0);
}

extern "C" DLLIMPORT VOID InitModClient(HANDLE hServer){
	TargetServer=(LPSERVER)hServer;
	return ;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			break;
		}
		case DLL_THREAD_ATTACH:
		{
			break;
		}
		case DLL_THREAD_DETACH:
		{
			break;
		}
	}
	
	/* Return TRUE on success, FALSE on failure */
	return TRUE;
}
