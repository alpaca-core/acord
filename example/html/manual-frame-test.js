import { encode, decode } from "https://cdn.jsdelivr.net/npm/cbor-x@1.6.0/+esm";
window.cborX = { encode, decode };

("use strict");

let cborX;
let output;
let cbor = false;
let ws;

function init() {
  output = document.getElementById("output");
  cborX = window.cborX;

  const input = document.getElementById("data_to_send");
  input.addEventListener("keydown", function (e) {
    if (e.key === "Enter") {
      wsSend();
    }
  });

  getLastCommands();
}

function disconnect() {
  ws.close();
}

function connect() {
  cbor = document.getElementById("connect_cbor").checked;
  let wsUri = "ws://" + window.location.hostname + ":7654";
  if (cbor) {
    wsUri += "/cbor";
  }
  ws = new WebSocket(wsUri);
  ws.binaryType = "arraybuffer";

  ws.onopen = () => {
    writeToScreen('<p style="color: var(--color-green);">CONNECTED</p>');
    const cbut = document.getElementById("connect");
    cbut.textContent = "Disconnect";
    cbut.onclick = disconnect;
    cbut.classList.remove("connect-button");
    cbut.classList.add("disconnect-button");
  };

  ws.onclose = () => {
    writeToScreen('<p style="color: var(--color-red);">DISCONNECTED</p>');
    const cbut = document.getElementById("connect");
    cbut.textContent = "Connect";
    cbut.onclick = connect;
    cbut.classList.remove("disconnect-button");
    cbut.classList.add("connect-button");
  };

  ws.onmessage = (msg) => {
    if (cbor) {
      const obj = cborX.decode(new Uint8Array(msg.data));

      writeToScreen(
        '<p style="color: var(--color-blue);">RESPONSE: ' +
          JSON.stringify(obj) +
          "</p>"
      );
    } else {
      writeToScreen(
        '<p style="color: var(--color-blue);">RESPONSE: ' + msg.data + "</p>"
      );
    }
  };
  ws.onerror = (msg) =>
    writeToScreen(
      '<p style="color: var(--color-red);">ERROR: ' + msg.data + "</p>"
    );
}

function wsSend() {
  const message = document.getElementById("data_to_send").value;
  if (!message) return;
  addLastCommand(message);

  writeToScreen("SENT: " + message);
  console.log(message);

  if (cbor) {
    ws.send(cborX.encode(JSON.parse(message)));
  } else {
    ws.send(message);
  }
}

function writeToScreen(message) {
  let p = document.createElement("p");
  p.innerHTML = message;
  output.appendChild(p);
  output.scrollTop = output.scrollHeight;
}

function addLastCommand(message) {
  const commands = JSON.parse(localStorage.getItem("last_commands")) || [];
  if (commands.includes(message)) return;

  commands.unshift(message);
  localStorage.setItem("last_commands", JSON.stringify(commands));
  getLastCommands();
}

function clearAllCommands() {
  localStorage.setItem("last_commands", JSON.stringify([]));
  getLastCommands();
}

function clearInput() {
  document.getElementById("data_to_send").value = '{"op": "xx", "data": {}}';
}

function getLastCommands() {
  const lastCommands = JSON.parse(localStorage.getItem("last_commands"));
  const listForLastCommands = document.getElementById("last_commands");
  listForLastCommands.innerHTML = "";

  if (lastCommands.length > 0) {
    lastCommands.forEach((command, index) => {
      const li = document.createElement("li");
      li.style.display = "flex";
      li.style.alignItems = "center";

      const textSpan = document.createElement("span");
      textSpan.textContent = command;
      li.appendChild(textSpan);

      const btnContainer = document.createElement("div");
      btnContainer.className = "button-container";
      const applyButton = createApplyButton(command);
      btnContainer.appendChild(applyButton);
      const deleteButton = createDeleteButton(index);
      btnContainer.appendChild(deleteButton);
      li.appendChild(btnContainer);

      listForLastCommands.appendChild(li);
    });
  } else {
    const li = document.createElement("li");
    li.textContent = "No last commands";
    li.classList.add("no-commands");
    listForLastCommands.appendChild(li);
  }
}

function createApplyButton(command) {
  const applyButton = document.createElement("button");
  applyButton.className = "apply-button";
  applyButton.textContent = "Apply";
  applyButton.onclick = () => {
    document.getElementById("data_to_send").value = command;
    const originalText = applyButton.textContent;
    applyButton.textContent = "Applied";
    setTimeout(() => {
      applyButton.textContent = originalText;
    }, 2000);
  };
  return applyButton;
}

function createDeleteButton(index) {
  const deleteButton = document.createElement("button");
  deleteButton.className = "delete-button";
  deleteButton.textContent = "✖";
  deleteButton.title = "Delete";
  deleteButton.onclick = () => {
    let commands = JSON.parse(localStorage.getItem("last_commands")) || [];
    commands.splice(index, 1);
    localStorage.setItem("last_commands", JSON.stringify(commands));
    getLastCommands();
  };
  return deleteButton;
}

window.addEventListener("load", init, false);

window.wsSend = wsSend;
window.connect = connect;
window.clearInput = clearInput;
window.clearAllCommands = clearAllCommands;
