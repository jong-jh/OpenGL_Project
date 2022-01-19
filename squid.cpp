/****************************************************************************************************************************/
/*                                                 HW#8 : 오징어게임 - 무궁화꽃이 피었습니다                      */
/*                                      작성자 : 정종현                    날짜: 2021.12.03                                 */
/*문제정의 : 넷플릭스 드라마 오징어게임에 나오는 무궁화꽃이 피었습니다 게임을 구현함 */
 /* 기능 : 1번,2번,3번,4번,5번 키로 다양한 시점에서 게임을 관찰할 수 있다.                                                   */
/*         또한 W A S D 와 방향키를 통해 시점을 이동시킬 수 있다.                                                              */
/*         스페이스바를 누르면 영희로봇이 조작이 가능하며, 무궁화 꽃이 피었습니다 게임 규칙과 동일하게, 고개가 참가자를 향하면*
 *         참가자들이 걸음을 멈추고, 고개를 돌릴시 참가자들이 움직인다. 이 때 line의 색상이 바뀐다.                            *
 *         참가자들의 speed는 랜덤이라 우승자가 누가될지 모른다. 제일 먼저 line에 도달한 사람이 우승자이며, 나머지 참가자들은 탈락처리된다.*
 */
/******************************************************************************************************************************/
#include <windows.h>
#include<time.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>

#include <mmsystem.h>    
#pragma comment(lib,"winmm.lib") 
#pragma comment(lib, "glaux.lib")
#pragma comment (lib,"legacy_stdio_definitions.lib")

static int body = 0, neck = 180, lshoulder = 0, rshoulder = 120, lelbow = -5, relbow = 120, lleg = 0, rleg = 0, lknee = 0, rknee = 0, rvelo = 5, lvelo = 5, yz = 100;
int r = 1, g = 1, b = 1, headk = 1, rarmk = 1, larmk = 1, rlegk = 1, llegk = 1, rkneek = -1, lkneek = -1, cnt = 0;    //각 관절 및 색상에 대한 방향
int glegr = 0, glegl = 0, garml = -15, garmr = 15, direc = 1;

static int off = 1; //신체 멈춤 flag
int kill = 0, stop = 1; //게임 종료 신호

GLUquadricObj* qobj = gluNewQuadric();  //새로운 Quadric Object 생성
GLuint list[10]; // DisplayList 의 정수 ID를 받을 배열

GLfloat red = 1.0, green = 0.0, blue = 0.0;   //색 인자를 구성하는 변수
GLdouble camx = 850.0, camy = 550.0, camz = -600.0, fx = 0.0, fy = 0.0, fz = 900.0;  //gluLookAt 함수의 인자 값 설정

/*게임 참자가들의 위치 정보*/
int movez[20] = { 2520,2510,2500,2570,2500,2600,2500,2530,2580,2490,2520,2510,2500,2570,2500,2600,2500,2530,2580,2490 };
int movex[20] = { -800,-700,-600,-550,-400,-380,-200,0,80,100,200,300,350,370,420,500,550,700,750,800 };
int speed[20] = {};

/*수동 Textture 매핑을 위한 파일 불러오기*/
void loadTexture(void) {
    AUX_RGBImageRec* pTextureImage = auxDIBImageLoad(L"apple.bmp"); // 유니코드 스트링임을 명시

    if (pTextureImage != NULL) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pTextureImage->sizeX, pTextureImage->sizeY, 0,
            GL_RGB, GL_UNSIGNED_BYTE, pTextureImage->data);
    }

    if (pTextureImage) {
        if (pTextureImage->data)
            free(pTextureImage->data);

        free(pTextureImage);
    }
}
void loadTexture_2(void) {
    AUX_RGBImageRec* pTextureImage = auxDIBImageLoad(L"ground.bmp"); // 유니코드 스트링임을 명시

    if (pTextureImage != NULL) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pTextureImage->sizeX, pTextureImage->sizeY, 0,
            GL_RGB, GL_UNSIGNED_BYTE, pTextureImage->data);
    }

    if (pTextureImage) {
        if (pTextureImage->data)
            free(pTextureImage->data);

        free(pTextureImage);
    }
}
/* s,t, x,y,z 정보 저장을 위한 Struct*/
struct Vertex {
    // GL_T2F_V3F
    float tu, tv;
    float x, y, z;
};

