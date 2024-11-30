from flask import Flask, jsonify, request

app = Flask(__name__)
count = 0

@app.route('/people_count', methods=['GET'])
def people_count():
    global count
    print(count)
    return jsonify({"count": count})

@app.route('/set_count', methods=['POST'])
def set_count():
    global count
    print(count)
    count = request.json.get('count')
    return jsonify({"message": "Count updated successfully"})

app.run(host='0.0.0.0', port=5000)
