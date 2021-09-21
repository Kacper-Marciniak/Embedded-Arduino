# Embedded-Arduino

## Manipulator control
Programme allows user to control the custom 3-DOF serial spatial manipulator using build-in Arduino serial port console.
Using various text commands user can control position and speed of device, as well as read its internal data, such as servo current.

Commands:
- *movej* - move to (x, x, x) with joint interpolation (position specified using theta angles)
- *go* - move t0 (x, x, x) without any interpolation
- *speed* - servo speed
- *home* - go to the default "home" position
- *contmovej* - continuous mode, movej
- *contgo* - continuous mode, go
- *sseq* - start the saved movement sequence
- *led* - toggle LED diode
- *safe* - toggle safe mode (requires user to hold a specified button in order to move)

![Manipulator](https://scontent-waw1-1.xx.fbcdn.net/v/t1.15752-9/s2048x2048/194222679_681966109430152_8747572018742862294_n.jpg?_nc_cat=102&ccb=1-5&_nc_sid=ae9488&_nc_ohc=nKHB-f_8nVQAX8YoXso&_nc_ht=scontent-waw1-1.xx&oh=c3e7b7f590d5ed9f3844c84a7872124e&oe=616E4D87)
