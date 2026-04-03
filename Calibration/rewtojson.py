import json

def get_filters_from_file(file_path) -> list:
    filters = []

    with open(file_path, "r") as f:
        lines = f.readlines()
        for line in lines[1:]:
            line = line.strip()
            if not line.startswith("Filter"):
                continue

            # E.g.
            # Filter  1: ON  PK       Fc   125.0 Hz  Gain -18.90 dB  Q  1.000

            parts = line.split()
            filter_num = int(parts[1].replace(":", ""))

            filter_type = parts[3]
            if filter_type == "None":
                continue

            if filter_type == "PK":
                fc = float(parts[parts.index("Fc") + 1])
                gain = float(parts[parts.index("Gain") + 1])
                q = float(parts[parts.index("Q") + 1])

            if filter_type == "HS" or filter_type == "LS" or filter_type == "HC" or filter_type == "LC":
                fc = float(parts[parts.index("Fc") + 1])
                gain = float(parts[parts.index("Gain") + 1])
                q = 0.707 # Assume as REW doesn't export

            filter = {
                "number": filter_num,
                "type": filter_type,
                "fc": fc,
                "gain": gain,
                "q": q
            }

            if q != -1:
                filter["q"] = q

            filters.append(filter)

    return filters

def get_metadata_from_file(file_path) -> dict:
    metadata = {}
    with open(settings_file_left, "r") as f:
        for line in f:
            line = line.strip()
            if line.startswith("Room EQ"):
                metadata["software"] = line;

            if line.startswith("Dated:"):
                metadata["date"] = line.replace("Dated:", "").strip()

    return metadata

# Set paths to filter settings files
settings_file_left = "filter_settings_left.txt"
settings_file_right = "filter_settings_right.txt"
output_file = "calibration_seinnheiser_hd280.json"

left_filters = get_filters_from_file(settings_file_left)
right_filters = get_filters_from_file(settings_file_right)

data = {}

# Manually enter metadata not present in the filter settings file
data["calibration_id"] = input("Enter calibration ID: ")
data["device_name"] = input("Enter device name: ")
data["target_spl"] = input("Enter target SPL: ")

# Extract date & software fields from the left file
data.update(get_metadata_from_file(settings_file_left))

data["left_filters"] = left_filters
data["right_filters"] = right_filters

with open(output_file, "w") as f:
    json.dump(data, f, indent=4)

print(f"Calibration data saved to {output_file}")

