# soleil

## Description

This project is a research on a game-engine and is written with OSG. Souces re currently in-progress.
This will be a Labyrinth that youhave to escape from with a tons of traps and not cool guys.


## Uage

Use the 'WASD' keys to move along the labyrinth and the mouse to orient yourself.
Start with '-d' to toggle the TrackBall view.



## In Progress
   * [DONE] Better collisions
   * [DONE] Speed not related to frames rate
   * Multiples textues configurable with multiples signs
      
## Road Map
   * Stressing light
   * Port compilation (GNU/Linux, Windows)
   * Some vicious monsters
   * A cool Menu
   * A next level zone
   * A end
   * A Tileable floor texture

## Bugs / PITA
   * The GetNextLine throw on EOF on Linux
   * On startup thre is a glitch with the mouse + the translation is shaking until we move the mouse

## Current features
   * Doom view with almost the collisions
   * loading map based on 'x' (wall) and '.' (space)
   * A floor
   * A wall with a texture
   * [Dev] Run with '-d' to start with a trackball
   * [Dev] Start a specific map with '-l /path/to/map/level'
