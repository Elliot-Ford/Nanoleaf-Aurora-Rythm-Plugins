/*
    Copyright 2017 Nanoleaf Ltd.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */



#include "AuroraPlugin.h"
#include "LayoutProcessingUtils.h"
#include "ColorUtils.h"
#include "DataManager.h"
#include <math.h>
#include "PluginFeatures.h"
#include "Logger.h"

#ifdef __cplusplus
extern "C" {
#endif

	void initPlugin();
	void getPluginFrame(Frame_t* frames, int* nFrames, int* sleepTime);
	void pluginCleanup();

#ifdef __cplusplus
}
#endif

#define TRANSITION_TIME 1
#define MAX_SOURCES 7
#define BASE_COLOR_R 0 // the next three are background colors
#define BASE_COLOR_G 0
#define BASE_COLOR_B 0
#define ADJACENT_PANEL_DISTANCE 86.599995 // hard coded distance between panel centeroids


typedef struct {
  float x;
  float y;
  int R;
  int G;
  int B;
} source_t;

static RGB_t* palettenColors = NULL;
static int nColors = 0;
static LayoutData *layoutData;
static source_t *sources;
static int nSources = 0;
static bool toggle = false;
static bool toggle1 = false;
static int movementSpeed = 5;

/**
 * @description: Initialize the plugin. Called once, when the plugin is loaded.
 * This function can be used to enable rhythm or advanced features,
 * e.g., to enable energy feature, simply call enableEnergy()
 * It can also be used to load the LayoutData and the colorPalette from the DataManager.
 * Any allocation, if done here, should be deallocated in the plugin cleanup function
 *
 */
void initPlugin(){
	getColorPalette(&palettenColors, &nColors);
	layoutData = getLayoutData();

  PRINTLOG("The layout has %d panels:\n", layoutData->nPanels);
  for (int i = 0; i < layoutData->nPanels; i++) {
      PRINTLOG("   Id: %d   X, Y: %lf, %lf\n", layoutData->panels[i].panelId,
             layoutData->panels[i].shape->getCentroid().x, layoutData->panels[i].shape->getCentroid().y);
  }

  sources = new source_t[1];
  sources[0].x = -299;
  sources[0].y = 0;
  sources[0].R = 0;
  sources[0].G = 255;
  sources[0].B = 255;
  nSources++;
}

/** Compute cartesian distance between two points */
float distance(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

/**
  * @description: This function will render the colour of the given single panel given
  * the positions of all the lights in the light source list.
  */
void renderPanel(Panel *panel, int *returnR, int *returnG, int *returnB) {
  float R = BASE_COLOR_R;
  float G = BASE_COLOR_G;
  float B = BASE_COLOR_B;

  //Iterate through all the sources
  //Depending how close the source is to the panel, we take some fraction of its color and mix it into an
  //accumulator. Newest soruces have the most weight. Old sources die away until they are gone.
  for(int i = 0; i < nSources; i++) {
    float d = distance(panel->shape->getCentroid().x, panel->shape->getCentroid().y,
                       sources[i].x, sources[i].y);
    d = d / ADJACENT_PANEL_DISTANCE;
    float d2 = d*d;
    float factor = 1.0 / (d2*1.5 + 1.0);

    R = R * (1.0 - factor) + sources[i].R * factor;
    G = G * (1.0 - factor) + sources[i].G * factor;
    B = B * (1.0 - factor) + sources[i].B * factor;
  }
  *returnR = (int)R;
  *returnG = (int)G;
  *returnB = (int)B;
}
/**
RGB_t calculateColor(RGB_t color, Frame_t panel) {
	HSV_t value;
	RGBtoHSV(color, &value);
	value.V /= 3;
	RGB_t ret;
	HSVtoRGB(value, &ret);
	return ret;
}
**/
/**
 * @description: this the 'main' function that gives a frame to the Aurora to display onto the panels
 * To obtain updated values of enabled features, simply call get<feature_name>, e.g.,
 * getEnergy(), getIsBeat().
 *
 * If the plugin is a sound visualization plugin, the sleepTime variable will be NULL and is not required to be
 * filled in
 * This function, if is an effects plugin, can specify the interval it is to be called at through the sleepTime variable
 * if its a sound visualization plugin, this function is called at an interval of 50ms or more.
 *
 * @param frames: a pre-allocated buffer of the Frame_t structure to fill up with RGB values to show on panels.
 * Maximum size of this buffer is equal to the number of panels
 * @param nFrames: fill with the number of frames in frames
 * @param sleepTime: specify interval after which this function is called again, NULL if sound visualization plugin
 */
void getPluginFrame(Frame_t* frames, int* nFrames, int* sleepTime){
  int R;
  int G;
  int B;
	for(int i =0; i < layoutData->nPanels; i++) {
		//RGB_t color = calculateColor(frameColors[i], frames[i]);
    renderPanel(&layoutData->panels[i], &R, &G, &B);
		frames[i].panelId = layoutData->panels[i].panelId;
		frames[i].r = R;
		frames[i].g = G;
		frames[i].b = B;
		frames[i].transTime = TRANSITION_TIME;
	}
  if(toggle && toggle1) {
    sources[0].x += movementSpeed;
  } else if(!toggle && !toggle1){
    sources[0].x -= movementSpeed;
  } else if(!toggle && toggle1) {
    sources[0].y += movementSpeed;
  } else if(toggle && !toggle1){
    sources[0].y -= movementSpeed;
  }

  if(sources[0].x >= -299 + ADJACENT_PANEL_DISTANCE * 2) {
    toggle = false;
  } else if(sources[0].x <= -299) {
    toggle = true;
  }
  if(sources[0].y >= -86 + ADJACENT_PANEL_DISTANCE) {
    toggle1 = false;
  } else if(sources[0].y <= -86) {
    toggle1 = true;
  }
  //PRINTLOG("X: %f Y: %f\n", sources[0].x, sources[0].y);
	*nFrames = layoutData->nPanels;
}

/**
 * @description: called once when the plugin is being closed.
 * Do all deallocation for memory allocated in initplugin here
 */
void pluginCleanup(){
	//do deallocation here
}
