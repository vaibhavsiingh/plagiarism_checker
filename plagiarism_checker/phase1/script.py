import os
import shutil
import subprocess

# Define the source and destination files
source_files = [f'checker_{i}.hpp' for i in range(1, 6)]
destination_file = 'match_submission.hpp'

# Check if source files exist
for source_file in source_files:
    if not os.path.exists(source_file):
        raise FileNotFoundError(f"Source file {source_file} not found")

# Copy the contents of each source file into the destination file
with open(destination_file, 'w') as dest:
    for source_file in source_files:
        with open(source_file, 'r') as src:
            shutil.copyfileobj(src, dest)
            dest.write('\n')  # Add a newline between files for clarity

# Run make command and capture the output
make_results = []
for i in range(1, 6):
    result = subprocess.run(['make'], capture_output=True, text=True)
    make_results.append((f'checker_{i}', result.stdout, result.stderr, result.returncode))

# Print the results of the make commands
for checker, stdout, stderr, returncode in make_results:
    print(f'Results for {checker}:')
    print('STDOUT:')
    print(stdout)
    print('STDERR:')
    print(stderr)
    if returncode != 0:
        print(f"Error: Make command for {checker} failed with return code {returncode}")
    print('-------------------------')
