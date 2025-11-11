# python script to run a simple webserver 
# use this as a basis to serve the data from the imu on the arduino

# trying out flask

# have the shebang
#!/bin/python3

from flask import Flask, render_template, jsonify


# making the flask app
app = Flask(__name__)

class Counter():
    def __init__(self, start, low_bound, high_bound):
        self.count = start
        self.max = high_bound
        self.min = low_bound

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

c1 = Counter(0, 0, 4)
c2 = Counter(0, -2, 2)
c3 = Counter(42, 42, 69)


# python decorator to add more functionality to the below function
# the decorator tells flask to call this function when they go to /
@app.route("/")
def index():
    return render_template("meme_index.html")

# adding route where the data is updated
@app.route("/data")
def data():
    c1.change()
    c2.change()
    c3.change()
    return jsonify({"val1" : c1.count, "val2" : c2.count, "val3" : c3.count})

# run flask 
# runs on any ip address in host network
# port is 50001
app.run(host="0.0.0.0", port=5001)

