
# my_sonoff_custom

Sonoff basic R2 V1.0 (2017-10-11) custom firmware.

## Features

- Manual button control.
- Control through Telegram.
- Configurable automatic on/off at specific times.
- Automatic NTP date syncronization.

## Notes

- This Sonoff version is an ESP8285 core device.

## Telegram Commands Usage

Use command **/start** to show Bot start text and request Reply Keyboard Markup:

```text
/start
```

Use command **/help** to show Bot help text (a list of all supported commands and descriptions of each one):

```text
/help
```

Use command **/on** to turn on the Realy:

```text
/on
```

Use command **/off** to turn on the Realy:

```text
/off
```

Use command **/status** to get current Relay status (is it on or off):

```text
/status
```

Use command **/time** to get/set actual system time:

```text
/time
/time 14:35:20
```

Use command **/timeon** to set/unset automatic turn on Relay time:

```text
/timeon set 22:00:00
/timeon unset 22:00:00
```

Use command **/timeoff** to set/unset automatic turn off Relay time:

```text
/timeoff set 01:00:00
/timeoff unset 01:00:00
```

Use command **/timecfg** to show or clrear all configured automatic turn on and off Relay times:

```text
/timecfg
/timecfg clear
```

Use command **/timezone** to get/set system time zone:

```text
/timezone
/timezone +0
/timezone +2
/timezone -1
```

Use command **/ntp** to enable, disable or force a system date synchronization through NTP:

```text
/ntp
/ntp status
/ntp off
/ntp on
/ntp force
```

Use command **/reboot** to force system reboot:

```text
/reboot
```
