# script to read the output of the arduino using py serial

import serial 
import json

# got from pyserial docs here: https://www.pyserial.com/docs/reading-data
def read_lines_forever(ser):
    """Read lines continuously with error handling"""
    while True:
        try:
            line = ser.readline()
            if line:
                text = line.decode('utf-8', errors='ignore').strip()
                if text:  # Skip empty lines
                    yield text
        except KeyboardInterrupt:
            break
        except Exception as e:
            print(f"Read error: {e}")
            break

# Usage


if __name__ == "__main__":
    imu = serial.Serial('/dev/ttyACM0', 9600, timeout=1)


    for line in read_lines_forever(imu):
        # print(line)
        try:
            data = json.loads(line)
            print(f"{json.dumps(data, indent=4)}")
        except:
            # skip the exceptions from the json parsing errors
            print(f"Failed to parse the json, heres the raw line: {line}")
            pass
        # data = json.loads(line)
        # print(f"{json.dumps(data, indent=4)}")