Vertex g_parallelogramVertices[] = {    //texture와 대응되는 좌표 설정
    { 0.0f,0.0f, -1000.0f,0.0f, 0.0f },
    { 5.0f,0.0f, 1000.0f,0.0f, 0.0f },
    { 5.0f,5.0f, 1000.0f, 0.0f, 2500.0f },
    { 0.0f,5.0f, -1000.0f, 0.0f, 2500.0f }
};

void MyCreateList() {
    list[0] = glGenLists(10);    //Display List 아이디를 할당한다. range: 10, list[0] : Sphere 
    list[1] = list[0] + 1;      //Cylinder for neck
    list[2] = list[1] + 1;      //Cylinder for amr & leg
    list[3] = list[2] + 1;      //Cylinder for body
    list[4] = list[3] + 1;      //Sphere for hand
    list[5] = list[4] + 1;      //Cylinder for heart
    list[6] = list[5] + 1;      //영희 허벅지
    list[7] = list[6] + 1;      //영희 종아리
    list[8] = list[7] + 1;      //영희 머리카락
    list[9] = list[8] + 1;      //세모 가면

   /*목을 표현할 cylinder*/
    glNewList(list[0], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);    //광원에 의한 객체의 Gouraud Shading
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 10.0, 10.0, 5.0, 50, 50); //반지름 10, 높이 5인 원통 모양의 실린더
    glEndList();

    /*머리를 표현할 Sphere*/
    glNewList(list[1], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluSphere(qobj, 100.0, 50, 50);  //반지름 30의 구
    glEndList();

    /*다목적 cylinder*/
    glNewList(list[2], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 7.0, 7.0, 40.0, 50, 50);  //반지름 7, 높이 40인 원통모양의 실린더
    glEndList();

    /*몸통을 표현할 cylinder*/
    glNewList(list[3], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 30.0, 30.0, 50.0, 50, 50);    //반지름 30, 높이 50인 원통모양의 실린더
    glEndList();

    /*몸통을 표현할 cylinder*/
    glNewList(list[4], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 30.0, 40.0, 60.0, 50, 50);    //반지름 30, 높이 50인 원통모양의 실린더
    glEndList();

    /*손을 표현할 Sphere*/
    glNewList(list[5], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_LINE);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluSphere(qobj, 7.0, 50, 50);       //반지름 7인 구
    glEndList();

    /*허벅지 표현할 cylinder*/
    glNewList(list[6], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 10.0, 10.0, 30.0, 50, 50);  //반지름 7, 높이 40인 원통모양의 실린더
    glEndList();

    /*다리 cylinder*/
    glNewList(list[7], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 11.0, 11.0, 70.0, 50, 50);  //반지름 7, 높이 40인 원통모양의 실린더
    glEndList();

    /*머리카락를 표현할 Sphere*/
    glNewList(list[8], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluSphere(qobj, 25.0, 50, 50);  //반지름 30의 구
    glEndList();

    /*세모 표현할 Sphere*/
    glNewList(list[9], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluSphere(qobj, 8, 3, 8);
    glEndList();

}

/*영희로봇을 그리는 함수*/
void DrawYounghi(void) {
   
    glRotated(90, 1.0, 0.0, 0.0);   //x축 기준으로 90도 회전한다. 따라서 로봇의 위아래에 관련된 중심축은 z축이 된다.
    /*몸통*/
    /*새로운 스택을 삽입한 뒤 몸통에 대한 모델을 그려주고 삭제한다 */
    glPushMatrix();
    glColor3f(1.0, 0.64, 0.0);  //주황색
    glCallList(list[3]);
    glPopMatrix();
    /*목*/
    /*새로운 스택을 삽입한 후 목을 그려준다*/
    glPushMatrix();
    glColor3f(0.98, 0.80, 0.69);
    glTranslated(0.0, 0.0, -5.0);
    glCallList(list[0]);
    /*머리*/
    /*목을 기준으로 sphere의 위치를 움직여준다.*/
        glPushMatrix();
        glColor3f(0.98, 0.80, 0.69);    //살구색
        glRotated((GLfloat)neck, 0.0, 0.0, 1.0); //원점을 기준으로 회전
        glTranslated(0.0, 0.0, -25.0);      //pivot으로 지정할 위치에 물체를 이동
        glCallList(list[1]);
        /*눈*/
    /*머리의 중심에 위치하고 있는 CTM을 push 해줘서 이동시킨 뒤 눈을 그려준다*/
            glPushMatrix();
            glColor3f(1.0, 0.0, 0.0);
            glTranslated(12.0, 22.0, -5.0); //오른쪽 눈의 위치로 이동
            glutSolidSphere(5.0, 30, 30);
            
        /*위의 내용을 pop 해줬기 때문에 중심은 머리에 위치하고 있다*/
            
            glColor3f(1.0, 0.0, 0.0);
            glTranslated(-24.0, 0.0, 0.0);    //왼쪽 눈의 위치로 이동
            glutSolidSphere(5.0, 30, 30);
            
            /*머리카락*/
            glPushMatrix();
            glTranslated(12.0, -22.0, 5.0);
            glColor3f(0.0, 0.0, 0.0);
            glTranslated(27.0, -10.0, 25.0); 
            glutSolidSphere(10.0, 30, 30);
            glTranslated(-54.0, 0.0, 0.0);    
            glutSolidSphere(10.0, 30, 30);
            glPopMatrix();

    glPopMatrix();  //원점에 위치
    glPopMatrix();
    glPopMatrix();

    /*오른쪽 팔*/
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslated(33.0, 0.0, 3.0);
    glRotatef((GLfloat)rshoulder, 0.0, 1.0, 0.0);
    glCallList(list[2]);
        glPushMatrix();
        glColor3f(0.98, 0.80, 0.69);    //살구색
        glTranslated(0.0, 0.0, 40.0);
        glRotatef((GLfloat)relbow, 0.0, 1.0, 0.0);
        glCallList(list[2]);
        /*오른쪽 손*/
        glTranslated(0.0, 0.0, 40.0);
        glCallList(list[5]);
        glPopMatrix();
    glPopMatrix();
    
    /*왼쪽 팔*/
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslated(-33.0, 0.0, 3.0);
    glRotatef((GLfloat)lshoulder, 0.0, 1.0, 0.0);
    glCallList(list[2]);
        glPushMatrix();
        glColor3f(0.98, 0.80, 0.69);    //살구색
        glTranslated(0.0, 0.0, 40.0);
        glRotatef((GLfloat)lelbow, 0.0, 1.0, 0.0);
        glCallList(list[2]);
        /*왼쪽 손*/
        glTranslated(0.0, 0.0, 40.0);
        glCallList(list[5]);
        glPopMatrix();
    glPopMatrix();

    /*몸통 하단부*/
    glPushMatrix();
    glTranslated(0.0, 0.0, 45.0);
    glColor3f(1.0, 0.64, 0.0);  //주황색
    glTranslated(0.0, 0.0, 5.0);
    glCallList(list[4]);
    glPopMatrix();

    /*오른쪽 다리*/
    glPushMatrix();
    glColor3f(0.98, 0.80, 0.69);    //살구색
    glTranslated(13.0, 0.0, 110);
    glRotatef((GLfloat)rleg, 1.0, 0.0, 0.0);
    glCallList(list[6]);
        glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslated(0.0, 0.0, 30.0);
        glRotatef((GLfloat)rknee, 1.0, 0.0, 0.0);
        glCallList(list[7]);
        /*발*/
        glTranslated(0.0, 0.0, 66.0);
        glColor3f(0.0, 0.0, 0.0);
        glutSolidSphere(13.0, 30, 30);
        glPopMatrix();
    glPopMatrix();

    /*왼쪽 다리*/
    glPushMatrix();
    glColor3f(0.98, 0.80, 0.69);    //살구색
    glTranslated(-13.0, 0.0, 110);
    glRotatef((GLfloat)lleg, 1.0, 0.0, 0.0);
    glCallList(list[6]);
        glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslated(0.0, 0.0, 30.0);
        glRotatef((GLfloat)lknee, 1.0, 0.0, 0.0);
        glCallList(list[7]);
        /*발*/
        glTranslated(0.0, 0.0, 66.0);
        glColor3f(0.0, 0.0, 0.0);
        glutSolidSphere(13.0, 30, 30);
        glPopMatrix();
    glPopMatrix();
}

/*오징어게임의 간부 (세모)를 그려주는 함수*/
void DrawSemo(void) {
    glTranslatef(0.0, 75, 0.0);
    /*몸통*/
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.2);  //빨간색
    glScalef(30, 50, 10);
    glutSolidCube(1);
    glPopMatrix();
    
    /*머리*/
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);    
    glTranslated(0.0, 40.0, 0.0);
    glutSolidSphere(16, 20, 20);
        glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslated(0.0, 0.0, 14.0);
        glCallList(list[9]);
        glPopMatrix();
    glPopMatrix();
    
    /*팔*/
    glPushMatrix();
    glTranslatef(0, 25, 0);
        /*오른쪽 팔*/
        glPushMatrix();
        glColor3f(1.0, 0.0, 0.2);  //빨간색
        glTranslatef(15.0, 0.0, 0.0);
        glRotatef(15, 0.0, 0.0, 1.0);
        glTranslatef(0.0, -25.0, 0.0);
        glScalef(10, 50, 10);
        glutSolidCube(1);
        glPopMatrix();
        /*왼쪽 팔*/
        glPushMatrix();
        glColor3f(1.0, 0.0, 0.2);  //빨간색
        glTranslatef(-15.0, 0.0, 0.0);
        glRotatef(-15, 0.0, 0.0, 1.0);
        glTranslatef(0.0, -25.0, 0.0);
        glScalef(10, 50, 10);
        glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
    /*다리*/
    glPushMatrix();
    glTranslatef(0, -50, 0);
        glPushMatrix();
        glTranslatef(7.0, 0.0, 0.0);
        glScalef(10, 50, 10);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-7.0, 0.0, 0.0);
        glScalef(10, 50, 10);
        glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();

}
/*오징어게임 참가자를 그려주는 함수*/
void DrawGamer(void) {
    glTranslatef(0.0, 75, 0.0);
    /*몸통*/
    glPushMatrix();
    glColor3f(0.05, 0.45, 0.05);  //초록색
    glScalef(30, 50, 10);
    glutSolidCube(1);
    glPopMatrix();

    /*머리*/
    glPushMatrix();
    glColor3f(0.98, 0.80, 0.69);    //살구색
    glTranslated(0.0, 40.0, 0.0);
    glutSolidSphere(16, 20, 20);
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);
        glTranslated(8.0, 1.0, 14.0);
        glutSolidSphere(3, 10, 10);
        glPopMatrix();
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);
        glTranslated(-8.0, 1.0, 14.0);
        glutSolidSphere(3, 10, 10);
        glPopMatrix();
    glPopMatrix();

    /*팔*/
    glPushMatrix();
    glTranslatef(0, 25, 0);
    /*오른쪽 팔*/
    glPushMatrix();
    glColor3f(0.05, 0.45, 0.05);  //초록색
    glTranslatef(15.0, 0.0, 0.0);
    glRotatef(garmr, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -25.0, 0.0);
    glScalef(10, 50, 10);
    glutSolidCube(1);
    glPopMatrix();
    /*왼쪽 팔*/
    glPushMatrix();
    glColor3f(0.05, 0.45, 0.05);  //초록색
    glTranslatef(-15.0, 0.0, 0.0);
    glRotatef(garml, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -25.0, 0.0);
    glScalef(10, 50, 10);
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();
    
    /*다리*/
    glPushMatrix();
    glTranslatef(0, -25, 0);
    glPushMatrix();
    glTranslatef(7.0, 0.0, 0.0);
    glRotatef(glegr, 1.0, 0.0, 0.0);
    glTranslatef(0.0, -25.0, 0.0);
    glScalef(10, 50, 10);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-7.0, 0.0, 0.0);
    glRotatef(glegl, 1.0, 0.0, 0.0);
    glTranslatef(0.0, -25.0, 0.0);
    glScalef(10, 50, 10);
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();

}
/*시작선, 도착선을 표현할 Line*/
void DrawLine() {
    glBegin(GL_QUADS);
    glColor3f(red, green, blue);
    glVertex3f(1000.0, 1.0, 1900.0);
    glVertex3f(-1000.0, 1.0, 1900.0);
    glVertex3f(-1000.0, 1.0, 1850.0);
    glVertex3f(1000.0, 1.0, 1850.0);
    glEnd();
}
/*나무를 그려주는 함수*/
void DrawTree() {
    /*나무 몸통*/
    glTranslatef(0.0, 200, 0.0);
    glPushMatrix();
    glColor3f(0.41, 0.23, 0.0);
    glScalef(100, 400, 50);
    glutSolidCube(1);
    glPopMatrix();
    /*나무가지들 표현*/
    glPushMatrix();
    glTranslatef(0, 200, 0);
        glPushMatrix();
        glRotatef(50, 0.0, 0.0, 1.0);
        glTranslatef(0.0, 150.0, 0.0);
        glScalef(20, 300, 12);
        glutSolidCube(1);
        glPopMatrix();
        
        glPushMatrix();
        glRotatef(-50, 0.0, 0.0, 1.0);
        glTranslatef(0.0, 150.0, 0.0);
        glScalef(12, 300, 12);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glRotatef(30, 0.0, 0.0, 1.0);
        glTranslatef(0.0, 80.0, 0.0);
        glScalef(20, 160, 20);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glRotatef(-70, 0.0, 0.0, 1.0);
        glTranslatef(0.0, 80.0, 0.0);
        glScalef(10, 160, 10);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glRotatef(0, 0.0, 0.0, 1.0);
        glTranslatef(0.0, 100.0, 0.0);
        glScalef(20, 200, 20);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glRotatef(-85, 0.0, 0.0, 1.0);
        glTranslatef(0.0, 160.0, 0.0);
        glScalef(20, 320, 20);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90, 0.0, 0.0, 1.0);
        glTranslatef(0.0, 140.0, 0.0);
        glScalef(10, 280, 10);
        glutSolidCube(1);
        glPopMatrix();

    glPopMatrix();
}


