from flask import Flask, request, session, make_response
import uuid

app = Flask(__name__)

# Set the secret key to some random bytes. Keep this really secret!
app.secret_key = b'_6#y2L"F4Q8z\n\xec]/'

@app.route('/id', methods=['GET'])
def testGet():
    id = uuid.uuid4()
    print('server creates a sessionId %s' % id)
    return str(id)

@app.route('/upload', methods=['POST'])
def testPost():
    body = request.get_json()
    print(body['Temp'])
    print(body['HR'])
    print(body['SPO'])
    return 'server gets some data!'
