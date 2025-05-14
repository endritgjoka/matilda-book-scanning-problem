import os
import shutil # Not strictly needed for rename, but os.rename is fine

def remove_prefix_from_filenames(directory_path, prefix_to_remove):
    """
    Renames files in the specified directory by removing a given prefix.

    Args:
        directory_path (str): The path to the directory containing the files.
        prefix_to_remove (str): The prefix string to remove from filenames.
    """
    if not os.path.isdir(directory_path):
        print(f"Error: Directory '{directory_path}' not found.")
        return

    renamed_count = 0
    skipped_count = 0
    prefix_len = len(prefix_to_remove)

    print(f"Scanning directory: '{directory_path}' for files starting with '{prefix_to_remove}'...")

    for filename in os.listdir(directory_path):
        old_filepath = os.path.join(directory_path, filename)

        # Ensure it's a file and starts with the prefix
        if os.path.isfile(old_filepath) and filename.startswith(prefix_to_remove):
            # Create the new filename by stripping the prefix
            new_filename = filename[prefix_len:]

            # Ensure the new filename is not empty (e.g., if the original was just "sol_")
            if not new_filename:
                print(f"Skipping '{filename}': removing prefix results in an empty name.")
                skipped_count += 1
                continue

            new_filepath = os.path.join(directory_path, new_filename)

            # Safety check: if a file with the new name already exists, skip to avoid overwrite
            # This is unlikely if you're just removing a prefix, but good practice.
            if os.path.exists(new_filepath):
                print(f"Skipping '{filename}': target file '{new_filename}' already exists.")
                skipped_count += 1
                continue

            try:
                os.rename(old_filepath, new_filepath)
                print(f"Renamed: '{filename}' -> '{new_filename}'")
                renamed_count += 1
            except OSError as e:
                print(f"Error renaming '{filename}': {e}")
                skipped_count += 1
        elif os.path.isfile(old_filepath):
            # It's a file but doesn't have the prefix
            # print(f"Skipping '{filename}': does not start with prefix.") # Optional: for verbosity
            pass


    print(f"\nFinished processing.")
    print(f"Successfully renamed: {renamed_count} file(s).")
    print(f"Skipped or errored: {skipped_count} file(s).")

if __name__ == "__main__":
    # Define the target directory and the prefix to remove
    target_directory = "output-ghc"
    prefix = "sol_"

    # Run the renaming function
    remove_prefix_from_filenames(target_directory, prefix)

    # Example of how the directory structure might look:
    # .
    # |-- your_cpp_executable
    # |-- input/
    # |   |-- a_example.txt
    # |   |-- b_read_on.txt
    # |-- output-optimizational-problem/  <-- Script will operate here
    # |   |-- sol_a_example.txt
    # |   |-- sol_b_read_on.txt
    # |-- rename_script.py  <-- This Python script