#!/usr/bin/env python3
"""NVC interpreter test runner."""
import glob
import os
import subprocess
import sys
from pathlib import Path

# Determine NVC binary path
repo_root = Path(__file__).resolve().parent.parent.parent
binary_paths = [
    repo_root / "nvc" / "build" / "nvc",
    repo_root / "nvc" / "build" / "nvc.exe",
]
nvc_bin = None
for p in binary_paths:
    if p.exists():
        nvc_bin = p
        break

if nvc_bin is None:
    print("ERROR: nvc binary not found. Build NVC first.")
    sys.exit(1)

print(f"Using NVC binary: {nvc_bin}")

tests_dir = Path(__file__).resolve().parent
failed = 0
passed = 0

for test_file in sorted(tests_dir.glob("*.ns")):
    # Skip files named *_fail.ns (expected failures / negative tests)
    if test_file.name.endswith("_fail.ns"):
        continue

    print(f"\nRunning {test_file.name} ...")
    expected_file = test_file.with_suffix(".expected")
    if not expected_file.exists():
        print(f"  SKIP: no {expected_file.name}")
        continue

    expected = expected_file.read_text(encoding="utf-8").strip()

    try:
        result = subprocess.run(
            [str(nvc_bin), "--run", str(test_file)],
            capture_output=True,
            text=True,
            timeout=30,
        )
    except subprocess.TimeoutExpired:
        print(f"  FAIL: timeout")
        failed += 1
        continue

    output = result.stdout + result.stderr
    if result.returncode != 0:
        print(f"  FAIL: exit code {result.returncode}")
        print(f"  stdout: {result.stdout}")
        print(f"  stderr: {result.stderr}")
        failed += 1
        continue

    # Expected strings must appear in output (one per line in .expected)
    missing = []
    for line in expected.splitlines():
        line = line.strip()
        if not line:
            continue
        if line not in output:
            missing.append(line)

    if missing:
        print(f"  FAIL: missing expected output: {missing}")
        print(f"  actual output: {output}")
        failed += 1
    else:
        print(f"  PASS")
        passed += 1

print(f"\n{passed} passed, {failed} failed")
sys.exit(1 if failed else 0)
