#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <math.h>


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


float Max_Speed = 0.015;
float Radi = 0.05;
int Cout = 7;
float Vision = 0.15;



float Alig_X = 0;
float Alig_Y = 0;
float Average_Alig_X = 0;
float Average_Alig_Y = 0;
int In_Vision = 0;



void Fun_Speed(float dx, float dy)
{
    if(sqrt(dx*dx+dy*dy) > Max_Speed)
    {
        dx = dx*0.95;
        dx = dx*0.95;
        Fun_Speed(dx, dy);
    }
}

float Distance_fun(float X1, float X2, float Y1, float Y2)
{
    return sqrt(pow(X2 - X1, 2) + pow(Y2 - Y1, 2));
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    //draw a triangle
    void DrawTr(int center_x, int center_y, float dx, float dy)
    {
        glBegin(GL_POINTS);
            for(int i=0; i < 360; i++)
                glVertex2f(Vision*cos(2*3.14159*i/360),Vision*sin(2*3.14159*i/360));
        glEnd();

        glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(center_x ,   center_y + Radi);
            glVertex2f(center_x - Radi,  center_y - Radi);
            glVertex2f(center_x + Radi , center_y - Radi);
        glEnd();

        glLineWidth(3);
        glBegin(GL_LINES);
            glColor3d(0,1,0);     // green
            glVertex2f(center_x, center_y);
            glVertex2f(center_x + dx*4, center_x + dy*4);
        glEnd();
    }

//struct for boid
    typedef struct
    {
        float x, y;
        float dx, dy;
        float ddx, ddy;
    } Boid;

//name boid - bird
    Boid bird[Cout];

//function Separation
    void Func_Ottalkivanie(Boid *Bird1, Boid *Bird2, float Distance)
{
    Bird1->ddx += (Bird1->x - Bird2->x)*7*((Vision - Distance)/Vision);

    Bird1->ddy += (Bird1->y - Bird2->y)*7*((Vision - Distance)/Vision);
}

//function Alignment
    void Func_Sonapravlenie(Boid *Bird, float Alig_X, float Alig_Y)
{
    Bird->ddx += Alig_X;

    Bird->ddy += Alig_Y;
}

//function Cohesion
void Func_Midle_hip (Boid *Bird, float X, float Y)
{
    Bird->ddx += X;

    Bird->ddy += Y;
}


//Initializing an object of type Boid

    void Boid_Init(Boid *obj, float x1, float y1,  float dx1, float dy1,  float ddx1, float ddy1)
    {
        obj->x = x1;
        obj->y = y1;
        obj->dx = dx1;
        obj->dy = dy1;
        obj->ddx = ddx1;
        obj->ddy = ddy1;
    }

//The procedure of drawing a Boid
    void Boid_Show(Boid obj)
    {
        glPushMatrix();
        glTranslatef(obj.x, obj.y, 0);
        DrawTr(0, 0, obj.dx, obj.dy);
        glPopMatrix();
    }

//The movement procedure of Boid
    void Boid_Move(Boid *obj)
    {
        if(obj->ddx != 0)
        {
            obj->dx += obj->ddx/60;
            obj->ddx = 0;
        }

        if(obj->ddy != 0)
        {
            obj->dy += obj->ddy/60;
            obj->ddy = 0;
        }

//speed normalization
        while(sqrt(obj->dx*obj->dx + obj->dy*obj->dy) > Max_Speed)
        {
            obj->dx = obj->dx*0.95;
            obj->dy = obj->dy*0.95;
        }


        obj->x += obj->dx;
        obj->y += obj->dy;

        //condition of edge
        if(obj->y > + 2-Radi)
        {
            obj->dy = obj->dy*(-1);
            obj->y = + 2-Radi;
        }

        if(obj->x > + 2-Radi)
        {
            obj->dx = obj->dx*(-1);
            obj->x = + 2 -Radi;
        }


        if(obj->y < Radi)
        {
            obj->dy = obj->dy*(-1);
            obj->y = +Radi;
        }

        if(obj->x <  + Radi)
        {
            obj->dx = obj->dx*(-1);
            obj->x = + Radi;
        }
    }

//Initialization of the game
    void Game_Init()
    {
        for(int k = 0; k < Cout; k++)
        {
            Boid_Init(&bird[k], (float) rand()/RAND_MAX*(1) + 0,
                  (float) rand()/RAND_MAX,
                  ((float) 1.*2*rand()/RAND_MAX - 1) / 100,
                  ((float) 1.*2*rand()/RAND_MAX - 1) / 100, 0 ,0);
        }
    }

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
                          800,
                          800,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    //zoom in to 2 on X and Y
    glScalef(1, 1, 1);
    glTranslated(-1, -1, 0);

    Game_Init();

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

            for(int k = 0; k < Cout; k++) //Cout - how many birds

                Boid_Show(bird[k]); //Draw

            for(int i = 0; i < Cout; i++)
            {
                Alig_X = 0;
                Alig_Y = 0;
                Average_Alig_X = 0;
                Average_Alig_Y = 0;
                In_Vision = 0;


                float Midle_X = 0;
                float Midle_Y = 0;

                float Distance;
                float Min_Distance = 0;
                int Index = -1; //индекс ближайшкей птички от I
                int Flag = 0; //первая минимальная дистанция
                for(int j = 0; j < Cout; j++)
                {
                //    printf("In_Vision = %d \n", In_Vision);
                    if(i != j)
                    {
                        Distance = Distance_fun(bird[j].x , bird[i].x, bird[j].y, bird[i].y);
                   // printf("Distance = %f\n", Distance);
                        if(Distance < Vision)
                        {
                            Alig_X = Alig_X + bird[j].dx;
                            Alig_Y = Alig_Y + bird[j].dy;
                            In_Vision++;

                         //========================================================

                            if(Flag == 0)
                            {
                                Index = j;
                                Flag++;
                                Min_Distance = Distance;

                                Midle_X = bird[j].x - bird[i].x;
                                Midle_Y = bird[j].y - bird[i].y;
                            }
                            else
                            {
                                Midle_X  = (Midle_X * (In_Vision - 1) + bird[j].x - bird[i].x)/In_Vision;
                                Midle_Y = (Midle_Y * (In_Vision - 1) + bird[j].y - bird[i].y)/In_Vision;
                            }

                            if(Min_Distance > Distance)
                            {
                                Min_Distance = Distance;
                                Index = j;
                            }

                            //=====================================================
                        }
                    }
                }

                Func_Midle_hip(&bird[i], Midle_X, Midle_Y);

//============================================

                if(In_Vision != 0 )
                {
                    Average_Alig_X = Alig_X / (In_Vision);
                    Average_Alig_Y = Alig_Y / (In_Vision);

                    Func_Sonapravlenie(&bird[i], Average_Alig_X, Average_Alig_Y);
                }
                else
                      Func_Sonapravlenie(&bird[i], 0, 0);

                //=======================================

               if(Index != -1 )
                    Func_Ottalkivanie(&bird[i], &bird[Index], Min_Distance);
            }




        for(int k = 0; k < Cout; k++)
            Boid_Move(&bird[k]);

            SwapBuffers(hDC);

            Sleep (10);
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
