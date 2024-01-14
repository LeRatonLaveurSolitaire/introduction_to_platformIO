import serial
import struct
import random


def send_float_array(port, float_array):
    # Open the serial port
    print("opening communication port")
    ser = serial.Serial(port, baudrate=9600, timeout=1)

    # Convert each float to bytes and send them
    print("sending values")
    for value in float_array:
        data = struct.pack('f', value)
        ser.write(data)
    print("values sent, waiting for answer :")

    # Print Pico response
    print(ser.readline().decode())
    print(ser.readline().decode())
    print("|"+"-"*27 + "|")
    sent_str,recieved_str = "sent","recieved"
    print("|"+f"{sent_str:^13}"+"|"+f"{recieved_str:^13}"+"|")
    
    for i in range(30):
        print("|"+"-"*13 + "|"+"-"*13 + "|")
        print(f"|{float_array[i]:^13.6f}|{ser.readline().decode()[:-2]:^13}|")
    print("|"+"-"*27 + "|")

    # Close the serial port
    ser.close()


def main():
    # Generate random values
    float_array_to_send = [random.random()*15 for i in range(30)]

    # Print the value
    print(f"Values : {float_array_to_send}")

    # Sent the values to Pico board with UART port
    send_float_array('COM6', float_array_to_send)

if __name__ == "__main__":
    main()