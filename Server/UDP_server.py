import socket
import time

UDP_IP = "0.0.0.0"        # Listen on all interfaces
UDP_PORT = 3335

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening on UDP {UDP_PORT}...")

while True:
    data, addr = sock.recvfrom(1024)
    decoded_data = data.decode()
    print(f"Received from {addr}: {decoded_data}")

    # Heuristic: Check if AX < -2g
    try:
        parts = decoded_data.strip().split()
        for part in parts:
            if part.startswith("AX="):
                ax_value = float(part.split("=")[1])
                if ax_value < -2.0:
                    print("🥊 Threw a left jab!")
                    time.sleep(0.75)  # Delay to avoid multiple triggers for the same punch
                break
    except Exception as e:
        print("Error parsing AX value:", e)
