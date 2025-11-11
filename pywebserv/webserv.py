# python script to run a simple webserver 
# use this as a basis to serve the data from the imu on the arduino

# trying out flask

# have the shebang
#!/bin/python3

from flask import Flask, render_template
import requests, json


# making the flask app
app = Flask(__name__)

class Counter():
    def __init__(self):
        self.count = 0
        self.max = 40
        self.min = 0

        # states
        self.DOWN = 0
        self.UP = 1
        self.state = self.UP

    def up(self):
        self.count = self.count + 1
    def down(self):
        self.count = self.count - 1
    
    def change(self):
        # changes count value based on
        # the last boundary it hit 
        if self.state == self.UP:
            if self.count == self.max:
                self.state = self.DOWN
            else:
                self.up()
        else:
            if self.count == self.min:
                self.state = self.UP
            else:
                self.down()

c = Counter()
# python decorator to add more functionality to the below function
# the decorator tells flask to call this function when they go to /
@app.route("/")
def index():
    c.change()
    return render_template("meme_index.html", val=c.count)

# run flask 
# runs on any ip address in host network
# port is 50001
app.run(host="0.0.0.0", port=5001)

