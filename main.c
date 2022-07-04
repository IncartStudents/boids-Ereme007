#include <windows.h>
#include <gl/gl.h>
#include <math.h>
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


//размер окна
int width = 800;
int height = 300;
float koef;
float Radi = 0.05;


//ВОТ ТУТ СКОРОСТЬ

float Change_x = 0.01;
float Change_y = 0.01;



void DrawTr(int center_x, int center_y)
{
    float x, y;
        float theta = 0 ;
        float Speed = sqrt((Change_y * Change_y + Change_x * Change_x));

            glBegin(GL_TRIANGLES);
                glColor3f(1.0f, 0.0f, 0.0f);
                glVertex2f(center_x + Radi,   center_y + Radi);

                glVertex2f(center_x - Radi,  center_y);

                glVertex2f(center_x , center_y - Radi);

            glEnd();
}



//Структура для Птицы

typedef struct
{
    float x, y;

    float dx, dy;
} Boid;


//Создадим саму птицу
Boid bird;

//Иницилизация объекта типа Boid

void Boid_Init(Boid *obj, float x1, float y1, float dx1, float dy1)
{
    obj->x = x1;
    obj->y = y1;
    obj->dx = dx1;
    obj->dy = dy1;
    //obj->x = x1;

}

//Процедура рисовки птицы
void Boid_Show(Boid obj)
{
    glPushMatrix();
        glTranslatef(obj.x, obj.y, 0);
        DrawTr(0, 0);

    glPopMatrix();

}

void Boid_Move(Boid *obj)
{
    obj->x += obj->dx;
    obj->y += obj->dy;



    if(obj->y > + 1-0.05)
    {
        Change_y = Change_y*(-1);
        obj->y = + 1-0.05;
    }
    if(obj->x > + koef-0.05)
   {
        Change_x = Change_x*(-1);
        obj->x = + koef-0.05;
    }


    if(obj->y < - 1+0.05)
    {
        Change_y = Change_y*(-1);
        obj->y = - 1+0.05;
    }
    if(obj->x < - koef + 0.05)
    {
        Change_x = Change_x*(-1);
        obj->x = - koef+0.05;
    }


obj->dy = Change_y;
obj->dx = Change_x;

}

//Иницилизация игры и туда птичку
void Game_Init()
{
        Boid_Init(&bird, 0, 0, 0, 0);


}







int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          width,
                          height,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    koef = (float)width / height;

   // glScalef( 1/koef, 1, 1);
   // printf("%f", koef);



    Game_Init();


    //главный цикл
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            Boid_Move(&bird);

            // glColor3f(1, 1, 0);//однотонный цвет желтый
            //DrawTr(1 , 0);
            Boid_Show(bird);








            SwapBuffers(hDC);

            theta += 1.0f;
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

