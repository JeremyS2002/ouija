from flask import Flask, redirect, url_for, render_template, request, flash
import os

app = Flask(__name__)
app.secret_key = os.urandom(24)

max_items = 10
queue = []

@app.route('/')
def home():
    return redirect(url_for('ouija'))

@app.route('/ouija', methods=["GET", "POST"])
def ouija():
    if request.method == "GET":
        return render_template('push.html')

    if request.method == "POST":
        if push(request.form.get("message")):
            flash("Added \"{}\" to queue".format(request.form.get("message")))
        else:
            flash("Error: queue full")

        return render_template('push.html')

@app.route('/push/<message>', methods=["GET", "POST"])
def push(message=""):
    if len(queue) < max_items:
        queue.append(message)
        return True
    else:
        return False

@app.route('/pull')
def pull():
    if len(queue) != 0:
        word = queue.pop(0)
        return word
    else:
        return ''

if __name__ == '__main__':
    app.run(debug=True)