void MyDisplay(void) {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(camx, camy, camz, fx, fy, fz, 0.0, 1.0, 0.0); //카메라 시점
    
    /*바닥을 그려준다 - 수동 텍스처매핑 사용*/
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    loadTexture_2();
    glColor3f(0.7, 0.7, 0.7);
    glInterleavedArrays(GL_T2F_V3F, 0, g_parallelogramVertices);
    glDrawArrays(GL_QUADS, 0, 4);
    glPopMatrix();

    /*영희로봇을 그려준다*/
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0.0, 220.0, yz);
    DrawYounghi();
    glPopMatrix();

    /*나무를 그려준다*/
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    loadTexture();
    glTranslatef(0.0, 0.0, 20.0);
    DrawTree();
    glPopMatrix();

    /*간부(세모)를 그려준다*/
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(100.0, 0.0, 100.0);
    DrawSemo(); //간부 1
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-100.0, 1.0, 100.0);
    DrawSemo(); //간부 2
    glPopMatrix();

    //*게임 플레이어들 생성*//
    for (int i = 0; i < 20; i++) {  //20명 생성
        
        glPushMatrix();
        glTranslatef(movex[i], 2.0, movez[i]-500);
        glRotatef(180, 0.0, 1.0, 0.0);
        if (kill) { //만약 게임이 종료가 되면
            if (movez[i] > 800) {
                glRotatef(90, 1.0, 0.0, 0.0);   //위의 조건에 충족한 인원들은 90도 회전하여 쓰러지게 만든다.
            }
        }
        DrawGamer();   //참가자 그리기
        glPopMatrix();
    }
    
    /*출발선 그리기*/
    glPushMatrix();
    DrawLine();
    glPopMatrix();
    /*도착선 그리기*/
    glPushMatrix();
    glTranslatef(0.0, 3.0, -1600);
    DrawLine();
    glPopMatrix();


    glutSwapBuffers();
}

