#!/usr/bin/env python3
"""
Generate C++ shoot calibration coefficients from a CSV file.

The script reads a CSV file containing, per robot, two calibration points:
(raw_1, calibrated_1) and (raw_2, calibrated_2), fits a line

    calibrated = a * raw + b

and prints a C++ array in this form:

    static Common::Vec2 shoot_coeffs[Common::Config::Common::kMaxRobots] = {
        {a0f, b0f}, // 0
        ...
    };

Empty or incomplete rows are emitted as:
    {1.0f, 0.0f}

Expected CSV row format:
    robot_id, calibrated_1, raw_1, calibrated_2, raw_2
"""

from __future__ import annotations

import argparse
import csv
import math
import sys
from pathlib import Path


DEFAULT_MAX_ROBOTS = 16
DEFAULT_FALLBACK_A = 1.0
DEFAULT_FALLBACK_B = 0.0


def positive_int(value: str) -> int:
    try:
        parsed = int(value)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(f"invalid integer value: {value!r}") from exc
    if parsed <= 0:
        raise argparse.ArgumentTypeError("value must be a positive integer")
    return parsed


def non_negative_int(value: str) -> int:
    try:
        parsed = int(value)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(f"invalid integer value: {value!r}") from exc
    if parsed < 0:
        raise argparse.ArgumentTypeError("value must be a non-negative integer")
    return parsed


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="gen_shoot_coefs.py",
        description="Read a calibration CSV and generate a C++ static Common::Vec2 shoot_coeffs[...] array.",
        epilog="""
Expected CSV layout per data row:
  robot_id, calibrated_1, raw_1, calibrated_2, raw_2

Notes:
  - The first row may be a header.
  - Empty or incomplete rows become {1.0f, 0.0f}.
  - Robot IDs outside the allowed range are rejected.
  - If raw_1 == raw_2 for a populated row, the script fails.

Examples:
  gen_shoot_coefs.py shoots.csv
  gen_shoot_coefs.py shoots.csv --max-robots 16
  gen_shoot_coefs.py shoots.csv -o shoot_coeffs.txt
  gen_shoot_coefs.py shoots.csv --symbol my_coeffs
""",
        formatter_class=argparse.RawTextHelpFormatter,
    )

    parser.add_argument(
        "csv_file",
        type=Path,
        help="Path to the input CSV file.",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        help="Write the generated C++ output to this file instead of stdout.",
    )
    parser.add_argument(
        "--max-robots",
        type=positive_int,
        default=DEFAULT_MAX_ROBOTS,
        help=f"Total number of robots in the generated array (default: {DEFAULT_MAX_ROBOTS}).",
    )
    parser.add_argument(
        "--symbol",
        default="shoot_coeffs",
        help='C++ variable name to generate (default: "shoot_coeffs").',
    )
    parser.add_argument(
        "--array-size",
        default="Common::Config::Common::kMaxRobots",
        help='Text placed inside the C++ array brackets (default: "Common::Config::Common::kMaxRobots").',
    )
    parser.add_argument(
        "--fallback-a",
        type=float,
        default=DEFAULT_FALLBACK_A,
        help="Fallback slope for empty/incomplete rows (default: 1.0).",
    )
    parser.add_argument(
        "--fallback-b",
        type=float,
        default=DEFAULT_FALLBACK_B,
        help="Fallback intercept for empty/incomplete rows (default: 0.0).",
    )
    parser.add_argument(
        "--indent",
        type=non_negative_int,
        default=4,
        help="Number of spaces used to indent array entries (default: 4).",
    )
    parser.add_argument(
        "--precision",
        type=positive_int,
        default=6,
        help="Maximum decimal precision for generated float literals (default: 6).",
    )
    parser.add_argument(
        "--no-header",
        action="store_true",
        help="Treat the first CSV row as data instead of as a possible header.",
    )

    return parser.parse_args()


def parse_float(cell):
    if cell is None:
        return None
    stripped = cell.strip()
    if stripped == "":
        return None
    return float(stripped)


