# DuPAL V3 firmware changeog
Changelog for the DuPAL V3 "dupico" board firmware.

## [0.1.3] - 2024-08-13
### Changed
- Slightly reduce delays in shifter code

## [0.1.2] - 2024-08-09
### Changed
- Updated pico-sdk and FreeRTOS-Kernel
- Forcing CLI interface task to a single core is no longer needed

## [0.1.1] - 2024-08-05
### Changed
- Avoid yielding continuously to other tasks when working with shift registers

## [0.1.0] - 2024-08-04
### Added
- Support for binary transmission protocol

### Removed
- Removed support for the text protocol

### Changed
- Reduced command buffer to 64 bytes
- Optimized the number of calls for data transmission via USB

## [0.0.5] - 2024-08-02
### Changed
- Optimized string to number (and vice-versa) conversions in strutils
- Avoid uselessly clearing the command buffer at each start of command

## [0.0.4] - 2024-07-29
### Changed
- The extended write now contains a dynamic number of parameters (still limited to 256 bytes of buffer size)

## [0.0.3] - 2024-07-29
### Added
- Command to read the firmware version

### Removed
- Removed the "Dupico FW" string at connection

## [0.0.2] - 2024-07-26

- Initial release