#include "gl_interface.hpp"

extern ik_t kinch;

void draw_axes(float length)
{
  glColor3f(1.0,0.0,0.0);
  glBegin(GL_LINES);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(length,0.0,0.0);
  glEnd();
  glColor3f(0.0,1.0,0.0);
  glBegin(GL_LINES);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(0.0,length,0.0);
  glEnd();
  glColor3f(0.0,0.0,1.0);
  glBegin(GL_LINES);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(0.0,0.0,length);
  glEnd();
  glColor3f(1.0,1.0,1.0);
  glPopMatrix();
}

void display_func(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(300,0,300,0,0,-1,0,1,0);

  glTranslatef(0.0,-50.0,0.0);
  glPushMatrix();
  kinch.render();

  draw_axes(100);  
  
  glutSwapBuffers();
}

void reshape_func(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  gluPerspective(60.0, 1.0, 1.0, 1000.0);
  
  glutPostRedisplay();
}

void keyboard_func (unsigned char key, int x, int y)
{
  switch (key)
    {
    case 27:
      exit(0);
      break;
    case 'u':
      kinch.update();
      glutPostRedisplay();
    default:
      break;
    }
}
