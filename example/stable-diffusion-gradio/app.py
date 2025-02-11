import numpy as np
import gradio as gr
from websockets.asyncio.client import connect
from PIL import Image
import json

ACORD_WEBSOCKET = "ws://localhost:7654"
LOADED_MODEL = ""
async def gen_image(desc, model_path, input_img):
    async with connect(ACORD_WEBSOCKET, max_size=10*1024*1024) as ws_connection:
        response = await ws_connection.recv()

        async def send_request(data):
            request = json.dumps(data)

            await ws_connection.send(request)

            response = await ws_connection.recv()
            print(response)
            response = await ws_connection.recv()
            print(response)

        await send_request({"op":"load_provider",
                        "data" : {
                            "name" : "stable-diffusion.cpp"
                        }})

        if model_path == "":
            model_path = "/Users/pacominev/repos/ac/ac-dev/ilib-sd.cpp/.cpm/ac-test-data-stable-diffusion/047dd6b83a8edf0b1a12930d3b4d72be19184faf/sd-v1-4.ckpt"

        await send_request({"op": "load-model",
                            "data" : {
                                "binPath": model_path
                            }})

        await send_request({"op": "start-instance"})

        respData = None
        if input_img is None:
            request = json.dumps({"op": "textToImage",
                                "data": {
                                    "prompt": desc,
                                    "width" : 512,
                                    "height" : 512,
                                    # "steps" : 30
                                }
                                })
            await ws_connection.send(request)

            response = await ws_connection.recv()
            respData = json.loads(response)
        else:
            request = json.dumps({"op": "imageToImage",
                                "data": {
                                    "prompt": desc,
                                    "imagePath": input_img,
                                    "width" : 512,
                                    "height" : 512,
                                }
                                })
            await ws_connection.send(request)

            response = await ws_connection.recv()
            print(response)
            respData = json.loads(response)

        if "data" in respData:
            data = respData["data"]
            width, height, channels = data.get("width"), data.get("height"), data.get("channel")
            _bytes = data.get("data")
            raw_bytes = bytes(_bytes.get("bytes"))  # Ensure it's in byte format

            # Convert raw bytes into a NumPy array
            img_array = np.frombuffer(raw_bytes, dtype=np.uint8).reshape((height, width, channels))

            # Convert NumPy array to PIL Image
            image = Image.fromarray(img_array)
            return image
        else:
            return "Error: No image received"

demo = gr.Interface(
    fn=gen_image,
    inputs=[gr.TextArea(), gr.Text(), gr.Image()],
    outputs=["image"],
)

demo.launch()

