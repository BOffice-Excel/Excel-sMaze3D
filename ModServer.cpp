/* Replace "dll.h" with the name of your header */
#include "ModServer.h"
#include <windows.h>
VOID Reset(LPQUEUE lpQueue){
	lpQueue->Last=lpQueue->Head=(LPNODE)calloc(1,sizeof(NODE));
	lpQueue->Head->Next=NULL;
	lpQueue->Head->Data=NULL;
}
LPNODE Push(LPQUEUE lpQueue,LPMESSAGE Data){
	LPNODE Tmp=new NODE;
	lpQueue->Last->Next=Tmp;
	lpQueue->Last=Tmp;
	Tmp->Next=NULL;
	Tmp->Data=(LPMESSAGE)calloc(1,sizeof(MESSAGE));
	Tmp->Data->Msg=Data->Msg;
	Tmp->Data->lParam1=Data->lParam1;
	Tmp->Data->lParam2=Data->lParam2;
	Tmp->Data->lParam3=Data->lParam3;
	Tmp->Data->Result=MSG_UNHANDLE;
	return Tmp;
}
VOID Pop(LPQUEUE lpQueue){
	LPNODE Tmp=lpQueue->Head->Next;
	free(lpQueue->Head);
	lpQueue->Head=Tmp;
}
LPMESSAGE Front(LPQUEUE lpQueue){
	if(lpQueue->Head->Next!=NULL) return lpQueue->Head->Next->Data;
	return NULL; 
}
BOOL IsEmpty(LPQUEUE lpQueue){
	return ((lpQueue->Head==lpQueue->Last)?TRUE:FALSE);
}
LRESULT SendMsg(LPQUEUE lpQueue,UINT Message,LPARAM lParam1,LPARAM lParam2,LPARAM lParam3){
	MESSAGE Msg;
	Msg.Msg=Message;
	Msg.lParam1=lParam1;
	Msg.lParam2=lParam2;
	Msg.lParam3=lParam3;
	LPNODE NewMsg=Push(lpQueue,&Msg);
	while(NewMsg->Data->Result==MSG_UNHANDLE) Sleep(10);
	return NewMsg->Data->Result;
}

extern "C" DLLIMPORT VOID CreateServer(LPSERVER lpServer){
	lpServer->Front=Front;
	lpServer->IsEmpty=IsEmpty;
	lpServer->MsgQueue=(LPQUEUE)calloc(1,sizeof(QUEUE));
	Reset(lpServer->MsgQueue);
	lpServer->Pop=Pop;
	lpServer->Push=Push;
	lpServer->Reset=Reset;
	lpServer->SendMsg=SendMsg;
	return ;
}

extern "C" DLLIMPORT HMODULE LoadMod(LPSERVER lpServer,LPCSTR lpstrMod){
	HMODULE res=LoadLibrary(lpstrMod);
	if(res==NULL) return NULL;
	INIT Init=(INIT)GetProcAddress(res,"Init");
	if(Init(lpServer)==TRUE) return res;
	return NULL;
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
