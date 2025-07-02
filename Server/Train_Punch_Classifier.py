import os
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report
from joblib import dump

# === CONFIGURATION ===
folder_path = r"C:\Users\Uzair\OneDrive\Documents\UDP_Server\Raw_Data"
model_output_path = os.path.join(folder_path, "punch_classifier.pkl")

# === Helper: Extract features from a DataFrame ===
def extract_features(df):
    features = {}
    for axis in ['AX', 'AY', 'AZ']:
        features[f'{axis}_mean'] = df[axis].mean()
        features[f'{axis}_std'] = df[axis].std()
        features[f'{axis}_max'] = df[axis].max()
        features[f'{axis}_min'] = df[axis].min()
        features[f'{axis}_range'] = df[axis].max() - df[axis].min()
    return features

# === Collect features and labels from files ===
data = []
labels = []

for filename in os.listdir(folder_path):
    if filename.endswith(".xlsx"):
        label = filename.split('_')[0].lower()  # e.g., "jab", "hook", "uppercut"
        file_path = os.path.join(folder_path, filename)
        try:
            df = pd.read_excel(file_path)
            if {"AX", "AY", "AZ"}.issubset(df.columns) and len(df) >= 10:
                features = extract_features(df)
                data.append(features)
                labels.append(label)
        except Exception as e:
            print(f"⚠️ Error reading {filename}: {e}")

# === Train the model ===
df_features = pd.DataFrame(data)
X = df_features
y = labels

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, stratify=y, random_state=42)

model = RandomForestClassifier(n_estimators=100, random_state=42)
model.fit(X_train, y_train)

# === Evaluate model ===
y_pred = model.predict(X_test)
print("📊 Classification Report:\n", classification_report(y_test, y_pred))

# === Save model ===
dump(model, model_output_path)
print(f"✅ Model saved to: {model_output_path}")
