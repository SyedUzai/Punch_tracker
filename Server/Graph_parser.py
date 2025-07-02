import os
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

# === CONFIGURATION ===
folder_path = r"C:\Users\Uzair\OneDrive\Documents\UDP_Server\Left_Hook_Data"  # <-- Update if needed
output_pdf_path = os.path.join(folder_path, "all_punch_plots.pdf")

# === Create a multipage PDF ===
with PdfPages(output_pdf_path) as pdf:
    for filename in os.listdir(folder_path):
        if filename.endswith(".xlsx"):
            file_path = os.path.join(folder_path, filename)
            print(f"📂 Processing: {filename}")

            try:
                df = pd.read_excel(file_path)

                # Check for required columns
                if {"timestamp_ms", "AX", "AY", "AZ"}.issubset(df.columns):
                    plt.figure(figsize=(10, 6))
                    plt.plot(df["timestamp_ms"], df["AX"], label='AX', marker='o')
                    plt.plot(df["timestamp_ms"], df["AY"], label='AY', marker='^')
                    plt.plot(df["timestamp_ms"], df["AZ"], label='AZ', marker='s')

                    plt.title(f"Accelerometer Data: {filename}")
                    plt.xlabel("Time (ms)")
                    plt.ylabel("Acceleration (g)")
                    plt.grid(True)
                    plt.legend()
                    plt.tight_layout()

                    pdf.savefig()  # Save current figure to PDF
                    plt.close()
                    print(f"✅ Added {filename} to PDF.")
                else:
                    print(f"⚠️ Skipped {filename}: missing AX/AY/AZ or timestamp_ms.")

            except Exception as e:
                print(f"❌ Error processing {filename}: {e}")

print(f"\n📄 Multi-page PDF saved to: {output_pdf_path}")
