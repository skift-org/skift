# Skift system layer `skift.system`

## PID 1 `init`

## Application container `apphost`
Responsible for
 - Checking if a application is still responding.
 - 

## Services container `srvhost`
Responsible for 
 - Restarting 'do not respond services'
 - Restarting when a config change is made.

## Input deamon `inputd`
Responsible for
 - Translating scancode to keyevent
 - Keybinding to command in a i3wm style.
 - On the fly layout switching like in window or linux with ibus.

## Notification deamon `notifd`