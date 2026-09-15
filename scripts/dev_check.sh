#!/usr/bin/env bash
#Artemis Autonomous Flight - Toolchain & Subsystem Verfictaion

echo "=== ARTEMIS SUBSYSTEM ENVIRONMENT AUDIT ==="

check_tool() {
    local tool_name=$1
    local command_check=$2

    if command -v "$command_check" >/dev/null 2>&1; then 
        echo "[ONLINE] $tool_name : $($command_check --version 2>&1 | head -n 1)"
    else
       echo "[offline] $tool_name: NOT DETECTED"
    fi
}

# Silicon & Systems Compilers
check_tool "C++ (GCC)" "g++"
check_tool "Rust (Cargo)" "cargo"

# Cloud & AI Runtimes
check_tool "Go Runtime" "go"
check_tool "Python Core" "python"

# Cockpit & Node Frontend
check_tool "Node Engine" "node"
check_tool "Typescript Compiler" "tsc"

echo "========================================="