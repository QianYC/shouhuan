## prerequisite
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
### get id
- url: post /id
- response: uuid
- 该方法只应该调用一次！手环获得了自己的id后应该保存起来，以后每次其他的请求都附上这个id！

### upload data to server
- url: post /upload
- request body: {"id" : "xxx", "Temp" : 1.2, "SPO" : 77, "HR" : 77}
- response: not important...
