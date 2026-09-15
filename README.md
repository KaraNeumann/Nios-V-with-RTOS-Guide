# Guide for implementing NIOS-V FreeRTOS on the DE23-LITE Board
## 1 Introduction
This is a step by step guide for running [FreeRTOS](https://www.freertos.org/) on the [Terasic DE-23 Lite FPGA Board](https://www.terasic.com.tw/cgi-bin/page/archive.pl?No=1383). It was compiled by Kara Neumann, along with NAME, NAME, NAME, NAME.

This repo also includes the full [archived quartus project](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/tree/main/CompletedProject/ArchivedQuartusProject), along with the [software folders](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/tree/main/CompletedProject/SoftwareFolders). Note that only the modified files are included in the software folders, all the rest should be generated using Section 7.



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

### 2.2 Altera Licensing
Go to the [Altera SSLC](https://www.altera.com/SSLC) and register an account.

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/LicencingCentre1.png)

Once you've logged in, select `Sign up for Evaluation or No-Cost Licences`.
Select the Agilex 3 licence and `next`

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/LicensingCentre2.png)

Select `Create a New Computer`
Set the `Computer Name` to whatever you want e.g. "Kara's laptop"
Set the `Licence Type` to `FIXED`
Set the `Computer Type` to `NIC ID`
Set the `Primary Computer ID` to the MAC address of your computer's main adapter.

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/LicencingCentre3.png)

You can find this by running `ipconfig /all` in your terminal

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/LicencingCentre4.png)

Select `save` and then `Generate`

You will receive an email with a file attached named `LR-XXXXXX_License.dat`. Download this file.

Launch Quartus. If this is the first time you've opened quartus you will need to go to `Tools/License Setup` and set the Licence File to the `LR-XXXXXX_License.dat` file.

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/LicenceSetup.png)

## 3 Project Setup
### 3.1 Project Creation
Select `File/new Project Wizard`
Choose a directory and a name for the project, and then press `Next`. I highly recommend choosing a path with no spaces on a local directory (not on a onedrive or dropbox folder etc).

For this example I've named my project `FreertosGuide`

Set the device to `A3CZ135BB18AE7S`.
All the other settings can be left as the default settings.

### 3.2 Pin Assignments
`Select Assignments/Device` and check the device is set to `A3CZ135BB18AE7S`
`Select Assignments/Import Assignments` and import the [DE23-Lite Pin Assignment file](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/RequiredResources/DE_23_Pin_Assignments.csv) which is taken from the [Terasic Resource Package](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=44&No=1383&PartNo=4#contents). You should now be able to see the pin assignments under `Assignments/Pin Planner`

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/PinPlanner.png)




## 4 Platform Designer
### 4.1 Adding SDRAM Controller IP 

The SDRAM controller IP core was deprecated in a previous Quartus update, so we are going to use an open source substitute. This was one again taken from the [Terasic Resource Package](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=44&No=1383&PartNo=4#contents).

First you should create a new folder in the project folder named `projectFolder/ip/`

Copy paste the `core_sdram_axi` folder from [here](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/tree/main/RequiredResources) into the `projectFolder/ip/` folder.
In my example the folder hierarchy should look as follows:

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/IPcoreFormat.png)

### 4.2 Opening the Platform Designer

You can launch the Platform designer from `Tools/Platform Designer` or the shortcut on the tool bar.
![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/PlatformDesignerShortcut.png)

Make sure the `Quartus project` is set to the current project (as a `.qpf` file). Select the new page button to create a new Platform Designer System.

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/PlatformDesigner1.png)

Choose a name in the default project directory and then select `Create`. For this example I've used `nios_system.qsys`
The `Quartus Project` and `Platform Designer System` should both be green. Select `Create` again.
This process may take a while, press `Close` when it is finished.

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/PlatformDesigner2.png)

At this point you should be able to see the custom imported SDRAM controller module in the IP Catalog.

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/PlatformDesigner3.png)

### 4.3 Create Components

