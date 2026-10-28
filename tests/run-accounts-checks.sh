#!/bin/bash
set -euo pipefail
root="$(cd -- "$(dirname -- "$0")/.." && pwd)"
scratch="$(mktemp -d "${TMPDIR:-/tmp}/rouge-accounts-tests.XXXXXX")"
trap 'rm -rf -- "$scratch"' EXIT
clang -std=gnu11 -g -fsanitize=address,undefined "$root/tests/accounts_checks.c" "$root/accounts.c" -lsqlite3 -o "$scratch/checks"
cd "$scratch"
./checks
