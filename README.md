# SensorCaptureDevelopmentFramework
Augmented Midi Controller Android/iPhone App

The project is structured with a common cross-platform layer in C++, that is able to collect and time synchronize
data from variuos sensors in order to build multi-sensors applications.
The synchronized data can be sent raw over wifi or can be processed by an algorithmic layer (eppidei/ADE) that is present as an external
repository. 

This common layer is then used by an Obj C layer for Iphone and a Java layer for android that are in charge of actually collecting sensors data
(Mic, Accelerometers, Gyros, Luminosity....)

The user customizable GUI has been developed using Cocos2D tp enforce the cross platform implementation.

The app is not updated to latest OS revisions but should be easily done.
Android project is still Eclipse based.



