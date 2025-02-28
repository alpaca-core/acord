# acord

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) 

The Alpaca Core daemon. A daemon that provides a WebSocket API for the [Alpaca Core Local SDK](https://github.com/alpaca-core/alpaca-core-sdk/).

> [!IMPORTANT]
> This project is still in in an alpha stage of development. Significant changes are very likely, or rather, certain, and backwards compatibility is disregarded.

The current release is a preview of daemon and SDK along with several AI-based demos.

## Preview Info and Limitations

This preview release loads several hard-coded plugins for AI inference of concrete models:

* [ilib-llama.cpp](https://github.com/alpaca-core/ilib-llama.cpp): Multiple LLM-s wrapping [ggerganov/llama.cpp](https://github.com/ggerganov/llama.cpp)
* [ilib-whisper.cpp](https://github.com/alpaca-core/ilib-whisper.cpp): Whisper ASR wrapping [ggerganov/whisper.cpp](https://github.com/ggerganov/whisper.cpp)
* [ilib-sd.cpp](https://github.com/alpaca-core/ilib-sd.cpp): Image generation with Stable Diffusion wrapping [leejet/stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp)
* [ilib-ac-tortoise](https://github.com/alpaca-core/ilib-ac-tortoise): TTS with tortoise. Based on [balisujohn/tortoise.cpp](https://github.com/balisujohn/tortoise.cpp)

For all of these the supported compute backends for now are CPU, CUDA and Metal on Apple Silicon.

If your device does not support CUDA or Metal, you can run on CPU, but all the demos here are such that the performance will be very poor, to the point of being unusable.

## Build and Run Acord

CMake 3.24+ and a C++20 capable compiler are required. Ninja is also required if you use the provided presets.

Instructions for setting up the recommended build environment for various platforms can be found [here](https://github.com/alpaca-core/alpaca-core-sdk/blob/master/doc/dev/dev-env.md)

If you have CMake, Ninja and a C++20 compiler installed, you should be able to build acord with the following commands:

```bash
$ git clone https://github.com/alpaca-core/acord.git
$ cd acord
$ cmake --preset=release-demo -B build
$ cmake --build build
```

This should produce the `acord` executable in `build/bin`. Simply run it with:

```bash
$ ./build/bin/acord
```

Or on Windows
:
```cmd
> .\build\bin\acord.exe
```

## Running the Demos

Follow the instructions for the individual demos in the [example](example) directory.

## License

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

This software is distributed under the MIT Software License. See accompanying file LICENSE or copy [here](https://opensource.org/licenses/MIT).

Copyright &copy; 2024-2025 [Alpaca Core, Inc](https://alpacacore.com).
