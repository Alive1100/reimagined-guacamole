// On the assumption that the canvas position does not move after load
var ws = null;

function openSession() {
    if (ws != null && ws.readyState == 1) {
        sendUrlUpdate();
        return;
    }

    ws = new WebSocket("ws://" + location.host + ":8443");
    // ws.binaryType = "arraybuffer";

    ws.onmessage = message => {
        var data = JSON.parse(message.data);
        console.log(data);
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
    x: 0,
    y: 0
}
var cage = null;

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
    ctx.fillRect(state.x - 5, state.y - 5, 10, 10);
    ctx.drawImage(cage, state.x, state.y);

    // Draw others
    // loop through array of other players: color, x, y
}

function loop(timestamp) {
    var elapsed = timestamp - lastUpdate;

    update(elapsed);
    //console.log(state);
    draw();
    var pos = {
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
    canvas = document.getElementById("canvas");
    ctx = canvas.getContext("2d");
    

    canvas.height = canvas.width;
    width = canvas.width;
    height = canvas.height;
    state.x = width / 2.;
    state.y = height / 2.;

    ctx.fillStyle = "red";
    
    status("initialized")
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