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

// Add your helper functions and classes here if needed

class plagiarism_checker_t {
    // You should NOT modify the public interface of this class.
public:
    plagiarism_checker_t(void);
    plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions);
    ~plagiarism_checker_t(void);
    void add_submission(std::shared_ptr<submission_t> __submission);

protected:
    // Members for managing the plagiarism checker

    // A database of all submissions
    std::vector<std::shared_ptr<submission_t>> database;

    // Map to store tokenized code for submissions
    std::unordered_map<std::shared_ptr<submission_t>, std::vector<int>> tokenized_database;

    // Map to store timestamps for submissions
    std::unordered_map<std::shared_ptr<submission_t>, std::chrono::time_point<std::chrono::steady_clock>> submission_timestamps;

    std::unordered_set<std::shared_ptr<student_t>> flagged_students;
    std::unordered_set<std::shared_ptr<professor_t>> flagged_professors;

    // Queue to hold new submissions to be processed
    std::queue<std::shared_ptr<submission_t>> processing_queue;

    // Mutex to synchronize access to the queue and database
    std::mutex db_mutex;            // Protects the database
    std::mutex queue_mutex;         // Protects the processing queue

    // Condition variable for worker thread notifications
    std::condition_variable cv;

    // Background worker thread for plagiarism checking
    std::thread worker;

    // Flag to indicate the worker thread should stop
    bool stop_thread;

    // Helper functions
    void plagiarism_check(); // Function run by the worker thread
    bool is_plagiarized(const std::vector<int>& new_tokens,
                                        const std::vector<int>& old_tokens);
    bool detect_patchwork(const std::shared_ptr<submission_t>& new_sub);

    // Utility functions
    std::vector<int> tokenize_code(const std::string& codefile);
    void add_to_database(std::shared_ptr<submission_t> submission, const std::vector<int>& tokens);
    void add_to_database(std::shared_ptr<submission_t> submission);
    std::chrono::time_point<std::chrono::steady_clock> get_submission_timestamp(
        const std::shared_ptr<submission_t>& submission) const;
};
