# Shiu

A simple, minimalist text editor written in **C** using **SDL2**.

## Features
* **Gap Buffer Data Structure:** High-efficiency $O(1)$ insertions and deletions at the cursor position.
* **Hardware Accelerated:** Uses SDL2 for smooth, flick-free text rendering.
* **Native Linux Integration:** Uses `Zenity` for system-native file selection and save dialogs.
* **Minimalist UI:** Focuses entirely on your code with a clean, dark-themed interface.
* **Ultra Lightweight:** The entire executable fits within the L1 cache of most modern CPUs (~23KB).

## Getting Started

### Prerequisites
You need the SDL2 and SDL2_ttf development libraries installed:
```bash
# Ubuntu/Debian/Mint
sudo apt-get install libsdl2-dev libsdl2-ttf-dev zenity
```
## Installation

Clone the repository then compile using gcc:

```bash
gcc Main.c -o ShiuEditor -lSDL2 -lSDL2_ttf
```
## Usage
Shortcut	Action
- Ctrl + S	Save File (Zenity popup)
- Ctrl + O	Open File (Zenity popup)
- Arrows	Navigate through text
