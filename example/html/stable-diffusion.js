"use strict";

const LOG_LEVLEL = {
  INFO: 0,
  SUCCESS: 1,
  ERROR: 2,
};

function setStatus(message, level) {
  const status = document.getElementById("status");
  status.innerText = message;
  if (level === LOG_LEVLEL.INFO) {
    status.style.color = "var(--color-header)";
  } else if (level === LOG_LEVLEL.SUCCESS) {
    status.style.color = "var(--color-green)";
  } else if (level === LOG_LEVLEL.ERROR) {
    status.style.color = "var(--color-red)";
  }
}

const AC_STATES = {
  CONNECTED: 0,
  INITIALIZED: 1,
  MODEL_LOADED: 2,
  INSTANCE_STARTED: 3,
  GENERATION: 4,
  DISCONNECTED: 5,
};

let acState = AC_STATES.ERROR;
const localhost = "ws://localhost:7654";
const ws = new WebSocket(localhost);

ws.onopen = () => {
  console.log("Connected to Stable Diffusion WebSocket.");
  setStatus("Connected!", LOG_LEVLEL.SUCCESS);
  acState = AC_STATES.CONNECTED;
};

ws.onerror = (error) => {
  console.error("WebSocket error:", error);
  setStatus(
    `Error: Couldn't connect to ${localhost}! Start the server...`,
    LOG_LEVLEL.ERROR
  );
  acState = AC_STATES.DISCONNECTED;
};

ws.onmessage = async (event) => {
  const response = JSON.parse(event.data);
  console.log(response);
};

function fillImageData(data) {
  const c = document.createElement("canvas");
  c.width = data.width;
  c.height = data.height;
  const ctx = c.getContext("2d");
  const imageData = ctx.createImageData(data.width, data.height);

  let colorI = 0;
  for (let i = 0; i < imageData.data.length; i += 4) {
    if (data.data) {
      imageData.data[i] = data.data.bytes[colorI];
      imageData.data[i + 1] = data.data.bytes[colorI + 1];
      imageData.data[i + 2] = data.data.bytes[colorI + 2];
      imageData.data[i + 3] = 255;
    } else {
      imageData.data[i] = Math.ceil(Math.random() * 255);
      imageData.data[i + 1] = Math.ceil(Math.random() * 255);
      imageData.data[i + 2] = Math.ceil(Math.random() * 255);
      imageData.data[i + 3] = 255;
    }
    colorI += 3;
  }

  ctx.putImageData(imageData, 0, 0);
  const dataURL = c.toDataURL();

  const img = document.getElementById("imgResult");
  img.src = dataURL;
  img.onload = () => {
    document.getElementById("skeleton").style.display = "none";
    document.getElementById("imgContainer").classList.remove("empty");
    img.style.display = "block";
  };
}

async function sendRequest(data, errorCb) {
  const request = JSON.stringify(data);
  ws.send(request);
  console.log(`Op - ${data.op}: Started`);

  if (data.op === "textToImage") {
    return new Promise((resolve) => {
      ws.onmessage = (event) => {
        const { data, op } = JSON.parse(event.data);
        console.log(`Op - ${op}: Finished`);
        resolve(data);
      };
    });
  } else {
    return new Promise((resolve) => {
      ws.onmessage = (event) => {
        const response = JSON.parse(event.data);
        console.log(response);
        if (response.op.startsWith("s:")) {
          console.log(`Op - ${response.op}: Finished`);
          // In the Acord we have 2 notifications
          // 1 - the op itself and if there is an error
          // 2 - the new state
          // So we resolve only we receive the new state in order to continue
          resolve(response);
        } else {
          console.log(`Op - ${response.op}: Received`);
          if (response.op === "error") {
            errorCb(response);
            resolve(response);
          }
        }
      };
    });
  }
}

async function prepare() {
  await sendRequest({
    op: "load_provider",
    data: { name: "stable-diffusion.cpp" },
  });

  acState = AC_STATES.INITIALIZED;
  console.log("Provider stable-diffusion.cpp is initialized!");
}

(async () => {
  while (ws.readyState !== WebSocket.OPEN) {
    await new Promise((resolve) => setTimeout(resolve, 1000));
  }
  await prepare();
})();

