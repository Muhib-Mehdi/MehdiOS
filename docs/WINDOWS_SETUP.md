# Running Antigravity OS on Windows

Since operating systems require low-level access that Windows protects, we need a "Cross-Compiler" environment. The best way to do this on Windows 10/11 is using **WSL2 (Windows Subsystem for Linux)**.

This runs a real Ubuntu terminal *inside* Windows.

## Step 1: Install Ubuntu (One-Time Setup)
You have WSL enabled, but no Linux distribution installed yet.

1.  Open **PowerShell** as Administrator (Right-click Start -> Terminal (Admin)).
2.  Run this command:
    ```powershell
    wsl --install -d Ubuntu
    ```
3.  **Restart your computer** if asked.
4.  After restart, open the "Ubuntu" app from your Start Menu.
5.  It will ask you to create a **username** and **password**. (Remember these!)

## Step 2: Install Build Tools
In your new Ubuntu terminal (not PowerShell!), paste these commands to get `gcc`, `make`, `nasm`, and `qemu`:

```bash
# Update package list
sudo apt update

# Install C compiler, Assembler, and Emulator
sudo apt install build-essential nasm qemu-system-x86

# Install 32-bit support (Critical for our OS!)
sudo apt install gcc-multilib
```

## Step 3: Accessing Your Code
WSL can access your Windows files via `/mnt/`.

1.  In the Ubuntu terminal, navigate to your project folder:
    ```bash
    # Note: 'd/11Operating system' in Windows is '/mnt/d/11Operating system' in WSL
    cd "/mnt/d/11Operating system"
    ```

## Step 4: Build and Run
Now you can use the standard commands!

1.  **Build the OS**:
    ```bash
    make
    ```
2.  **Run the OS**:
    ```bash
    # This runs QEMU without a GUI (headless) if you don't have an X server
    # Since you want to see the window on Windows, you might need to install an X Server like 'VcXsrv'
    # OR simpler:
    make run
    ```

> **Note on QEMU GUI**: If `make run` gives an error about "SDL" or "Display not found":
> 1. Install **VcXsrv** on Windows.
> 2. Launch "XLaunch" in Windows.
> 3. In Ubuntu: `export DISPLAY=:0`
> 4. `make run`

## Troubleshooting
If you just want to see if it compiles, run:
```bash
make
```
If you see `os-image.bin` created, you are successful!
