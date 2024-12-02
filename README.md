# Plagiarism Checker for Code Submissions

## Overview

This project implements a plagiarism detection system for source code submissions. It is designed to check for similarities and potential plagiarism in code files submitted by students, flagging them for possible cheating based on token patterns. The system works in multiple phases, each improving the efficiency and accuracy of plagiarism detection.

The project involves:
1. **Pattern Matching**: Detects exact and approximate pattern matches in tokenized code.
2. **Plagiarism Flagging**: Flags plagiarized code based on defined criteria, such as repeated matches and timestamps.
3. **Efficiency**: Optimizes the process using multithreading and parallel processing to handle a large number of submissions.

The system is implemented in C++ and follows an object-oriented approach, with key classes such as `PlagiarismChecker`, `Submission`, `Student`, and `Professor` interacting with each other. The solution involves tokenizing code, finding patterns, and reporting possible plagiarism cases.

---

## Phases

### Phase 1: **Basic Pattern Matching and Plagiarism Detection**

- **Exact Pattern Matching**: Detects exact matches of code patterns, typically of length 10-20 tokens.
- **Approximate Matching for Longer Patterns**: Finds patterns of 30+ tokens that are approximately identical (i.e., with up to 20% modifications).
- **Flagging Plagiarism**: Submissions are flagged as plagiarized based on the number and size of matching patterns.
- **Output**: The system reports five key values to indicate the degree of plagiarism, including a flag indicating whether the submissions are plagiarized, total length of pattern matches, the longest approximate pattern match, and start indices of the match in both files.

### Phase 2: **Efficient Plagiarism Checking**

- **Multithreading**: Uses parallel processing to handle plagiarism checking efficiently, especially with large sets of submissions.
- **Timestamp Management**: The timestamp of submission is considered when determining which submission is flagged for plagiarism.
- **Patchwork Plagiarism**: Flags submissions based on multiple smaller matching patterns sourced from other submissions.
- **Optimized for Speed**: Prioritizes efficiency and scalability, ensuring that the plagiarism checker can handle large volumes of submissions.

### Phase 3: **Plagiarism Bypass Challenge**

- **Identifying False Positives/Negatives**: The goal is to test and bypass the plagiarism checkers by providing cases that incorrectly trigger plagiarism flags or miss plagiarized submissions.
- **Creativity**: This phase encourages creativity in bypassing the checker by modifying code in subtle ways (such as renaming variables or reordering statements) to avoid detection.

---

## Features

- **Pattern Matching**: Supports exact and approximate pattern matching for plagiarism detection.
- **Multithreading Support**: Efficiently handles multiple submissions by using parallel threads.
- **Timestamp Management**: Flags plagiarized submissions based on timestamps and submission order.
- **Flagging System**: Alerts students and professors via APIs when plagiarism is detected.
- **Patchwork Detection**: Detects complex plagiarism patterns where parts of code are copied from multiple sources.

---

## Getting Started

To run the plagiarism checker, follow these steps:

### Prerequisites
- **C++14** or later
- **Docker**: For setting up the environment
- **Clang Compiler**: Used for parsing and tokenizing code

### Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/vaibhavsiingh/plagiarism_checker.git
   cd plagiarism-checker/plagiarism-checker
   ```
2. Build the Docker image:
   ```bash
   docker build . -t plagiarism checker
   ```
3. Run the project in a Docker container:
   ```bash
   run --rm -it -v .:/plagiarism checker plagiarism checker
   ```
4. To run phase 1:
   ```bash
   cd phase1
   make
   ```
   To run phase 2:
   ```bash
   cd phase2
   make
   ```
### Key Classes
#### ```PlagiarismChecker```
- Main class that handles the detection of plagiarism across multiple submissions.
- Contains methods for adding submissions, checking for plagiarism, and flagging students/professors.
#### ```Submission```
- Represents a student's submission, including the code file, timestamp, and associated student/professor.
#### ```Student``` and ```Professor```
- Classes representing the entities who can be flagged when plagiarism is detected.
#### ```Tokenizer```
- Converts raw code files into a stream of tokens for pattern matching.

### Authors
- [Vaibhav Singh](https://github.com/vaibhavsiingh)
- [Satyam Sinoliya](https://github.com/satyam314)
- [Aryan Mahajan](https://github.com/Aryan-Mahajan-79)
   
