#ifndef MODSERVER
#define MODSERVER

#include<windows.h>
#ifndef DLLIMPORT
#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif
#endif

#define MSG_UNHANDLE (-(0xFFFFFFFF))
struct MESSAGE{
	UINT Msg;
	LPARAM lParam1,lParam2,lParam3;
	LRESULT Result;
};
typedef MESSAGE *LPMESSAGE;
struct NODE{
	LPMESSAGE Data;
	NODE *Next;
};
typedef NODE *LPNODE;
struct QUEUE{
	LPNODE Head,Last;
};
typedef QUEUE *LPQUEUE;
typedef VOID (*RESET)(LPQUEUE);
typedef LPNODE (*PUSH)(LPQUEUE,LPMESSAGE);
typedef VOID (*POP)(LPQUEUE);
typedef LPMESSAGE (*FRONT)(LPQUEUE);
typedef BOOL (*ISEMPTY)(LPQUEUE);
typedef LRESULT (*SENDMSG)(LPQUEUE,UINT,LPARAM,LPARAM,LPARAM);
typedef BOOL (*INIT)(HANDLE);
struct SERVER{
	LPQUEUE MsgQueue;
	RESET Reset;
	PUSH Push;
	POP Pop;
	FRONT Front;
	ISEMPTY IsEmpty;
	SENDMSG SendMsg;
};
typedef SERVER *LPSERVER;

#ifndef MODCLIENT
extern "C" DLLIMPORT HMODULE LoadMod(LPSERVER lpServer,LPCSTR lpstrMod);
extern "C" DLLIMPORT VOID CreateServer(LPSERVER lpServer);
#endif
#endif
