# Changelog

Notable changes to this project are documented in this file.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and project firmware versions use the `MMAJOR.MINOR.PATCH` scheme.

## [Unreleased]

## [MMAJOR.MINOR.PATCH] - YYYY-MM-DD

### Added

- Describe new features.

### Changed

- Describe changes to existing functionality.

### Fixed

- Describe bug fixes.

### Removed

- Describe removed features or functionality.

## [M0.0.1] - 2026-11-08

### Added

- Initial BLE firmware-update bootloader for STM32WB55.
- Generate `FwVersion.h` during the CMake build from Git tag and commit
  metadata, with a configurable fallback version when Git metadata is
  unavailable.
- BLE command and data-batch frames with input validation and CRC checking.
- Flash erase and firmware-write flows, including firmware image validation.
- BLE system-information characteristics for firmware version, wireless
  firmware version, serial number, and hardware revision.
- Startup request flow for entering device firmware update mode.
- Firmware flash-memory layout and project build configuration.
