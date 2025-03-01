# acord

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) 

The Alpaca Core daemon. A daemon that provides a WebSocket API for the [Alpaca Core SDK](https://github.com/alpaca-core/alpaca-core-sdk/).

> [!IMPORTANT]
> This project is still in in an alpha stage of development. Significant changes are very likely, or rather, certain, and backwards compatibility is disregarded.

The current release is a preview of the daemon with several AI-based demos.

## Preview Info and Limitations

This preview release loads several hard-coded plugins for AI inference:

* [ilib-llama.cpp](https://github.com/alpaca-core/ilib-llama.cpp): Multiple LLM-s wrapping [ggerganov/llama.cpp](https://github.com/ggerganov/llama.cpp)
* [ilib-sd.cpp](https://github.com/alpaca-core/ilib-sd.cpp): Image generation with Stable Diffusion wrapping [leejet/stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp)

For all of these the supported compute backends for now are CPU, CUDA, and Metal on Apple Silicon.

If your device does not support CUDA or Metal, you can run on CPU, but we fear that all the demos here are such that the performance will be very poor, to the point of being unusable.

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

## Making Your Own Apps

Feel free to experiment and make your own apps. You can do them in any language you want as long as you have a WebSocket client library.

## Contributing

As mentioned above, the project is still in an alpha stage of development. We are open to contributions, but please be aware that the codebase is still very much in flux. If you're thinking of doing something more than a minor change, please open an issue or a discussion first.

## Roadmap

Here's a list of the major acord-related features we're planning to work on in the coming weeks on the road to beta.

> [!NOTE]
> Most of the work for the roadmap will be done in [alpaca-core/alpaca-core-sdk](https://github.com/alpaca-core/alpaca-core-sdk/) and other repos.

* Specify and implement a more robust schema system. This includes:
    * Specifying schema definitions
    * Defining schema-based codegen
    * Defining the frame format and the schema-based serialization/deserialization
    * Defining a schema distribution system (a package manager of sorts)
* Refine and improve features of the existing plugins. Some priorities include:
    * Video generation via Stable Diffusion
    * Improving llama.cpp-based chat
* Stability improvements and integration tests
* Add more plugins. Some priorities here include:
    * Text-to-speech
    * Alternative LLM plugin
* Support more compute backends
* Improve asset management
* Create an installable package for the daemon
* Create some actual acord-based applications as opposed to just demos and examples. Ideas are welcome here!

Some more distant goals include:

* Add a basic authentication system so people can choose to expose acord to the internet
* Create an AI model repository so models could queried by features and tags, and accessed by id
* Create a plugin repository and a plugin management system

## License

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

This software is distributed under the MIT Software License. See accompanying file LICENSE or copy [here](https://opensource.org/licenses/MIT).

Copyright &copy; 2024-2025 [Alpaca Core, Inc](https://alpacacore.com).