To create the system you will need to add the following IP cores. For now leave all the settings at the default:
 - Clock In (included by default)
 - Reset In (included by default)
 - IOPLL
 - NIOS V/g General Purpose Processor
 - On-Chip Memory II
 - JTAG UART
 - 3x PIO
 - SDRAM AXI4 (imported in section 4.1 above)
 - Interval Timer

### 4.4 Component Settings

The following settings need to be set in the components.

#### 4.4.1 Clock Bridge Component
In the System View check the `in_clk` Clock Input to export to `clk`.

#### 4.4.2 IOPLL Component
**Under the PLL Tab**

Set `General/Reference Clock Frequency` to `50`

Set `General/Enable Locked Output Port` to `False`

Set `Output Clocks/Number of Clocks` to `3`

Set `outclk0/Desired Frequency` to `80`

Set `outclk1/Desired Frequency` to `80`

Set `outclk1/Phase Shift Units` to `degrees`

Set `outclk1/Desired Phase Shift` to `-70`

Set `outclk2/Desired Frequency` to `80`

Set `outclk2/Phase Shift Units` to `degrees`

Set `outclk2/Desired Phase Shift` to `125`

**Under the Settings Tab**

Set `Physical PLL Settings / PLL Auto Rest` to `True`

The IOPLL settings should look as follows:

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/iopll1.png)
![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/iopll2.png)
![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/iopll3.png)

In the System View set the `outclk1` Clock Output to export to `sdram_clk`

#### 4.4.3 NIOS V/g General Purpose Processor Component
Set `Debug/Enable Reset from Debug Mode` to `True`

Set `Memory Configurations/Peripheral Regions/Peripheral Region A` to `512 KBytes`

The NIOS V settings should look as follows:

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/nios1.png)
![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/nios2.png)

#### 4.4.4 On-Chip Memory II Component
Set `Size/Total Memory Size` to `524280`

The Memory settings should look as follows:

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/memory.png)

#### 4.4.5 First PIO Component

Set the component name to `PIO_KEY`

Set `Basic Settings/Width` to `4`

Set `Basic Settings/Direction` to `Input`

Set `Edge Capture Register/Synchronous Capture` to `True`

Set `Edge Capture Register/Edge Type` to `ANY`

Set `Interrupt/Generate IRQ` to `True`

Set `Interrupt/IRQ Type` to `EDGE`

The KEY PIO settings should look as follows:

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/pio_key.png)

In the System View set the `external_connection` Conduit to export to `key_external_connection`

#### 4.4.6 Second PIO Component

Set the component name to `PIO_LEDR`

Set `Basic Settings/Width` to `10`

Set `Basic Settings/Direction` to `Output`

The KEY PIO settings should look as follows:

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/pio_ledr.png)

In the System View set the `external_connection` Conduit to export to `ledr_external_connection`

#### 4.4.7 Third PIO Component

Set the component name to `PIO_SW`

Set `Basic Settings/Width` to `10`

Set `Basic Settings/Direction` to `Input`

Set `Edge Capture Register/Synchronous Capture` to `True`

Set `Edge Capture Register/Edge Type` to `ANY`

Set `Interrupt/Generate IRQ` to `True`

Set `Interrupt/IRQ Type` to `EDGE`

The KEY PIO settings should look as follows:

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/pio_sw.png)

In the System View set the `external_connection` Conduit to export to `sw_external_connection`

#### 4.4.8 SDRAM AXI4 Component
In the System View set the `sdram` Conduit to export to `sdram`

#### 4.4.9 Interval Timer Component
Set the component name to `sys_clk`
**NB: if this is not set to exactly the correct name, FreeRTOS won't recognise it later**

### 4.5 Component Connections

Make all the component connections as shown in the image below:

Set the IRQ numbers as follows:
 - `sys_clk` = `IRQ 0`
 - `JTAG UART` = `IRQ 1`
 - `PIO_KEY` = `IRQ 2`
 - `PIO_SW` = `IRQ 3`

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/connections1.png)
![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/connections2.png)

