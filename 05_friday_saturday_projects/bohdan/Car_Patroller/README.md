# Fixing "gamepad won't connect" / "already bound to wrong gamepad"

## The problem

Cheap PS3-style ("SixAxis") clone gamepads don't do normal Bluetooth pairing.
Each pad has a **master MAC address burned into its own memory**, and it will
only ever try to connect to that one host. If it doesn't match your ESP32's
Bluetooth MAC, the pad just blinks forever — the ESP32 never even sees a
connection attempt, no matter what the firmware does.

Fix: write the ESP32's MAC into the controller using `sixaxispairer`.

---

## 1. Get the ESP32's Bluetooth MAC

Open Serial Monitor (115200 baud), reset the board, and look for a boot line like:

```
BTstack up and running at CC:50:E3:AF:E2:96
```

That address is what you'll use below.

## 2. Build the pairing tool (Ubuntu)

```bash
sudo apt install build-essential pkg-config libhidapi-dev git
git clone --depth 1 https://github.com/ricardoquesada/bluepad32.git
cd bluepad32/tools/sixaxispairer
make
```

## 3. Write the ESP32's MAC into the controller

Plug the gamepad into your PC via USB, then:

```bash
sudo ./sixaxispairer XX:XX:XX:XX:XX:XX   # your ESP32's MAC from step 1
```

## 4. Connect

Unplug the USB cable, press the PS button. May take up to ~10s. LEDs should
stop blinking and settle on player 1.

> Note: this is a one-time write **per pad, per host**. If the pad is later
> plugged into another PC/console/ESP32, you'll need to re-run
> `sixaxispairer` to point it back here.

---

## Clearing an existing binding (car already locked to a gamepad)

The firmware stores the bound pad's MAC in NVS. To clear it, either:

**A — Hold BOOT button** on the ESP32 for 2 seconds.
Triggers `resetBinding()`: stops motors, erases stored MAC, calls
`BP32.forgetBluetoothKeys()`, beeps, and restarts.

**B — Serial command**
With Serial Monitor open (115200 baud), send:

```
r
```

After reset, the board reboots and is free again — the next gamepad that
connects and presses PS becomes the new bound controller.

If you're switching to a *different physical pad*, make sure to also redo
steps 2–3 above for that pad (each pad needs its own master-address write).
