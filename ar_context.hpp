/*!
This file defines the ar_context_t class that contains everything needed to setup an AR context.
Here we are specifically looking for exactly 2 marker patterns. 
The words pattern and marker may be used interchangeably in further documentation.
*/

#ifndef _AR_CONTEXT_HPP_
#define _AR_CONTEXT_HPP_

#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>

/*! This is the marker pattern class. */
class ar_pattern_t
{
public:
  //! Width of the pattern in pixel units	
  //! Note if the two patterns used are of different size, relative pixel widths must 
  //! match actual relative widths
  ARdouble gPatt_width;

  //! This is going to contain the transformation from the camera coordinate system to the 
  //! pattern coordinate system.
  ARdouble gPatt_trans[3][4];
  
  //! Flag indicating whether pattern was found in seen image or not 
  bool gPatt_found;

  //! An id assigned to the pattern by AR context
  int gPatt_id;

  //!Default Constructor
  ar_pattern_t()
  {
    gPatt_width = 80.0;
    gPatt_found = false;
  }
};

/*! This is the AR context class */
class ar_context_t
{
public:

  //! Flag indicating if the augmented image frames are saved.
  bool gARTImageSavePlease;

  //! Handle for the 2D AR context, like detected patterns
  ARHandle *gARHandle;

  //! Pattern list handle for the context
  ARPattHandle *gARPattHandle;

  //! Number of image frames grabbed for processing
  long gCallCountMarkerDetect;

  //! Handle for the 3D AR context like transformations from the camera to marker
  AR3DHandle *gAR3DHandle;
  
  //! Parameters
  ARParamLT *gCparamLT;

  //! ARToolKit and OpenGL context settings
  ARGL_CONTEXT_SETTINGS_REF gArglSettings;
  
  //! Flag indicating if the pattern handle has been initialized or not
  bool pattern_init_flag;

  //! Pattern 1
  ar_pattern_t patt1;

  //! Pattern 2
  ar_pattern_t patt2;

  //!Default Constructor
  ar_context_t()
  {
    gARTImageSavePlease = false;
    gARHandle = NULL;
    gARPattHandle = NULL;
    gCallCountMarkerDetect = 0;
    gAR3DHandle = NULL;
    gCparamLT = NULL;
    gArglSettings = NULL;
    pattern_init_flag = false;
  }

  //! Initializing the pattern handle
  void init_pattern_handle(void)
  {
    gARPattHandle = arPattCreateHandle();
    pattern_init_flag = true;
  }

  //! Read the camera_params file and setup an input video stream
  int setup_camera(const char *cparam_name, char *vconf);

  //! Read the marker pattern files and load them into memory
  int setup_marker(const char *patt_name, int pid);

  //! Free memory and cleanup
  void cleanup(void);
};



#endif
