import cv2
import numpy as np
import serial
import time
import mss
import pygetwindow as gw

# --- Try to connect to Arduino ---
try:
    arduino = serial.Serial('COM3', 9600, timeout=2)
    time.sleep(2)
    arduino.reset_input_buffer()
    arduino_connected = True
    print("✅ Arduino successfully connected.")
except Exception as e:
    arduino = None
    arduino_connected = False
    print(f"⚠️ Running without Arduino: {e}")

# ============================================================
#  🪟 FIND DS CAPTURE WINDOW
# ============================================================

def get_ds_capture_region(window_names=("DS Capture", "3DSCapture")):
    """Finds the DS Capture window and returns its screen coordinates."""
    for name in window_names:
        wins = gw.getWindowsWithTitle(name)
        if wins:
            win = wins[0]
            if not win.isMinimized:
                win.activate()
                time.sleep(0.3)
                left, top = win.left, win.top
                width, height = win.width, win.height
                print(f"✅ Found window '{name}' at ({left}, {top}) size ({width}x{height})")
                return {"top": top, "left": left, "width": width, "height": height}
    print("❌ Could not find DS Capture window. Make sure it's open and visible.")
    return None

monitor = get_ds_capture_region()
if monitor is None:
    raise SystemExit("❌ Stopping — no DS Capture window found.")

sct = mss.mss()

# ============================================================
#  🎯 REGION OF INTEREST (ROI)
# ============================================================
# Adjust these based on your previous test
x_start, x_end = 55, 70
y_start, y_end = 145, 155

# ============================================================
#  🎨 COLOR SETTINGS
# ============================================================
# BGR color for desired Pokemon from your test
normal_color = np.array([132, 125, 30])  
tolerance = 5  # can tweak this if detection is too strict or too loose

count = 0
print("🎮 DS Capture shiny detection active! Press 'q' to quit.")

# ============================================================
#  🧠 MAIN LOOP
# ============================================================
while True:
    frame = np.array(sct.grab(monitor))
    frame = cv2.cvtColor(frame, cv2.COLOR_BGRA2BGR)

    # Draw ROI box
    cv2.rectangle(frame, (x_start, y_start), (x_end, y_end), (0, 255, 0), 1)

    # Extract ROI and get average color
    roi = frame[y_start:y_end, x_start:x_end]
    avg_color = np.array(cv2.mean(roi)[:3], dtype=int)

    cv2.imshow("🎥 DS Capture Shiny Detector", frame)

    # --- Arduino communication ---
    if arduino_connected and arduino.in_waiting:
        line = arduino.readline().decode(errors="ignore").strip()
        if line == "start":
            print("\n-------------------------")
            print("🔁 New reset detected!")
            print("Arduino sent: START signal.")
            print("-------------------------")

            time.sleep(2.0)  # wait for encounter to appear

            color_diff = np.abs(avg_color - normal_color)
            is_shiny = np.any(color_diff > tolerance)

            print(f"Avg color: {avg_color}, Diff: {color_diff}, Shiny: {is_shiny}")

            if is_shiny:
                arduino.write(b"shiny\n")
                print("📤 Sent to Arduino: shiny")
            else:
                arduino.write(b"not_shiny\n")
                count += 1
                print("📤 Sent to Arduino: not_shiny")
                print(f"🧮 Encounter count: {count}")

            arduino.flush()
            time.sleep(0.3)
        else:
            print(f"[Arduino] {line}")

    # Quit with Q
    if cv2.waitKey(1) & 0xFF == ord('q'):
        print("👋 Quitting shiny detector...")
        break

cv2.destroyAllWindows()
if arduino_connected:
    arduino.close()

