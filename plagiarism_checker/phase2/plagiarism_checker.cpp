#include "plagiarism_checker.hpp"

// Constant Global variables used
const int MOD = 1e9 + 7;
const int BASE = 31;
const int MIN_LENGTH = 15;
const int EXACT_LENGTH = 75;

// Making matches_patch atomic to prevent data races
static std::atomic<int> matches_patch{0};

// Function to get the current time
static std::chrono::time_point<std::chrono::steady_clock> get_current_time() {
    return std::chrono::steady_clock::now();
}

// Constructor #1
plagiarism_checker_t::plagiarism_checker_t() : stop_thread(false) {
    worker = std::thread(&plagiarism_checker_t::plagiarism_check, this);
}

// Constructor #2
plagiarism_checker_t::plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions)
    : stop_thread(false) {
    auto timestamp = get_current_time();
    {
        std::lock_guard<std::mutex> lock(mtx);
        for (const auto& submission : __submissions) {
            auto tokens = tokenize_code(submission->codefile);
            submission_timestamps[submission] = timestamp;
            tokenized_database[submission] = tokens;
        }
    }
    worker = std::thread(&plagiarism_checker_t::plagiarism_check, this);
}

// Destructor
plagiarism_checker_t::~plagiarism_checker_t() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop_thread = true;
    }
    cv.notify_all();
    if (worker.joinable()) {
        worker.join();
    }
}

// Function to add submission to the processing queue
void plagiarism_checker_t::add_submission(std::shared_ptr<submission_t> __submission) {
    auto timestamp = get_current_time();
    {
        std::lock_guard<std::mutex> lock(mtx);
        processing_queue.push(__submission);
        submission_timestamps[__submission] = timestamp;
    }
    cv.notify_one();
}

// Function to flag submissions
void plagiarism_checker_t::flag_submission(std::shared_ptr<submission_t> submission) {
    bool should_flag = false;
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (!flagged_sub.count(submission)) {
            flagged_sub.insert(submission);
            should_flag = true;
        }
    }
    
    if (should_flag) {
        if (submission->student) {
            submission->student->flag_student(submission);
        }
        if (submission->professor) {
            submission->professor->flag_professor(submission);
        }
    }
}

// Main function to check plagiarism
void plagiarism_checker_t::plagiarism_check() {
    while (true) {
        std::shared_ptr<submission_t> submission;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return !processing_queue.empty() || stop_thread; });
            
            if (stop_thread && processing_queue.empty()) {
                break;
            }
            
            submission = processing_queue.front();
            processing_queue.pop();
        }

        auto tokens = tokenize_code(submission->codefile);
        
        
        // Get current submission time while holding the lock
        std::chrono::time_point<std::chrono::steady_clock> curr_submission_time;
        std::unordered_map<std::shared_ptr<submission_t>, std::vector<int>> current_database;
        {
            std::lock_guard<std::mutex> lock(mtx);
            curr_submission_time = submission_timestamps[submission];
            current_database = tokenized_database; // Make a copy to work with
        }

        for (const auto& [old_submission, old_tokens] : current_database) {
            if (is_plagiarized(tokens, old_tokens)) {
                
                std::chrono::time_point<std::chrono::steady_clock> old_submission_time;
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    old_submission_time = submission_timestamps[old_submission];
                }

                auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(
                    curr_submission_time - old_submission_time).count();
                
                if (time_diff > 1) {
                    flag_submission(submission);
                } else {
                    flag_submission(old_submission);
                    flag_submission(submission);
                }
            }
        }

        int current_matches = matches_patch.load();
        if (current_matches >= 20) {
            bool should_flag = false;
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (!flagged_sub.count(submission)) {
                    should_flag = true;
                }
            }
            
            if (should_flag) {
                if (submission->student) submission->student->flag_student(submission);
                if (submission->professor) submission->professor->flag_professor(submission);
            }
        }
        
        matches_patch.store(0);
        
        // Add submission to database
        {
            std::lock_guard<std::mutex> lock(mtx);
            tokenized_database[submission] = tokens;
        }
    }
}

