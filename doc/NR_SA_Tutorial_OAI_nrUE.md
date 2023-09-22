<table style="border-collapse: collapse; border: none;">
  <tr style="border-collapse: collapse; border: none;">
    <td style="border-collapse: collapse; border: none;">
      <a href="http://www.openairinterface.org/">
         <img src="./images/oai_final_logo.png" alt="" border=3 height=50 width=150>
         </img>
      </a>
    </td>
    <td style="border-collapse: collapse; border: none; vertical-align: center;">
      <b><font size = "5">OAI 5G NR SA tutorial with OAI nrUE</font></b>
    </td>
  </tr>
</table>

**Table of Contents**

[[_TOC_]]

#  1. Scenario
In this tutorial we describe how to configure and run a 5G end-to-end setup with OAI CN5G, OAI gNB and OAI nrUE.

Minimum hardware requirements:
- Laptop/Desktop/Server for OAI CN5G and OAI gNB
    - Operating System: [Ubuntu 22.04 LTS](https://releases.ubuntu.com/22.04/ubuntu-22.04.3-desktop-amd64.iso)
    - CPU: 8 cores x86_64 @ 3.5 GHz
    - RAM: 32 GB
- Laptop for UE
    - Operating System: [Ubuntu 22.04 LTS](https://releases.ubuntu.com/22.04/ubuntu-22.04.3-desktop-amd64.iso)
    - CPU: 8 cores x86_64 @ 3.5 GHz
    - RAM: 8 GB
- [USRP B210](https://www.ettus.com/all-products/ub210-kit/), [USRP N300](https://www.ettus.com/all-products/USRP-N300/) or [USRP X300](https://www.ettus.com/all-products/x300-kit/)
    - Please identify the network interface(s) on which the USRP is connected and update the gNB configuration file


# 2. OAI CN5G

## 2.1 OAI CN5G pre-requisites

Please install and configure OAI CN5G as described here:
[OAI CN5G](NR_SA_Tutorial_OAI_CN5G.md)


# 3. OAI gNB and OAI nrUE

## 3.1 OAI gNB and OAI nrUE pre-requisites

### Build UHD from source
```bash
sudo apt install -y libboost-all-dev libusb-1.0-0-dev doxygen python3-docutils python3-mako python3-numpy python3-requests python3-ruamel.yaml python3-setuptools cmake build-essential

git clone https://github.com/EttusResearch/uhd.git ~/uhd
cd ~/uhd
git checkout v4.4.0.0
cd host
mkdir build
cd build
cmake ../
make -j $(nproc)
make test # This step is optional
sudo make install
sudo ldconfig
sudo uhd_images_downloader
```

## 3.2 Build OAI gNB and OAI nrUE

```bash
# Get openairinterface5g source code
git clone https://gitlab.eurecom.fr/oai/openairinterface5g.git ~/openairinterface5g
cd ~/openairinterface5g
git checkout develop

# Install OAI dependencies
cd ~/openairinterface5g/cmake_targets
./build_oai -I

# nrscope dependencies
sudo apt install -y libforms-dev libforms-bin

# Build OAI gNB
cd ~/openairinterface5g
source oaienv
cd cmake_targets
./build_oai -w USRP --ninja --nrUE --gNB --build-lib "nrscope" -c
```

# 4. Run OAI CN5G and OAI gNB

## 4.1 Run OAI CN5G

```bash
cd ~/oai-cn5g
docker compose up -d
```

## 4.2 Run OAI gNB

### USRP B210
```bash
cd ~/openairinterface5g
source oaienv
cd cmake_targets/ran_build/build
sudo ./nr-softmodem -O ../../../targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band78.fr1.106PRB.usrpb210.conf --gNBs.[0].min_rxtxtime 6 --sa -E --continuous-tx
```
### USRP N300
```bash
cd ~/openairinterface5g
source oaienv
cd cmake_targets/ran_build/build
sudo ./nr-softmodem -O ../../../targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band77.fr1.273PRB.2x2.usrpn300.conf --gNBs.[0].min_rxtxtime 6 --sa --usrp-tx-thread-config 1
```

### USRP X300
```bash
cd ~/openairinterface5g
source oaienv
cd cmake_targets/ran_build/build
sudo ./nr-softmodem -O ../../../targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band77.fr1.273PRB.2x2.usrpn300.conf --gNBs.[0].min_rxtxtime 6 --sa --usrp-tx-thread-config 1 -E --continuous-tx
```

### RFsimulator
```bash
cd ~/openairinterface5g
source oaienv
cd cmake_targets/ran_build/build
sudo ./nr-softmodem -O ../../../targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band78.fr1.106PRB.usrpb210.conf --gNBs.[0].min_rxtxtime 6 --rfsim --sa
```

# 5. OAI UE

## 5.1  SIM Card
Edit openair3/UICC/usim_interface.c
```bash
#define UICC_PARAMS_DESC {\
    {"imsi",             "USIM IMSI\n",          0,         strptr:&(uicc->imsiStr),              defstrval:"001010000000001",           TYPE_STRING,    0 },\
    {"nmc_size"          "number of digits in NMC", 0,      iptr:&(uicc->nmc_size),               defintval:2,         TYPE_INT,       0 },\
    {"key",              "USIM Ki\n",            0,         strptr:&(uicc->keyStr),               defstrval:"fec86ba6eb707ed08905757b1bb44b8f", TYPE_STRING,    0 },\
    {"opc",              "USIM OPc\n",           0,         strptr:&(uicc->opcStr),               defstrval:"c42449363bbad02b66d16bc975d77cc1", TYPE_STRING,    0 },\
    {"amf",              "USIM amf\n",           0,         strptr:&(uicc->amfStr),               defstrval:"8000",    TYPE_STRING,    0 },\
    {"sqn",              "USIM sqn\n",           0,         strptr:&(uicc->sqnStr),               defstrval:"000000",  TYPE_STRING,    0 },\
    {"dnn",              "UE dnn (apn)\n",       0,         strptr:&(uicc->dnnStr),               defstrval:"oai",     TYPE_STRING,    0 },\
    {"nssai_sst",        "UE nssai\n",           0,         iptr:&(uicc->nssai_sst),              defintval:1,    TYPE_INT,    0 }, \
    {"nssai_sd",         "UE nssai\n",           0,         iptr:&(uicc->nssai_sd),               defintval:0xffffff,    TYPE_INT,    0 }, \
};
```

## 5.2 Testing OAI nrUE with USRP B210

Important notes:
- This should be run in a second Ubuntu 22.04 host, other than gNB
- It only applies when running OAI gNB with USRP B210

Run OAI nrUE
```bash
cd ~/openairinterface5g
source oaienv
cd cmake_targets/ran_build/build
sudo ./nr-uesoftmodem -r 106 --numerology 1 --band 78 -C 3619200000 --ue-fo-compensation --sa -E --uicc0.imsi 001010000000001
```

## 5.2 Testing OAI nrUE with RFsimulator
Important notes:
- This should be run on the same host as the OAI gNB
- It only applies when running OAI gNB with RFsimulator

Run OAI nrUE with RFsimulator
```bash
cd ~/openairinterface5g
source oaienv
cd cmake_targets/ran_build/build
sudo ./nr-uesoftmodem -r 106 --numerology 1 --band 78 -C 3619200000 --rfsim --sa --uicc0.imsi 001010000000001 --rfsimulator.serveraddr 127.0.0.1
```

### 5.2.1 Ping test
- UE host
```bash
ping 192.168.70.135 -I oaitun_ue1
```

# 6. Advanced configurations (optional)

## 6.1 USRP N300 and X300 Ethernet Tuning

Please also refer to the official [USRP Host Performance Tuning Tips and Tricks](https://kb.ettus.com/USRP_Host_Performance_Tuning_Tips_and_Tricks) tuning guide.

The following steps are recommended. Please change the network interface(s) as required. Also, you should have 10Gbps interface(s): if you use two cables, you should have the XG firmware. Refer to the [N300 Getting Started Guide](https://kb.ettus.com/USRP_N300/N310/N320/N321_Getting_Started_Guide) for more information.

* Use an MTU of 9000: how to change this depends on the network management tool. In the case of Network Manager, this can be done from the GUI.
* Increase the kernel socket buffer (also done by the USRP driver in OAI)
* Increase Ethernet Ring Buffers: `sudo ethtool -G <ifname> rx 4096 tx 4096`
* Disable hyper-threading in the BIOS (This step is optional)
* Optional: Disable KPTI Protections for Spectre/Meltdown for more performance. **This is a security risk.** Add `mitigations=off nosmt` in your grub config and update grub. (This step is optional)

Example code to run:
```
for ((i=0;i<$(nproc);i++)); do sudo cpufreq-set -c $i -r -g performance; done
sudo sysctl -w net.core.wmem_max=62500000
sudo sysctl -w net.core.rmem_max=62500000
sudo sysctl -w net.core.wmem_default=62500000
sudo sysctl -w net.core.rmem_default=62500000
sudo ethtool -G enp1s0f0 tx 4096 rx 4096
```

## 6.2 Real-time performance workarounds
- Enable Performance Mode `sudo cpupower idle-set -D 0`
- If you get real-time problems on heavy UL traffic, reduce the maximum UL MCS using an additional command-line switch: `--MACRLCs.[0].ul_max_mcs 14`.

## 6.3 Uplink issues related with noise on the DC carriers
- There is noise on the DC carriers on N300 and especially the X300 in UL. To avoid their use or shift them away from the center to use more UL spectrum, use the `--tune-offset <Hz>` command line switch, where `<Hz>` is ideally half the bandwidth, or possibly less.
