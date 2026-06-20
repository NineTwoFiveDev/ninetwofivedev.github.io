# Artifact 2: Algorithms and data structure

## Project Links
* [Original Code Source](LINK_TO_BEFORE_CODE)

* [Enhanced Code Source](LINK_TO_AFTER_CODE)

---

## Narrative Reflection

This project is a 3D graphics program I originally built for CS-330. It uses C++ and OpenGL to draw a 3D scene of a side table with a lamp, a candle, coasters, and remotes. Originally, the code was very basic: every time the screen refreshed, the program simply read a long list of instructions to draw each piece of the scene, one by one.

I chose this project because 3D graphics require a lot of computational power, making it a great opportunity to show how to make code run faster and more efficiently. In the original version, the computer was working hard. It was drawing every object in the room, even if the camera was facing the opposite direction and looking at a blank wall. To fix this, I added an optimization called View Frustum Culling. Think of it like looking through a camera viewfinder; the program now calculates exactly what the camera can see.

To make this work, I did two things:
Organized the Data: Instead of hardcoding exactly where and how to draw each object, I created a clean list (a database) of all the objects in the room and their physical boundaries.
Added the Math: I wrote an algorithm that checks whether an object's boundaries are within the camera's current view.

If an object is behind the camera or off-screen, the program completely skips it. This frees up the graphics card and makes the program run much more smoothly.

This project meets Outcome 3, which focuses on designing solutions that use sound algorithms to solve problems and balance trade-offs. I had to make a direct trade-off here: I forced the computer's main processor to do a little extra math up front to figure out where the camera was pointing. This small cost saved a massive amount of work for the graphics card, proving that I know how to balance a computer's workload to achieve optimal performance.

This project taught me a lot about how 3D space is calculated behind the scenes, specifically how to use math to determine the exact, invisible boundaries of what a camera can see. The biggest challenge was actually reorganizing the old code. Because the original project only drew things step by step, I couldn't just drop my new math in. I had to completely tear apart the old drawing loop and separate the "what" (the object data) from the "how" (the drawing action). It took a lot of tedious work to turn every piece of the lamp and remote into organized data, but once I did, adding the camera logic was actually very clean and easy.

---
[Back to Homepage](index.md)