### 4.6 Final Steps and Generating HDL

Select `System/Assign Base Addresses` from the top toolbar

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/assignBaseAddresses.png)

Go to the Nios V component settings.
Check that `Traps, Exceptions, and Interrupts/reset Agent` is set to `intel_onchip_memory_0.s1`

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/nios3.png)

Select `Sync System Infos` at the bottom right corner.

At this point there should be no errors or warnings.

Select `Generate HDL` at the bottom right corner, and then `Generate`. Make sure to save your system.
This process may take a few minutes.

After that finishes you can close the Platform Designer.


## 5 Creating the Top Level File
Go back to the base Quartus Prime Pro software and select `File/New` and choose `Verilog HDL File`.

I've modified the existing Terasic Golden Top File from  the [Terasic Resource Package](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=44&No=1383&PartNo=4#contents) to instantiate a version of our system. You can copy this file from <LINK>.
Make sure the file name matches your project name and the module name on line 34.
Make sure the ip core names all match the instantiation definition in lines 105-122, otherwise the compilation will fail.
You can find the ip core names here: `/<ProjectFolder>/<SystemName>/synth/<SystemName>.v` which in my case is `.../FreertosGuide/nios_system/synth/nios_system.v`.


Save this file with the same name as your project (in my case `.../FreertosGuide/FreertosGuide.v`).
This should automatically be set as the top level project, but you can test this by checking it is visible in the `Project navigator Tab/Hierarchy`.

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/hierarchy.png)


## 6 Compiling and Programming

At this point you can select `Processing/Start Compilation`, or select the blue triangle from the toolbar.


This compilation may take a few minutes to compile, depending on your hardware. It might throw a few warnings but there shouldn't be any errors.

After this has completed select `Tools/Programmer` or select the Programmer icon in the toolbar.

Make sure your DE23 cable is connected to your computer, and also connected to power.

Select `Hardware Setup/Hardware Settings/Currently Selected Hardware` and set it to `DE23-Lite [USB-X]. You may need to adjust the settings if the FPGA isn't automatically recognised.

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/programmer.png)

Once the connection is configured press `Start` on the Programmer. After this is complete you can close the Programmer so that the UART connection closes. You don't need to save the configuration.

All the FPGA lights should stop cycling through the preset pattern, which indicates the quartus project is successfully programmed.


## 7 Verilog and Programming

This section will be divided into 2 parts. The first section (7.1) will implement a basic HAL test involving printing to the console.
The second section(7.2) will implement the full RTOS system and demonstrate it.

### 7.1 Software - HAL
The first step is to run a basic print script to test the NIOS V Processor.

#### 7.1.1 Creating the File Structure
In the main project directory you should create a `software` folder. Inside there should be 2 folders.

 - A `bsp` folder which is empty.
 - A `app` folder which contains a blank file called `helloWorld.c`

The file hierarchy should look as follows:

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/hierarchyHal.png)

#### 7.1.2 Setup the Project
Launch the NIOS V Terminal. This can be found in the Quartus program files, which is my case is `C:\altera_pro\26.1\niosv\bin\niosv-shell.exe`

Navigate in the NIOS terminal to the software folder you created.

In order to generate the BSP, you should run the following command in the NIOS terminal:

`> niosv-bsp -c -t=hal -p=../FreertosGuide.qpf -s=../nios_system.qsys bsp/settings.bsp`

 - `-p=` should be set to the path of the quartus project file
 - `-s=` should be set to the path of the Platform Designer system
 - The last argument should be the location where the bsp file will be saved, which should be in the `bsp` folder

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/generateBsp.png)

This should generate a whole bunch of files in the `/bsp/` directory, including a `settings.bsp` file.

Next run the following command on the NIOS terminal, which creates the CMAKE file for the application:
`> niosv-app -a=app -b=bsp -s=app/helloWorld.c`

 - `-a` should be set to the path of the Application folder
 - `-b` should be set to the path of the BSP folder
 - `-s` should be set to the path of the source file you created earlier

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/generateBsp.png)


This will generate a `CMakeLists.txt` file in the `./app/` directory. 


Next, open the Ashling RiscFree IDE. You can do this either by running `> riscfree` in the NIOS V terminal, or running the application which on my system is at: `C:\altera_pro\26.1\riscfree\RiscFree.exe`

This will open a window that asks for the workspace directory. Set the directory to the `/software/` older, and select `Launch`

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/riscfree1.png)

Once the software opens, select `Import Nios V CMake project` from the Project Explorer on the left.
Set the location to the `./app/` folder and the name to `app`, and select `Finish`.

Right click on the Project explorer pane, select `Import Nios V CMake project`, and repeat this process for the `./bsp folder`

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/riscfree2.png)


Your Project Explorer should look something like this:

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/riscfree3.png)


#### 7.1.3 Writing the Script and Building the Project

Open the `helloWorld.c` script and add the following code:

```
#include <stdio.h>