/*영희로봇이 게임이 종료되면 달리는 TimerCallback*/
void RunTimer(int value) {
    if (yz > 1200) { larmk = larmk * -1; }  //z축의 direction 설정  
    if (yz < 100) { return; }   //다음 조건에 함수 종료

    yz += 5*larmk;  //영희로봇의 z좌표가 변화한다
    neck += 10;     //목이 계속 돌아가게끔 한다

    //차렷자세로 변환
    if (rshoulder ==0 ) rshoulder=rshoulder;    
    else{
        rshoulder = (rshoulder - 10 * rarmk);
    }
    if (relbow ==0 ) relbow = relbow;
    else { relbow = (relbow - 10) % 360; }


    if (rleg > 48 || rleg < -48) rlegk = -rlegk;    //골반과 연결된 다리 관절을 8만큼씩 96도 회전시킨다.  
    rleg = (rleg + 8 * rlegk) % 360;

    if (lleg > 48 || lleg < -48) llegk = -llegk;
    lleg = (lleg - 8 * llegk) % 360;

    if (rleg < 0) rvelo = 15;   //골반과 연결된 다리 관절이 앞쪽을 향할 땐 무릎이 회전하는 속도가 15로 설정한다.
    if (rleg > 0) rvelo = 5;    //골반과 연결된 다리 관절이 뒷쪽을 향할 때 무릎이 회전하는 속도를 5로 설정한다.
    if (lleg < 0) lvelo = 15;
    if (lleg > 0) lvelo = 5;

    rknee = (rleg + (rleg / 8) * rvelo * rkneek) % 360; //  leg의 증감값에 따라 무릎회전각도를 설정한다.
    lknee = (lleg + (lleg / 8) * lvelo * lkneek) % 360;

    glutPostRedisplay();
    glutTimerFunc(40, RunTimer, 1);
}

