from flask import Flask, redirect, url_for, render_template, request, flash
import os
import json

app = Flask(__name__)
app.secret_key = os.urandom(24)

# the current state of the arduino
current_state = 1
# the letters that should be on in static mode
static_word = "abcdefghijklmnopqrstuvwxyz"
# time in ms between updates on random mode
random_interval = 100
# the maximum items to be kept while the arduino is displaying
max_items = 10
# the queue of words in ouija mode
queue = []

@app.route('/')
def home():
    return redirect(url_for('ouija'))

@app.route('/ouija/')
def ouija():
    global current_state
    return render_template('ouija.html', current_state=current_state)

@app.route('/ouija/state', methods=["POST"])
def set_state():
    global current_state
    current_state = int(request.form.get("state_radios"))
    flash("Set state to {}".format(current_state))
    return redirect(url_for('ouija'))

# pushes a word to the queue to be displayed
@app.route('/ouija/push', methods=["POST"])
def push():
    global queue
    global max_items
    message = request.form.get("message")
    if len(queue) < max_items:
        queue.append(message)
        flash("Added \"{}\" to queue".format(message))
    else:
        flash("Error: queue full")
    return redirect(url_for('ouija'))

# set the static word
@app.route('/ouija/set_static', methods=["POST"])
def set_static():
    global static_word
    static_word = request.form.get("static_word")
    flash("Set static word to {}".format(static_word))
    return redirect(url_for('ouija'))

# sets the timing on the random timer
@app.route('/ouija/set_random', methods=["POST"])
def set_random():
    global random_interval
    try:
        random_interval = int(request.form.get("random_interval"))
        flash("Set random interval to {}".format(random_interval))
    except ValueError:
        flash("Invalid time")
    return redirect(url_for('ouija'))

#ouija/arduino/
@app.route('/ouija/arduino', methods=["GET"])
def arduino():
    global current_state
    global static_word
    global random_interval
    global queue
    result = ""
    if current_state == 1:
        message = ""
        if len(queue) != 0:
            message = queue.pop(0)
        else:
            message = ""
        result = json.dumps({"current_state" : current_state, "message": message })
    elif current_state == 2:
        result = json.dumps({"current_state" : current_state, "static_word" : static_word })
    elif current_state == 3:
        result = json.dumps({"current_state" : current_state, "random_interval" : random_interval })
    return result

if __name__ == '__main__':
    app.run(debug=True)
