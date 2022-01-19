/****************************************************************************************************************************/
/*                                                 HW#8 : ��¡����� - ����ȭ���� �Ǿ����ϴ�                      */
/*                                      �ۼ��� : ������                    ��¥: 2021.12.03                                 */
/*�������� : ���ø��� ��� ��¡����ӿ� ������ ����ȭ���� �Ǿ����ϴ� ������ ������ */
 /* ��� : 1��,2��,3��,4��,5�� Ű�� �پ��� �������� ������ ������ �� �ִ�.                                                   */
/*         ���� W A S D �� ����Ű�� ���� ������ �̵���ų �� �ִ�.                                                              */
/*         �����̽��ٸ� ������ ����κ��� ������ �����ϸ�, ����ȭ ���� �Ǿ����ϴ� ���� ��Ģ�� �����ϰ�, ���� �����ڸ� ���ϸ�*
 *         �����ڵ��� ������ ���߰�, ���� ������ �����ڵ��� �����δ�. �� �� line�� ������ �ٲ��.                            *
 *         �����ڵ��� speed�� �����̶� ����ڰ� �������� �𸥴�. ���� ���� line�� ������ ����� ������̸�, ������ �����ڵ��� Ż��ó���ȴ�.*
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
int r = 1, g = 1, b = 1, headk = 1, rarmk = 1, larmk = 1, rlegk = 1, llegk = 1, rkneek = -1, lkneek = -1, cnt = 0;    //�� ���� �� ���� ���� ����
int glegr = 0, glegl = 0, garml = -15, garmr = 15, direc = 1;

static int off = 1; //��ü ���� flag
int kill = 0, stop = 1; //���� ���� ��ȣ

GLUquadricObj* qobj = gluNewQuadric();  //���ο� Quadric Object ����
GLuint list[10]; // DisplayList �� ���� ID�� ���� �迭

GLfloat red = 1.0, green = 0.0, blue = 0.0;   //�� ���ڸ� �����ϴ� ����
GLdouble camx = 850.0, camy = 550.0, camz = -600.0, fx = 0.0, fy = 0.0, fz = 900.0;  //gluLookAt �Լ��� ���� �� ����

/*���� ���ڰ����� ��ġ ����*/
int movez[20] = { 2520,2510,2500,2570,2500,2600,2500,2530,2580,2490,2520,2510,2500,2570,2500,2600,2500,2530,2580,2490 };
int movex[20] = { -800,-700,-600,-550,-400,-380,-200,0,80,100,200,300,350,370,420,500,550,700,750,800 };
int speed[20] = {};

