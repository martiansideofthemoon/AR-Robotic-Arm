#include "ar_context.hpp"

int ar_context_t::setup_camera(const char *cparam_name, char *vconf)
{
    ARParam cparam;
    int	xsize, ysize;
    AR_PIXEL_FORMAT pixFormat;

    // Open the video path.
    if (arVideoOpen(vconf) < 0)
      {
    	ARLOGe("setupCamera(): Unable to open connection to camera.\n");
    	return (FALSE);
      }
	
    // Find the size of the window.
    if (arVideoGetSize(&xsize, &ysize) < 0)
      {
        ARLOGe("setupCamera(): Unable to determine camera frame size.\n");
        arVideoClose();
        return (FALSE);
      }
    ARLOGi("Camera image size (x,y) = (%d,%d)\n", xsize, ysize);
	
    // Get the format in which the camera is returning pixels.
    pixFormat = arVideoGetPixelFormat();
    if (pixFormat == AR_PIXEL_FORMAT_INVALID)
      {
    	ARLOGe("setupCamera(): Camera is using unsupported pixel format.\n");
        arVideoClose();
	return (FALSE);
      }
    
    // Load the camera parameters, resize for the window and init.
    if (arParamLoad(cparam_name, 1, &cparam) < 0)
      {
	ARLOGe("setupCamera(): Error loading parameter file %s for camera.\n", cparam_name);
        arVideoClose();
        return (FALSE);
      }
    if (cparam.xsize != xsize || cparam.ysize != ysize)
      {
        ARLOGw("*** Camera Parameter resized from %d, %d. ***\n", cparam.xsize, cparam.ysize);
        arParamChangeSize(&cparam, xsize, ysize, &cparam);
      }
#ifdef DEBUG
    ARLOG("*** Camera Parameter ***\n");
    arParamDisp(&cparam);
#endif
    if ((gCparamLT = arParamLTCreate(&cparam, AR_PARAM_LT_DEFAULT_OFFSET)) == NULL)
      {
        ARLOGe("setupCamera(): Error: arParamLTCreate.\n");
        return (FALSE);
      }
    
    if ((gARHandle = arCreateHandle(gCparamLT)) == NULL)
      {
        ARLOGe("setupCamera(): Error: arCreateHandle.\n");
        return (FALSE);
      }
    if (arSetPixelFormat(gARHandle, pixFormat) < 0)
      {
        ARLOGe("setupCamera(): Error: arSetPixelFormat.\n");
        return (FALSE);
      }
    if (arSetDebugMode(gARHandle, AR_DEBUG_DISABLE) < 0)
      {
	ARLOGe("setupCamera(): Error: arSetDebugMode.\n");
	return (FALSE);
      }
    if ((gAR3DHandle = ar3DCreateHandle(&cparam)) == NULL)
      {
        ARLOGe("setupCamera(): Error: ar3DCreateHandle.\n");
        return (FALSE);
      }
    
    if (arVideoCapStart() != 0)
      {
    	ARLOGe("setupCamera(): Unable to begin camera data capture.\n");
	return (FALSE);		
      }
    return (TRUE);
}

int ar_context_t::setup_marker(const char *patt_name, int pid)
{
  if (!pattern_init_flag) init_pattern_handle();
  
  if (pid == 1)
    {
      if ((patt1.gPatt_id = arPattLoad(gARPattHandle, patt_name)) < 0) {
	ARLOGe("setupMarker(): Error loading pattern file %s.\n", patt_name);
	arPattDeleteHandle(gARPattHandle);
	return (FALSE);
      }
    }
  else if (pid == 2)
    {
       if ((patt2.gPatt_id = arPattLoad(gARPattHandle, patt_name)) < 0) {
	ARLOGe("setupMarker(): Error loading pattern file %s.\n", patt_name);
	arPattDeleteHandle(gARPattHandle);
	return (FALSE);
      }
    }
  
  arPattAttach(gARHandle, gARPattHandle);
  
  return (TRUE);
}

void ar_context_t::cleanup(void)
{
  arglCleanup(gArglSettings);
  gArglSettings = NULL;
  arPattDetach(gARHandle);
  arPattDeleteHandle(gARPattHandle);
  arVideoCapStop();
  ar3DDeleteHandle(&gAR3DHandle);
  arDeleteHandle(gARHandle);
  arParamLTFree(&gCparamLT);
  arVideoClose();
}
