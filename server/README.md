## prerequisit
pip install flask

## run
### linux
```
cd server/
export FLASK_APP=App.py
flask run --host=0.0.0.0
```

### windows powershell
```
cd server/
$env:FLASK_APP="App.py"
flask run --host=0.0.0.0
```

## api
### get
- url: get /
- response: uuid

### upload data to server
- url: post /upload
- request body: {"Temp" : 1.2, "SPO" : 77, "HR" : 77}
- response: not important...