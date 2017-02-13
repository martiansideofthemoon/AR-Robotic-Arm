
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <iostream>
#include <Eigen/Dense>

#include "ar_context.hpp"
#include "ar_gl_interface.hpp"
#include "ik.hpp"

//! Units received from ARToolKit tracking will be multiplied
//! by this factor before being used in OpenGL drawing.
#define VIEW_SCALEFACTOR		1.0
//! Objects closer to the camera than this will not be
//! displayed. OpenGL units.
#define VIEW_DISTANCE_MIN		40.0
//! Objects further away from the camera than this will
//! not be displayed. OpenGL units.
#define VIEW_DISTANCE_MAX		10000.0

typedef Matrix<float, 1, 1> Vector1f;

// Drawing.

int gDrawRotate = FALSE;
float gDrawRotateAngle = 0;
int gARTImageSavePlease = FALSE;
int gCallCountMarkerDetect = 0;

extern int windowWidth;
extern int windowHeight;

extern ar_context_t art;
extern ik_t kinch;

void draw_cube(void)
{
    // Colour cube data.
  int i;
  float fSize = 10.0f;
  const GLfloat cube_vertices [8][3] =
    {
      /* +z */ {0.5f, 0.5f, 0.5f},
      {0.5f, -0.5f, 0.5f},
      {-0.5f, -0.5f, 0.5f},
      {-0.5f, 0.5f, 0.5f},
      /* -z */ {0.5f, 0.5f, -0.5f},
      {0.5f, -0.5f, -0.5f},
      {-0.5f, -0.5f, -0.5f},
      {-0.5f, 0.5f, -0.5f}
    };
    const GLubyte cube_vertex_colors [8][4] =
      {
        {255, 255, 255, 255},
	{255, 255, 0, 255},
	{0, 255, 0, 255},
	{0, 255, 255, 255},
        {255, 0, 255, 255},
	{255, 0, 0, 255},
	{0, 0, 0, 255},
	{0, 0, 255, 255}
      };
    const GLubyte cube_faces [6][4] =
      { /* ccw-winding */
        /* +z */ {3, 2, 1, 0},
	/* -y */ {2, 3, 7, 6},
	/* +y */ {0, 1, 5, 4},
        /* -x */ {3, 0, 4, 7},
	/* +x */ {1, 2, 6, 5},
	/* -z */ {4, 5, 6, 7}
      };

    glPushMatrix(); // Save world coordinate system.
    glRotatef(gDrawRotateAngle, 0.0f, 0.0f, 1.0f); // Rotate about z axis.
    glScalef(fSize, fSize, fSize);
    glTranslatef(0.0f, 0.0f, 0.5f); // Place base of cube on marker surface.
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, cube_vertex_colors);
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    for (i = 0; i < 6; i++)
      {
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
      }
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4ub(0, 0, 0, 255);
    for (i = 0; i < 6; i++)
      {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, &(cube_faces[i][0]));
      }
    glPopMatrix();    // Restore world coordinate system.
}

void draw_cube_update(float timeDelta)
{
  if (gDrawRotate)
    {
      gDrawRotateAngle += timeDelta * 45.0f; // Rotate cube at 45 degrees per second.
      if (gDrawRotateAngle > 360.0f) gDrawRotateAngle -= 360.0f;
    }
}

//! TODO CS775
//! Draw the kinematic chain kinch
void draw_kin_chain(void)
{
  glColor3ub(0,0,0);
  kinch.render();
}

void draw_axis(void)
{
	glColor3ub(255,0,0);
    glBegin(GL_LINES);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(200.0,0.0,0.0);
    glEnd();

    glColor3ub(0,255,0);
    glBegin(GL_LINES);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,200.0,0.0);
    glEnd();

    glColor3ub(0,0,255);
    glBegin(GL_LINES);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,200.0);
    glEnd();
}

