# Zombified

# Installation
-go to build folder
-“Cmake ..”
-copy folders “data” and “shaders” to the build folder
-“make”
-“./zombie”

# Implemented:
-Profiling
-Fixed destroying function bug
-Fixed texture issues (textures randomly disappearing/not rendering properly)
-Changed spawning mechanics
-Scoreboard system (Each time you win, you add a point to your score tally by entering the same name)
	-scoreboard on the main menu
	-inputting name of winner at gameover
-Change map and fix map collisions with new map
-Animations
	-blood
	-mushroom cloud
	-limbs
	-zombies
	-flashing players to indicate damage
-Rendering UI on screen instead of header
	-timer
	-life bar
	-legs collected
-added music
-New textures
	-information page
	-buttons
	-start page images
	-winner page, gameover page
	-pause texture
	-new background
-Punching visualization
-Info page
-fixed elastic collision for moving bomb collisions
-fixing mapgrid so items don’t spawn on top each other
-fixing pathfinding
-implementing player to items collision
