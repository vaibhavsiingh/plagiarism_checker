#include "plagiarism_checker.hpp"
#include "../tokenizer.hpp"
#include <fstream>
#include <iostream>

// Constructor - Initializes and starts the worker thread
plagiarism_checker_t::plagiarism_checker_t() : stop_thread(false) {
    worker = std::thread(&plagiarism_checker_t::plagiarism_check, this);
}

// Constructor with initial submissions
plagiarism_checker_t::plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions)
    : stop_thread(false), database(std::move(__submissions)) {
    worker = std::thread(&plagiarism_checker_t::plagiarism_check, this);
}

// Destructor - Stops the worker thread and cleans up
plagiarism_checker_t::~plagiarism_checker_t() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        stop_thread = true; // Signal the thread to stop
    }
    cv.notify_all(); // Wake up the thread
    if (worker.joinable()) {
        worker.join(); // Wait for the thread to exit
    }
}

// Add a new submission
void plagiarism_checker_t::add_submission(std::shared_ptr<submission_t> __submission) {
    // Add the submission to the queue
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        processing_queue.push(__submission);
    }

    // Notify the worker thread
    cv.notify_one();
}

// Worker thread function
void plagiarism_checker_t::plagiarism_check() {
    while (true) {
        std::shared_ptr<submission_t> submission;

        // Wait for a new submission or stop signal
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            cv.wait(lock, [this] { return !processing_queue.empty() || stop_thread; });

            if (stop_thread && processing_queue.empty()) {
                break; // Exit the loop if stop is signaled and queue is empty
            }

            // Get the next submission from the queue
            submission = processing_queue.front();
            processing_queue.pop();
        }

        // Tokenize the submission's code file
        std::vector<int> new_tokens = tokenize_code(submission->codefile);

        // Process the submission (compare with existing database)
        {
            std::lock_guard<std::mutex> lock(db_mutex);
            bool plagiarized = false;

            for (const auto& old_submission : database) {
                std::vector<int> old_tokens = tokenize_code(old_submission->codefile);

                if (is_plagiarized(new_tokens, old_tokens)) {
                    plagiarized = true;

                    // Handle flagging based on timestamps
                    auto time_diff = submission->timestamp - old_submission->timestamp;
                    if (time_diff >= std::chrono::seconds(1)) {
                        if (submission->student) submission->student->flag_student(submission);
                        if (submission->professor) submission->professor->flag_professor(submission);
                    } else {
                        if (submission->student) submission->student->flag_student(submission);
                        if (submission->professor) submission->professor->flag_professor(submission);
                        if (old_submission->student) old_submission->student->flag_student(old_submission);
                        if (old_submission->professor) old_submission->professor->flag_professor(old_submission);
                    }
                }
            }

            // Check for patchwork plagiarism
            if (!plagiarized && detect_patchwork(new_tokens)) {
                if (submission->student) submission->student->flag_student(submission);
                if (submission->professor) submission->professor->flag_professor(submission);
            }

            // Add the submission to the database
            database.push_back(submission);
        }
    }
}

// Helper: Tokenize a code file into a vector of tokens
std::vector<int> plagiarism_checker_t::tokenize_code(const std::string& codefile) {
    tokenizer_t tokenizer(codefile);
    return tokenizer.get_tokens();
}

// Helper: Check for exact plagiarism
bool plagiarism_checker_t::is_plagiarized(const std::vector<int>& new_tokens,
                                          const std::vector<int>& old_tokens) {
    int new_size = new_tokens.size();
    int old_size = old_tokens.size();

    int pattern_match_count = 0;

    for (int i = 0; i <= new_size - 15; ++i) {
        for (int j = 0; j <= old_size - 15; ++j) {
            int match_length = 0;

            while (i + match_length < new_size && j + match_length < old_size &&
                   new_tokens[i + match_length] == old_tokens[j + match_length]) {
                match_length++;
                if (match_length >= 75) {
                    return true; // Exact match found
                }
            }

            if (match_length >= 15) {
                pattern_match_count++;
                if (pattern_match_count >= 10) {
                    return true; // Enough small pattern matches found
                }
            }
        }
    }

    return false; // No plagiarism detected
}

// Helper: Check for patchwork plagiarism
bool plagiarism_checker_t::detect_patchwork(const std::vector<int>& new_tokens) {
    std::unordered_set<std::string> matched_patterns;

    for (const auto& old_sub : database) {
        std::vector<int> old_tokens = tokenize_code(old_sub->codefile);

        for (int i = 0; i <= new_tokens.size() - 15; ++i) {
            for (int j = 0; j <= old_tokens.size() - 15; ++j) {
                int match_length = 0;

                while (i + match_length < new_tokens.size() && j + match_length < old_tokens.size() &&
                       new_tokens[i + match_length] == old_tokens[j + match_length]) {
                    match_length++;
                }

                if (match_length >= 15) {
                    std::string pattern;
                    for (int k = 0; k < match_length; ++k) {
                        pattern += std::to_string(new_tokens[i + k]) + "-";
                    }
                    matched_patterns.insert(pattern);

                    if (matched_patterns.size() >= 20) {
                        return true; // Enough distinct patterns found
                    }
                }
            }
        }
    }

    return false; // No patchwork plagiarism detected
}
