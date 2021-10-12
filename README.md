# Arduino-Auto-Water
Automatic Plant Watering System via Arduino. Overall goal is a fairly basic, automated plant watering system that works on soil moisture readings. The current project, on a firmware level works fairly well, but the physical implementation needs work. On a functional level, it works fine, but nobody in their right mind is going to put it on their counter with a mess of wiring.
As I have more time, will switch over to ESP32 to first shrink the hardware package, then design an all inclusive flower pot that contains the water resivoir, plant pot itself, and hardware.


In order to get the hardware to a reasonable size, a PCB will be designed centered around the ESP32 uC, with most of the components attached. With the ESP32 being as efficient as it is, I can make the entire flower pot wireless, using a 9V battery as overall power for the motor and board, with a coin cell backup to ensure it keeps running. 
A battery power level indicator circuit can be used to check power on the 9V to then notify the user, and switch to backup 3V power, with functions in a reduced mode state to account for the lack of power.
This notification will be coupled with an LED indicator tied to the current battery state.


Next, some sort of interface will be designed to make this project scalable. This will either be a tablet device that sits near your plants with all the information tied to it, or a mobile app. To reduce costs, I'll likely start with a mobile app.
The nice feature of using a mobile app, with an ESP32, is that a simple subroutine can supply up to date information at a periodic interval in a way that saves power, and the user can access it remotely due to built in wifi.
The scalability factor is nice as well, because using a modern uC in combo with an app allows the user to have an overview with multiple plants, and even presets on a per plant basis tied to optimal soil moisture levels. 


I would like to make this project as open as possible, so the chassis will be designed with 3D printers in mind. An obvious challenge comes down to waterproofing, and tolerances will neeed to be tight to ensure that it does not leak.
Basic Concept Drawing:

https://imgur.com/a/OKBxoMz 
