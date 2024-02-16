import serial
import struct
import random
from dataset_class import CustomDataset

def delinearise_R_l(R_l: float = 0):
    return 10 ** ((0.15 * R_l) + 0.5)


def delinearise_M(M: float = 0):
    L1 = 236e-6
    L2 = 4.82e-6
    return 10 ** ((0.1 * M)) * (0.1 * (L1 * L2) ** 0.5)

def send_float_array(port, float_array):
    # Open the serial port
    print("Opening communication port...")
    ser = serial.Serial(port, baudrate=115200, timeout=1)

    # Convert each float to bytes and send them
    print("Sending values...")
    for value in float_array:
        data = struct.pack('f', value)
        ser.write(data)
    print("Values sent, waiting for answer :")

    # Print Pico response
    print(ser.readline().decode())
    print(ser.readline().decode())
    print("|"+"-"*27 + "|")
    sent_str,recieved_str = "sent","recieved"
    print("|"+f"{sent_str:^13}"+"|"+f"{recieved_str:^13}"+"|")
    
    for i in range(30):
        print("|"+"-"*13 + "|"+"-"*13 + "|")
        print(f"|{float_array[i]:^13.6f}|{ser.readline().decode()[:-2]:^13}|")
    print("|"+"-"*27 + "|\n")

    # Close the serial port
    ser.close()


def main():
    # Generate random values
    dataset_path = "dataset/dataset.pkl"
    print("Loading dataset...")
    dataset = CustomDataset()
    dataset.load(dataset_path)
    print("Dataset sucessfully loaded !\n")
    data = random.choice(dataset)
    reel_values = data[1].tolist()
    data = data[0].tolist()
    # Print the value
    # print(f"Values : {float_array_to_send}")

    # Sent the values to Pico board with UART port
    send_float_array('COM6', data)
    print(f"real values should be : \n - R = {delinearise_R_l(reel_values[0]):.2f}Ohm\n - M = {delinearise_M(reel_values[1])*1e6:.2f}µH")

if __name__ == "__main__":
    main()