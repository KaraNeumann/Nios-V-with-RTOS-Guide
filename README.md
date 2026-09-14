# DE-23 Lite Nios V with RTOS Guide

## 1 Introduction
This is a step by step guide for running [FreeRTOS](https://www.freertos.org/) on the [Terasic DE-23 Lite FPGA Board](https://www.terasic.com.tw/cgi-bin/page/archive.pl?No=1383).


## 2 Installing Software and Licensing
### 2.1 Download Quartus
Download the [Quartus Prime Pro Installer](https://www.altera.com/downloads/fpga-development-tools/quartus-prime-pro-edition-design-software-version-26-1-windows). I'm running version 26.1 on Windows 11, but the latest version *should* work.

When you select download you will be prompted to create an account, fill in a bunch of information, and accept a bunch of T&Cs.
![Screencap of Personal Info required by Alterra](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/AlterraPersonalInfo.png)

Launch the Installer.
You will need to install the following components:
- Quartus Prime Pro Edition Software
- Agilex 3 common Files
- Agilex 3 Device Support
- Everything in the Drivers folder
- Ashling RiscFree IDE for Altera

This should require about 50 GB of space.
![Screencap of Quartus download settings](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/InstallSettings.png)

While you are waiting for this to download, you can set up the licencing.