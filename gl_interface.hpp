#ifndef _GL_INTERFACE_HPP_
#define _GL_INTERFACE_HPP_

#include <iostream>
#include <fstream>
#include <string>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include "ik.hpp"

void display_func(void);

void reshape_func(int w, int h);

void keyboard_func(unsigned char key, int x, int y);

#endif
