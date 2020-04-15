#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include "sqlite3.h"
#include <commctrl.h>
#include <stdio.h>
#define SQL_UPIT 100
#define WORK 101
#define RESUT_INFO 102
#define SHOW_INFO_SUCCESS 103
#define SHOW_INFO_DONE 104
#define SHOW_INFO_ERROR 105
#define ID_LIST 106
#include <queue>
#include <string>
#include <iostream>

/*  This function is called by the Windows function DispatchMessage()  */

sqlite3 *db;
HWND info;
int first_line=1;
int size_column,num_col=0;
int select_query = 0;
char* err = 0;
static HWND hwndlist;
std::string ostatak;
std::string upit;
int callback(void *pArg, int argc, char **argv, char **imekolone);

LRESULT CALLBACK MainWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT clientRect;
    HINSTANCE hinst ;
    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
    {
        GetClientRect(hwnd,&clientRect);
        hinst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
        CreateWindow(
            "edit",
            "",
            WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE |WS_VSCROLL,
            0,
            0,
            clientRect.right,
            clientRect.bottom -100,
            hwnd,
            (HMENU)SQL_UPIT,
            hinst,
            NULL);
        CreateWindow("button", "Work", WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_PUSHLIKE,
                     (clientRect.right-140)/2,clientRect.bottom - 70,140,45, hwnd,(HMENU)WORK, hinst, NULL);

        break;
    }
    case WM_COMMAND:
    {
        switch(LOWORD(wParam))
        {
        case WORK:
        {
            sqlite3_open("baza", &db);
            int status;

            int counter = 0;
            char sql[1000];
            HWND hwndedit = GetDlgItem(hwnd, SQL_UPIT);
            memset(&sql,'\0',sizeof(sql)/sizeof(sql[0]));
            GetWindowText(hwndedit, sql, sizeof(sql));

            if(strlen(sql) == 0 )
            {
                break;
            }

            int begins = 0;
            for(unsigned int i = 0; i<strlen(sql); ++i)
            {
                if(sql[i] == ';' && sql[i+2] == '\n')
                {
                    while(sql[i+2] == '\n')
                        ++i;
                    upit = std::string(sql,0,i+2);
                    begins = i+2;
                    ostatak = std::string(sql,begins,strlen(sql)-begins);
                    strcpy(sql,upit.c_str());
                    break;
                }
                else if(sql[i] == ';' || sql[i] == '\n')
                {
                    while(sql[i+1] == '\n')
                        ++i;
                    upit = std::string(sql,0,i+1);
                    begins = i+1;
                    ostatak = std::string(sql,begins,strlen(sql)-begins);
                    strcpy(sql,upit.c_str());
                    break;
                }
            }

            printf(sql);
            printf("\n");


            for(unsigned int i = 0; i< strlen(sql); ++i)
            {
                while(sql[i] == ' ' || sql[i] == '\t')
                    ++i;
                if( (sql[i] == 'S' ||sql[i] == 's') &&
                        (sql[i+1] == 'E' ||sql[i+1] == 'e') &&
                        (sql[i+2] == 'L' ||sql[i+2] == 'l') &&
                        (sql[i+3] == 'E' ||sql[i+3] == 'e') &&
                        (sql[i+4] == 'C' ||sql[i+4] == 'c') &&
                        (sql[i+5] == 'T' ||sql[i+5] == 't'))
                {
                    select_query = 1;
                }
                break;
            }
            if(select_query == 1)
            {
                ListView_DeleteAllItems(hwndlist);
                int i = num_col;
                while(i>=0)
                {
                    ListView_DeleteColumn(hwndlist,i);
                    --i;
                }
                num_col = 0;
                first_line = 1;
                status = sqlite3_exec(db, sql, callback, 0, &err);
            }
            else
            {
                ListView_DeleteAllItems(hwndlist);
                int i = num_col;
                while(i>=0)
                {
                    ListView_DeleteColumn(hwndlist,i);
                    --i;
                }
                status = sqlite3_exec(db, sql, 0, 0, &err);
            }

            if(status == SQLITE_OK)
            {
                sqlite3_free(err);
                sqlite3_close(db);

                SendMessage(info,WM_USER,SHOW_INFO_SUCCESS,0);
                upit.clear();
                SetWindowText(hwndedit,ostatak.c_str());
                select_query = 0;
                ostatak.clear();
                break;
            }
            else if (status == SQLITE_CONSTRAINT)
            {
                SendMessage(info,WM_USER,SHOW_INFO_DONE,0);
                sqlite3_free(err);
                sqlite3_close(db);
                break;
            }
            else
            {
                SendMessage(info,WM_USER,SHOW_INFO_ERROR,0);
                sqlite3_free(err);
                sqlite3_close(db);
                break;
            }
            break;
        }
        }
        break;
    }
    case WM_DESTROY:
        sqlite3_close(db);
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
/*  This function is called by the Windows function DispatchMessage()  */