int main( void) {
	printf("Hello from Nios V :) \n");
	return 0;
}
```

Save this file. Fight click on the app folder in the Project Explorer pane and select `Build Project`. Once this is finished you should have a `.../app/build/Default/app.elf` file generated.

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/riscfree4.png)


#### 7.1.4 Running the Script

Go back to the NIOS Terminal and run the following command:
`> jtag-uart`

Keep this terminal window open in the background, this is where the output messages will be displayed.

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/riscfree5.png)

Go back to the Ashling RiscFree software. Right click on the `./app/` folder and select `Run As/2 Ashling RISC-V Hardware Debugging`

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/riscfree6.png)


Set the local application to `app.elf`

This will open up the `Edit Cpnfiguration` window.
Check the `Main/Project` is set to `app`
Check sure the `Main/C/C++ Application` is set to `build/Default/app.elf`

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/riscfree7.png)

Check the `Debugger/Debug Probe Configuration/Debug Probe` is set to `DE23-Lite [USB-X]`

Select `Debugger/Target Configuration/Auto-detect Scan Chain` This should automatically populate the `Device/TAP selection` and `Core Selection`

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/riscfree8.png)


Select `Apply` and then `Run`

Swap back to the NIOS Terminal, and you should see the print message.

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/riscfree9.png)


**NB: You must manually terminate the program, otherwise you will not be able to rerun a new program.**
To do this you must go to the Ashling Software terminal, right click on the terminal, and select `Terminate/Disconnect All`

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/riscfree10.png)


### 7.2 Software - FreeRTOS
The next step is to setup the full FreeRTOS system.

Before starting this section you should close Ashling RiscFree IDE and any NIOS terminals from Section 7.1.


#### 7.2.1 Creating the File Structure
In the main project directory you should create a `software_rtos` folder. Inside there should be 2 folders, and 1 file.

 - A `bsp` folder which is empty.
 - A `app` folder which contains a blank file called `rtos.c`
 - A file named `setting.tcl`

The file hierarchy should look as follows:

![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/rtos1.png)

The `settings.tcl` script should contain the following:
```
set_setting hal.enable_reduced_device_drivers true


```
This is a custom command which modifies the drivers using in the JTAG, becasue the default drivers do not print at all.


#### 7.2.2 Setup the Project
Launch the NIOS V Terminal. This can be found in the Quartus program files, which is my case is `C:\altera_pro\26.1\niosv\bin\niosv-shell.exe`

Navigate in the NIOS terminal to the `software_rtos` folder you created.

In order to generate the BSP, you should run the following command in the NIOS terminal:

`> niosv-bsp -c -t=freertos -p=../FreertosGuide.qpf -s=../nios_system.qsys -x=settings.tcl bsp/settings.bsp`

 - `-p=` should be set to the path of the quartus project file
 - `-s=` should be set to the path of the Platform Designer system
 - `-x=` should set a custom script to also be run while creating the bsp
 - The last argument should be the location where the bsp file will be saved, which should be in the `bsp` folder

In addition to the usual bsp files, there should also be a `.../bsp/FREERTOS/` directory.


Next run the following command on the NIOS terminal, which creates the CMAKE file for the application:
`> niosv-app -a=app -b=bsp -s=app/rtos.c`
 - `-a` should be set to the path of the Application folder
 - `-b` should be set to the path of the BSP folder
 - `-s` should be set to the path of the source file you created earlier

This will generate a `CMakeLists.txt` file in the `./app/` directory. 

Next, you need to manually add a FreeRTOS config line. Open the file `.../software_rtos/bsp/FREERTOS/inc/FreeRTOSConfig.h` and add `#define configISR_STACK_SIZE_WORDS 512` to the top of the config file.

