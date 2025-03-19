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
./dtv foo.dtb
```

This will create a `foo.svg` as output, now your device tree has a graphical representation.

## tl;dr

foo.dtb -> [ dtv ] -> foo.svg