def is_probable_header(row) -> bool:
    if not row:
        return False

    first = row[0].strip().lower()
    if first in {"robot", "robot_id", "id", "robot id"}:
        return True

    try:
        int(row[0].strip())
        return False
    except ValueError:
        return True


def compute_line(raw1: float, cal1: float, raw2: float, cal2: float):
    if math.isclose(raw1, raw2, rel_tol=0.0, abs_tol=1e-12):
        raise ValueError(
            f"cannot compute coefficients because raw values are identical: {raw1} and {raw2}"
        )
    a = (cal2 - cal1) / (raw2 - raw1)
    b = cal1 - a * raw1
    return a, b


def format_cpp_float(value: float, precision: int) -> str:
    if math.isclose(value, 0.0, rel_tol=0.0, abs_tol=1e-12):
        value = 0.0

    text = f"{value:.{precision}f}".rstrip("0").rstrip(".")
    if "." not in text:
        text += ".0"
    return f"{text}f"


def load_coeffs(csv_path: Path, max_robots: int, fallback, no_header: bool):
    coeffs = [fallback for _ in range(max_robots)]

    with csv_path.open("r", newline="", encoding="utf-8-sig") as handle:
        reader = csv.reader(handle)
        rows = list(reader)

    if not rows:
        return coeffs

    start_idx = 0
    if not no_header and is_probable_header(rows[0]):
        start_idx = 1

    for line_number, row in enumerate(rows[start_idx:], start=start_idx + 1):
        if not row or all(cell.strip() == "" for cell in row):
            continue

        if row[0].strip() == "":
            raise ValueError(f"line {line_number}: missing robot_id")

        try:
            robot_id = int(row[0].strip())
        except ValueError as exc:
            raise ValueError(f"line {line_number}: invalid robot_id {row[0]!r}") from exc

        if not (0 <= robot_id < max_robots):
            raise ValueError(
                f"line {line_number}: robot_id {robot_id} is out of range [0, {max_robots - 1}]"
            )

        cal1 = parse_float(row[1]) if len(row) > 1 else None
        raw1 = parse_float(row[2]) if len(row) > 2 else None
        cal2 = parse_float(row[3]) if len(row) > 3 else None
        raw2 = parse_float(row[4]) if len(row) > 4 else None

        if None in (cal1, raw1, cal2, raw2):
            coeffs[robot_id] = fallback
            continue

        a, b = compute_line(raw1, cal1, raw2, cal2)
        coeffs[robot_id] = (a, b)

    return coeffs


def generate_cpp(coeffs, symbol: str, array_size: str, indent: int, precision: int) -> str:
    pad = " " * indent
    lines = [f"static Common::Vec2 {symbol}[{array_size}] = {{"]

    for robot_id, (a, b) in enumerate(coeffs):
        a_str = format_cpp_float(a, precision)
        b_str = format_cpp_float(b, precision)
        lines.append(f"{pad}{{{a_str}, {b_str}}}, // {robot_id}")

    lines.append("};")
    return "\n".join(lines)


def main() -> int:
    args = parse_args()

    if not args.csv_file.exists():
        print(f"error: input file does not exist: {args.csv_file}", file=sys.stderr)
        return 1

    if not args.csv_file.is_file():
        print(f"error: input path is not a file: {args.csv_file}", file=sys.stderr)
        return 1

    fallback = (args.fallback_a, args.fallback_b)

    try:
        coeffs = load_coeffs(
            csv_path=args.csv_file,
            max_robots=args.max_robots,
            fallback=fallback,
            no_header=args.no_header,
        )
        cpp_text = generate_cpp(
            coeffs=coeffs,
            symbol=args.symbol,
            array_size=args.array_size,
            indent=args.indent,
            precision=args.precision,
        )
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    if args.output is not None:
        try:
            args.output.write_text(cpp_text + "\n", encoding="utf-8")
        except Exception as exc:
            print(f"error: failed to write output file {args.output}: {exc}", file=sys.stderr)
            return 1
    else:
        print(cpp_text)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())