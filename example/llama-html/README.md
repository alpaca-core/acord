This is an pretty simple HTML example app of llama.cpp plugin integration with JS.

1. Add `ilib-llama.cpp` as a local plugin in root `CMakeLists.txt`.

Example:

```
    make_ac_local_plugin_available(
        NAME ilib-llama.cpp
        VERSION 1.0.0
        GITHUB alpaca-core/ilib-llama.cpp
    )
```


2. Run the the Acord server

3. Open `index.html`


Note: You need to have the models downloaded already in the filesystem. You can pass
it to the `model_path` input field or hardcode the model you want directly in the `app.py` code.