void keyboard_func(unsigned char key, int x, int y)
{
  int mode, threshChange = 0;
  AR_LABELING_THRESH_MODE modea;

  switch (key)
    {
    //Quit
    case 0x1B:
    case 'Q':
    case 'q':
      art.cleanup();
      exit(0);
      break;
    //Cube, if drawn, starts rotating
    case ' ':
      gDrawRotate = !gDrawRotate;
      break;
    //Change image processing mode for ARToolkit
    case 'X':
    case 'x':
      arGetImageProcMode(art.gARHandle, &mode);
      switch (mode) {
      case AR_IMAGE_PROC_FRAME_IMAGE:  mode = AR_IMAGE_PROC_FIELD_IMAGE; break;
      case AR_IMAGE_PROC_FIELD_IMAGE:
      default: mode = AR_IMAGE_PROC_FRAME_IMAGE; break;
      }
      arSetImageProcMode(art.gARHandle, mode);
      break;
    //Log Camera capture FPS
    case 'C':
    case 'c':
      ARLOGe("*** Camera - %f (frame/sec)\n", (double)gCallCountMarkerDetect/arUtilTimer());
      gCallCountMarkerDetect = 0;
      arUtilTimerReset();
      break;
    //Change binarization thresholding mode for ARToolkit
    case 'a':
    case 'A':
      arGetLabelingThreshMode(art.gARHandle, &modea);
      switch (modea) {
      case AR_LABELING_THRESH_MODE_MANUAL:        modea = AR_LABELING_THRESH_MODE_AUTO_MEDIAN; break;
      case AR_LABELING_THRESH_MODE_AUTO_MEDIAN:   modea = AR_LABELING_THRESH_MODE_AUTO_OTSU; break;
      case AR_LABELING_THRESH_MODE_AUTO_OTSU:     modea = AR_LABELING_THRESH_MODE_AUTO_ADAPTIVE; break;
      case AR_LABELING_THRESH_MODE_AUTO_ADAPTIVE: modea = AR_LABELING_THRESH_MODE_AUTO_BRACKETING; break;
      case AR_LABELING_THRESH_MODE_AUTO_BRACKETING:
      default: modea = AR_LABELING_THRESH_MODE_MANUAL; break;
      }
      arSetLabelingThreshMode(art.gARHandle, modea);
      break;
    //Change threshold mode for ARToolkit
    case '-':
      threshChange = -5;
      break;
    //Change threshold mode for ARToolkit
    case '+':
    case '=':
      threshChange = +5;
      break;
    //Enable/Disable Debug mode
    case 'D':
    case 'd':
      arGetDebugMode(art.gARHandle, &mode);
      arSetDebugMode(art.gARHandle, !mode);
      break;
    //Save an augmented image
    case 's':
    case 'S':
      if (!gARTImageSavePlease) gARTImageSavePlease = TRUE;
      break;
    //Update the Kinematic Chain
    case 'u':
      kinch.update();
      glutPostRedisplay();
      break;
    //Reset the Kinematic Chain. Use this, to interactively break free of gimbal locks.
    case 'r':
      kinch.reset();
      glutPostRedisplay();
      break;

    default:
      break;
    }
  if (threshChange)
    {
      int threshhold;
      arGetLabelingThresh(art.gARHandle, &threshhold);
      threshhold += threshChange;
      if (threshhold < 0) threshhold = 0;
      if (threshhold > 255) threshhold = 255;
      arSetLabelingThresh(art.gARHandle, threshhold);
    }
}

void visibility_func(int visible)
{
  if (visible == GLUT_VISIBLE)
    {
      glutIdleFunc(idle_func);
    }
  else
    {
      glutIdleFunc(NULL);
    }
}

void reshape_func(int w, int h)
{
  windowWidth = w;
  windowHeight = h;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void display_func(void)
{
  ARdouble p[16];
  ARdouble m[16];

  // Select correct buffer for this context.
  glDrawBuffer(GL_BACK);
  // Clear the buffers for new frame.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  arglDispImage(art.gArglSettings);

  // Setup Projection transformation.
  arglCameraFrustumRH(&(art.gCparamLT->param), VIEW_DISTANCE_MIN, VIEW_DISTANCE_MAX, p);
  glMatrixMode(GL_PROJECTION);
#ifdef ARDOUBLE_IS_FLOAT
  glLoadMatrixf(p);
#else
  glLoadMatrixd(p);
#endif
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_DEPTH_TEST);

  // Viewing transformation.
  glLoadIdentity();
  // Lighting and geometry that _moves with_ the camera should go here.
  // (i.e., must be specified before viewing transformations.)
  // none

  Matrix4f mat, mat2;
  Matrix4f imat, imat2;

  //If pattern 1 is detected, do this
  if (art.patt1.gPatt_found) {

    // Calculate the camera position relative to the marker.
    // Replace VIEW_SCALEFACTOR with 1.0 to make one drawing unit equal to 1.0 ARToolKit units (usually millimeters).
    arglCameraViewRH((const ARdouble (*)[4])art.patt1.gPatt_trans, m, VIEW_SCALEFACTOR);
    glPushMatrix();
#ifdef ARDOUBLE_IS_FLOAT
    glLoadMatrixf(m);
#else
    glLoadMatrixd(m);
#endif

    //Initialize an Eigen matrix from the camera to pattern 1 transform
    mat << art.patt1.gPatt_trans[0][0],art.patt1.gPatt_trans[0][1],art.patt1.gPatt_trans[0][2],art.patt1.gPatt_trans[0][3],
      -art.patt1.gPatt_trans[1][0],-art.patt1.gPatt_trans[1][1],-art.patt1.gPatt_trans[1][2],-art.patt1.gPatt_trans[1][3],
      -art.patt1.gPatt_trans[2][0],-art.patt1.gPatt_trans[2][1],-art.patt1.gPatt_trans[2][2],-art.patt1.gPatt_trans[2][3],
      0.0,0.0,0.0,1.0;
    imat=mat.inverse();

    // All lighting and geometry to be drawn relative to the marker goes here.

    //! TODO CS775
    //! Replace this cube by the kinematic chain
    draw_kin_chain();
    draw_axis();
    glPopMatrix();

  } // gPatt_found

  //If pattern 2 is detected, do this
  if (art.patt2.gPatt_found) {

    // Calculate the camera position relative to the marker.
    // Replace VIEW_SCALEFACTOR with 1.0 to make one drawing unit equal to 1.0 ARToolKit units (usually millimeters).
    arglCameraViewRH((const ARdouble (*)[4])art.patt2.gPatt_trans, m, VIEW_SCALEFACTOR);
    glPushMatrix();
#ifdef ARDOUBLE_IS_FLOAT
    glLoadMatrixf(m);
#else
    glLoadMatrixd(m);
#endif
    //Initialize an Eigen matrix from the camera to pattern 2 transform
    mat2 << art.patt2.gPatt_trans[0][0],art.patt2.gPatt_trans[0][1],art.patt2.gPatt_trans[0][2],art.patt2.gPatt_trans[0][3],
      -art.patt2.gPatt_trans[1][0],-art.patt2.gPatt_trans[1][1],-art.patt2.gPatt_trans[1][2],-art.patt2.gPatt_trans[1][3],
      -art.patt2.gPatt_trans[2][0],-art.patt2.gPatt_trans[2][1],-art.patt2.gPatt_trans[2][2],-art.patt2.gPatt_trans[2][3],
      0.0,0.0,0.0,1.0;
	  imat2 = mat2.inverse();
    //! TODO CS775
    //Get the coordinates of the target in the frame of the marker in which the Kinematic Chain
    //is being drawn.
    Vector4f target = imat*mat2*Vector4f(0,0,0,1);
    //! TODO CS775
 	  //Set that as the goal for the kinematic chain
    kinch.target = Vector3f(target[0]/target[3], target[1]/target[3], target[2]/target[3]);
    //! TODO CS775
   	//Update the chain
    kinch.update();
    glPopMatrix();
  }

  glutSwapBuffers();
}

