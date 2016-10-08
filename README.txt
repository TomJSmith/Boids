Boids!!!!

--About--
A birdoid flocking simulation built with openGL and C++.
Check out the video demo: https://youtu.be/LIO83XVNYVE

--info.txt--
To change the properties of the simulation, edit the values in the info.txt (dont remove any spaces though).
The values in info.txt are scaling values, not actual values. For example, changing the value of 'boid repelling radius' from '1.0'
to '2.0' will double the radius at which boids will repel each other. This is true for all the values except for "number of boids", 
"number of spheres", and all the values to do with the bait ball (excluding "bait ball force"). 

--Bait Ball Mode--
By default, this is not enabled. To enable, set the value of "bait ball mode" to "1" in info.txt.
The location of the bait ball can also be changed, as well as the scaling of the force the bait ball exerts. 
The bait ball looks best with lots of boids.

--Camera Controls--
There are a very very basic camera controls. 
To stop the continual rotation of the scene, press F8. 
To move forward and back, use the forward and back keys.
To spin the scene, use the left and right keys. 
