# Nethorix - Network Guard

A Windows desktop app built with Python (Tkinter) that scans your network and
alerts you about what it finds.

<p align="center">
  <img src="assets/icon.png" alt="Nethorix logo" width="150">
</p>

<h1 align="center">Nethorix - Network Guard</h1>


## Features
- Network scanning with a desktop GUI (dark mode supported)
- Security monitoring (`security_monitor.py`)
- Phone dashboard (`dashboard.py`) to view status from your phone
- Arduino hardware alerts: LEDs and a buzzer over serial
- Email alerts
- MAC spoofing detection (same IP seen with two MAC addresses) [confirm]
- Excel alert report (`alerts_report.xlsx`) with export [confirm]
- Email password stored in encrypted storage (keyring) [confirm]

## Requirements
- Windows
- Arduino board (optional, for LED and buzzer alerts)

## Download

[⬇️ Download Nethorix v1.0](https://github.com/SamiruNinduwara/Nethorix/releases/download/v1.0/Nethorix.v1.0.exe)


Builds a single-file exe with PyInstaller. The installer is made with Inno Setup (`installer.iss`).


## Brand
See the [Brand Book]([docs/Nethorix_Brand_Book.pdf](https://github.com/SamiruNinduwara/Nethorix/blob/main/docs/Nethorix_Brand_Book.pdf)) for the logo, colors and typography.


## Configuration
Copy `config.example.json` to `config.json` and fill in your own settings.
Never commit your real `config.json`.

## License
- MIT

- Developed by Samiru Ninduwara
