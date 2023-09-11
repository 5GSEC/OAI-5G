<table style="border-collapse: collapse; border: none;">
  <tr style="border-collapse: collapse; border: none;">
    <td style="border-collapse: collapse; border: none;">
      <a href="http://www.openairinterface.org/">
         <img src="./images/oai_final_logo.png" alt="" border=3 height=50 width=150>
         </img>
      </a>
    </td>
    <td style="border-collapse: collapse; border: none; vertical-align: center;">
      <b><font size = "5">OAI Build Procedures</font></b>
    </td>
  </tr>
</table>

[[_TOC_]]

This page is valid on tags starting from **`2019.w09`**.

# Overview

The [OAI EPC](https://github.com/OPENAIRINTERFACE/openair-epc-fed/blob/master/docs/DEPLOY_HOME_MAGMA_MME.md) and [OAI 5GC](https://gitlab.eurecom.fr/oai/cn5g/oai-cn5g-fed/-/blob/master/docs/DEPLOY_HOME.md) are developed in distinct projects with their own documentation and are not further described here.

OAI softmodem sources, which aim to implement 3GPP compliant UEs, eNodeB and gNodeB can be downloaded from the Eurecom [gitlab repository](./GET_SOURCES.md).

Sources come with a build script [build_oai](../cmake_targets/build_oai) located at the root of the `openairinterface5g/cmake_targets` directory. This script is developed to build the oai binaries (executables,shared libraries) for different hardware platforms, and use cases.

The main oai binaries, which are tested by the Continuous Integration process are:

-  The LTE UE: `lte-uesoftmodem`
-  The 5G UE: `nr-uesoftmodem`
-  The LTE eNodeB: `lte-softmodem`
-  The 5G gNodeB: `nr-softmodem`
-  The 5G CU-UP: `nr-cuup`
-  The LTE PHY simulators: `dlsim` and `ulsim`
-  The 5G PHY simulators: `nr_dlschsim`, `nr_dlsim`, `nr_pbchsim`, `nr_pucchsim`, `nr_ulschsim`, `nr_ulsim`, `polartest`, `smallblocktest`, `nr _ulsim`, `ldpctest`

Running the  [build_oai](../cmake_targets/build_oai) script also generates some utilities required to build and/or run the oai softmodem binaries:

- `conf2uedata`: a binary used to build the (4G) UE data from a configuration file. The created file emulates the sim card  of a 3GPP compliant phone.
- `nvram`: a binary used to build (4G) UE (IMEI...) and EMM (IMSI, registered PLMN) non volatile data.
- `rb_tool`: radio bearer utility for (4G) UE
- `genids` T Tracer utility, used at build time to generate `T_IDs.h` include file. This binary is located in the [T Tracer source file directory](../common/utils/T) .

The build system for OAI uses [cmake](https://cmake.org/) which is a  tool to generate makefiles. The `build_oai` script is a wrapper using `cmake` and `make`/`ninja` to ease the oai build and use. It logs the `cmake` and `ninja`/`make` commands it executes. The file describing how to build the executables from source files is the [CMakeLists.txt](../CMakeLists.txt), it is used as input by cmake to generate the makefiles.

The oai softmodem supports many use cases, and new ones are regularly added. Most of them are accessible using the configuration file or the command line options and continuous effort is done to avoid introducing build options as it makes tests and usage more complicated than run-time options. The following functionalities, originally requiring a specific build are now accessible by configuration or command line options:

- s1, noS1
- all simulators as the rfsimulator, the L2 simulator, with exception of PHY simulators, which are distinct executables. 

# Running `build_oai`

## List of options

Calling the `build_oai` script with the `-h` option gives the list of all available options. A number of important ones:

- The `-I` option is to install pre-requisites, you only need it the first time you build the softmodem or when some oai dependencies have changed.
- The `-w` option is to select the radio head support you want to include in your build. Radio head support is provided via a shared library, which is called the "oai device" The build script creates a soft link from `liboai_device.so` to the true device which will be used at run-time (here the USRP one, `liboai_usrpdevif.so`). The RF simulator[RF simulator](../radio/rfsimulator/README.md) is implemented as a specific device replacing RF hardware, it can be specifically built using `-w SIMU` option, but is also built during any softmodem build.
- `--eNB` is to build the `lte-softmodem` executable and all required shared libraries
- `--gNB` is to build the `nr-softmodem` and `nr-cuup` executables and all required shared libraries
- `--UE` is to build the `lte-uesoftmodem` executable and all required shared libraries
- `--nrUE` is to build the `nr-uesoftmodem` executable and all required shared libraries
- `--ninja` is to use the `ninja` build tool, which speeds up compilation.
- `-c` is to clean the workspace and force a complete rebuild.

## Installing dependencies

Install all dependencies by issuing the `-I` option. To install furthermore libraries for optional libraries, use the `--install-optional-packages` option. The `-I` option will also install dependencies for an SDR when paired with `-w`. For instance, in order to install all dependencies and the ones for USRP, run:

```bash
cd openairinterface5g/cmake_targets/
./build_oai -I --install-optional-packages -w USRP
```

Note the section on installing UHD further down for more information.

## Installing (new) asn1c from source

With tag 2023.w22, we switch from our [own
`asn1c`](https://gitlab.eurecom.fr/oai/asn1c.git) to a [community-maintained
`asn1c`](https://github.com/mouse07410/asn1c). This new version has many
bugfixes, but is incompatible with the previous version. To ease the
transition, both versions can be installed in parallel. Assuming you installed
`asn1c` using `build_oai`, tags before 2023.w22 will use the version under
`/usr/local/`; tag 2023.w22 and newer will use the version under `/opt/asn1c/`
(if present) or any system directory (e.g., also `/usr/local/`), and
additionally check that all command line options of the new `asn1c` are
supported.

To install the new `asn1c`, either run `build_oai -I`. To not re-install all
packages, you can also just install `asn1c` like this:
```
cd openairinterface5g
sudo ls                               # open sudo session, required by install_asn1c_from_source
. oaienv                              # read of default variables
. cmake_targets/tools/build_helper    # read in function
install_asn1c_from_source             # install under `/opt/asn1c`
```

Additionally, you can also point to a specific `asn1c` to use if you chose to
install elsewhere, using one of these two methods:
```
./build_oai --ninja <other-options> --cmake-opt -DASN1C_EXEC=/opt/asn1c/bin/asn1c
cmake .. -GNinja -DASN1C_EXEC=/opt/asn1c/bin/asn1c
```

## Installing UHD from source

Previously for Ubuntu distributions, when installing the pre-requisites, most of the packages are installed from PPA.

Especially the `UHD` driver, but you could not easily manage the version of `libuhd` that will be installed.

Now, when installing the pre-requisites, especially the `UHD` driver, you can now specify if you want to install from source or not.

- For `fedora`-based OS, it was already the case all the time. But now you can specify which version to install.
- For `ubuntu` OS, you can still install from the Ettus PPA or select a version to install from source.
  * In case of PPA installation, you do nothing special, the script will install the latest version available on the PPA.
    - `./build_oai -I -w USRP`
  * In case of a installation from source, you do as followed:

```bash
export BUILD_UHD_FROM_SOURCE=True
export UHD_VERSION=3.15.0.0
./build_oai -I -w USRP
```

The `UHD_VERSION` env variable `SHALL` be a valid tag (minus `v`) from the `https://github.com/EttusResearch/uhd.git` repository.

**CAUTION: Note that if you are using the OAI eNB in TDD mode with B2xx boards, a patch is mandatory.**

Starting this commit, the patch is applied automatically in our automated builds.

See:

* `cmake_targets/tools/uhd-3.15-tdd-patch.diff`
* `cmake_targets/tools/uhd-4.x-tdd-patch.diff`
* `cmake_targets/tools/build_helper` --> function `install_usrp_uhd_driver_from_source`

## Building PHY Simulators

The PHY layer simulators (LTE and NR) can be built as follows:

```bash
cd openairinterface5g/cmake_targets/
./build_oai --phy_simulators
```

After completing the build, the binaries are available in the `cmake_targets/ran_build/build` directory.

Detailed information about these simulators can be found [in this dedicated page](https://gitlab.eurecom.fr/oai/openairinterface5g/wikis/OpenAirLTEPhySimul)

## Building UEs, eNodeB and gNodeB Executables

After downloading the source files, a single build command can be used to get the binaries supporting all the oai softmodem use cases (UE and [eg]NodeB):

```bash
cd openairinterface5g/cmake_targets/
./build_oai -w USRP --eNB --UE --nrUE --gNB
```

You can build any oai softmodem executable separately, you may not need all of them depending on your oai usage.

After completing the build, the binaries are available in the `cmake_targets/ran_build/build` directory.

## Building Optional Binaries

There are a number of optional libraries that can be built in support of the
RAN, such as telnet, scopes, offloading libraries, etc.

Using the help option of the build script you can get the list of available optional libraries.

```bash
./build_oai --build-lib all # build all
./build_oai --build-lib telnet  # build only telnet
./build_oai --build-lib "telnet enbscope uescope nrscope nrqtscope"
./build_oai --build-lib telnet --build-lib nrqtscope
```

The following libraries are build in CI and should always work: `telnet`,
`enbscope`, `uescope`, `nrscope`, `nrqtscope`.

Some libraries have further dependencies and might not build on every system:
- `enbscope`, `uescope`, `nrscope`: libforms/X
- `nrqtscope`: Qt5
- `ldpc_cuda`: CUDA
- `ldpc_t1`: DPDK and VVDN T1
- `websrv`: npm and others

# Running `cmake` directly

`build_oai` is a wrapper on top of `cmake`. It is therefore possible to run `cmake` directly. An example using `ninja`: to build all "main targets" for 5G, excluding additional libraries:
```
cd openairinterface5g
mkdir build && cd build
cmake .. -GNinja && ninja nr-softmodem nr-uesoftmodem nr-cuup params_libconfig coding rfsimulator ldpc
```

To build additional libraries, e.g., telnet, do the following:
```bash
cmake .. -GNinja -DENABLE_TELNETSRV=ON && ninja telnetsrv
```

A list of all libraries can be seen using `ccmake ..` or `cmake-gui ..`.

It is currently not possible to build all targets in the form of `cmake ..
-GNinja && ninja`: currently, SDRs are always exposed, even if you don't have
the dependencies, and some targets are simply broken. Again, `build_oai` list
all targets that it builds, and you can use them with `ninja`

The default target directory of `build_oai` is the following, for historical reasons:
```bash
cd openairinterface5g/cmake_targets/ran_build/build
cmake ../../.. -GNinja
ccmake ../../..
cmake-gui ../../..
```
You can of course use all standard cmake/ninja/make commands in this directory.
