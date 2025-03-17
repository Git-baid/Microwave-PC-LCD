# Requires Open Hardware Monitor to be running before running this script
import time
import wmi
import subprocess
import socket
from PIL import Image
import pystray

active = True
icon = pystray.Icon("Status")

def show_system_tray_icon():

    def on_clicked(icon, item):
        global active
        if str(item) == "Change ESP32 Target IP":
            subprocess.Popen('notepad C:\\Users\\Clinterpottrmus\\PycharmProjects\\SendTemperatureData\\IPAddress.txt')
        elif str(item) == "Start":
            active = True
        elif str(item) == "Stop":
            active = False
        elif str(item) == "Exit":
            icon.stop()
            active = False
            exit()
    icon.title = "Uwa~ So Temperate"
    icon.icon = Image.open("status-good.png")
    icon.menu = pystray.Menu(
        pystray.MenuItem("Change ESP32 Target IP", on_clicked),
        pystray.MenuItem("Start", on_clicked),
        pystray.MenuItem("Stop", on_clicked),
        pystray.MenuItem("Exit", on_clicked)
    )


    icon.run_detached()


def send_temp_data():

    PORT = 65432  # Port to listen on (non-privileged ports are > 1023)
    w = wmi.WMI(namespace="root\OpenHardwareMonitor")

    while True:
        if not active:  # reset loop if active = false
            continue

        # read ip address stored in IPAddress.txt into HOST variable
        with open("C:\\Users\\Clinterpottrmus\\PycharmProjects\\SendTemperatureData\\IPAddress.txt", 'r') as file:
            HOST = file.read()

        # Try connecting to server using HOST ip and port
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(5)
            try:
                print(f"Connecting on host {HOST}")
                s.connect((HOST, PORT))
            # If connection fails, re-prompt for new IP and restart loop
            except Exception as e:
                print(e)
                icon.icon = Image.open("status-bad.jpg")
                continue
            icon.icon = Image.open("status-good.png")
            # get sensor info
            temperature_infos = w.Sensor()
            cpuTemp = 0
            gpuTemp = 0

            # filter to only cpu and gpu temperature info
            for sensor in temperature_infos:
                if sensor.SensorType == u'Temperature':
                    if sensor.Name == u'CPU Package':
                        cpuTemp = int(sensor.Value)
                        print(sensor.Name)
                        print(sensor.Value)
                    elif sensor.Name == u'GPU Core':
                        gpuTemp = int(sensor.Value)
                        print(sensor.Name)
                        print(sensor.Value)

            time.sleep(1)

            # send data to esp32 server
            if cpuTemp != 0 and gpuTemp != 0:
                out_pkg = f"{cpuTemp}{gpuTemp}\n"
                s.sendall(bytes(out_pkg.encode()))


show_system_tray_icon()
send_temp_data()
