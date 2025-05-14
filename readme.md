# Matilda Book Scanning Problem

This repository presents an analysis of the **Book Scanning problem dataset** using the **Matilda tool**.

## 📊 Dataset Overview

- **Instances:** 56 input files
- **Columns:** 16 total  
  - Each row corresponds to a problem instance.  
  - The dataset includes **scores from 6 different algorithms**, each stored in separate columns.

## 🔧 Tools and Technologies

- **Matilda** – for analyzing the dataset and comparing algorithm performance.
- **Python** – used for data processing, automation, and also for running one of the algorithms.
- **C++** – used for implementing and executing the remaining algorithms.

## 🚀 Processing Steps

1. Each of the 6 algorithms was executed on all 56 book scanning instances.
2. The output score from each algorithm was recorded and added to the dataset as a new column.
3. The final dataset was analyzed using the Matilda tool to visualize and compare performance across algorithms.

## 📁 Repository Structure

- `data/` – Contains input files and the final dataset with scores.
- `scripts/` – Python and C++ scripts used for algorithm execution and data handling.
- `results/` – metadata.csv is saved here.

## 📌 Notes

This project demonstrates how Matilda can assist in evaluating algorithmic performance on problem-specific datasets using a hybrid Python/C++ workflow.