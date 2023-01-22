import sys
from pathlib import Path
from subprocess import Popen, PIPE
from typing import Optional
from unittest import TestCase, main

EXEC: str


def run_with_args(input_text: str = "", max_attempts: Optional[int] = None) -> int:
    process = Popen([EXEC, str(max_attempts)] if max_attempts else EXEC, stdin=PIPE)
    # process.stdin.write((input_text + "\n").encode())
    process.communicate(input=(input_text + "\n").encode(), timeout=1.0)
    # process.wait()
    return process.returncode


class GetYesOrNoTests(TestCase):
    def test_for_yes_variants(self):
        success_cases = ["Y", "Yes", "y", "yes", "y e s", "Y e S", "ynononono"]
        for case in success_cases:
            with self.subTest(msg=case):
                exit_code = run_with_args(input_text=case, max_attempts=1)
                self.assertEqual(exit_code, 0)

    def test_for_no_variants(self):
        success_cases = ["N", "No", "n", "no", "n o", "N o", "nyesyesyes"]
        for case in success_cases:
            with self.subTest(msg=case):
                exit_code = run_with_args(input_text=case, max_attempts=1)
                self.assertEqual(exit_code, 1)


if __name__ == "__main__":
    EXEC = sys.argv[1]
    assert Path(EXEC).is_absolute()
    assert Path(EXEC).exists()

    main(argv=[sys.argv[0]])
