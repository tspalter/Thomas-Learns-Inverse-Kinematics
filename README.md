# Thomas-Learns-Inverse-Kinematics
Project for Advanced Animation class on Inverse Kinematics, learned through tutorials on darwin3d.com

Controls
Left-click: Use this to drag and constantly update the end effector position
Right-click/Middle-click:  Use these to update the effector position once, does not drag

In the View tab:
Click the Geometry tab to toggle between the drawn 3D model and the skeletal figure drawn

In the Options tab:
Click the Damping and DOF constraints tabs to toggle damping and depth of field constraints on the model (The DOF constraints are in a bit of a bugged state where they only
work curving to the left and not the right, not sure what the issue there is)

NOTE:  THIS PROJECT CAN ONLY COMPILE THROUGH THE ADDITION OF MODERN MFC LIBRARIES INSTALLED THROUGH THE VS INSTALLER.  I ATTEMPTED TO COMPILE THESE ON THE LAB COMPUTERS, 
BUT WAS UNABLE TO AS I CAN'T INSTALL ADDITIONAL LIBRARIES SINCE I'M NOT AN ADMIN.

This application was created with the assistance of Jeff Lander's tutorial on Inverse Kinematics found on darwin3d.com, where I modernized it to be accessible through 
VS 2019 while applying inverse kinematics through a cyclic coordinate descent algorithm to things learned from Projects 1 and 2.  Taking the form of a snake with 5 bones 
(1 for the head, 4 for the body), this algorithm iterates through the different bones in the system and determines where they will end up upon final drawing.  A single iteration
starts by finding the position of the root bone in use and the position of the end effector.  Once we determine those, we find the desired end position for the end effector.  
if the squared distance between the current end effector and the desired end position is greater than 1, we find the vectors to the current end effector position and the target 
effector position, then normalize the two vectors.  We find the cosine angle from the dot product of these two vectors, then check if our angle requires a rotation (if it's less 
than 1).  Should we need to rotate, we take the cross product of our two vectors, then based on the cross product's value about the z-axis, determine if the model will be rotating 
clockwise or counter-clockwise.  From there, we determine the turning angle, set our damping, rotate the actual bone, and restrict the bone's movement based on our set depth of field.

This function is done twice, once iterating through all of the bones repeatedly (ComputeCCDLink) and once starting the iteration at a specific bone going up to the end effector once 
(ComputeOneCCDLink) and they are both found in OGLView.cpp.  In Quaternion.cpp, there is also a function that converts a given quaternion to Axis-Angle representation (QuatToAxisAngle), 
which was necessary to getting the program to work properly.