/*���� Textture ������ ���� ���� �ҷ�����*/
void loadTexture(void) {
    AUX_RGBImageRec* pTextureImage = auxDIBImageLoad(L"apple.bmp"); // �����ڵ� ��Ʈ������ ���

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
    AUX_RGBImageRec* pTextureImage = auxDIBImageLoad(L"ground.bmp"); // �����ڵ� ��Ʈ������ ���

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
/* s,t, x,y,z ���� ������ ���� Struct*/
struct Vertex {
    // GL_T2F_V3F
    float tu, tv;
    float x, y, z;
};

Vertex g_parallelogramVertices[] = {    //texture�� �����Ǵ� ��ǥ ����
    { 0.0f,0.0f, -1000.0f,0.0f, 0.0f },
    { 5.0f,0.0f, 1000.0f,0.0f, 0.0f },
    { 5.0f,5.0f, 1000.0f, 0.0f, 2500.0f },
    { 0.0f,5.0f, -1000.0f, 0.0f, 2500.0f }
};

void MyCreateList() {
    list[0] = glGenLists(10);    //Display List ���̵� �Ҵ��Ѵ�. range: 10, list[0] : Sphere 
    list[1] = list[0] + 1;      //Cylinder for neck
    list[2] = list[1] + 1;      //Cylinder for amr & leg
    list[3] = list[2] + 1;      //Cylinder for body
    list[4] = list[3] + 1;      //Sphere for hand
    list[5] = list[4] + 1;      //Cylinder for heart
    list[6] = list[5] + 1;      //���� �����
    list[7] = list[6] + 1;      //���� ���Ƹ�
    list[8] = list[7] + 1;      //���� �Ӹ�ī��
    list[9] = list[8] + 1;      //���� ����

   /*���� ǥ���� cylinder*/
    glNewList(list[0], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);    //������ ���� ��ü�� Gouraud Shading
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 10.0, 10.0, 5.0, 50, 50); //������ 10, ���� 5�� ���� ����� �Ǹ���
    glEndList();

    /*�Ӹ��� ǥ���� Sphere*/
    glNewList(list[1], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluSphere(qobj, 100.0, 50, 50);  //������ 30�� ��
    glEndList();

    /*�ٸ��� cylinder*/
    glNewList(list[2], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 7.0, 7.0, 40.0, 50, 50);  //������ 7, ���� 40�� �������� �Ǹ���
    glEndList();

    /*������ ǥ���� cylinder*/
    glNewList(list[3], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 30.0, 30.0, 50.0, 50, 50);    //������ 30, ���� 50�� �������� �Ǹ���
    glEndList();

    /*������ ǥ���� cylinder*/
    glNewList(list[4], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 30.0, 40.0, 60.0, 50, 50);    //������ 30, ���� 50�� �������� �Ǹ���
    glEndList();

    /*���� ǥ���� Sphere*/
    glNewList(list[5], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_LINE);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluSphere(qobj, 7.0, 50, 50);       //������ 7�� ��
    glEndList();

    /*����� ǥ���� cylinder*/
    glNewList(list[6], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 10.0, 10.0, 30.0, 50, 50);  //������ 7, ���� 40�� �������� �Ǹ���
    glEndList();

    /*�ٸ� cylinder*/
    glNewList(list[7], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluCylinder(qobj, 11.0, 11.0, 70.0, 50, 50);  //������ 7, ���� 40�� �������� �Ǹ���
    glEndList();

    /*�Ӹ�ī���� ǥ���� Sphere*/
    glNewList(list[8], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluSphere(qobj, 25.0, 50, 50);  //������ 30�� ��
    glEndList();

    /*���� ǥ���� Sphere*/
    glNewList(list[9], GL_COMPILE);
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    gluQuadricTexture(qobj, GL_FALSE);
    gluSphere(qobj, 8, 3, 8);
    glEndList();

}

/*����κ��� �׸��� �Լ�*/
void DrawYounghi(void) {
   
    glRotated(90, 1.0, 0.0, 0.0);   //x�� �������� 90�� ȸ���Ѵ�. ���� �κ��� ���Ʒ��� ���õ� �߽����� z���� �ȴ�.
    /*����*/
    /*���ο� ������ ������ �� ���뿡 ���� ���� �׷��ְ� �����Ѵ� */
    glPushMatrix();
    glColor3f(1.0, 0.64, 0.0);  //��Ȳ��
    glCallList(list[3]);
    glPopMatrix();
    /*��*/
    /*���ο� ������ ������ �� ���� �׷��ش�*/
    glPushMatrix();
    glColor3f(0.98, 0.80, 0.69);
    glTranslated(0.0, 0.0, -5.0);
    glCallList(list[0]);
    /*�Ӹ�*/
    /*���� �������� sphere�� ��ġ�� �������ش�.*/
        glPushMatrix();
        glColor3f(0.98, 0.80, 0.69);    //�챸��
        glRotated((GLfloat)neck, 0.0, 0.0, 1.0); //������ �������� ȸ��
        glTranslated(0.0, 0.0, -25.0);      //pivot���� ������ ��ġ�� ��ü�� �̵�
        glCallList(list[1]);
        /*��*/
    /*�Ӹ��� �߽ɿ� ��ġ�ϰ� �ִ� CTM�� push ���༭ �̵���Ų �� ���� �׷��ش�*/
            glPushMatrix();
            glColor3f(1.0, 0.0, 0.0);
            glTranslated(12.0, 22.0, -5.0); //������ ���� ��ġ�� �̵�
            glutSolidSphere(5.0, 30, 30);
            
        /*���� ������ pop ����� ������ �߽��� �Ӹ��� ��ġ�ϰ� �ִ�*/
            
            glColor3f(1.0, 0.0, 0.0);
            glTranslated(-24.0, 0.0, 0.0);    //���� ���� ��ġ�� �̵�
            glutSolidSphere(5.0, 30, 30);
            
            /*�Ӹ�ī��*/
            glPushMatrix();
            glTranslated(12.0, -22.0, 5.0);
            glColor3f(0.0, 0.0, 0.0);
            glTranslated(27.0, -10.0, 25.0); 
            glutSolidSphere(10.0, 30, 30);
            glTranslated(-54.0, 0.0, 0.0);    
            glutSolidSphere(10.0, 30, 30);
            glPopMatrix();

    glPopMatrix();  //������ ��ġ
    glPopMatrix();
    glPopMatrix();

    /*������ ��*/
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslated(33.0, 0.0, 3.0);
    glRotatef((GLfloat)rshoulder, 0.0, 1.0, 0.0);
    glCallList(list[2]);
        glPushMatrix();
        glColor3f(0.98, 0.80, 0.69);    //�챸��
        glTranslated(0.0, 0.0, 40.0);
        glRotatef((GLfloat)relbow, 0.0, 1.0, 0.0);
        glCallList(list[2]);
        /*������ ��*/
        glTranslated(0.0, 0.0, 40.0);
        glCallList(list[5]);
        glPopMatrix();
    glPopMatrix();
    
    /*���� ��*/
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslated(-33.0, 0.0, 3.0);
    glRotatef((GLfloat)lshoulder, 0.0, 1.0, 0.0);
    glCallList(list[2]);
        glPushMatrix();
        glColor3f(0.98, 0.80, 0.69);    //�챸��
        glTranslated(0.0, 0.0, 40.0);
        glRotatef((GLfloat)lelbow, 0.0, 1.0, 0.0);
        glCallList(list[2]);
        /*���� ��*/
        glTranslated(0.0, 0.0, 40.0);
        glCallList(list[5]);
        glPopMatrix();
    glPopMatrix();

    /*���� �ϴܺ�*/
    glPushMatrix();
    glTranslated(0.0, 0.0, 45.0);
    glColor3f(1.0, 0.64, 0.0);  //��Ȳ��
    glTranslated(0.0, 0.0, 5.0);
    glCallList(list[4]);
    glPopMatrix();

    /*������ �ٸ�*/
    glPushMatrix();
    glColor3f(0.98, 0.80, 0.69);    //�챸��
    glTranslated(13.0, 0.0, 110);
    glRotatef((GLfloat)rleg, 1.0, 0.0, 0.0);
    glCallList(list[6]);
        glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslated(0.0, 0.0, 30.0);
        glRotatef((GLfloat)rknee, 1.0, 0.0, 0.0);
        glCallList(list[7]);
        /*��*/
        glTranslated(0.0, 0.0, 66.0);
        glColor3f(0.0, 0.0, 0.0);
        glutSolidSphere(13.0, 30, 30);
        glPopMatrix();
    glPopMatrix();

    /*���� �ٸ�*/
    glPushMatrix();
    glColor3f(0.98, 0.80, 0.69);    //�챸��
    glTranslated(-13.0, 0.0, 110);
    glRotatef((GLfloat)lleg, 1.0, 0.0, 0.0);
    glCallList(list[6]);
        glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslated(0.0, 0.0, 30.0);
        glRotatef((GLfloat)lknee, 1.0, 0.0, 0.0);
        glCallList(list[7]);
        /*��*/
        glTranslated(0.0, 0.0, 66.0);
        glColor3f(0.0, 0.0, 0.0);
        glutSolidSphere(13.0, 30, 30);
        glPopMatrix();
    glPopMatrix();
}

/*��¡������� ���� (����)�� �׷��ִ� �Լ�*/
void DrawSemo(void) {
    glTranslatef(0.0, 75, 0.0);
    /*����*/
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.2);  //������
    glScalef(30, 50, 10);
    glutSolidCube(1);
    glPopMatrix();
    
    /*�Ӹ�*/
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
    
    /*��*/
    glPushMatrix();
    glTranslatef(0, 25, 0);
        /*������ ��*/
        glPushMatrix();
        glColor3f(1.0, 0.0, 0.2);  //������
        glTranslatef(15.0, 0.0, 0.0);
        glRotatef(15, 0.0, 0.0, 1.0);
        glTranslatef(0.0, -25.0, 0.0);
        glScalef(10, 50, 10);
        glutSolidCube(1);
        glPopMatrix();
        /*���� ��*/
        glPushMatrix();
        glColor3f(1.0, 0.0, 0.2);  //������
        glTranslatef(-15.0, 0.0, 0.0);
        glRotatef(-15, 0.0, 0.0, 1.0);
        glTranslatef(0.0, -25.0, 0.0);
        glScalef(10, 50, 10);
        glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
    /*�ٸ�*/
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
/*��¡����� �����ڸ� �׷��ִ� �Լ�*/
void DrawGamer(void) {
    glTranslatef(0.0, 75, 0.0);
    /*����*/
    glPushMatrix();
    glColor3f(0.05, 0.45, 0.05);  //�ʷϻ�
    glScalef(30, 50, 10);
    glutSolidCube(1);
    glPopMatrix();

    /*�Ӹ�*/
    glPushMatrix();
    glColor3f(0.98, 0.80, 0.69);    //�챸��
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

    /*��*/
    glPushMatrix();
    glTranslatef(0, 25, 0);
    /*������ ��*/
    glPushMatrix();
    glColor3f(0.05, 0.45, 0.05);  //�ʷϻ�
    glTranslatef(15.0, 0.0, 0.0);
    glRotatef(garmr, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -25.0, 0.0);
    glScalef(10, 50, 10);
    glutSolidCube(1);
    glPopMatrix();
    /*���� ��*/
    glPushMatrix();
    glColor3f(0.05, 0.45, 0.05);  //�ʷϻ�
    glTranslatef(-15.0, 0.0, 0.0);
    glRotatef(garml, 0.0, 0.0, 1.0);
    glTranslatef(0.0, -25.0, 0.0);
    glScalef(10, 50, 10);
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();
    
    /*�ٸ�*/
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
/*���ۼ�, �������� ǥ���� Line*/
void DrawLine() {
    glBegin(GL_QUADS);
    glColor3f(red, green, blue);
    glVertex3f(1000.0, 1.0, 1900.0);
    glVertex3f(-1000.0, 1.0, 1900.0);
    glVertex3f(-1000.0, 1.0, 1850.0);
    glVertex3f(1000.0, 1.0, 1850.0);
    glEnd();
}
/*������ �׷��ִ� �Լ�*/
void DrawTree() {
    /*���� ����*/
    glTranslatef(0.0, 200, 0.0);
    glPushMatrix();
    glColor3f(0.41, 0.23, 0.0);
    glScalef(100, 400, 50);
    glutSolidCube(1);
    glPopMatrix();
    /*���������� ǥ��*/
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
    
    gluLookAt(camx, camy, camz, fx, fy, fz, 0.0, 1.0, 0.0); //ī�޶� ����
    
    /*�ٴ��� �׷��ش� - ���� �ؽ�ó���� ���*/
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    loadTexture_2();
    glColor3f(0.7, 0.7, 0.7);
    glInterleavedArrays(GL_T2F_V3F, 0, g_parallelogramVertices);
    glDrawArrays(GL_QUADS, 0, 4);
    glPopMatrix();

    /*����κ��� �׷��ش�*/
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0.0, 220.0, yz);
    DrawYounghi();
    glPopMatrix();

    /*������ �׷��ش�*/
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    loadTexture();
    glTranslatef(0.0, 0.0, 20.0);
    DrawTree();
    glPopMatrix();

    /*����(����)�� �׷��ش�*/
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(100.0, 0.0, 100.0);
    DrawSemo(); //���� 1
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-100.0, 1.0, 100.0);
    DrawSemo(); //���� 2
    glPopMatrix();

    //*���� �÷��̾�� ����*//
    for (int i = 0; i < 20; i++) {  //20�� ����
        
        glPushMatrix();
        glTranslatef(movex[i], 2.0, movez[i]-500);
        glRotatef(180, 0.0, 1.0, 0.0);
        if (kill) { //���� ������ ���ᰡ �Ǹ�
            if (movez[i] > 800) {
                glRotatef(90, 1.0, 0.0, 0.0);   //���� ���ǿ� ������ �ο����� 90�� ȸ���Ͽ� �������� �����.
            }
        }
        DrawGamer();   //������ �׸���
        glPopMatrix();
    }
    
    /*��߼� �׸���*/
    glPushMatrix();
    DrawLine();
    glPopMatrix();
    /*������ �׸���*/
    glPushMatrix();
    glTranslatef(0.0, 3.0, -1600);
    DrawLine();
    glPopMatrix();


    glutSwapBuffers();
}

/*����κ��� ������ ����Ǹ� �޸��� TimerCallback*/
void RunTimer(int value) {
    if (yz > 1200) { larmk = larmk * -1; }  //z���� direction ����  
    if (yz < 100) { return; }   //���� ���ǿ� �Լ� ����

    yz += 5*larmk;  //����κ��� z��ǥ�� ��ȭ�Ѵ�
    neck += 10;     //���� ��� ���ư��Բ� �Ѵ�

    //�����ڼ��� ��ȯ
    if (rshoulder ==0 ) rshoulder=rshoulder;    
    else{
        rshoulder = (rshoulder - 10 * rarmk);
    }
    if (relbow ==0 ) relbow = relbow;
    else { relbow = (relbow - 10) % 360; }


    if (rleg > 48 || rleg < -48) rlegk = -rlegk;    //��ݰ� ����� �ٸ� ������ 8��ŭ�� 96�� ȸ����Ų��.  
    rleg = (rleg + 8 * rlegk) % 360;

    if (lleg > 48 || lleg < -48) llegk = -llegk;
    lleg = (lleg - 8 * llegk) % 360;

    if (rleg < 0) rvelo = 15;   //��ݰ� ����� �ٸ� ������ ������ ���� �� ������ ȸ���ϴ� �ӵ��� 15�� �����Ѵ�.
    if (rleg > 0) rvelo = 5;    //��ݰ� ����� �ٸ� ������ ������ ���� �� ������ ȸ���ϴ� �ӵ��� 5�� �����Ѵ�.
    if (lleg < 0) lvelo = 15;
    if (lleg > 0) lvelo = 5;

    rknee = (rleg + (rleg / 8) * rvelo * rkneek) % 360; //  leg�� �������� ���� ����ȸ�������� �����Ѵ�.
    lknee = (lleg + (lleg / 8) * lvelo * lkneek) % 360;

    glutPostRedisplay();
    glutTimerFunc(40, RunTimer, 1);
}

/*����κ��� �Ӹ� �۵� TimerCallback*/
void HeadMoveTimer(int value) {
    
    if (neck > 180 || neck < 0) {  //�ش� ���ǿ� direction �� �ٲ�
        headk = -headk;
        
    }
    neck = (neck - 5 * headk) % 360;
    
    if (headk > 0) {
        red = 1, green = 0, blue = 0;   //line�� ������ �������ش�. (���� �����ڰ� �����̸� �� �� �� : ������)
    }
    else {
        red = 0, green = 1, blue = 0.2; //���� �����ڵ��� ������ �� : �ʷϻ�
    }

    if (neck == -5 || neck == 185) {    //���� ȸ���������� �Լ� ����
        return;
    }


    glutPostRedisplay();
    glutTimerFunc(40, HeadMoveTimer, 1);
}

/*���� ������ �̵� TimerCallback*/
void PlayerMoveTimer(int value) {
    if (off < 0) { return; }    //off���� ���� �Լ� ����
    

    for (int i = 0; i < 20; i++) {  //20���� player�� ���� �ٸ� speed���� ����
        speed[i] = rand() % 15; //���� �������� ����
        movez[i] -= speed[i];
        if (movez[i] < 800) { //z���� ���� ������ �����ϸ�
            kill = 1;
            glutTimerFunc(40, RunTimer, 1); //���� �����̴� timercallback ���� �� ����
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
/*���� �� ���ϸ��̼� ȿ�� TimerCallback*/
void InitCamera(int value) {

    if (camz > 2500) { stop = -1; return; } //camz�� 2500�� ��ġ�ϸ� stop

    camz += 18;
    camy += 1;

    glutPostRedisplay();
    glutTimerFunc(40, InitCamera, 1);
}

void MySpecial(int key, int x, int y) {
    if (key == GLUT_KEY_UP) camy += 10.0;    //���� ����Ű�� ������ �� ī�޶��� y���� 10 �����Ѵ�
    if (key == GLUT_KEY_DOWN) camy -= 10.0;  //�Ʒ��� ����Ű�� ������ �� ī�޶��� y���� 10 ����
    if (key == GLUT_KEY_LEFT)  camx -= 10.1; //���� ����Ű�� ������ �� ī�޶��� x�� 10.1 ����
    if (key == GLUT_KEY_RIGHT) camx += 10.1; //������ ����Ű�� ������ �� ī�޶��� x�� 10.1 ����
    glutPostRedisplay();
}

/*keyboardcallback �Լ�.
 *1, 2, 3, 4, 5 : �پ��� ���� Ȯ�� ����
 * W, A, S, D : �������� ���� ���� ����
 �����̽��� : �κ� ���� ����
 */
void MyKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        if (stop > 0) { break; }
        glMatrixMode(GL_PROJECTION);	//���� ������ ���� ��� �������� �ٲ۴�.
        glLoadIdentity();
        gluPerspective(50, 1, 0.01, 2000.0);    //����������� ��ȯ
        camx = 0.0, camy = 500.0, camz = 800.0, fx = 0.0, fy = 100.0, fz = 0;  //ī�޶� ��ġ �� ���� ����
        break;

    case '2':
        if (stop>0) { break; }
        glMatrixMode(GL_PROJECTION);	//���� ������ ���� ��� �������� �ٲ۴�.
        glLoadIdentity();
        glOrtho(-1200.0, 1200.0, -1000.0, 1000.0, 0.0, 4000.0); //����������ĺ�ȯ
        camx = 850.0, camy = 550.0, camz = -600.0, fx = 0.0, fy = 0.0, fz = 900.0;;//ī�޶� ��ġ �� ���� ����
        break;
    case '3':
        if (stop > 0) { break; }
        glMatrixMode(GL_PROJECTION);	//���� ������ ���� ��� �������� �ٲ۴�.
        glLoadIdentity();
        gluPerspective(50, 1, 0.01, 2000.0); //����������� ��ȯ
        camx = -120.0, camy = 320.0, camz = -100.0, fx = 0.0, fy = -50.0, fz = 1500;//ī�޶� ��ġ �� ���� ����
        break;
    case '4':
        if (stop > 0) { break; }
        glMatrixMode(GL_PROJECTION);	//���� ������ ���� ��� �������� �ٲ۴�.
        glLoadIdentity();
        gluPerspective(50, 1, 0.01, 2000.0); //����������� ��ȯ
        camx = 120.0, camy = 330.0, camz = -100.0, fx = 0.0, fy = -100.0, fz = 1500;//ī�޶� ��ġ �� ���� ����
        break;
    case '5':
        if (stop > 0) { break; }
        glMatrixMode(GL_PROJECTION);	//���� ������ ���� ��� �������� �ٲ۴�.
        glLoadIdentity();
        glOrtho(-1200.0, 1200.0, -1000.0, 1000.0, 0.0, 4000.0);//����������ĺ�ȯ
        camx = 0.0, camy = 800.0, camz = 1200.0, fx = 0.0, fy = 0.0, fz = 900.0;;//ī�޶� ��ġ �� ���� ����
        break;

    case 'w':
    case 'W':
        camz -= 30; //ī�޶� Z��ǥ �̵�
        break;
    
    case 's':
    case 'S':
        camz += 30;//ī�޶� Z��ǥ �̵�
        break;
    case 'a':
    case 'A':
        camx -= 30;//ī�޶� X��ǥ �̵�
        break;
    case 'd':
    case 'D':
        camx += 30;//ī�޶� X��ǥ �̵�
        break;

    case ' ':   //�����̽���
        off = off * -1;
        glutTimerFunc(40, PlayerMoveTimer, 1);  //���� ������ �̵� timercallback
        glutTimerFunc(40, HeadMoveTimer, 1);    //���� �κ� ���� timercallback
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
    PlaySound(TEXT("squidgame.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT| SND_NOSTOP);    //������� ����
    
    glutTimerFunc(40, InitCamera, 1);
    GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };     //�ֺ��ݻ縦 ���� ���� RGBA
    GLfloat mat_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };        //���ݻ縦 ���� ���� RGBA
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };    //���ݻ縦 ���� ���� RGBA
    GLfloat mat_shininess[] = { 30.0 };                 //���ݻ� ���� ���
    GLfloat light_position[] = { 400.0, 400.0, 500.0, 0.0 };  //������ ��ġ, w=0 ����, ������������ ��ǥ�� ���ϴ� ���͹����� ���⼺ ����
    GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };   //���� �ֺ��� RGBA

    glClearColor(0.5, 0.8, 0.9, 0.0);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);        //Front �鿡 ���� �ֺ��� ��� : �ֺ� �ݻ翡 ���� ��ü��
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);        //Front �鿡 ���� ���ݻ籤 ��� : ���ݻ翡 ���� ��ü��
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);      //Front �鿡 ���� ���ݻ籤 ���, ���ݻ翡 ���� ��ü��
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);    //Front �鿡 ���� ���ݻ��� ���� ���
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  //������ ��ġ ����
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);  //���� �ֺ� ���� ��
   // glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);    //��ü���� �����ϴ� ��� �ٰ��� ���� ������ �������͸� ���
    /*���� ��� Ȱ��ȭ*/
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);    //��������
    glEnable(GL_NORMALIZE);
 
    glMatrixMode(GL_PROJECTION);	//���� ������ ���� ��� �������� �ٲ۴�.
    glLoadIdentity();
    gluPerspective(50, 1, 0.01, 2000.0);
    camx = 0.0, camy = 2.0, camz = 100.0, fx = 0.0, fy = 2.0, fz = 0;

    srand((unsigned)time(NULL));    //time seed �ʱ�ȭ
    
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
    gluDeleteQuadric(qobj); // Quadric Object�� ����

    return 0;
}