int callback(void *pArg, int argc, char **argv, char **imekolone)
{
    LVITEM lvi;
    if(first_line)
    {
        LVCOLUMN lvc;
        num_col = argc - 1;
        for(int i = 0; i<argc; ++i)
        {
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM ;
            lvc.fmt = LVCFMT_LEFT;
            lvc.cx = size_column / argc;
            lvc.pszText = imekolone[i];
            lvc.iSubItem = i;
            ListView_InsertColumn(hwndlist,i,&lvc);
        }
        first_line = 0;
    }
    memset(&lvi,0,sizeof(lvi));
    lvi.mask = LVIF_TEXT;
    lvi.pszText = argv[0];
    lvi.iSubItem = 0;
    ListView_InsertItem(hwndlist, &lvi);
    printf("|\t%s\t", argv[0]);
    for (int i = 1; i < argc; i++)
    {
        printf("|\t%s\t", argv[i]);
        lvi.pszText = argv[i];
        lvi.iSubItem = i;
        ListView_SetItem(hwndlist, &lvi);
    }
    printf("| \n");
    return 0;
}
LRESULT CALLBACK SecondWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT clientRect;
    HINSTANCE hinst ;

    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
    {
        info = hwnd;
        GetClientRect(hwnd,&clientRect);
        hinst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
        InitCommonControls();
        hwndlist = CreateWindow(WC_LISTVIEW, "",
                                WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | LVS_ALIGNLEFT|LVS_REPORT |LVS_AUTOARRANGE,
                                0,0,
                                clientRect.right,
                                (clientRect.bottom+100)/2,
                                hwnd, (HMENU)ID_LIST, hinst, 0);
        CreateWindow("edit","",
                     WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_READONLY,
                     0,
                     (clientRect.bottom+100)/2,
                     clientRect.right,
                     clientRect.bottom,
                     hwnd,
                     (HMENU)RESUT_INFO,
                     hinst,
                     NULL);
        size_column=clientRect.right;
        break;
    }
    case WM_USER:
    {
        switch(wParam)
        {
        case SHOW_INFO_SUCCESS:
        {
            HWND hwndresult = GetDlgItem(hwnd, RESUT_INFO);
            if(select_query && num_col <= 0)
            {
                SetWindowText(hwndresult,"The query was successful but the table you chose is empty");
            }
            else
            {
                SetWindowText(hwndresult,"The query was successful");
            }
            break;
        }
        case SHOW_INFO_DONE:
        {

            HWND hwndresult = GetDlgItem(hwnd, RESUT_INFO);
            SetWindowText(hwndresult,"The Query was done already");
            break;
        }
        case SHOW_INFO_ERROR:
        {
            HWND hwndresult = GetDlgItem(hwnd, RESUT_INFO);
            SetWindowText(hwndresult,err);
            break;

        }
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our MAIN window */
    HWND secondHwnd;               /* This is the handle for our SECOND window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = "Main";
    wincl.lpfnWndProc = MainWindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    wincl.lpszClassName = "Second";
    wincl.lpfnWndProc = SecondWindowProcedure;      /* This function is called by windows */

    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    int temp=40;
    SIZE sz;
    sz.cx = 544;
    sz.cy = 375;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               "Main",         /* Classname */
               _T("Moj upit"),       /* Title Text */
               WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, /* default window */
               temp,       /* Windows decides the position */
               temp,       /* where the window ends up on the screen */
               sz.cx,                 /* The programs width */
               sz.cy,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* The class is registered, let's create the program*/
    secondHwnd = CreateWindowEx (
                     0,                   /* Extended possibilites for variation */
                     "Second",          /* Classname */
                     _T("Rezultat upita"),       /* Title Text */
                     WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,  /* default window */
                     temp+sz.cx,       /* Windows decides the position */
                     temp,       /* where the window ends up on the screen */
                     sz.cx-100,                 /* The programs width */
                     sz.cy,                 /* and height in pixels */
                     hwnd,        /* The window is a child-window to desktop */
                     NULL,                /* No menu */
                     (HINSTANCE) GetWindowLong(hwnd,GWLP_HINSTANCE),       /* Program Instance handler */
                     NULL                 /* No Window Creation data */
                 );

    /* Make the window visible on the screen */
    ShowWindow (secondHwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


