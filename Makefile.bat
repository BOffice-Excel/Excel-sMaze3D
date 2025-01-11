@echo off
rem Please Replace all the 'C:\Users\ywh11\AppData\Local\Programs\Dev-Cpp\MinGW64' to your gcc compiler path
echo Compiling Project Files...
set COM_PATH=C:\Users\ywh11\AppData\Local\Programs\Dev-Cpp\MinGW64\bin
echo ---------------------------------------------------

echo Compiling 'ModServer.dll'
echo Compiling 'ModServer.cpp' to 'ModServer.o'
%COM_PATH%\g++.exe -c ModServer.cpp -o ModServer.o -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/include" -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/x86_64-w64-mingw32/include" -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include" -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include/c++" -m32 -lgdi32 -luser32 -lmsimg32 -luxtheme -lws2_32 -DBUILDING_DLL=1
echo Compiling 'ModServer.o' to 'ModServer.dll'
%COM_PATH%\g++.exe -shared ModServer.o -o ModServer.dll -L"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/lib32" -L"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib32" -static-libgcc -O2 -s -flto -m32 -Wl,--output-def,libModServer.def,--out-implib,libModServer.a,--add-stdcall-alias
echo Compiled 'ModServer.dll' successfully.
echo ---------------------------------------------------

echo Compiling 'ModClient.dll'
echo Compiling 'ModClient.cpp' to 'ModClient.o'
%COM_PATH%\g++.exe -c ModClient.cpp -o ModClient.o -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/include" -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/x86_64-w64-mingw32/include" -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include" -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include/c++" -m32 -lgdi32 -luser32 -lmsimg32 -luxtheme -lws2_32 -DBUILDING_DLL=1
echo Compiling 'ModClient.o' to 'ModClient.dll'
%COM_PATH%\g++.exe -shared ModClient.o -o ModClient.dll -L"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/lib32" -L"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib32" -static-libgcc -O2 -s -flto -m32 -Wl,--output-def,libModClient.def,--out-implib,libModClient.a,--add-stdcall-alias
echo Compiled 'ModClient.dll' successfully.
echo ---------------------------------------------------

echo Compiling 'Maze3D.exe'
echo Compiling Resource File...
%COM_PATH%\windres.exe -i Maze3D_private.rc -F pe-i386 --input-format=rc -o Maze3D_private.res -O coff 
echo Compiling 'main.cpp' to 'main.o'...
%COM_PATH%\g++.exe -c main.cpp -o main.o -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/include" -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/x86_64-w64-mingw32/include" -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include" -I"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include/c++" -m32 -lgdi32 -luser32 -lmsimg32 -luxtheme -lws2_32 -std=c++11
echo Compiling 'main.o' to 'Maze3D.exe'...
%COM_PATH%\g++.exe main.o Maze3D_private.res -o Maze3D.exe -L"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/lib32" -L"C:/Users/ywh11/AppData/Local/Programs/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib32" -static-libgcc -O2 -s -flto -mwindows -m32 libModServer.a
echo Compiled 'Maze3D.exe' successfully.
echo ---------------------------------------------------

echo Compiled Project successfully!
pause