/*영희로봇의 머리 작동 TimerCallback*/
void HeadMoveTimer(int value) {
    
    if (neck > 180 || neck < 0) {  //해당 조건에 direction 값 바꿈
        headk = -headk;
        
    }
    neck = (neck - 5 * headk) % 360;
    
    if (headk > 0) {
        red = 1, green = 0, blue = 0;   //line의 색상을 변경해준다. (게임 참가자가 움직이면 안 될 때 : 빨간색)
    }
    else {
        red = 0, green = 1, blue = 0.2; //게임 참가자들이 움직일 때 : 초록색
    }

    if (neck == -5 || neck == 185) {    //다음 회전각도에서 함수 종료
        return;
    }


    glutPostRedisplay();
    glutTimerFunc(40, HeadMoveTimer, 1);
}

/*게임 참가자 이동 TimerCallback*/
void PlayerMoveTimer(int value) {
    if (off < 0) { return; }    //off값에 따라 함수 종료
    

    for (int i = 0; i < 20; i++) {  //20명의 player에 각기 다른 speed값을 설정
        speed[i] = rand() % 15; //값을 램덤으로 설정
        movez[i] -= speed[i];
        if (movez[i] < 800) { //z값이 다음 조건을 충족하면
            kill = 1;
            glutTimerFunc(40, RunTimer, 1); //영희가 움직이는 timercallback 실행 후 종료
            glutPostRedisplay();
            return; }
    }

    if (glegr > 0||glegr<-30) { direc = direc * -1; }
    glegr += 3 * direc;
    glegl -= 3 * direc;
    garmr -= 3 * direc;
    garml += 3 * direc;



    glutPostRedisplay();
    glutTimerFunc(40, PlayerMoveTimer, 1);
}
/*실행 시 에니메이션 효과 TimerCallback*/
void InitCamera(int value) {

    if (camz > 2500) { stop = -1; return; } //camz가 2500에 위치하면 stop

    camz += 18;
    camy += 1;

    glutPostRedisplay();
    glutTimerFunc(40, InitCamera, 1);
}

