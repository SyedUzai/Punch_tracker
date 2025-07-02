import socket
import time
import pandas as pd
import os
from joblib import load

# === CONFIGURATION ===
UDP_IP = "0.0.0.0"
UDP_PORT = 3335
save_dir = r"C:\Users\Uzair\OneDrive\Documents\UDP_Server"
model_path = os.path.join(save_dir, "punch_classifier.pkl")

# === Load trained Random Forest model ===
model = load(model_path)

# === Socket setup === 
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening on UDP {UDP_PORT}...")

# === Helper to parse sensor string into dictionary ===
def parse_sensor_data(data_string):
    parts = data_string.strip().split()
    data_dict = {}
    for part in parts:
        if '=' in part:
            key, value = part.split('=')
            data_dict[key] = float(value)
    return data_dict

# === Feature extraction for classification ===
def extract_features(df):
    features = {}
    for axis in ['AX', 'AY', 'AZ']:
        features[f'{axis}_mean'] = df[axis].mean()
        features[f'{axis}_std'] = df[axis].std()
        features[f'{axis}_max'] = df[axis].max()
        features[f'{axis}_min'] = df[axis].min()
        features[f'{axis}_range'] = df[axis].max() - df[axis].min()
    return features

# === Main loop ===
while True:
    data, addr = sock.recvfrom(1024)
    decoded_data = data.decode()
    print(f"Received from {addr}: {decoded_data}")

    try:
        sensor_data = parse_sensor_data(decoded_data)
        ax_value = sensor_data.get("AX", 0)  

        if ax_value < -2.0:
            print("🥊 Threw a Punch!")

            # Collect 10 samples
            start_time = time.time()
            collected_data = []
            for sample_index in range(10):
                new_data, new_addr = sock.recvfrom(1024)
                new_decoded = new_data.decode().strip()
                print(f"Captured from {new_addr}: {new_decoded}")
                parsed = parse_sensor_data(new_decoded)
                parsed["sample"] = sample_index + 1
                parsed["timestamp_ms"] = round((time.time() - start_time) * 1000, 3)
                collected_data.append(parsed)

            # Save raw data
            df = pd.DataFrame(collected_data)
            # === Predict punch type ===
            features = extract_features(df)
            X_live = pd.DataFrame([features])
            predicted_punch = model.predict(X_live)[0]
            print(f"🧠 Predicted Punch Type: {predicted_punch.upper()} 🥊")

            time.sleep(0.75)

    except Exception as e:
        print("Error parsing data:", e)