async function loadModel() {
  if (ws.readyState !== WebSocket.OPEN) {
    setStatus("WebSocket not connected!", LOG_LEVLEL.ERROR);
    return;
  }
  if (acState === AC_STATES.INSTANCE_STARTED) {
    setStatus(
      "Model is already loaded. Refresh to load a new one.",
      LOG_LEVLEL.ERROR
    );
    return;
  }
  if (acState === AC_STATES.INITIALIZED) {
    const modelInput = document.getElementById("model");
    const modelLoadBtn = document.getElementById("loadBtn");
    const modelPath = modelInput.value;
    if (modelPath === "") {
      setStatus("Please enter the path to the model!", LOG_LEVLEL.ERROR);
      return;
    }
    modelLoadBtn.disabled = true;
    modelLoadBtn.style.opacity = "0.6";
    modelLoadBtn.style.pointerEvents = "none";
    modelInput.disabled = true;
    modelInput.style.opacity = "0.6";
    modelInput.style.pointerEvents = "none";
    const promptInput = document.getElementById("prompt");
    const genBtn = document.getElementById("genBtn");
    promptInput.disabled = true;
    promptInput.style.opacity = "0.6";
    promptInput.style.pointerEvents = "none";
    genBtn.disabled = true;
    genBtn.style.opacity = "0.6";
    genBtn.style.pointerEvents = "none";

    setStatus("Loading...", LOG_LEVLEL.INFO);
    let loadingError = false;
    let handleError = (data) => {
      setStatus(
        `Error loading model! Message: "${data.data}"`,
        LOG_LEVLEL.ERROR
      );
      modelLoadBtn.disabled = false;
      modelLoadBtn.style.opacity = "1";
      modelLoadBtn.style.pointerEvents = "auto";
      modelInput.disabled = false;
      modelInput.style.opacity = "1";
      modelInput.style.pointerEvents = "auto";
      promptInput.disabled = false;
      promptInput.style.opacity = "1";
      promptInput.style.pointerEvents = "auto";
      genBtn.disabled = false;
      genBtn.style.opacity = "1";
      genBtn.style.pointerEvents = "auto";
      loadingError = true;
    };
    await sendRequest(
      {
        op: "load-model",
        data: { binPath: modelPath },
      },
      handleError
    );
    if (loadingError) return;

    acState = AC_STATES.MODEL_LOADED;

    console.log(`Model ${modelPath.split("/").pop()} loaded!`);
    await sendRequest({ op: "start-instance" });

    promptInput.disabled = false;
    promptInput.style.opacity = "1";
    promptInput.style.pointerEvents = "auto";
    genBtn.disabled = false;
    genBtn.style.opacity = "1";
    genBtn.style.pointerEvents = "auto";

    acState = AC_STATES.INSTANCE_STARTED;
    console.log("Instance started!");
    setStatus("Model loaded!", LOG_LEVLEL.SUCCESS);

    const modelBox = document.getElementById("modelBox");
    modelBox.innerHTML = "";
    const p = document.createElement("p");
    p.id = "modelStatus";
    p.style.color =
      "color: var(--color-header); margin-bottom: var(--spacing);";
    p.innerText = `Model "${modelPath
      .split("/")
      .pop()}" is loaded! To load a new one, please refresh the page.`;
    const reloadBtn = document.createElement("button");
    reloadBtn.classList.add("reload-button");
    reloadBtn.onclick = () => location.reload();
    reloadBtn.innerText = "Refresh";
    modelBox.appendChild(p);
    modelBox.appendChild(reloadBtn);
  } else {
    setStatus("Provider is not loaded yet!", LOG_LEVLEL.ERROR);
  }
}

async function sendPrompt() {
  const promptInput = document.getElementById("prompt");
  const genBtn = document.getElementById("genBtn");

  if (ws.readyState !== WebSocket.OPEN) {
    setStatus("WebSocket not connected!", LOG_LEVLEL.ERROR);
    return;
  }

  if (acState === AC_STATES.INSTANCE_STARTED) {
    document.getElementById("skeleton").style.display = "block";
    promptInput.disabled = true;
    promptInput.style.opacity = "0.6";
    promptInput.style.pointerEvents = "none";
    genBtn.disabled = true;
    genBtn.style.opacity = "0.6";
    genBtn.style.pointerEvents = "none";
    setStatus("Generating...", LOG_LEVLEL.INFO);

    const responseData = await sendRequest({
      op: "textToImage",
      data: { prompt: promptInput.value, width: 512, height: 512 },
    });

    fillImageData(responseData);
    setStatus(
      "Generation Done! Try again with a new prompt.",
      LOG_LEVLEL.SUCCESS
    );

    promptInput.disabled = false;
    promptInput.style.opacity = "1";
    promptInput.style.pointerEvents = "auto";
    genBtn.disabled = false;
    genBtn.style.opacity = "1";
    genBtn.style.pointerEvents = "auto";
  } else {
    setStatus("Model is not loaded yet!", LOG_LEVLEL.ERROR);
  }
}

window.loadModel = loadModel;
window.sendPrompt = sendPrompt;
