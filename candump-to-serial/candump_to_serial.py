"""
# PSEUDOCODE IDEA

producer
open file and serial
for line
  parse line
  timestamp interface id#content
  (0.000000) secondary 4EC#00140000FFFF0000
  send interface, id and content

consumer
while(true) {
  riceve interface, id and content
}
"""

import sys
import serial
import datetime
import time

DEFAULT_DELAY = 0.1


def delta_us(ctime, ptime):
    delta = ctime - ptime
    return delta.total_seconds() * 1e6


def send_message(si, mint, mid, mdata):
    smsg = 0 if (mint == "primary") else 1
    msg = f"{smsg}&{mid}&{mdata}$".ljust(23, "0")
    si.write(str.encode(msg))
    sys.stdout.write(f"Sent message: {str.encode(msg)}\n")


if __name__ == "__main__":
    argc = len(sys.argv)
    if argc < 3:
        sys.stderr.write(
            "Usage: python3 candump_to_serial.py <candump_file_path> <serial_port>\n"
        )
        exit(1)
    candump_file_path = sys.argv[1]
    serial_port = sys.argv[2]
    try:
        serial_interface = serial.Serial(
            port=serial_port,
            baudrate=115200,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
        )
    except OSError:
        sys.stderr.write(f"Usage: could not open the serial port {serial_port}\n")
        exit(1)
    try:
        candump_file = open(candump_file_path, "r")
    except OSError:
        sys.stderr.write(f"Could not open/read file: {candump_file_path}\n")
        exit(1)

    ptime = datetime.datetime.now()
    ploggingtime = datetime.datetime.now()
    pdelta = 0

    with candump_file:
        for row in candump_file.readlines():
            message_contents = row.split()
            message_delta = float(str.strip(message_contents[0].strip("()")))
            message_interface = message_contents[1]
            msg_tmp = str.split(message_contents[2], "#")
            message_id = msg_tmp[0]
            message_data = msg_tmp[1]
            while delta_us(datetime.datetime.now(), ptime) < (message_delta - pdelta):
                pass
            send_message(serial_interface, message_interface, message_id, message_data)
            ptime = datetime.datetime.now()
            pdelta += message_delta
            time.sleep(DEFAULT_DELAY)

