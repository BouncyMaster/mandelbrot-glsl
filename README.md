# pi-collision-calculator
A program written in C using OpenGL that calculates the digits of pi by counting
collisions between 2 squares. Inspired from 3Blue1Brown.

This should be used as an example for rendering a (relatively small) finite set
of objects because it is more efficient than having a single VAO and continually
changing it's state. This would become impractical with a program with an unknown
number of objects.
