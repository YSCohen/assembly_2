#!/usr/bin/env python3
from pathlib import Path
import re
import sys

SOURCE = Path(__file__).resolve().parents[1] / "hw1011.s"
text = SOURCE.read_text(encoding="utf-8")

ALLOWED_INSTRUCTIONS = {
    "movq", "movl", "movslq",
    "addq", "subq", "imulq", "cmpq",
    "jmp", "je", "jne", "jl", "jle", "jg", "jge",
    "jb", "jbe", "ja", "jae",
    "pushq", "call", "leave", "ret",
}

FUNCTIONS = [
    "sum_positive_ints", "reverse_longs", "sensor_score",
    "sensor_array_total", "make_arithmetic_sequence",
    "make_sensor", "sensor_destroy",
]

ALLOWED_CALLS = {"sensor_score", "malloc", "free"}
errors = []
found_labels = set()

for line_number, original_line in enumerate(text.splitlines(), start=1):
    line = original_line.split("#", 1)[0].strip().lower()
    if not line:
        continue
    if line.startswith("."):
        continue
    if line.endswith(":"):
        found_labels.add(line[:-1].strip())
        continue

    instruction = line.split(None, 1)[0]
    if instruction not in ALLOWED_INSTRUCTIONS:
        errors.append(
            f"line {line_number}: instruction '{instruction}' is not in the lecture whitelist"
        )

    if instruction == "call":
        parts = line.split()
        target = parts[1] if len(parts) >= 2 else "<missing>"
        if target not in ALLOWED_CALLS:
            errors.append(f"line {line_number}: unauthorized call target '{target}'")

for label in FUNCTIONS:
    if label not in found_labels:
        errors.append(f"missing function label: {label}")

clean = re.sub(r"#.*", "", text.lower())
if re.search(r"\([^\n)]*,[^\n)]*\)", clean):
    errors.append("scaled/indexed addressing is not allowed")
if re.search(r"(?m)^\s*\.(?:data|bss|comm)\b", clean):
    errors.append("writable global storage is not allowed")

positions = []
for label in FUNCTIONS:
    match = re.search(rf"(?m)^\s*{re.escape(label)}\s*:\s*$", clean)
    if match:
        positions.append((match.start(), match.end(), label))
positions.sort()
bodies = {}
for index, (start, body_start, label) in enumerate(positions):
    body_end = positions[index + 1][0] if index + 1 < len(positions) else len(clean)
    bodies[label] = clean[body_start:body_end]

required_call_counts = {
    "sensor_array_total": {"sensor_score": 1},
    "make_arithmetic_sequence": {"malloc": 1},
    "make_sensor": {"malloc": 1},
    "sensor_destroy": {"free": 1},
}
for function_name, call_counts in required_call_counts.items():
    body = bodies.get(function_name, "")
    for target, expected_count in call_counts.items():
        actual_count = len(re.findall(rf"\bcall\s+{re.escape(target)}\b", body))
        if actual_count != expected_count:
            errors.append(
                f"{function_name} must contain exactly {expected_count} {target} call site"
            )

# Functions without a required call are not allowed to call anything.
for function_name, body in bodies.items():
    calls = re.findall(r"\bcall\s+([.$\w]+)", body)
    permitted_here = set(required_call_counts.get(function_name, {}))
    for target in calls:
        if target not in permitted_here:
            errors.append(f"{function_name} may not call {target}")

if errors:
    print("source-rule check failed:")
    for error in errors:
        print(f"  - {error}")
    sys.exit(1)

print("source-rule check passed")
