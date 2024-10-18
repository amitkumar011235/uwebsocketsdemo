const editor = document.getElementById('editor');

// Connect to the WebSocket server
const socket = new WebSocket('ws://localhost:9001');

// When the connection is opened, log it
socket.onopen = () => {
    console.log('Connected to the server.');
};

// When receiving a message from the server, update the text area
socket.onmessage = (event) => {
    editor.value = event.data;
};

// When the user types, send the updated content to the server
editor.addEventListener('input', () => {
    socket.send(editor.value);
});
