#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

TARGET="${1:-all}"
if [[ "$TARGET" == "all" ]]; then
    ./build.sh us
    ./build.sh eu
    exit 0
fi

if [[ "$TARGET" != "us" && "$TARGET" != "eu" ]]; then
    echo "Usage: ./build.sh [us|eu|all]" >&2
    exit 2
fi

VER="$TARGET"
CFG="config/$VER/wipeout64.yaml"
LD_SCRIPT="build/$VER/wipeout64.ld"
CONTAINER="wipeout64-cc"
IMAGE="${WIPEOUT64_IMAGE:-wipeoutxl:latest}"

if [[ ! -f "orig/$VER/baserom.z64" ]]; then
    echo "Missing orig/$VER/baserom.z64" >&2
    exit 1
fi

# Split if linker script is missing or config/symbols changed
if [[ ! -f "$LD_SCRIPT" ]] || [[ "$CFG" -nt "$LD_SCRIPT" ]] || [[ "config/$VER/symbol_addrs.txt" -nt "$LD_SCRIPT" ]]; then
    echo "=== Splitting $VER ==="
    python3 -m splat split "$CFG"
fi

# Ensure long-lived binutils container is running
if [[ "$(docker inspect -f '{{.State.Running}}' "$CONTAINER" 2>/dev/null || true)" != "true" ]]; then
    docker rm -f "$CONTAINER" >/dev/null 2>&1 || true
    docker run -d --rm --name "$CONTAINER" -v "$ROOT:/work" -w /work "$IMAGE" sleep infinity >/dev/null
fi

# Compile any C files referenced by the linker script
python3 - "$VER" "$LD_SCRIPT" << 'EOF'
import os, re, subprocess, sys
from pathlib import Path

ver, ld_path = sys.argv[1], Path(sys.argv[2])
text = ld_path.read_text(encoding="utf-8")
c_objs = sorted(set(re.findall(rf"build/{ver}/(src/[^\s(]+\.c)\.o", text)))
for src_rel in c_objs:
    src_p = Path(src_rel)
    out_p = Path(f"build/{ver}/{src_rel}.o")
    if not src_p.is_file():
        raise SystemExit(f"Missing C source referenced by linker script: {src_p}")
    # Always recompile if out is missing or older than src or any header
    need = not out_p.is_file() or src_p.stat().st_mtime > out_p.stat().st_mtime
    if not need:
        out_m = out_p.stat().st_mtime
        for h in Path("include").rglob("*.h"):
            if h.stat().st_mtime > out_m:
                need = True
                break
    if need:
        print(f"CC [{ver}] {src_p}")
        subprocess.run(
            [sys.executable, "tools/cc.py", "--version", ver, str(src_p), str(out_p)],
            check=True,
        )
EOF

# Assemble .s files, wrap .bin files, link, and objcopy inside the container
docker exec -w /work "$CONTAINER" sh -eu -c "
VER='$VER'
AS='mips-linux-gnu-as -EB -march=vr4300 -mtune=vr4300 -mabi=32 -no-pad-sections -G0 -Iinclude'
LD='mips-linux-gnu-ld -EB --no-check-sections -nostdlib'
OBJCOPY='mips-linux-gnu-objcopy'

for f in \$(find asm/\$VER -maxdepth 3 -name '*.s' ! -path 'asm/'\$VER'/nonmatchings/*' ! -path 'asm/'\$VER'/matchings/*' | sort); do
    out=\"build/\$VER/\${f}.o\"
    if [ ! -f \"\$out\" ] || [ \"\$f\" -nt \"\$out\" ]; then
        mkdir -p \"\$(dirname \"\$out\")\"
        \$AS -o \"\$out\" \"\$f\"
    fi
done

for f in \$(find assets/\$VER -name '*.bin' | sort); do
    out=\"build/\$VER/\${f}.o\"
    if [ ! -f \"\$out\" ] || [ \"\$f\" -nt \"\$out\" ]; then
        mkdir -p \"\$(dirname \"\$out\")\"
        \$LD -r -b binary -o \"\$out\" \"\$f\"
    fi
done

\$LD -T config/common_syms.ld \
    -T config/\$VER/syms.ld \
    -T build/\$VER/wipeout64.ld \
    -T build/\$VER/undefined_funcs_auto.txt \
    -T build/\$VER/undefined_syms_auto.txt \
    -Map build/\$VER/wipeout64.map \
    -o build/\$VER/wipeout64.elf

\$OBJCOPY -O binary build/\$VER/wipeout64.elf build/\$VER/wipeout64.z64
"

if cmp -s "orig/$VER/baserom.z64" "build/$VER/wipeout64.z64"; then
    SHA=$(shasum -a 1 "build/$VER/wipeout64.z64" | awk '{print $1}')
    echo "OK $VER: $SHA (byte-identical)"
else
    echo "MISMATCH $VER: build/$VER/wipeout64.z64 differs from orig/$VER/baserom.z64" >&2
    cmp "orig/$VER/baserom.z64" "build/$VER/wipeout64.z64" || true
    exit 1
fi
