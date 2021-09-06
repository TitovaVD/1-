#pragma warning(disable : 4996)

#define WINVER 0x0A00
//#define PIPE_EXE_NAME _T("p64.exe")
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <fileapi.h>
#include <profileapi.h>
//#include <io.h> 
//#include <fcntl.h>

#define BUFSIZE MAX_PATH
#define FILESYSNAMEBUFSIZE MAX_PATH
#define SIZE_BUFF 256
#define MAX_LENGTH 255

using namespace std;

double WFreq=0.0;
__int64 Counter_Start = 0;

double Query()
{
    LARGE_INTEGER Frequency;
    QueryPerformanceCounter(&Frequency);
    Counter_Start = Frequency.QuadPart;
    if (!QueryPerformanceFrequency(&Frequency))
        cout << "Некорректно выполнена функция QueryPerformanceFrequency()!\n";
        WFreq = double(Frequency.QuadPart);
    printf("\nРабочая частота ЦП: %u Гц\n", Frequency);
    QueryPerformanceCounter(&Frequency);
    return double ((Frequency.QuadPart - Counter_Start) * 1000000) / WFreq;
}

int main()
{
    HANDLE hFindVol;
    bool hCloseVol, hNextVol;
    //1.1 Определение версии ОС:
    OSVERSIONINFO osVer = { 0 };
    osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osVer);
    const signed majornum = osVer.dwMajorVersion;
    const signed minornum = osVer.dwMinorVersion;
    const signed buildnum = osVer.dwBuildNumber;
    setlocale(LC_ALL, "Russian");
    printf("Версия ОС: v%d.%d.%d \n", majornum, minornum, buildnum);
   
    DWORD bufChar = SIZE_BUFF;
    TCHAR Buf[SIZE_BUFF];

    //1.2 Определение системной директории:
    GetSystemDirectory( Buf, SIZE_BUFF);
    printf("Системная директория : %s \n", Buf);

    //1.3 Определение названия компьютера и имени пользователя:
    GetComputerName(Buf, &bufChar);
    printf("Название компьютера: %s \n", Buf);

    GetUserName(Buf, &bufChar);
    printf("Имя пользователя: %s\n", Buf);

    //1.4 Вывод характеристик томов:
    TCHAR buff[MAX_PATH];
    DWORD CBufLen = MAX_PATH;
    TCHAR name[MAX_PATH];
    CHAR VolumeName[MAX_PATH];

    HANDLE search = FindFirstVolume(name, BUFSIZE);
    ULARGE_INTEGER total, free;
   
    do {
            GetVolumePathNamesForVolumeName(name, buff, CBufLen, &CBufLen);
            printf("\nИмя тома:");
            printf(name);
            printf("\nПервый путь в томе:");
            printf(buff);
            if (GetDiskFreeSpaceEx(name,&free ,&total,NULL) == 0)
            {
                printf("\nНет информации\n");
            }
            else
            {
                cout << "\nОбъём тома:" << total.QuadPart;
                cout << "\nКоличество свободного места:" << free.QuadPart<<"\n";
            }
    } while (FindNextVolume(search, name, BUFSIZE)==1);
    FindVolumeClose(search);
  //1.5 Вывод списка программ, запускающихся при старте системы:
    DWORD dw_size;
    TCHAR sz_name[MAX_LENGTH];
    HKEY key;
    DWORD index = 0;
    DWORD retCode;
    DWORD buffSize = 8192;
    DWORD PerfData = (DWORD)malloc(buffSize);
    DWORD cbData = buffSize;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run",0, KEY_ALL_ACCESS, &key) == !ERROR_SUCCESS)
        printf("Некорректно выполнена функция RegOpenKeyEx()!\n");
    else 
        printf("\nСписок программ:\n");

    while (1) 
    {
        dw_size = sizeof(sz_name);
        retCode = RegEnumValue(key, index, sz_name, &dw_size, NULL, NULL, NULL, NULL);
        if (retCode == ERROR_SUCCESS)
        {
            RegQueryValueEx(key, sz_name, NULL, NULL, (LPBYTE)PerfData, &cbData);
            printf("%d: %s:  %s\n", index + 1, sz_name, PerfData);
            index++;
        }
        else break;
    }
    RegCloseKey(key);
    cout << "Измерение частоты ЦП происходит за " << Query() << "  мкс \n";
}
