import sys
from pathlib import Path
from subprocess import Popen, PIPE
from typing import Optional
from unittest import TestCase, main

EXEC: str  # Populated by the main from a CL argument.
EXIT_CODE_YES = 0
EXIT_CODE_NO = 1
EXIT_CODE_WRONG = 2


def run_with_args(input_text: str = "", max_attempts: Optional[int] = None) -> int:
    process = Popen([EXEC, str(max_attempts)] if max_attempts else EXEC, stdin=PIPE)
    process.communicate(input=(input_text + "\n").encode(), timeout=.5)
    return process.returncode


class GetYesOrNoTests(TestCase):
    def test_for_yes_variants(self):
        success_cases = ["Y", "Yes", "y", "yes", "y e s", "Y e S", " Y", "ynononono"]
        for case in success_cases:
            with self.subTest(msg=case):
                exit_code = run_with_args(input_text=case, max_attempts=1)
                self.assertEqual(exit_code, EXIT_CODE_YES)

        failure_cases = ["aY", "o", "oui", "ja"]
        for case in failure_cases:
            with self.subTest(msg=case):
                exit_code = run_with_args(input_text=case, max_attempts=1)
                self.assertEqual(exit_code, EXIT_CODE_WRONG)

    def test_for_no_variants(self):
        success_cases = ["N", "No", "n", "no", "n o", "N o", " N", "nyesyesyes"]
        for case in success_cases:
            with self.subTest(msg=case):
                exit_code = run_with_args(input_text=case, max_attempts=1)
                self.assertEqual(exit_code, EXIT_CODE_NO)

        failure_cases = ["aN"]
        for case in failure_cases:
            with self.subTest(msg=case):
                exit_code = run_with_args(input_text=case, max_attempts=1)
                self.assertEqual(exit_code, EXIT_CODE_WRONG)


if __name__ == "__main__":
    EXEC = sys.argv[1]
    assert Path(EXEC).is_absolute()
    assert Path(EXEC).exists()

    main(argv=[sys.argv[0]])
