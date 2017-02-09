/*!
This file defines the OpenGL interface for interacting with the ARApp.
*/
#ifndef _AR_GL_INTERFACE_HPP_
#define _AR_GL_INTERFACE_HPP_

//! Draw a colored cube
void draw_cube(void);
//! Draw a colored rotating cube
void draw_cube_update(float timeDelta);
//! Draw the IK Chain
void draw_kin_chain(void);
//! Draw the coordinate axis
void draw_axis(void);

//! GLUT keyboard callback
//! Look at this function to figure out what keyboard controls are available
void keyboard_func(unsigned char key, int x, int y);

//! GLUT visibility callback
//! This registers the idle callback, when the window is visible
void visibility_func(int visible);

//! GLUT window reshape callback
void reshape_func(int w, int h);

//! GLUT display callback - all drawing takes place here
void display_func(void);

//! GLUT idle callback. This runs whenever the GLUT event queue has no other event to process
//! Handles image frame capture and marker detection.
void idle_func(void);

#endif
