#!/usr/bin/env python3
"""
Generate metadata for Google Hash Code 2020 Book Scanning solutions.
Scans the specified input directory for .txt files, matches each with a corresponding .out file
in the output directory, computes features, and writes a metadata.csv.
"""
import os
import csv
import argparse
import statistics

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

def compute_metadata(input_path, output_path):
    instance_name_base = os.path.splitext(os.path.basename(input_path))[0]
    
    B_total_books, L_total_libraries, D_total_days, book_scores_list, libraries_input_tuples = read_input_file(input_path)

    feature_num_books = B_total_books
    feature_num_libraries = L_total_libraries
    feature_num_days = D_total_days
    
    avg_book_score_raw = statistics.mean(book_scores_list) if book_scores_list else 0
    feature_avg_book_score = round(avg_book_score_raw, 2)
    feature_stddev_book_score = round(statistics.stdev(book_scores_list), 2) if len(book_scores_list) > 1 else 0.00

    if libraries_input_tuples: 
        avg_books_per_lib_raw = statistics.mean(lib_tuple[0] for lib_tuple in libraries_input_tuples) 
        feature_avg_books_per_library_input = round(avg_books_per_lib_raw, 2)

        avg_signup_raw = statistics.mean(lib_tuple[1] for lib_tuple in libraries_input_tuples)           
        feature_avg_signup_time = round(avg_signup_raw, 2)

        avg_shipped_raw = statistics.mean(lib_tuple[2] for lib_tuple in libraries_input_tuples) 
        feature_avg_books_shipped_per_day_input = round(avg_shipped_raw, 2)
    else:
        feature_avg_books_per_library_input = 0.00
        feature_avg_signup_time = 0.00
        feature_avg_books_shipped_per_day_input = 0.00

    _, solution_libraries = read_output_file(output_path)
    
    scanned_book_ids = set()
    for _, _, books_in_lib_solution in solution_libraries:
        for book_id in books_in_lib_solution:
            scanned_book_ids.add(book_id)
            
    total_score = 0 
    for book_id in scanned_book_ids:
        if 0 <= book_id < len(book_scores_list):
            total_score += book_scores_list[book_id]
        else:
            print(f"Warning: Scanned book ID {book_id} out of range for instance {instance_name_base}. Max ID is {len(book_scores_list)-1}")

    metadata_dict = {
        'instance_name': instance_name_base,
        'feature_num_books': feature_num_books,
        'feature_num_libraries': feature_num_libraries, 
        'feature_num_days': feature_num_days,
        'feature_avg_book_score': feature_avg_book_score,
        'feature_stddev_book_score': feature_stddev_book_score,
        'feature_avg_books_per_library': feature_avg_books_per_library_input,
        'feature_avg_signup_time': feature_avg_signup_time, 
        'feature_avg_books_shipped_per_day': feature_avg_books_shipped_per_day_input, 
        'algo_greedy_approach': total_score 
    }
    
    return metadata_dict

def main():
    parser = argparse.ArgumentParser(
        description="Generate metadata.csv from Hash Code 2020 solutions"
    )
    parser.add_argument(
        '--input_dir', '-i', default='input',
        help='Directory containing .txt input files'
    )
    parser.add_argument(
        '--output_dir', '-o', default='output-greedy-approach',
        help='Directory containing .out output files'
    )
    parser.add_argument(
        '--metadata', '-m', default='metadata.csv',
        help='Path to write the metadata CSV'
    )
    args = parser.parse_args()

    records = []
    # Iterate over input .txt files
    for fname in os.listdir(args.input_dir):
        if not fname.endswith('.txt'):
            continue
        in_path = os.path.join(args.input_dir, fname)
        base = os.path.splitext(fname)[0]
        
        # Expect output filename as input filename + '.out'
        out_filename = fname + '.out'
        out_path = os.path.join(args.output_dir, out_filename)
        if not os.path.exists(out_path):
            print(f"Warning: no matching output for {fname} (expected {out_filename}), skipping.")
            continue
        rec = compute_metadata(in_path, out_path)
        records.append(rec)

    if not records:
        print("No valid instances found. Exiting.")
        return

    # Write CSV
    fieldnames = list(records[0].keys())
    with open(args.metadata, 'w', newline='') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(records)

    print(f"Metadata written to {args.metadata} containing {len(records)} entries.")


if __name__ == '__main__':
    main()
