#include <unordered_map>
#include <chrono>
#include "./plagiarism_checker.hpp"

// Helper to get the current timestamp
static std::chrono::time_point<std::chrono::steady_clock> get_current_time() {
    return std::chrono::steady_clock::now();
}

plagiarism_checker_t::plagiarism_checker_t() : stop_thread(false){
    worker = std::thread(&plagiarism_checker_t::plagiarism_check, this);
}

plagiarism_checker_t::plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions)
    : stop_thread(false) {
        auto timestamp = get_current_time();
    for (const auto& submission : __submissions) {
        add_to_database(submission);
    }
    worker = std::thread(&plagiarism_checker_t::plagiarism_check, this);
}

plagiarism_checker_t::~plagiarism_checker_t() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex); // Lock the queue for thread-safe access
        stop_thread = true; // Signal the worker thread to stop processing
    }
    cv.notify_all(); // Wake up the worker thread if it's waiting
    if (worker.joinable()) { // Ensure the worker thread is active
        worker.join(); // Wait for the thread to finish execution
    }
}

void plagiarism_checker_t::add_submission(std::shared_ptr<submission_t> __submission) {
    auto timestamp = get_current_time(); // Record the current time of submission
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        processing_queue.push(__submission); // Push the submission into the queue
        submission_timestamps[__submission] = timestamp; // Record the timestamp
    }
    cv.notify_one(); // Notify the worker thread
}


void plagiarism_checker_t::plagiarism_check() {
    while (true) {
        std::shared_ptr<submission_t> submission;

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            cv.wait(lock, [this] { return !processing_queue.empty() || stop_thread; });
            if (stop_thread && processing_queue.empty()) {
                break; // Exit loop if stop is signaled and queue is empty
            }
            submission = processing_queue.front();
            processing_queue.pop();
        }

        auto tokens = tokenize_code(submission->codefile);
        {
            std::lock_guard<std::mutex> lock(db_mutex);
            bool plagiarized = false;

            auto curr_submission_time = submission_timestamps[submission]; // Use the recorded timestamp

            for (const auto& [old_submission, old_tokens] : tokenized_database) {
                if (is_plagiarized(tokens, old_tokens)) {
                    plagiarized = true;
                    auto old_submission_time = submission_timestamps[old_submission];

                    // Handle flagging based on timestamps
                    auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(
                        curr_submission_time - old_submission_time).count();
                    if (time_diff > 1) {
                        if (submission->student) submission->student->flag_student(submission);
                        if (submission->professor) submission->professor->flag_professor(submission);
                    } else {
                        if (submission->professor) submission->professor->flag_professor(submission);
                        if (submission->student) submission->student->flag_student(submission);
                        if (old_submission->student) old_submission->student->flag_student(old_submission);
                        if (old_submission->professor) old_submission->professor->flag_professor(old_submission);
                    }
                }
            }

            // Add submission to the database with tokens
            add_to_database(submission, tokens); // Do not update the timestamp here
        }
    }
}


std::vector<int> plagiarism_checker_t::tokenize_code(const std::string& codefile) {
    tokenizer_t tokenizer(codefile);
    return tokenizer.get_tokens();
}

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

// Add submission to database with tokenization and timestamp
void plagiarism_checker_t::add_to_database(std::shared_ptr<submission_t> submission,
                                           const std::vector<int>& tokens) {
                                            auto timestamp = get_current_time();
    submission_timestamps[submission] = timestamp; // Record the timestamp
    tokenized_database[submission] = tokens;
}

// Overload for when tokens are not precomputed
void plagiarism_checker_t::add_to_database(std::shared_ptr<submission_t> submission) {
    auto tokens = tokenize_code(submission->codefile);
    add_to_database(submission, tokens);
}

// Get timestamp for a submission
std::chrono::time_point<std::chrono::steady_clock> plagiarism_checker_t::get_submission_timestamp(
    const std::shared_ptr<submission_t>& submission) const {
    auto it = submission_timestamps.find(submission);
    if (it != submission_timestamps.end()) {
        return it->second;
    }
    throw std::runtime_error("Submission timestamp not found!");
}
