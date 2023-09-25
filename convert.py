import sys
import os
import subprocess

ui = sys.argv[1]
target = sys.argv[2]

# Use subprocess to run the command on Ubuntu
command = f'python -m PyQt5.uic.pyuic -x {ui}.ui -o {target}.py'
result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

if result.returncode == 0:
    print("Conversion successful.")
else:
    print("Error during conversion:")
    print(result.stderr)
