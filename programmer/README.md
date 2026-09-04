## Adding the udev rule

Subsequent to installing Batronix 32P ROM burner software via flatpak

* `wget https://www.batronix.com/exe/Batronix/Prog-Express/flatpak/prog-express-3.9.3.flatpak`

The device is not seen on Debian. In order to have it be seen:

* Copy the `udev` rule file, mode `su`
  - `cp /etc/udev/rules.d/85-batronix-devices.rules`
* Reload the rules
  - `sudo udevadm control --reload-rules`
  - `sudo udevadm trigger`
* Test the rule
  - `udevadm test /sys/class/tty/ttyUSB0`

The device will now be seen by the software.

![seen](/programmer/device-seen.jpg)