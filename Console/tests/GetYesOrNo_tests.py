import subprocess
import sys
from typing import Optional

EXEC: str = sys.argv[1]


def run_with_args(input_text: str = "", max_attempts: Optional[int] = None) -> int:
    process = subprocess.Popen(f"{EXEC} {max_attempts}" if max_attempts else EXEC, stdin=subprocess.PIPE)
    process.stdin.write((input_text + "\n").encode())
    process.wait()
    return process.returncode


if __name__ == "__main__":
    assert run_with_args(input_text="Y") == 0
    assert run_with_args(input_text="Yes") == 0
    assert run_with_args(input_text="y") == 0
    assert run_with_args(input_text="yes") == 0
    assert run_with_args(input_text="y e s") == 0
    assert run_with_args(input_text="ynonononono") == 0
