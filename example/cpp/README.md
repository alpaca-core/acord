# Acord C++ Examples

## Build

Enabling `ACORD_BUILD_EXAMPLES` when configuring should be enough. 

There is however and additional option: `ACORD_EXAMPLES_SAME_PROCESS_SERVER`. Setting it to `ON` will run the Acord server in the same process as the example. This is useful for debugging. Be careful to not have `acord` running at the same time as you run such examples, as they will fail to run (port already in use).
