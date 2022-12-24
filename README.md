# ThirstingTannenbaum
### **ThistingTannenbaum** is an overengineered Christmas Tree water level monitor using a Raspberry Pi Pico W and a VL53L1X Time-of-Flight distance sensor

# Requirements
- Raspberry Pi Pico W
    - Micro-USB cable
    - USB power supply
- VL53L1X sensor (Pololu #3415)
- Adafruit PiCowbell Proto
- Connectors (Pin headers, wire, cable, etc.)

# Setup
First, run initialize the required library submodule:
```
cd external_VL53L1X
git submodule update --init
```
Then, copy `credentials_template.h` to `credentials.h` and then add your WIFI information.
Finally, build the project:
```
mkdir build
cd build
cmake ..
make
```
The resulting executable is in `main.uf2`. To load this onto the Pico W, hold the BOOTSEL button on the board and connect it to your computer via USB. The board should mount as a drive. Copy the `main.uf2` file onto this drive, and the board should reboot (and the drive will be unmounted) as soon as it is loaded.