// Function to tokenize code
std::vector<int> plagiarism_checker_t::tokenize_code(const std::string codefile) {
    tokenizer_t tokenizer(codefile);
    return tokenizer.get_tokens();
}

// Function to compute hashes for a given length
std::unordered_set<int> compute_hashes(const std::vector<int>& tokens, int length, const std::vector<int>& power) {
    std::unordered_set<int> hashes;
    if (tokens.size() < length) return hashes;

    int hash = 0;
    for (int i = 0; i < length; ++i) {
        hash = (1LL * hash * BASE + tokens[i]) % MOD;
    }
    hashes.insert(hash);

    for (size_t i = length; i < tokens.size(); ++i) {
        hash = (1LL * hash * BASE - 1LL * tokens[i - length] * power[length] % MOD + MOD) % MOD;
        hash = (hash + tokens[i]) % MOD;
        hashes.insert(hash);
    }
    return hashes;
}

// Helper function to check plagiarism
bool plagiarism_checker_t::check_plagiarism(const std::vector<int>& tokens, int length, 
    const std::unordered_set<int>& old_hashes, int threshold, const std::vector<int>& power) {
    
    if (tokens.size() < length) return false;

    int hash = 0;
    std::atomic<int> match_count{0};

    for (int i = 0; i < length; ++i) {
        hash = (1LL * hash * BASE + tokens[i]) % MOD;
    }
    
    if (old_hashes.count(hash)) {
        match_count++;
        if (length == EXACT_LENGTH || match_count >= threshold) {
            return true;
        }
    }

    for (size_t i = length; i < tokens.size(); ++i) {
        hash = (1LL * hash * BASE - 1LL * tokens[i - length] * power[length] % MOD + MOD) % MOD;
        hash = (hash + tokens[i]) % MOD;
        
        if (old_hashes.count(hash)) {
            match_count++;
            if (length == EXACT_LENGTH) {
                return true;
            }
            
            i += length - 1;
            if (i >= tokens.size()) break;

            hash = 0;
            for (int j = i - length + 1; j <= i; ++j) {
                hash = (1LL * hash * BASE + tokens[j]) % MOD;
            }
        }
    }

    if (match_count > threshold) {
        return true;
    }

    matches_patch.fetch_add(match_count);
    return false;
}

// Function to check if two submissions are plagiarized
bool plagiarism_checker_t::is_plagiarized(const std::vector<int>& new_tokens, const std::vector<int>& old_tokens) {
    int new_size = new_tokens.size(), old_size = old_tokens.size();
    std::vector<int> power(std::max({new_size, old_size, EXACT_LENGTH + 1}), 1);
    
    for (int i = 1; i < power.size(); i++) {
        power[i] = (1LL * power[i - 1] * BASE) % MOD;
    }

    auto old_hashes_15 = compute_hashes(old_tokens, MIN_LENGTH, power);
    auto old_hashes_75 = compute_hashes(old_tokens, EXACT_LENGTH, power);

    if (check_plagiarism(new_tokens, EXACT_LENGTH, old_hashes_75, 0, power)) {
        return true;
    }
    
    return check_plagiarism(new_tokens, MIN_LENGTH, old_hashes_15, 10, power);
}

// Function to add submissions to database
void plagiarism_checker_t::add_to_database(std::shared_ptr<submission_t> submission, const std::vector<int>& tokens) {
    auto timestamp = get_current_time();
    {
        std::lock_guard<std::mutex> lock(mtx);
        submission_timestamps[submission] = timestamp;
        tokenized_database[submission] = tokens;
    }
}

// Overload of add_to_database
void plagiarism_checker_t::add_to_database(std::shared_ptr<submission_t> submission) {
    auto tokens = tokenize_code(submission->codefile);
    add_to_database(submission, tokens);
}

// Function to get submission timestamp from hashmap
std::chrono::time_point<std::chrono::steady_clock> plagiarism_checker_t::get_submission_timestamp(
    const std::shared_ptr<submission_t> submission) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = submission_timestamps.find(submission);
    if (it != submission_timestamps.end()) {
        return it->second;
    }
    throw std::runtime_error("Submission timestamp not found!");
}