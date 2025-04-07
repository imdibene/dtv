# ps2gv: Process Tree to Graph Visualiser

This cli tool is inspired by Brendan Gregg's [Colony Graphs](https://www.brendangregg.com/colonygraphs.html) scripts, this capture a snapshop of your system processes and their parent and child relationships.

The tool supports these usage patterns:

- Default config and `ps` command, output `ptree.svg`

```shell
./ps2gv
```

- Custom config location

```shell
./ps2gv -c ~/myconfig.conf
```

- Process single file

```shell
./ps2gv foo
```

- Process multiple files with default config

```shell
./ps2gv foo bar baz
```

- Process files with custom config

```shell
./ps2gv -c settings.conf log1.txt log2.txt
```

The utility script [ps-snapshot.sh](../../utils/ps-snapshot.sh) can be use to create the input files:

```shell
bash ps-snapshot.sh > ps-snapshot.trace
```

## tl;dr

Usage: ./ps2gv [-c config_file] [input_files...]

## References

- [Colony Graphs](https://www.brendangregg.com/colonygraphs.html)
- [ps2gv.conf example](../../examples/ps2gv/ps2gv.conf)
- [ptree.svg examples](../../examples/ps2gv)
