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

static RGB_t* palettenColors = NULL;
static RGB_t* frameColors = NULL;
static int nColors = 0;
static LayoutData *layoutData;
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
	frameColors = new RGB_t[layoutData->nPanels];
	for(int i =0; i < layoutData->nPanels; i++) {
		int color = drand48() * nColors;
		frameColors[i] = palettenColors[color];
	}
}

RGB_t calculateColor(RGB_t color, Frame_t panel) {
	HSV_t value;
	RGBtoHSV(color, &value);
	value.V /= 3;
	RGB_t ret;
	HSVtoRGB(value, &ret);
	return ret;
}
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
	for(int i =0; i < layoutData->nPanels; i++) {
		RGB_t color = calculateColor(frameColors[i], frames[i]);
		frames[i].panelId = layoutData->panels[i].panelId;
		frames[i].r = color.R;
		frames[i].g = color.G;
		frames[i].b = color.B;
		frames[i].transTime = TRANSITION_TIME;
	}
	*nFrames = layoutData->nPanels;
}

/**
 * @description: called once when the plugin is being closed.
 * Do all deallocation for memory allocated in initplugin here
 */
void pluginCleanup(){
	//do deallocation here
}
