# This is a fork of [esphome-modular-lvgl-buttons](https://github.com/agillis/esphome-modular-lvgl-buttons)

I have forked this repo and made multiple modifications to suite my needs. Keeping a fork would have been
too much of a hassle, as I have removed a lot of code that is not relatable to me. I will try to keep a list
of what I have done.

## Modifications

* Added support for cyrillic
* Fixed the themes and added mechanism for automatically applying dark theme at night
* Removed the bootscreen that did not disappear after the predefined period, but after 60 secs