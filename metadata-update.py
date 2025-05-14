#!/usr/bin/env python3
"""
Update metadata.csv by computing 'algo_ils' scores from input/output files.
- Matches input .txt files from the input_dir with output files in output_dir
  (either with the same base name or base + '.out').
- Adds a new column 'algo_ils' to existing metadata.csv for each matched instance.
"""

import os
import csv
import argparse


def read_input_file(input_path):
    with open(input_path, 'r') as file:
        lines = file.readlines()
    B, L, D = map(int, lines[0].strip().split())
    book_scores = list(map(int, lines[1].strip().split()))
    libraries = []
    idx = 2
    while idx < len(lines) - 1:
        N, T, M = map(int, lines[idx].strip().split())
        books = list(map(int, lines[idx + 1].strip().split()))
        libraries.append((N, T, M, books))
        idx += 2
    return B, L, D, book_scores, libraries


def read_output_file(output_path):
    with open(output_path, 'r') as file:
        lines = [line.strip() for line in file]
    if not lines:
        return 0, []
    num_libraries = int(lines[0])
    solution = []
    idx = 1
    for _ in range(num_libraries):
        if idx >= len(lines):
            break
        lib_id, num_books = map(int, lines[idx].split())
        idx += 1
        if idx >= len(lines):
            break
        books = list(map(int, lines[idx].split()))
        idx += 1
        solution.append((lib_id, num_books, books))
    return num_libraries, solution


def compute_total_score(input_path, output_path):
    B, L, D, book_scores, libraries = read_input_file(input_path)
    _, solution = read_output_file(output_path)

    all_books = [b for _, _, books in solution for b in books]
    unique_books = set(all_books)
    total_score = sum(book_scores[b] for b in unique_books if 0 <= b < len(book_scores))
    return total_score


def main():
    parser = argparse.ArgumentParser(description="Append algo_ils scores to metadata.csv.")
    parser.add_argument('--input_dir', '-i',default='input', help='Directory with .txt input files')
    parser.add_argument('--output_dir', '-o',default='output-genetic-algorithm', help='Directory with output files (.out or same name)')
    parser.add_argument('--metadata', '-m', default='metadata.csv', help='Path to existing metadata.csv')
    args = parser.parse_args()

    # Load existing metadata.csv
    if not os.path.exists(args.metadata):
        print(f"Metadata file '{args.metadata}' not found.")
        return

    with open(args.metadata, 'r', newline='') as csvfile:
        reader = list(csv.DictReader(csvfile))
        if not reader:
            print("Metadata CSV is empty.")
            return
        fieldnames = reader[0].keys()

    new_field = 'algo_genetic_algorithm'
    updated_records = []

    for row in reader:
        instance_name = row['instance_name']
        input_file = instance_name + '.txt'
        input_path = os.path.join(args.input_dir, input_file)

        # Try both same name and .out variants
        output_candidates = [
            os.path.join(args.output_dir, input_file),
            os.path.join(args.output_dir, input_file + '.out')
        ]
        output_path = next((p for p in output_candidates if os.path.exists(p)), None)

        if not os.path.exists(input_path) or output_path is None:
            print(f"Missing input or output for '{instance_name}', skipping.")
            updated_records.append(row)
            continue

        score = compute_total_score(input_path, output_path)
        row[new_field] = score
        updated_records.append(row)

    # Add new column if not already present
    fieldnames = list(fieldnames)
    if new_field not in fieldnames:
        fieldnames.append(new_field)

    with open(args.metadata, 'w', newline='') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(updated_records)

    print(f"Metadata updated with '{new_field}' column for {len(updated_records)} instances.")


if __name__ == '__main__':
    main()