void MySpecial(int key, int x, int y) {
    if (key == GLUT_KEY_UP) camy += 10.0;    //위쪽 방향키를 눌렀을 때 카메라의 y축이 10 증가한다
    if (key == GLUT_KEY_DOWN) camy -= 10.0;  //아래쪽 방향키를 눌렀을 때 카메라의 y축이 10 감소
    if (key == GLUT_KEY_LEFT)  camx -= 10.1; //왼쪽 방향키를 눌럿을 때 카메라의 x축 10.1 감소
    if (key == GLUT_KEY_RIGHT) camx += 10.1; //오른쪽 방향키를 눌렀을 때 카메라의 x축 10.1 증가
    glutPostRedisplay();
}

/*keyboardcallback 함수.
 *1, 2, 3, 4, 5 : 다양한 시점 확인 가능
 * W, A, S, D : 수동으로 시점 변경 가능
 스페이스바 : 로봇 조작 가능
 */
void MyKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        if (stop > 0) { break; }
        glMatrixMode(GL_PROJECTION);	//현재 스택을 투영 행렬 스택으로 바꾼다.
        glLoadIdentity();
        gluPerspective(50, 1, 0.01, 2000.0);    //원근투영행렬 변환
        camx = 0.0, camy = 500.0, camz = 800.0, fx = 0.0, fy = 100.0, fz = 0;  //카메라 위치 및 시점 조정
        break;

    case '2':
        if (stop>0) { break; }
        glMatrixMode(GL_PROJECTION);	//현재 스택을 투영 행렬 스택으로 바꾼다.
        glLoadIdentity();
        glOrtho(-1200.0, 1200.0, -1000.0, 1000.0, 0.0, 4000.0); //직교투영행렬변환
        camx = 850.0, camy = 550.0, camz = -600.0, fx = 0.0, fy = 0.0, fz = 900.0;;//카메라 위치 및 시점 조정
        break;
    case '3':
        if (stop > 0) { break; }
        glMatrixMode(GL_PROJECTION);	//현재 스택을 투영 행렬 스택으로 바꾼다.
        glLoadIdentity();
        gluPerspective(50, 1, 0.01, 2000.0); //원근투영행렬 변환
        camx = -120.0, camy = 320.0, camz = -100.0, fx = 0.0, fy = -50.0, fz = 1500;//카메라 위치 및 시점 조정
        break;
    case '4':
        if (stop > 0) { break; }
        glMatrixMode(GL_PROJECTION);	//현재 스택을 투영 행렬 스택으로 바꾼다.
        glLoadIdentity();
        gluPerspective(50, 1, 0.01, 2000.0); //원근투영행렬 변환
        camx = 120.0, camy = 330.0, camz = -100.0, fx = 0.0, fy = -100.0, fz = 1500;//카메라 위치 및 시점 조정
        break;
    case '5':
        if (stop > 0) { break; }
        glMatrixMode(GL_PROJECTION);	//현재 스택을 투영 행렬 스택으로 바꾼다.
        glLoadIdentity();
        glOrtho(-1200.0, 1200.0, -1000.0, 1000.0, 0.0, 4000.0);//직교투영행렬변환
        camx = 0.0, camy = 800.0, camz = 1200.0, fx = 0.0, fy = 0.0, fz = 900.0;;//카메라 위치 및 시점 조정
        break;

    case 'w':
    case 'W':
        camz -= 30; //카메라 Z좌표 이동
        break;
    
    case 's':
    case 'S':
        camz += 30;//카메라 Z좌표 이동
        break;
    case 'a':
    case 'A':
        camx -= 30;//카메라 X좌표 이동
        break;
    case 'd':
    case 'D':
        camx += 30;//카메라 X좌표 이동
        break;

    case ' ':   //스페이스바
        off = off * -1;
        glutTimerFunc(40, PlayerMoveTimer, 1);  //게임 참가자 이동 timercallback
        glutTimerFunc(40, HeadMoveTimer, 1);    //영희 로봇 조작 timercallback
        break;

    case 27:
        exit(0);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void MyInit(void) {
    PlaySound(TEXT("squidgame.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT| SND_NOSTOP);    //배경음악 설정
    
    glutTimerFunc(40, InitCamera, 1);
    GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };     //주변반사를 위한 광원 RGBA
    GLfloat mat_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };        //난반사를 위한 광원 RGBA
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };    //정반사를 위한 광원 RGBA
    GLfloat mat_shininess[] = { 30.0 };                 //정반사 광택 계수
    GLfloat light_position[] = { 400.0, 400.0, 500.0, 0.0 };  //광원의 위치, w=0 으로, 원점에서부터 좌표를 향하는 벡터방향의 방향성 광원
    GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };   //전역 주변광 RGBA

    glClearColor(0.5, 0.8, 0.9, 0.0);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);        //Front 면에 대한 주변광 계수 : 주변 반사에 대한 물체색
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);        //Front 면에 대한 난반사광 계수 : 난반사에 대한 물체색
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);      //Front 면에 대한 정반사광 계수, 정반사에 대한 물체색
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);    //Front 면에 대한 정반사의 광택 계수
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  //광원의 위치 설정
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);  //전역 주변 광원 색
   // glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);    //물체면을 구성하는 모든 다각형 마다 별도의 시점벡터를 계산
    /*조명 기능 활성화*/
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);    //은면제거
    glEnable(GL_NORMALIZE);
 
    glMatrixMode(GL_PROJECTION);	//현재 스택을 투영 행렬 스택으로 바꾼다.
    glLoadIdentity();
    gluPerspective(50, 1, 0.01, 2000.0);
    camx = 0.0, camy = 2.0, camz = 100.0, fx = 0.0, fy = 2.0, fz = 0;

    srand((unsigned)time(NULL));    //time seed 초기화
    
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1300, 1300);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Robot");
    MyInit();
    glutDisplayFunc(MyDisplay);
    glutKeyboardFunc(MyKeyboard);
    glutSpecialFunc(MySpecial);
    MyCreateList();

    glutMainLoop();
    gluDeleteQuadric(qobj); // Quadric Object를 삭제

    return 0;
}