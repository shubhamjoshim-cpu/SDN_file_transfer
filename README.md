# sdn file transfer

A reliable file transfer system for UDP in C, using Kathara.

# features

- Stop-and-Wait ARQ with acknowledgements, timeouts & retransmissions
- Duplicate packet handling using sequence numbers
- Support for text & binary files
- Local transfers & transfers between Kathara hosts
- Automatic path failover & packet loss testing

# dependencies

- A C compiler (GCC or clang)
- [CMake](https://cmake.org/download/)
- [Kathara](https://www.kathara.org/download.html) and [Docker](https://www.docker.com/products/docker-desktop/) (for the network lab)
- Linux networking tools inside the lab: `ip` for routing and `tc` for packet loss testing

Kathara supports Linux, macOS & Windows. Setup links:
[Linux](https://github.com/KatharaFramework/Kathara/wiki/Linux),
[macOS](https://github.com/KatharaFramework/Kathara/wiki/MacOS),
[Windows](https://github.com/KatharaFramework/Kathara/wiki/Windows).

## Ubuntu

Install the compiler, CMake & Kathara:

```bash
sudo apt update
sudo apt install build-essential cmake software-properties-common
sudo add-apt-repository ppa:katharaframework/kathara
sudo apt update
sudo apt install kathara
```

If Docker is not already installed:

```bash
sudo apt install docker.io
sudo systemctl enable --now docker
```

See the [Ubuntu Docker setup](https://ubuntu.com/server/docs/how-to/containers/docker-for-system-admins/) for Docker access configuration.

## macOS

With [Homebrew](https://brew.sh/) installed, run the commands for anything you don't already have:

```bash
xcode-select --install
brew install cmake
brew install --cask docker-desktop
brew tap KatharaFramework/kathara
brew install --cask kathara
open -a Docker
```

Finish Docker Desktop's first-time setup before using Kathara.

## Windows

Install [Docker Desktop](https://www.docker.com/products/docker-desktop/) & follow the
[Kathara Windows setup](https://github.com/KatharaFramework/Kathara/wiki/Windows).
Use Linux containers and build the project inside the lab with the tools below.

## inside the lab

If needed, install the compiler, CMake & networking tools as root inside a Debian/Ubuntu-based lab container:

```bash
apt update
apt install build-essential cmake iproute2
```

`iproute2` provides both `ip` & `tc`.

# build instructions

From the project directory containing `CMakeLists.txt`:

```bash
cmake -S . -B build
cmake --build build
```

The executables are created in `build`. For the Kathara lab, build inside Linux;
macOS or Windows binaries will not run in the Linux containers.

# how it works

After sending a packet, the sender waits for an acknowledgement before sending the next packet. The packet is retransmitted if the ACK doesn't arrive before the timeout.

The receiver uses sequence numbers to avoid writing duplicate data.

The controller checks both paths & updates routes when a path fails. The sender keeps using the same destination address throughout the transfer.
