
#!/usr/bin/env bash

set -euo pipefail

for command in \
    'help' \
    'fcfs' \
    'rr 2' \
    'priority' \
    'compare' \
    'logger' \
    'ipc' \
    'banker-safe' \
    'banker-unsafe' \
    'worker-ok' \
    'worker-fail' \
    'limit' \
    'demo'
do
    echo "==> ./campuscompute $command"

    # Intentional word splitting for command plus optional argument.
    ./campuscompute $command >/dev/null

    echo 'PASS'
done

if ./campuscompute not-a-command >/dev/null 2>&1
then
    echo 'FAIL: invalid command accepted'
    exit 1
fi

if ./campuscompute rr 0 >/dev/null 2>&1
then
    echo 'FAIL: invalid quantum accepted'
    exit 1
fi

echo 'All smoke tests passed.'
