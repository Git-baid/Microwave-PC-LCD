# Code is unused now, as image data is sent directly from baidbot hosted on a raspberry pi to the ESP32 now.

import time
import serial
import socket

bisectIP = '51.81.232.163'
Port = 26935
baudrate = 115200
serial_port = "COM4"
timeout = 5  # seconds
packet_delay = 0.3  # seconds
max_image_size = 102400  # bytes


def send_data_to_esp(full_list):
    if __name__ == '__main__':
        link = serial.Serial(
        port=serial_port,       # Replace 'COM1' with the appropriate port
        baudrate=baudrate,     # Set the baud rate
        parity=serial.PARITY_EVEN,     # Set parity (None, Even, Odd, Mark, Space)
        stopbits=serial.STOPBITS_ONE,  # Set number of stop bits (1, 1.5, 2)
        bytesize=serial.EIGHTBITS,     # Set data byte size (5, 6, 7, 8)
        timeout=timeout,         # Set timeout value in seconds
        xonxoff=False,     # Enable/disable software flow control (XON/XOFF)
        rtscts=False      # Enable/disable hardware (RTS/CTS) flow control
        )
        link.setRTS(False)
        link.setDTR(False)
        out_pkg = "<"  # start of data flag
        out_pkg += str(len(full_list))

        count = 0
        for i in full_list:
            out_pkg += ","
            out_pkg += str(i)
            count += 1
            print(bytes(str(i), 'utf-8'))
            print("count: ", count)

            if count % 1024 == 0:
                link.write(bytes(out_pkg, 'utf-8'))  # send out_pkg as bytes
                out_pkg = ""
                time.sleep(packet_delay)
        out_pkg += ">"  # end of data flag
        print(out_pkg)
        print(len(str(full_list)))
        link.write(bytes(out_pkg, 'utf-8'))  # send out_pkg as bytes
        link.close()


def socket_input():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        while True:
            try:
                soc = socket.create_connection((bisectIP, Port), timeout=None)
                print("Connection Established")
                full_list = []
                temp = ""
                time.sleep(2)

                while True:
                    recvfile = soc.recv(max_image_size).decode()
                    for i in range(0, len(recvfile)):
                        if recvfile[i] != ",":
                            temp += recvfile[i]
                        else:
                            full_list.append(int(temp))
                            temp = ""

                    if not recvfile:
                        break
                if len(full_list) == 0:
                    continue
                send_data_to_esp(full_list)
                print("Data Sent")
                s.close()
            except Exception as e:
                print("Connection Failed \"" + str(e) + "\"")
                continue



socket_input()
