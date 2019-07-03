import serial
import serial.tools.list_ports

# Just to show the serial information
if __name__ == '__main__':
    port_list = list(serial.tools.list_ports.comports())

    if len(port_list) <= 0:
        print("The Serial port can't find!")

    else:
        # port_list_0 = list(port_list[0])
        # port_serial = port_list_0[0]

        for i in range(len(port_list)):
            print(i+1, ":", port_list[i])

        while True:
            num = input("Which one do you like to open? Please type the order number.")
            if num.isdigit() and 0 < int(num) <= len(port_list):
                num = int(num)
                break
        ser = serial.Serial(port_list[num-1][0], 115200, timeout=1)

        print("check which port was really used >", ser.name)

        while 1:
            s = ser.read()
            print(s, end="")
