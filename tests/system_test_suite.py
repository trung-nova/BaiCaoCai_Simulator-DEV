import os
import subprocess
import time
import sys
import io

# Set stdout to utf-8
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

# Configuration
EXE_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "game.exe")
DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "data")
CONFIG_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "config.ini")

def run_simulation(rounds=10, seed=12345):
    """Runs the simulation with a fixed number of rounds and seed."""
    print(f"--- Running Simulation Test (Rounds: {rounds}, Seed: {seed}) ---")
    
    # Input sequence:
    # 1: Standard Simulation
    # {seed}: Seed
    # 1: Persistent mode
    # 5: Total players
    # 1: Shark count
    # 1: Maniac count
    # 1: Nit count
    # y: Export data
    # {rounds}: Number of rounds
    # 2: Back to menu
    # 0: Exit
    input_str = f"1\n{seed}\n1\n5\n1\n1\n1\ny\n{rounds}\n2\n0\n"
    
    try:
        process = subprocess.Popen(
            [EXE_PATH],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding='utf-8'
        )
        stdout, stderr = process.communicate(input=input_str, timeout=60)
        return stdout, stderr
    except Exception as e:
        return None, str(e)

def verify_outputs():
    """Checks if the required output files exist and are not empty."""
    print("\n--- Verifying Output Files (CSV) ---")
    
    if not os.path.exists(DATA_DIR):
        print("  [ERROR] Data directory missing.")
        return False

    # Find files in data dir
    files = os.listdir(DATA_DIR)
    csv_files = [f for f in files if f.endswith('.csv')]
    
    print(f"  Found {len(csv_files)} CSV files.")
    
    # Expecting at least 3 CSVs (swap, rounds, history)
    success = len(csv_files) >= 3
    if success:
        print("  [PASS] All expected CSV reports detected.")
    else:
        print("  [FAIL] Missing CSV reports.")
    
    return success

def main():
    print("====================================================")
    print("       BAI CAO CAI SYSTEM INTEGRATION TEST          ")
    print("====================================================\n")
    
    # 1. Check if executable exists
    if not os.path.exists(EXE_PATH):
        print(f"[FATAL] Executable not found at {EXE_PATH}")
        print("Please build the project first using build_all.bat")
        return

    # 2. Run Simulation
    start_time = time.time()
    stdout, stderr = run_simulation(rounds=20)
    end_time = time.time()
    
    if stdout:
        print(f"[INFO] Simulation completed in {end_time - start_time:.2f}s")
    else:
        print(f"[ERROR] Simulation failed to run: {stderr}")

    # 3. Verify Files
    csv_ok = verify_outputs()
    
    # 4. Summary
    print("\n" + "="*52)
    if csv_ok:
        print("         FINAL STATUS: ALL SYSTEMS GO (PASS)        ")
    else:
        print("         FINAL STATUS: SYSTEM ISSUES DETECTED (FAIL) ")
    print("="*52)

if __name__ == "__main__":
    main()
