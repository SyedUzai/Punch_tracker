import socket
import time
import pandas as pd
import os

UDP_IP = "0.0.0.0"
UDP_PORT = 3335

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening on UDP {UDP_PORT}...")

def parse_sensor_data(data_string):
    # Returns a dictionary of parsed sensor values
    parts = data_string.strip().split()
    data_dict = {}
    for part in parts:
        if '=' in part:
            key, value = part.split('=')
            data_dict[key] = float(value)
    return data_dict

while True:
    data, addr = sock.recvfrom(1024)
    decoded_data = data.decode()
    print(f"Received from {addr}: {decoded_data}")

    try:
        sensor_data = parse_sensor_data(decoded_data)
        ax_value = sensor_data.get("AX", 0)

        if ax_value < -2.0:
            print("🥊 Threw a Punch!")

            # Start timestamp for elapsed time calculation
            start_time = time.time()

            # Collect next 10 samples with relative timestamps
            collected_data = []
            for sample_index in range(10):
                new_data, new_addr = sock.recvfrom(1024)
                new_decoded = new_data.decode().strip()
                print(f"Captured from {new_addr}: {new_decoded}")
                parsed = parse_sensor_data(new_decoded)
                parsed["sample"] = sample_index + 1
                parsed["timestamp_ms"] = round((time.time() - start_time) * 1000, 3)
                collected_data.append(parsed)

            # Save to Excel
            df = pd.DataFrame(collected_data)
            timestamp_str = time.strftime("%Y%m%d_%H%M%S")
            save_dir = r"C:\Users\Uzair\OneDrive\Documents\UDP_Server"
            filename = os.path.join(save_dir, f"hook_capture_{timestamp_str}.xlsx")
            df.to_excel(filename, index=False)
            print(f"✅ Saved 10 parsed samples with timestamps to: {filename}")

            time.sleep(0.75)  # Delay before checking for next punch

    except Exception as e:
        print("Error parsing data:", e)