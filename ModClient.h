#ifndef MODCLIENT
#define MODCLIENT

#ifndef DLLIMPORT
#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif
#endif

#endif
