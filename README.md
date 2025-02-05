# System Monitor

Sujal Timilsina (stimils)

----
### Technologies Used
- *XQuartz* - GUI display
- *GitHub* - version control
- *C/C++* - coding language
---
### What does the System Monitor do?
- The user is able to see their system's information, graphs of the current resource consumption, details on their different mount points/file systems.
- Similar to the computer's Task Manager, you will be able to see information about running processes and have access to perform actions such as stop, kill, and continue.

---
### How do I use System Monitor?
1. Download this repository
2. Navigate to the TaskManager directory
3. Run `make clean` to restart the files
4. Then run `make`
    - You must have `XQuartz` installed on the device
5. Run `systemmonitor`
6. From here you should be able to navigate through the GUI and see your system information and the current processes running!
---
### Tab Details

##### System

- Summary of the system information including:
    - Network Title
    - OS Name
    - OS Version
    - Kernel Version
    - Amount of Memory
    - Processor Version
    - Disk Storage

##### Processes

- Shows processes that are on the system
- You can use filters *Active Processes*, *All Processes*, and *My Processes* in the **View** tab at the top left.
- Each process includes information on:
    - Process Name
    - Status
    - % CPU
    - (Process) ID
    - Memory
- Use the **Refresh** button in **View** to refresh the processes. A prompt will show when refresh was successful
- You can right click on a process for a drop down to see more views and commands including
    - Stop Process
    - Continue Process
    - Kill Process
    - Memory Maps
    - Open Files
    - See Properties
    - (Note: some actions will not work if process is not owned by the current user or if the process was killed or ended while the monitor is being run)

##### Resources

- Shows live graphs on the **CPU Usage**, **Memory and Swap History**, and **Network History**.

##### File Systems

- Shows File Systems information for each mount point including
    - Device
    - Directory
    - Type
    - Total (memory)
    - Available (memory)
    - Used (memory)
    - % Used (memory)


