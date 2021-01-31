![Banner](./header.png)
# How to run skiftOS on Virtual Box

## Preparing the host OS
1. Download the latest stable version of _Virtual Box_ for your host OS from [here](https://www.virtualbox.org/wiki/Downloads) or any other _similar_ service.
2. Download the latest release `.img` file of skiftOS from [here](https://github.com/skiftOS/skift/releases/latest).
3. Rename the extension of the downloaded `.img` file from `.img` to `.hdd`.

> **NOTE:** Incase you are using windows, file extensions may not be displayed. Read this [guide](https://www.partitionwizard.com/partitionmanager/change-file-extension-windows-10.html) to enable extensions and then rename the file.

4. Great! You now have all the files required to run skiftOS

## Creating the VM on Virtual Box

5. Create a new VM by clicking on the **New** button.
6. Give the OS a meaningful name and set **Type: Other** and **Version: Other/Unknown** and click on next.
7. Allocate a minimum of **128MB RAM** _(512MB recommended)_ and click on next.
8. Choose **Do not add a virtual hard disk**, click on _Create_ and ignore the warning message and press **Continue**.

  ![Steps 5-8 gif](running_in_vbox_step1.gif)

9. Click on **Settings**, go to **Display** and increase **Video Memory** to 32MB.
10. Next, click on **Storage** and under storage devices, click on the icon that looks like a _hard disk_ (located next to the CD icon) and in the window that appears,click on **Add**.
11. Navigate to the `.hdd` file created in Step 3 and **choose** the file.
12. You're all set to run skiftOS! Click on start to see it boot up.

  ![Steps 9-12 gif](running_in_vbox_step2.gif)

## Increasing display resolution (optional)
13. Click on **applications** and search for **terminal**.
14. Run `displayctl -s 1920x1080` or `displayctl -s 1280x720` to increase the display resolution as required. Other resolutions _may not be stable!_.

## Feedback and Issues
Feel free to report errors,bugs and make feature requests [here](https://github.com/skiftOS/skift/issues) any and do consider contributing to the [skiftOS repository](https://github.com/skiftOS/skift/).
Also checkout the [skiftOS webpage](https://skiftos.org/).


