# BreakingPoint
A real-time fracturing tool for Houdini by Mohamad Moneimne and Charles Wang.

[Watch the full demo video here.](https://vimeo.com/229512570)

## Overview
Fracturing preexisting models has its foundations in many entertainment industries such as games and film. _Breaking Point_ is an authoring tool that allows the user to fracture models in a highly interactive manner. This means that instead of needing to wait for lengthy physically-based simulations or real-time simulations that produce the same output every time, this plugin provides the best of both worlds by allowing the user to specify the location number of fractured pieces in real-time. With this in mind, users can iterate through simulations at a fast rate before deciding to bake the animation and export it for use elsewhere. 

As the plugins typical users, technical artists would use this tool to create fractures from any kind of mesh in a scene. Once the fracture simulation is complete, it can then be sent to effects artists who can composite additional destruction effects as they please. This tool would typically be used in an offline rendering shot of a fracturing object to be moved into productions such as games and animations. As mentioned before, an artist can iterate quickly, testing different impact points with real-time response. This produces a simulation of a fractured object based on some specified parameters such as a point of impact and the number of fractured pieces. The simulation can be expected to baked to an exportable format such as .fbx.

This project was created in four weeks for a course that focused on research concepts in computer graphics. It was inspired by the 2013 SIGGRAPH paper, _Real Time Dynamic Fracture with Volumetric Approximate Convex Decomposition_, by Muller, Chentanez, and Kim.
