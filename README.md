## Adout
This repo is a test experimental setup for learing around platformIO.
A RaspberryPi Pico board is used with a small 0.96" OLED display.
To spice things up a bit, a neural network from https://github.com/LeRatonLaveurSolitaire/Wireless-power-transfer-system_simulation is embedded using TensorFlow Lite for Microcontroller.

## Working principle
The neural network is run at start with defined values and rerun once when recieving data from the serial port. 
The output data of the NN are then printed on the display along with the computation time of the NN inference.