The first few lines should look something like this:

```
...
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configISR_STACK_SIZE_WORDS 512


#include "sys/alt_tls.h"

#include "system.h"
#include "os_cpu.h"
...

```



Next, open the Ashling RiscFree IDE. You can do this either by running `> riscfree` in the NIOS V terminal, or running the application which on my system is at: `C:\altera_pro\26.1\riscfree\RiscFree.exe`

This will open a window that asks for the workspace directory. Set the directory to the `/software/` older, and select `Launch`


Once the software opens, select `Import Nios V CMake project` from the Project Explorer on the left.
Set the location to the `./app/` folder and the name to `app`, and select `Finish`.

Right click on the Project explorer pane, select `Import Nios V CMake project`, and repeat this process for the `./bsp folder`

#### 7.2.3 Writing the Script and Building the Project

Open the `rtos.c` script and add the code from[here](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/RequiredResources/rtos.c)

This script will test all the basic functionality of the RTOS. It initialises 2 tasks:
- Task 1 reads the values of the values of the SW every second and saves it to the SDRAM.
- Task 2 reads the value from the SDRAM and outputs them to the LEDR

Additonally, an interrupt is generated each time the KEY changes. Note that this will trigger once when the button is pressed and once when it is depressed.


Save this file. Fight click on the app folder in the Project Explorer pane and select `Build Project`. Once this is finished you should have a `.../app/build/Default/app.elf` file generated.

#### 7.2.4 Running the Script

Go back to the NIOS Terminal and run the following command:
`> jtag-uart`

Keep this terminal window open in the background, this is where the output messages will be displayed.

Go back to the Ashling RiscFree software. Right click on the `./app/` folder and select `Run As/2 Ashling RISC-V Hardware Debugging`

Set the local application to `app.elf`

This will open up the `Edit Configuration` window.
Check the `Main/Project` is set to `app`
Check sure the `Main/C/C++ Application` is set to `build/Default/app.elf`

Check the `Debugger/Debug Probe Configuration/Debug Probe` is set to `DE23-Lite [USB-X]`

Select `Debugger/Target Configuration/Auto-detect Scan Chain` This should automatically populate the `Device/TAP selection` and `Core Selection`

Select `Apply` and then `Run`

Swap back to the NIOS Terminal, and the output should look something like this:
![alt text](https://github.com/KaraNeumann/Nios-V-with-RTOS-Guide/blob/main/Images/rtosOutput.png)


**NB: You must manually terminate the program, otherwise you will not be able to rerun a new program.**
To do this you must go to the Ashling Software terminal, right click on the terminal, and select `Terminate/Disconnect All`

## 8 References

Thank you James Salamy for all your help with this project.

The following resources were used to create this guide:
https://www.freertos.org
[Altera Embedded Peripherals IP User Guide](https://docs.altera.com/r/docs/683130/24.1/embedded-peripherals-ip-user-guide/download-document)
[Nios V Embedded Processor Design Handbook](https://docs.altera.com/r/docs/726952/26.1.1/nios-v-embedded-processor-design-handbook/about-the-nios-v-embedded-processor)
[Hello World - Nios V video](https://www.youtube.com/watch?v=c6t-MVQ_j8Y)
[Hello Nios using FreeRTOS video](https://www.youtube.com/watch?v=uVQmrPffRhU)
[Terasic Resource Package](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=44&No=1383&PartNo=4#contents)