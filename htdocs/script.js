// On the assumption that the canvas position does not move after load
var ws = null;
var sessionId = null;
var incoming = new Map();

function openSession() {
    if (ws != null && ws.readyState == 1) {
        sendUrlUpdate();
        return;
    }

    ws = new WebSocket("wss://" + location.host + ":8443");

    ws.onmessage = message => {
        var data = JSON.parse(message.data);
        // Grab session Id from initial message (without position info)
        if (data.x == null) {
            sessionId = data.id;
        } else if (data.remove != null) {
            incoming.delete(data.id);
        } else {
            if (incoming.get(data.id)) {
                incoming.delete(data.id);
            }
            incoming.set(data.id,data);
        }
    }

    // Minimal information available on the status
    ws.onopen = () => {
        status("Connected");
    }
    ws.onerror = () => status("Connection Error");
    ws.onclose = () => status("Connection Closed");
}

function status(message) {
    var status = document.getElementById("status");
    status.innerText = message;
}

function send(data) {
    if (ws != null && ws.readyState == 1) {
        ws.send(data);
    }
}

// globals for rendering
var canvas = null;
var ctx = null;

var width = null;
var height = null;
var state = {
    x: 0,
    y: 0,
    keysDown: {
        left: false,
        right: false,
        up: false,
        down: false
    }
}
var lastUpdate = 0;
var lastPos = {
    id: null,
    x: 0,
    y: 0
}
var cage = null;
var trav = null;
var avatar = null;
var enemy = null;

var keyMap = {
    68: 'right',
    65: 'left',
    87: 'up',
    83: 'down'
  }

// input functions
function keyDown(event) {
    var key = keyMap[event.keyCode];
    state.keysDown[key] = true;
}

function keyUp(event) {
    var key = keyMap[event.keyCode];
    state.keysDown[key] = false;
}


// main rendering loop
function update(elapsed) {
    // Use fraction of render difference as movement amount
    var delta = elapsed / 5.;
    if (state.keysDown.left) {
        state.x -= delta
    }
    if (state.keysDown.right) {
        state.x += delta
    }
    if (state.keysDown.up) {
        state.y -= delta
    }
    if (state.keysDown.down) {
        state.y += delta
    }

    // Wrap across boundaries instead of stopping movement
    if (state.x > width) {
        state.x -= width;
    } else if (state.x < 0) {
        state.x += width;
    }

    if (state.y > height) {
        state.y -= height;
    } else if (state.y < 0) {
        state.y += height;
    }

    // Send update of x,y to webserver
}

function draw() {
    // erase everything
    ctx.clearRect(0, 0, width, height);

    //redraw
    ctx.fillStyle = "cyan";
    //ctx.fillRect(state.x - 5, state.y - 5, 10, 10);
    ctx.drawImage(avatar, state.x - (avatar.width / 2.), state.y - (avatar.height / 2.));

    // Draw others
    // loop through array of other players: color, x, y
    ctx.fillStyle = "red";
    for (let [key, value] of incoming) {
        //ctx.fillRect(value.x - 5, value.y - 5, 10, 10);
	ctx.drawImage(enemy, value.x - (enemy.width / 2.), value.y - (enemy.height / 2.));
    }
}

function loop(timestamp) {
    var elapsed = timestamp - lastUpdate;

    update(elapsed);
    //console.log(state);
    draw();
    var pos = {
        id: sessionId,
        x: state.x,
        y: state.y
    }
    if (pos.x != lastPos.x || pos.y != lastPos.y) {
        send(JSON.stringify(pos));
        lastPos = pos;
    }

    lastUpdate = timestamp;
    window.requestAnimationFrame(loop);
}

function initialize() {
    cage = document.getElementById("cage");
    trav = document.getElementById("trav");
    avatar = trav;
    enemy = cage;
    canvas = document.getElementById("canvas");
    ctx = canvas.getContext("2d");
    

    canvas.height = canvas.width;
    width = canvas.width;
    height = canvas.height;
    state.x = width / 2.;
    state.y = height / 2.;
    
    status("initialized")
}

function faceOff() {
    if (avatar == cage) {
        avatar = trav;
	enemy = cage;
    } else {
	    avatar = cage;
	    enemy = trav;
    }
}

function start() {
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("keyup", keyUp, false);
    window.requestAnimationFrame(loop);
}

// Start when page loads completely
document.addEventListener('readystatechange', event => { 
    if (event.target.readyState === "interactive") {
        initialize();
        openSession();
    }
    if (event.target.readyState === "complete") {
        start();
    }
});
