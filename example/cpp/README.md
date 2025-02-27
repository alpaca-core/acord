# Acord C++ Examples

## Build

Enabling `ACORD_BUILD_EXAMPLES` when configuring should be enough. It is `ON` by default, so acord using the steps in the README should also build the examples here.

There is however and additional option: `ACORD_EXAMPLES_SAME_PROCESS_SERVER`. Setting it to `ON` will run the Acord server in the same process as the example. This is useful for debugging. In this case you don't need have acord running for the examples. In fact you must not have it running at the same time as you run such examples, as they will fail to run (port already in use).
