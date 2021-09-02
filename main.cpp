#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <psapi.h>
#include <shlwapi.h>

using std::cout;
using std::wcout;


void tolwr(TCHAR* str, TCHAR* target)
{
    TCHAR b;
    int i = 0;
    while((b = str[i]))
    {
        target[i] = _towlower_l(b, _get_current_locale());
        i++;
    }
}

int main()
{
    DWORD aProcesses[1024], cbNeeded, cProcesses, pID;

    if(!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
        return 1;

    cProcesses = cbNeeded / sizeof(DWORD);

    for(unsigned int i = 0; i < cProcesses; i++)
    {
        if((pID = aProcesses[i]) != 0)
        {
            cout << "Handling PID: " << pID << '\n';

            TCHAR szProcessPath[MAX_PATH];
            TCHAR lwrPath[MAX_PATH];
            HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                           PROCESS_VM_READ |
                                           PROCESS_TERMINATE,
                                           FALSE, pID );

            if(hProcess != NULL)
            {
                HMODULE hMod;
                DWORD cbNeeded;

                if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod),
                     &cbNeeded) )
                {
                    GetProcessImageFileName( hProcess, szProcessPath,
                                             sizeof(szProcessPath)/sizeof(TCHAR) );

                    tolwr(szProcessPath, lwrPath);
                    bool d = StrStrW(lwrPath, TEXT("adobe"));
                    wcout << "strstr returned " << (d ? "true" : "false") << " (\"" << lwrPath << "\")\n";
                    if(d)
                    {
                        wcout << "Killing process: " << pID << " (" << szProcessPath << ")" << std::endl;
                        int r = TerminateProcess(hProcess, 1);
                        if(r == 0)
                            cout << "Failed to kill process: " << pID;
                    }
                }
            } else
            {
                cout << "Process handle null for PID: " << pID << '\n';
            }

            cout << '\n';
            CloseHandle(hProcess);
        }
    }
}
