# Device Tree Visualiser

This small cli tool generates a graphical representation of a *device-tree-blob*\
if you are like me, and have a monkey brain and need to see the pretty pictures to\
understand all those fancy words, this tool might help you to to understand the relationships\
of the devices of your platform.

## How to build it

First you need to *build* the build system, and then use it to build the application, i.e. run the following commands

```shell
clang -o nob nob.c
./nob
```

Now you can use the tool

```shell
./dtv foo.dtb <render_engine: dot|fdp>

./dtv foo.dtb fdp
./dtv foo.dtb dot
```

This will create a `foo.svg` as output, now your device tree has a graphical representation.

## tl;dr

foo.dtb -> [ dtv : dot|fdp ] -> foo.svg

## References

- [git kernel tree: Documentation/devicetree](https://web.git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/devicetree)
- [Website: https://www.devicetree.org](https://www.devicetree.org)
- [Video: Device Tree for Dummies! - Thomas Petazzoni, Free Electrons](https://youtu.be/m_NyYEBxfn8?si=gxfQnGGdv6R7iz1e)
- [Video: Device Tree: hardware description for everybody!](https://youtu.be/Nz6aBffv-Ek?si=vWhUn6WJIg9Tt-xT)
