# DTV: Development Tools & Visualisers

This repository holds a small collection of CLI tools that provide a **graphical visualisations** for various aspects of your POSIX/POSIX-like system (currently tested on **MacOS** and **Linux**).

## 🔧 How to Build

First, build the build system:

```shell
clang -o nob nob.c
```

then use it to build all tools:

```shell
./nob
```

This will create a `build` directory with all compiled binaries.

## Repository Layout

```shell
./
├── LICENSE     # MIT license
├── README.md   # This file
├── build/      # Built artifacts
│   ├── dt2gv   # Device tree visualiser
│   └── ps2gv   # Process tree visualiser
├── examples/   # Example graph outputs
├── external/   # External static libraries and dependencies
│   └── nob.h   # 'nob' build system header
├── nob         # Build system
├── nob.c       # Build system source
├── src/        # Source code for each tool
│   ├── dt2gv/  # Device Tree to Graph Visualiser
│   └── ps2gv/  # Process Tree to Graph Visualiser
└── utils/              # Utility scripts
    └── dependencies    # Dependency checker
```

## 🛠️ Tools

- [`dt2gv`](src/dt2gv/README.md) - Device Tree to Graph Visualiser
Converts Linux device trees blobs (`.dtb`) into visual diagrams.

- [`ps2gv`](src/ps2gv/README.md) - Process Tree to Graph Visualiser
Parses process listings (`ps -eo ppid,pid,rss,pcpu,comm`) into hierarchical process tree graphs.

## 💡 Philosophy

Keep it simple, keep it visual.

DTV aim to create fast, easy to use tools that produce clear visualisations to help developers understand complex systems, embedded platforms, and runtime behaviour.
