#include <iostream>
#include <string>
#include <unistd.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif


#include <Eigen/Dense>
#include "ik.hpp"
#include "gl_interface.hpp"

typedef Matrix<float, 1, 1> Vector1f;

ik_t kinch;

void usage(void) 
{
    exit(0);
}

//-----------------------------------------------------------------------

int main(int argc, char **argv)
{
  kinch.bone_length=80;
  Vector1f nc2, nc3, nc4;
  nc2<<-20.0;
  nc3<<-30.0;
  nc4<<-40.0;
  //std::cerr<<nc2.size()<<std::endl;
  ik_node_t n1(3,Vector3f(0.0,180,0));
  ik_node_t n2(1,nc2);
  ik_node_t n3(1,nc3);
  ik_node_t n4(1,nc4);
  
  kinch.add_node(n1);
  kinch.add_node(n2);
  kinch.add_node(n3);
  kinch.add_node(n4);
  kinch.target = Vector3f(100,7.0,100.0);

  kinch.init();
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(512,512);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("IK Test App");
  glutDisplayFunc(display_func);
  glutReshapeFunc(reshape_func);
  glutKeyboardFunc(keyboard_func);
  glEnable(GL_DEPTH_TEST);

  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };

  GLint shininess = 100;

  GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);

  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_LIGHT0);

  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // glEnable( GL_BLEND );

  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
  glutMainLoop();
      
  return 0;
}

//-----------------------------------------------------------------------
