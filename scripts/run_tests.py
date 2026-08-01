#!/usr/bin/env python3
"""Run the compiler's valid and invalid regression suites.

The suite validates exit status and phase-specific output markers. It can also
refresh deterministic expected-output snapshots used during grading.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
COMPILER = ROOT / "compiler"


@dataclass
class TestResult:
    path: Path
    expected_success: bool
    returncode: int
    stdout: str
    stderr: str
    passed: bool
    reason: str


def run_case(path: Path, expected_success: bool) -> TestResult:
    completed = subprocess.run(
        [str(COMPILER), str(path)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )

    if expected_success:
        passed = (
            completed.returncode == 0
            and "Abstract Syntax Tree" in completed.stdout
            and "Symbol Table" in completed.stdout
            and "Three Address Code" in completed.stdout
            and "Compilation succeeded" in completed.stdout
        )
        reason = "ok" if passed else "valid program did not complete the full pipeline"
    else:
        passed = completed.returncode != 0 and "Compilation failed" in completed.stderr
        reason = "ok" if passed else "invalid program was not rejected with a diagnostic"

    return TestResult(
        path=path,
        expected_success=expected_success,
        returncode=completed.returncode,
        stdout=completed.stdout,
        stderr=completed.stderr,
        passed=passed,
        reason=reason,
    )


def snapshot_text(result: TestResult) -> str:
    return (
        f"SOURCE: {result.path.relative_to(ROOT).as_posix()}\n"
        f"EXPECTED_EXIT: {'0' if result.expected_success else 'non-zero'}\n"
        f"ACTUAL_EXIT: {result.returncode}\n\n"
        "===== STDOUT =====\n"
        f"{result.stdout.rstrip()}\n\n"
        "===== STDERR =====\n"
        f"{result.stderr.rstrip()}\n"
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--update-expected",
        action="store_true",
        help="rewrite tests/expected snapshots from the current verified compiler",
    )
    args = parser.parse_args()

    if not COMPILER.exists():
        print("error: compiler binary not found; run 'make' first", file=sys.stderr)
        return 2

    cases: list[tuple[Path, bool]] = []
    cases.extend((path, True) for path in sorted((ROOT / "tests/valid").glob("*.md")))
    cases.extend((path, False) for path in sorted((ROOT / "tests/invalid").glob("*.md")))

    results = [run_case(path, expected) for path, expected in cases]
    print(f"{'RESULT':<8} {'EXIT':<5} TEST")
    print("-" * 78)
    for result in results:
        label = "PASS" if result.passed else "FAIL"
        print(f"{label:<8} {result.returncode:<5} {result.path.relative_to(ROOT)}")
        if not result.passed:
            print(f"         reason: {result.reason}")

        if args.update_expected and result.passed:
            category = "valid" if result.expected_success else "invalid"
            target = ROOT / "tests/expected" / category / f"{result.path.stem}.txt"
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_text(snapshot_text(result), encoding="utf-8")

    passed_count = sum(result.passed for result in results)
    print("-" * 78)
    print(f"Summary: {passed_count}/{len(results)} tests passed")
    return 0 if passed_count == len(results) else 1


if __name__ == "__main__":
    raise SystemExit(main())
