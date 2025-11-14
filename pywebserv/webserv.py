# python script to run a simple webserver 
# use this as a basis to serve the data from the imu on the arduino

# trying out flask

# have the shebang
#!/bin/python3

from flask import Flask, render_template, jsonify
import serial
import threading
import json

# making the flask app
app = Flask(__name__)

# imu data is global to share between threads
# got from here: 
# https://stackoverflow.com/questions/17774768/python-creating-a-shared-variable-between-threads
imu_data = {"heading":"CALIBRATING", "roll":"CALIBRATING", "pitch":"CALIBRATING"}

lock = threading.Lock()


class  arduinoIMU(threading.Thread):
    def __init__(self, name,  dev, baud):
        threading.Thread.__init__(self)
        # create the serial here
        self.dev = serial.Serial(dev, baud)
        self.data = {"heading":"Starting", "roll":" Up ", "pitch":" Now"}
        
    def run(self):
        global imu_data
        # run this in a separate thread
        # from webserver so the serial is always open
        #  this is from the pyserial docs on continuous reading
        # from a serial port
        while True:
            # always read the data from the imu
            try:
                line = self.dev.readline()
                if line:
                    text = line.decode('utf-8', errors='ignore').strip()
                    if text:
                        self.data = json.loads(text)
            except KeyboardInterrupt:
                break
            except json.JSONDecodeError:
                # skip all JSON decode errors
                pass
            except Exception as e:
                print(f"Read error {e}")
                break

            # try to update the data 
            # published to the site
            with lock:
                imu_data = self.data


# python decorator to add more functionality to the below function
# the decorator tells flask to call this function when they go to /
@app.route("/")
def index():
    return render_template("index.html")

# route for the imu data specifically
@app.route("/imudata")
def imudata():
    global imu_data
    # grab the latest from the imu if flask can access the lock
    with lock:
        return jsonify(imu_data)

    

if __name__ == "__main__":
    # run imu as background thread
    imu_thread = arduinoIMU("imu", '/dev/ttyACM0', 9600)
    imu_thread.daemon = True
    imu_thread.start()

    # run flask 
    app.run("0.0.0.0", port=5001, debug=False, use_reloader=False)

