from flask import Flask, request, session
import uuid

app = Flask(__name__)

# Set the secret key to some random bytes. Keep this really secret!
app.secret_key = b'_6#y2L"F4Q8z\n\xec]/'

@app.route('/', methods=['GET'])
def testGet():
    if session['sessionId']:
        session['sessionId'] = uuid.uuid4()
    return 'server gets a request! : %s' % session['sessionId']

@app.route('/upload', methods=['POST'])
def testPost():
    body = request.get_json()
    print(body['Temp'])
    print(body['HR'])
    print(body['SPO'])
    return 'server gets some data!'