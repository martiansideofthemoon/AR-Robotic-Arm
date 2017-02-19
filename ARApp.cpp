#include <cstdio>
#include <cstring>
#include <cstdlib>				       
#include <iostream>

#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>		
#include <AR/ar.h>
#include <AR/gsub_lite.h>

#include "ik.hpp"
#include "ar_context.hpp"
#include "ar_gl_interface.hpp"

//Convenience typedef
typedef Matrix<float, 1, 1> Vector1f;

// Preferences.
bool windowed = true;           // Use windowed (true) or fullscreen mode (false) on launch.
int windowWidth = 1280;			// Initial window width, also updated during program execution.
int windowHeight = 720;         // Initial window height, also updated during program execution.
int windowDepth = 32;			// Fullscreen mode bit depth.
int windowRefresh = 0;			// Fullscreen mode refresh rate. Set to 0 to use default rate.

//AR Context
ar_context_t art;

//Kinematic Chain
ik_t kinch;

//Setup the kinematic chain
void setup_kinematic_chain(void)
{
  kinch.bone_length=80;
  Vector1f nc2, nc3, nc4;
  nc2<<20.0;
  nc3<<30.0;
  nc4<<40.0;
  std::cerr<<nc2.size()<<std::endl;
  ik_node_t n1(3,Vector3f(0.0,-45.0,0.0));
  ik_node_t n2(1,nc2);
  ik_node_t n3(1,nc3);
  ik_node_t n4(1,nc4);

  kinch.add_node(n1);
  kinch.add_node(n2);
  kinch.add_node(n3);
  kinch.add_node(n4);

  kinch.init();
}

int main(int argc, char** argv)
{
    char glutGamemode[32];
    //Camera parameters file - obtained from camera calibration
    char cparam_name[] = "./data/camera_params.dat";
    char vconf[512];

    //Pattern filenames
    char patt_name[]  = "./data/hiro.patt";
    char patt_name2[]  = "./data/kanji.patt";
    int  i;

    if( argc == 1 ) vconf[0] = '\0';
    else
      {
        strcpy( vconf, argv[1] );
        for( i = 2; i < argc; i++ )
	  {
	    strcat(vconf, " ");
	    strcat(vconf,argv[i]);
	  }
      }
    
    glutInit(&argc, argv);

    setup_kinematic_chain();

    if (!art.setup_camera(cparam_name, vconf))
      {
		ARLOGe("main(): Unable to set up AR camera.\n");
		exit(-1);
      }
    
    // Set up GL context(s) for OpenGL to draw into.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    if (!windowed)
      {
		if (windowRefresh) sprintf(glutGamemode, "%ix%i:%i@%i", windowWidth, windowHeight, windowDepth, windowRefresh);
		else sprintf(glutGamemode, "%ix%i:%i", windowWidth, windowHeight, windowDepth);
		glutGameModeString(glutGamemode);
		glutEnterGameMode();
      }
    else
      {
		glutInitWindowSize(windowWidth, windowHeight);
		glutCreateWindow(argv[0]);
      }

    // Setup ARgsub_lite library for current OpenGL context.
    if ((art.gArglSettings = arglSetupForCurrentContext(&(art.gCparamLT->param), arVideoGetPixelFormat())) == NULL)
      {
		ARLOGe("main(): arglSetupForCurrentContext() returned error.\n");
		art.cleanup();
		exit(-1);
      }
    arglSetupDebugMode(art.gArglSettings, art.gARHandle);
    arUtilTimerReset();
	
    // Load marker 1
    if (!art.setup_marker(patt_name, 1))
      {
		ARLOGe("main(): Unable to set up AR marker.\n");
		art.cleanup();
	exit(-1);
      }
    // Load marker 2
    if (!art.setup_marker(patt_name2, 2))
      {
		ARLOGe("main(): Unable to set up AR marker.\n");
		art.cleanup();
		exit(-1);
      }
    //Scaling second marker to be half of first.
    //Remove this if both markers are of same size.  
    art.patt2.gPatt_width=40.0;
      
    // Register GLUT event-handling callbacks.
    glutDisplayFunc(display_func);
    glutReshapeFunc(reshape_func);
    glutVisibilityFunc(visibility_func);
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
    
    return (0);
}

