# Advanced-Game-Technologies

To run set Game Tech as the startup project & software config as x64.

## Features:
* Triggerable blue stateful objects (Spinning capsule, Spring Box) (left click on objects for info)
* Swinging capsule which makes use of physical positional constrainsts
* Moving Stateful Cube Barriers
* Collectable Coins
* Main Player Ball makes use of friction while rolling
* Collisions:
  * AABB AABB – Bouncing floor at the end of the ramp (high coeff of restitution)
  * Sphere Capsule (also contains sphere sphere) - Player Ball with stateful capsule objects
  * Sphere OBB - Player ball & Ramp
  * Sphere AABB - player ball & base floor
* Box trigger over the green platform lets the player win
* Draw in sky (Press p to activate) (Ray vs Plane Interaction)
* Pushdown Automata Menu
* Grid-Based Path Finding in Level2
