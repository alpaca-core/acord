This is an pretty simple example app of stable-diffusion.cpp plugin integration
using Python's Gradio library as a frontend. To run the example
follow the commands:

0. Add `ilib-sd.cpp` as a local plugin in root `CMakeLists.txt`.

Example:

```
    make_ac_local_plugin_available(
        NAME ilib-sd.cpp
        VERSION 1.0.0
        GITHUB alpaca-core/ilib-sd.cpp
    )
```


1. Install the Python's required packages

```
    python3 -m pip install -r requirements.txt
```

2. Run the acord server

3. Start the gradio app

```
python app.py
```

You need to have the models downloaded already in the filesystem. You can pass
it to the `model_path` input field or hardcode the model you want directly in the `app.py` code.
