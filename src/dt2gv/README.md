# dt2gv: Device Tree to Graph Visualiser

This small cli tool generates a graphical representation of a *device-tree-blob*. If you are like me, and need to see the pretty pictures to understand all those fancy words, this tool might help you to understand the relationships of the devices of your platform.

```shell
./dt2gv foo.dtb <render_engine: dot|fdp>

./dt2gv foo.dtb fdp
./dt2gv foo.dtb dot
```

This will create a `foo.svg` as output, now your device tree has a graphical representation. See [here for a DOT example](../../examples/dt2gv/am335x-bone__dot__layout.svg) and [here for a FDP example](../../examples/dt2gv/am335x-bone__fdp__layout.svg)

## tl;dr

foo.dtb -> [ dt2gv : dot|fdp ] -> foo.svg

## References

- [git kernel tree: Documentation/devicetree](https://web.git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/devicetree)
- [Website: https://www.devicetree.org](https://www.devicetree.org)
- [Video: Device Tree for Dummies! - Thomas Petazzoni, Free Electrons](https://youtu.be/m_NyYEBxfn8?si=gxfQnGGdv6R7iz1e)
- [Video: Device Tree: hardware description for everybody!](https://youtu.be/Nz6aBffv-Ek?si=vWhUn6WJIg9Tt-xT)