void idle_func(void)
{
  static int imageNumber = 0;
  static int ms_prev;
  int ms;
  float s_elapsed;
  AR2VideoBufferT *image;
  ARdouble err;

  int j, k, l;

  // Find out how long since idle_func() last ran.
  ms = glutGet(GLUT_ELAPSED_TIME);
  s_elapsed = (float)(ms - ms_prev) * 0.001f;
  if (s_elapsed < 0.01f) return; // Don't update more often than 100 Hz.
  ms_prev = ms;

  // Update drawing (for rotating cube)
  draw_cube_update(s_elapsed);

  // Grab a video frame.
  if ((image = arVideoGetImage()) != NULL)
    {

      arglPixelBufferDataUpload(art.gArglSettings, image->buff);

      if (gARTImageSavePlease)
	{
	  char imageNumberText[15];
	  sprintf(imageNumberText, "image-%04d.jpg", imageNumber++);
	  if (arVideoSaveImageJPEG(art.gARHandle->xsize, art.gARHandle->ysize, art.gARHandle->arPixelFormat, image->buff, imageNumberText, 75, 0) < 0)
	    {
	      ARLOGe("Error saving video image.\n");
	    }
	  gARTImageSavePlease = false;
        }

      // Increment ARToolKit FPS counter.
      gCallCountMarkerDetect++;

      // Detect the markers in the video frame.
      if (arDetectMarker(art.gARHandle, image) < 0)
		{
	 	 exit(-1);
		}

      // Check through the marker_info array for highest confidence
      // visible marker matching our preferred pattern.
      k = -1;
      l = -1;

      for (j = 0; j < art.gARHandle->marker_num; j++)
		{
	  		if (art.gARHandle->markerInfo[j].id == art.patt1.gPatt_id)
	    	{
	      		if (k == -1 && l == -1) k = j; // First marker detected.
	      		else if (art.gARHandle->markerInfo[j].cf > art.gARHandle->markerInfo[k].cf)
					k = j; // Higher confidence marker detected.
	    	}
	    	else if (art.gARHandle->markerInfo[j].id == art.patt2.gPatt_id)
	    	{
	     	 	if (l == -1 && k == -1) l = j; // First marker detected.
	      		else if (art.gARHandle->markerInfo[j].cf > art.gARHandle->markerInfo[l].cf)
					l = j; // Higher confidence marker detected.
	   	 }
		}

      if (k != -1)
		{
	  		// Get the transformation between the marker and the real camera into gPatt_trans.
	  		err = arGetTransMatSquare(art.gAR3DHandle, &(art.gARHandle->markerInfo[k]), art.patt1.gPatt_width, art.patt1.gPatt_trans);
	  		art.patt1.gPatt_found = true;
		}
      else
		{
	  		art.patt1.gPatt_found = false;
		}
      if (l != -1)
		{
	  		// Get the transformation between the marker and the real camera into gPatt_trans.
	  		err = arGetTransMatSquare(art.gAR3DHandle, &(art.gARHandle->markerInfo[l]), art.patt2.gPatt_width, art.patt2.gPatt_trans);
	  		art.patt2.gPatt_found = true;
		}
      else
		{
	 	 art.patt2.gPatt_found = false;
		}
      // Tell GLUT the display has changed.
      glutPostRedisplay();
    }
}
