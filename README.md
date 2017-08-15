# Nanoleaf-Aurora-Game-of-Life-Audio-Visualizer
  This repository contains a collection of Rhythm Plugins (and some fun visual-only plugins) for the Nanoleaf Aurora. To use the plugins checkout the provided dev tools repos from Nanoleaf (https://github.com/nanoleaf). Below are some sort descriptions of each plugin.

## DancingTiles
  Uses a "diffusion" effect and beat detection found in one of the "FrequncyStars" example from the nanoleaf dev tools. The beat detection sets the color of a new light source from one of the 7 colors in the provided pallete based on the frequency and the light source is spawned at the center of a random panel.

  There's also the ability to enable tempo to change the diffusion amount, as the tempo increases the diffusion decreases logarithmically. The idea being that as you increase the number of light sources on the panel you want each individual light source to be more distict against the other light sources. A faster tempo song will in theory spawn more light sources. There's a chance that this will be changed to energy of the song.
  
  To decrease "strobe" effect each light source has a "lifespan" so that it will last (assuming the it's not removed from the array for a new light source) to the next loop of getPluginFrame().

## DancingTilesOld
  Old implementation of DancingTiles, probably will be removed.

## GameOfLife
  similar to DancingTiles except the lightsources are cells in Conway's Game of Life. where DancingTiles will spawn only one light source, GameOfLife will spawn 5 in a glider formation. The GameOfLife implementation isn't correctly working right now, ideally I'd like to represent the grid "behind" the panels in an array (which might not be the most resource friendly approach) as opposed to just a list of all light sources since figuring out where to spawn a new light source currently takes a triple nested loop thru the array, but I currently don't have the time to fix it.

## MovingLightSource

## StainGlass
  from 7 pallete colors assigns a random constant color to each panel. creating a "stain glass" effect. This is currently hacked together.

## StainGlassDancingTiles
  Combines together StainGlass and DancingTiles where the light sources do the exact opposite and divide the current panel's color in half.
