import os
import subprocess
import time
import sqlite3
import sys
import io

# Set stdout to utf-8
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

# Configuration
EXE_PATH = r"d:\Tài liệu học tập\OOP\game_sql.exe"
DATA_DIR = r"d:\Tài liệu học tập\OOP\data"
CONFIG_PATH = r"d:\Tài liệu học tập\OOP\config.ini"

def run_simulation(rounds=10, seed=12345):
    """Runs the simulation with a fixed number of rounds and seed."""
    print(f"--- Running Simulation Test (Rounds: {rounds}, Seed: {seed}) ---")
    
    # In a real scenario, we might want to pass these as CLI args if the exe supports it.
    # For now, we assume it reads from config.ini or takes inputs via stdin.
    # If the program is interactive, we use stdin.
    
    # Simulation input sequence for Mode 1 (Standard Simulation)
    # 1 (Choice 1)
    # {seed} (Seed)
    # 1 (Submode: Persistent)
    # 5 (nP)
    # 0.2 (minS)
    # 0.8 (maxS)
    # 50 (conc)
    # 30 (sP)
    # 30 (mP)
    # y (Export)
    # {rounds} (numR)
    # 2 (Menu)
    # 0 (Exit)
    input_str = f"1\n{seed}\n1\n5\n0.2\n0.8\n50\n30\n30\ny\n{rounds}\n2\n0\n"
    
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
    print("\n--- Verifying Output Files ---")
    
    # Find the latest files in data dir
    files = os.listdir(DATA_DIR)
    csv_files = [f for f in files if f.endswith('.csv')]
    db_files = [f for f in files if f.endswith('.db')]
    log_files = [f for f in files if f.endswith('.log')]
    
    results = {
        "csv_count": len(csv_files),
        "db_exists": "simulation_results.db" in files or len(db_files) > 0,
        "logs_found": len(log_files) > 0
    }
    
    for k, v in results.items():
        status = "PASSED" if v else "FAILED"
        print(f"  {k}: {v} [{status}]")
    
    return results

def verify_database():
    """Connects to SQLite and checks for data consistency."""
    print("\n--- Verifying SQLite Database Integrity ---")
    db_path = os.path.join(DATA_DIR, "simulation_results.db")
    if not os.path.exists(db_path):
        # Check if there's any other .db file (sometimes timestamped)
        db_files = [f for f in os.listdir(DATA_DIR) if f.endswith('.db')]
        if db_files:
            db_path = os.path.join(DATA_DIR, db_files[0])
        else:
            print("  [ERROR] Database file not found!")
            return False

    try:
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()
        
        # Check rounds table
        cursor.execute("SELECT COUNT(*) FROM rounds")
        round_count = cursor.fetchone()[0]
        print(f"  Total Rounds in DB: {round_count}")
        
        # Check swaps table
        cursor.execute("SELECT COUNT(*) FROM swaps")
        swap_count = cursor.fetchone()[0]
        print(f"  Total Swap Decisions in DB: {swap_count}")
        
        # Check for Ba Tien logic in data
        cursor.execute("SELECT COUNT(*) FROM swaps WHERE score_after = 10")
        ba_tien_count = cursor.fetchone()[0]
        print(f"  Ba Tien completions detected: {ba_tien_count}")
        
        conn.close()
        return round_count > 0
    except Exception as e:
        print(f"  [ERROR] SQLite Verification failed: {e}")
        return False

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
        if "ALL SIMULATION DATA SAVED" in stdout or "Report generated" in stdout:
            print("  Status: Program reported success.")
        else:
            print("  Warning: Success message not detected in stdout.")
    else:
        print(f"[ERROR] Simulation failed to run: {stderr}")

    # 3. Verify Files
    file_results = verify_outputs()
    
    # 4. Verify DB
    db_ok = verify_database()
    
    # 5. Summary
    print("\n" + "="*52)
    if file_results["db_exists"] and db_ok:
        print("         FINAL STATUS: ALL SYSTEMS GO (PASS)        ")
    else:
        print("         FINAL STATUS: SYSTEM ISSUES DETECTED (FAIL) ")
    print("="*52)

if __name__ == "__main__":
    main()
