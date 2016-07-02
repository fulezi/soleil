# soleil

## Description

This project is a research on a game-engine and is written with OSG. Souces are currently in-progress.
This will be a Labyrinth that youhave to escape from with a tons of traps and not cool guys.


## Uage

Use the 'WASD' keys to move along the labyrinth and the mouse to orient yourself.
Start with '-d' to toggle the TrackBall view.



## In Progress
   * A next level zone  
   * A end
   * Death when player colliding with the monster
   * Some anxious sounds

   
## Road Map
   * Stressing light
   * Port compilation (GNU/Linux, Windows)
   * Bump textures
   * Allow the configuration of the texture of the floor
   * Transparency?
   * Include medias in the commit
   
## Bugs / PITA
   * On startup there is a glitch with the mouse + the translation is shaking until we move the mouse

## Current features
   * Doom view with the collisions
   * loading map based on YAML with configurable walls textures
   * A floor with repeated texture
   * A wall with a texture
   * Some vicious monsters   
   * [Dev] Run with '-d' to start with a trackball
   * [Dev] Start a specific map with '-l /path/to/map/level'
