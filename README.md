# Yx-RoutineTracker
Yx-RoutineTracker is a lightweight routine to-do app I built with Qt to manage my own routines. 
I designed it with the philosophy of having it as functionality and aesthetically minimalistic as possible as that is my preference. 
It contains only the functionalities that I need. 

The app consists of functionalities to manage daily and recurring routines, track completion history, and prioritize routines through an intuitive and compact interface. 
It runs as a system tray icon and can be accessed on the laptop by clicking on the system tray icon. 

As of now, the app supports:
- Daily
- Weekly
- Biweekly
- Monthly
- Quarterly
- Semiannually
- Annually
- Day
- Interval

I personally run this app in the background on my laptop 24/7. 
It had saved me the effort to keep track of my routines manually using Microsoft Excel, and had helped me streamlined my routine tracking process.
In addition to that, I built this with the aim that it could help others that want a minimalistic routine tracker in their life. 
The app have proved to be a reliable utility program for my life. 

Feel to to try it out, share it with others, and let me know if the app personally helped you in any way!

Contributions, issues, and pull requests are welcomed!

## Requirements
- Windows 10 (or above, not tested)

## Installation
### Method 1: Build from Source
1. Download **Qt Online Installer** from the **Qt Official Website**.
2. Install **Qt 6.8.2** (or above, not tested), **latest MinGW**, and **Qt Creator** through **Qt Online Installer**.
3. Download this repository.
4. Launch **Qt Creator**.
5. Open the repository folder in **Qt Creator**.

### Method 2: Use Pre-Built Executable
1. Download this directory.
   ```
   Yx_RoutineTracker\build\Desktop_Qt_6_8_2_MinGW_64_bit-Release\release
   ```
2. Place the folder anywhere in your computer.
3. Start the app by running the **Yx-RoutineTracker.exe** file.
4. Rename the folder to whatever name you want. (Optional) 
5. Create a shortcut for the **Yx-RoutineTracker.exe** file. (Optional)
6. Press **Win+R** and type **shell:startup** to open the startup folder of the computer, then paste the created shortcut in it. This allows the app to run automatically every time the computer boots up. (Optional)

## Usage
(Use case overview. To be updated.)

## Future Roadmap
### Possible new features:
- Ability to edit routine details.
- New routine type: N times per [routine type].  
  (Excluding daily, day, and interval.
  Reason being that one of the explicit principle of the app is that each routine should only be checked once, a.k.a. one "checked" record, per day.)

### Possible improvements and bug fixes:
- As of now, when sorting the priorities too fast in the "routine page", the priorities of the routines may end up glitched causing them to share the same priority value.  
(Suspicion: The current process gets interupted by the new process when the user clicks on the "+" or "-" button subsequently too fast.)

## Trivia
- I drew the app icon myself! It is my own special rendition of Clockmon (Xros Wars) from digimon in pixel art! (An app that revolves around time = Clockmon :D)

