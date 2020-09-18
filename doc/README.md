# Code Documentation

_WARNING: This is the Degate **code** documentation, not the **software** documentation._

## Build

There is not prebuilt code documentation, you have to build it yourself.

### Dependencies

The only dependency is Doxygen. Pre-built binaries are available [here](https://www.doxygen.nl/download.html#srcbin).

### Quick start

A specific "Documentation" target is available. Just have an up-to-date Doxygen installation and go to your **root** build folder.
For example:
```console
> cmake ..
> cmake --build . --target Documentation
```

The documentation will be generated in the "/doc/build" folder.