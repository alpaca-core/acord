("use strict");

let output;
let ws;

const LOG_LEVEL = {
  INFO: 0,
  SUCCESS: 1,
  ERROR: 2,
};

function setStatus(message, level) {
  const status = document.getElementById("status");
  status.innerText = message;
  if (level === LOG_LEVEL.INFO) {
    status.style.color = "var(--color-header)";
  } else if (level === LOG_LEVEL.SUCCESS) {
    status.style.color = "var(--color-green)";
  } else if (level === LOG_LEVEL.ERROR) {
    status.style.color = "var(--color-red)";
  }
}

const AC_STATES = {
  CONNECTED: 0,
  INITIALIZED: 1,
  MODEL_LOADED: 2,
  INSTANCE_STARTED: 3,
  BEGIN_CHAT: 4,
  ADD_CHAT_MESSAGE: 5,
  GET_CHAT_RESPONSE: 6,
  DISCONNECTED: 7,
};
let acState = AC_STATES.DISCONNECTED;

async function init() {
  output = document.getElementById("output");

  const input = document.getElementById("data_to_send");
  input.addEventListener("keydown", function (e) {
    if (e.key === "Enter") {
      addChatMessage();
    }
  });

  disableLoadModelRow();
  disableSendRow();

  await connect();
}

async function connect() {
  let wsUri = "ws://" + window.location.hostname + ":7654";
  ws = new WebSocket(wsUri);
  ws.binaryType = "arraybuffer";

  (async () => {
    while (ws.readyState !== WebSocket.OPEN) {
      await new Promise((resolve) => setTimeout(resolve, 1000));
    }
    await loadProvider();
  })();

  ws.onopen = () => {
    console.log("Connected to Stable Diffusion WebSocket.");
    setStatus("Connected!", LOG_LEVEL.SUCCESS);
    acState = AC_STATES.CONNECTED;
  };

  ws.onerror = () =>
    setStatus(
      `Error: Couldn't connect to ${localhost}! Start the server...`,
      LOG_LEVEL.ERROR
    );
  acState = AC_STATES.DISCONNECTED;
}

async function loadProvider() {
  await wsSend({
    op: "load_provider",
    data: { name: "llama" },
  });
  acState = AC_STATES.INITIALIZED;
  setStatus("Provider loaded!", LOG_LEVEL.SUCCESS);
  enableLoadModelRow();
}

async function loadModel() {
  if (ws.readyState !== WebSocket.OPEN) {
    setStatus("WebSocket not connected!", LOG_LEVEL.ERROR);
    return;
  }
  if (acState === AC_STATES.INSTANCE_STARTED) {
    setStatus(
      "Model is already loaded. Refresh to load a new one.",
      LOG_LEVEL.ERROR
    );
    return;
  }

  if (acState === AC_STATES.INITIALIZED) {
    const loadModelInput = document.getElementById("modelFile").value;
    if (loadModelInput === "") {
      setStatus("Please enter the path to the model!", LOG_LEVEL.ERROR);
      return;
    }
    disableLoadModelRow();

    setStatus("Loading...", LOG_LEVEL.INFO);
    await wsSend({
      op: "load-model",
      data: { gguf: loadModelInput },
    });
    acState = AC_STATES.MODEL_LOADED;
    setStatus("Model loaded!", LOG_LEVEL.SUCCESS);

    await startInstance();
    acState = AC_STATES.INSTANCE_STARTED;
    setStatus("Instance started!", LOG_LEVEL.SUCCESS);

    await beginChat();
    acState = AC_STATES.BEGIN_CHAT;
    setStatus("Chat started!", LOG_LEVEL.SUCCESS);

    enableLoadModelRow();
    enableSendRow();
  }
}

async function startInstance() {
  await wsSend({
    op: "start-instance",
    data: {},
  });
}

async function beginChat() {
  await wsSend({
    op: "begin-chat",
    data: { setup: "Chat between a user and a helpful AI assistant." },
  });
}

async function addChatMessage() {
  const message = document.getElementById("data_to_send").value;
  if (!message) return;

  writeToScreen(
    '<p><span style="color: var(--color-blue);">me:</span> ' + message + "</p>"
  );
  document.getElementById("data_to_send").value = "";

  await wsSend({
    op: "add-chat-prompt",
    data: { prompt: message },
  });
  acState = AC_STATES.ADD_CHAT_MESSAGE;
  setStatus("Answering...", LOG_LEVEL.INFO);

  setStatus("Ask me anything!", LOG_LEVEL.SUCCESS);
  acState = AC_STATES.GET_CHAT_RESPONSE;
  const output = document.getElementById("output");
  const p = document.createElement("p");
  p.innerHTML = '<span style="color: var(--color-red);">model: </span>';
  output.appendChild(p);
  output.scrollTop = output.scrollHeight;

  // if you don't want to stream the result add a parameter - "stream": false
  // const response = await wsSend({
  //   op: "get-chat-response",
  //   data: { stream: false },
  // });
  const response = await wsSend({
    op: "get-chat-response",
    data: null,
  });
  p.innerHTML += `<span>${response.data.response}</span>`;
}

async function wsSend(obj) {
  const message = JSON.stringify(obj);

  ws.send(message);

  return new Promise((resolve) => {
    ws.onmessage = (event) => {
      const response = JSON.parse(event.data);
      console.log("onmessage", response);

      if (response.op.startsWith("s:")) {
        console.log(`Op - ${response.op}: Finished`);
        resolve(response);
      } else if (response.op === "token") {
        const output = document.getElementById("output");
        output.lastElementChild.innerHTML += `<span>${response.data}</span>`;
        output.scrollTop = output.scrollHeight;
      } else {
        console.log(`Op - ${response.op}: Received`);
        resolve(response);
      }
    };
  });
}

function writeToScreen(message) {
  let p = document.createElement("p");
  p.innerHTML = message;
  output.appendChild(p);
  output.scrollTop = output.scrollHeight;
}

function toggleElements(ids, enabled) {
  const state = enabled
    ? { disabled: false, opacity: 1, pointerEvents: "auto" }
    : { disabled: true, opacity: 0.6, pointerEvents: "none" };

  ids.forEach((id) => {
    const el = document.getElementById(id);
    if (el) {
      el.disabled = state.disabled;
      el.style.opacity = state.opacity;
      el.style.pointerEvents = state.pointerEvents;
    }
  });
}

function disableLoadModelRow() {
  toggleElements(["modelFile", "loadBtn"], false);
}
function enableLoadModelRow() {
  toggleElements(["modelFile", "loadBtn"], true);
}

function disableSendRow() {
  toggleElements(["data_to_send", "sender"], false);
}
function enableSendRow() {
  toggleElements(["data_to_send", "sender"], true);
}

window.addEventListener("load", init, false);

window.addChatMessage = addChatMessage;
window.loadModel = loadModel;
window.wsSend = wsSend;
