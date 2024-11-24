#include "structures.hpp"
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <atomic>

// Constants used in plagiarism detection

// Helper function declaration for hash computation
std::unordered_set<int> compute_hashes(const std::vector<int>& tokens, int length, const std::vector<int>& power);

class plagiarism_checker_t {
public:
    // Public interface remains unchanged
    plagiarism_checker_t(void);
    plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions);
    ~plagiarism_checker_t(void);
    void add_submission(std::shared_ptr<submission_t> __submission);

protected:
    // Database and data structures
    std::unordered_map<std::shared_ptr<submission_t>, std::vector<int>> tokenized_database;
    std::unordered_map<std::shared_ptr<submission_t>, std::chrono::time_point<std::chrono::steady_clock>> submission_timestamps;
    std::unordered_set<std::shared_ptr<submission_t>> flagged_sub;
    std::queue<std::shared_ptr<submission_t>> processing_queue;

    // Synchronization primitives
    mutable std::mutex mtx; 
    std::condition_variable cv;
    std::thread worker;
    bool stop_thread;

    // Core plagiarism detection methods
    void plagiarism_check();
    bool is_plagiarized(const std::vector<int>& new_tokens,
                       const std::vector<int>& old_tokens);
    bool check_plagiarism(const std::vector<int>& tokens, 
                         int length,
                         const std::unordered_set<int>& old_hashes,
                         int threshold,
                         const std::vector<int>& power);

    // Utility methods
    void flag_submission(std::shared_ptr<submission_t> submission);
    std::vector<int> tokenize_code(const std::string codefile);
    void add_to_database(std::shared_ptr<submission_t> submission,
                        const std::vector<int>& tokens);
    void add_to_database(std::shared_ptr<submission_t> submission);
    std::chrono::time_point<std::chrono::steady_clock> get_submission_timestamp(
        const std::shared_ptr<submission_t> submission) const;
};