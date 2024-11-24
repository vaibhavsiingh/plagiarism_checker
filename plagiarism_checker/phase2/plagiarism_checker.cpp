#include <unordered_map>
#include <chrono>
#include "./plagiarism_checker.hpp"

#include<iostream>
const int MOD = 1e9 + 7; // Large prime for hash modulus
const int BASE = 31;     // Base for rolling hash
const int MIN_LENGTH = 15;
const int EXACT_LENGTH = 75;
int matches_patch=0;


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
                       
                        if (submission->student && submission->professor){
                            if(!flagged_sub.count(submission)){
                                submission->student->flag_student(submission);
                                submission->professor->flag_professor(submission);
                                flagged_sub.insert(submission);
                            }
                        }
                        else if(submission->student){
                            if(!flagged_sub.count(submission)){
                                submission->student->flag_student(submission);
                                flagged_sub.insert(submission);
                            }
                        }
                        else if(submission->professor){
                            if(!flagged_sub.count(submission)){
                                submission->professor->flag_professor(submission);
                                flagged_sub.insert(submission);
                            }
                        }
                    } else {

                        if (old_submission->student && old_submission->professor){
                            if(!flagged_sub.count(old_submission)){
                                old_submission->student->flag_student(old_submission);
                                old_submission->professor->flag_professor(old_submission);
                                flagged_sub.insert(old_submission);
                            }
                        }
                        else if(old_submission->student){
                            if(!flagged_sub.count(old_submission)){
                                old_submission->student->flag_student(old_submission);
                                flagged_sub.insert(old_submission);
                            }
                        }
                        else if(old_submission->professor){
                            if(!flagged_sub.count(submission)){
                                old_submission->professor->flag_professor(old_submission);
                                flagged_sub.insert(old_submission);
                            }
                        }

                        if (submission->student && submission->professor){
                            if(!flagged_sub.count(submission)){
                                submission->student->flag_student(submission);
                                submission->professor->flag_professor(submission);
                                flagged_sub.insert(submission);
                            }
                        }
                        else if(submission->student){
                            if(!flagged_sub.count(submission)){
                                submission->student->flag_student(submission);
                                flagged_sub.insert(submission);
                            }
                        }
                        else if(submission->professor){
                            if(!flagged_sub.count(submission)){
                                submission->professor->flag_professor(submission);
                                flagged_sub.insert(submission);
                            }
                        }
                        
                    }
                }
            }
            if(matches_patch>=35 && !flagged_sub.count(submission)){
                
                if (submission->student) submission->student->flag_student(submission);
                if (submission->professor) submission->professor->flag_professor(submission);
                std::cout<<"patch"<<std::endl;
                
            }
            matches_patch=0;

            
            // Add submission to the database with tokens
            add_to_database(submission, tokens); // Do not update the timestamp here
        }
    }
}


std::vector<int> plagiarism_checker_t::tokenize_code(const std::string& codefile) {
    tokenizer_t tokenizer(codefile);
    return tokenizer.get_tokens();
}



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

bool check_plagiarism(const std::vector<int>& tokens, int length, const std::unordered_set<int>& old_hashes, int threshold, const std::vector<int>& power) {
    if (tokens.size() < length) return false;

    int hash = 0, match_count = 0;
    for (int i = 0; i < length; ++i) {
        hash = (1LL * hash * BASE + tokens[i]) % MOD;
    }
    if (old_hashes.count(hash)) {
        match_count++;
        if (length == EXACT_LENGTH || match_count >= threshold){
            //std::cout<<"Exact 75"<<std::endl;
            return true;
        }
    }

    for (int i = length; i < tokens.size(); ++i) {
        hash = (1LL * hash * BASE - 1LL * tokens[i - length] * power[length] % MOD + MOD) % MOD;
        hash = (hash + tokens[i]) % MOD;
        if (old_hashes.count(hash)) {
            match_count++;
            if (length == EXACT_LENGTH ){
                //std::cout<<"10 or more"<<match_count<<std::endl;
                return true;
            } 
            i += length - 1; // Move to the end of the matched window
            if (i >= tokens.size()) break; // Prevent out-of-bounds access

            // Recompute the hash for the next window
            hash = 0;
            for (int j = i - length + 1; j <= i; ++j) {
                hash = (1LL * hash * BASE + tokens[j]) % MOD;
            }
        }
    }
    if(match_count>threshold){
        
        return true;
    }
    matches_patch+=match_count;
    return false;
}

bool plagiarism_checker_t::is_plagiarized(const std::vector<int>& new_tokens, const std::vector<int>& old_tokens) {
    int new_size = new_tokens.size(), old_size = old_tokens.size();
    std::vector<int> power(std::max({new_size, old_size, EXACT_LENGTH + 1}), 1);
    for (int i = 1; i < power.size(); i++) {
        power[i] = (1LL * power[i - 1] * BASE) % MOD;
    }

    auto old_hashes_15 = compute_hashes(old_tokens, MIN_LENGTH, power);
    auto old_hashes_75 = compute_hashes(old_tokens, EXACT_LENGTH, power);

    if (check_plagiarism(new_tokens, EXACT_LENGTH, old_hashes_75, 0, power)) return true;
    
    return check_plagiarism(new_tokens, MIN_LENGTH, old_hashes_15, 10, power);

    
}


// Add submission to database with tokenization and timestamp
void plagiarism_checker_t::add_to_database(std::shared_ptr<submission_t> submission, const std::vector<int>& tokens) {
    
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
