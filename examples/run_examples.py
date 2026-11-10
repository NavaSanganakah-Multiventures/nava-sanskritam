#!/usr/bin/env python3
"""Runs every example program in this folder and reports any that fail.

Examples in ``legacy/`` are skipped: they use pre-v18 syntax the current parser
does not accept, and they are kept for reference only.
"""
import subprocess
import sys
from pathlib import Path

repo_root = Path(__file__).resolve().parent.parent
candidates = [
    repo_root / "nvc" / "build" / "nvc",
    repo_root / "nvc" / "build" / "nvc.exe",
]
nvc_bin = next((p for p in candidates if p.exists()), None)
if nvc_bin is None:
    print("ERROR: nvc binary not found. Build NVC first.")
    sys.exit(1)

print(f"Using NVC binary: {nvc_bin}")

examples = sorted((Path(__file__).resolve().parent).glob("*.ns"))
if not examples:
    print("ERROR: no examples found.")
    sys.exit(1)

failed = []
for program in examples:
    result = subprocess.run(
        [str(nvc_bin), "--run", str(program)],
        capture_output=True,
        text=True,
        timeout=30,
    )
    if result.returncode == 0:
        print(f"  PASS  {program.name}")
    else:
        print(f"  FAIL  {program.name} (exit {result.returncode})")
        detail = (result.stderr or result.stdout).strip().splitlines()
        if detail:
            print(f"        {detail[0]}")
        failed.append(program.name)

print(f"\n{len(examples) - len(failed)} passed, {len(failed)} failed")
sys.exit(1 if failed else 0)
