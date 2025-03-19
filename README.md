# Device Tree Visualiser

This small cli tool transform a device-tree-blob into a graphical representation\
if you are like me and need to see pictures to understand the relationships, this tool might help you.

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

