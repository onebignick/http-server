import requests
import time
import threading
from concurrent.futures import ThreadPoolExecutor

# --- Configuration ---
SERVER_URL = "http://localhost:5000/" # Your server's address
NUM_REQUESTS = 1000                  # Total number of requests to send
CONCURRENCY = 100                    # Number of simultaneous requests
REQUEST_TIMEOUT = 5                  # Timeout for each request in seconds

# --- Global variables for statistics ---
successful_requests = 0
failed_requests = 0
total_latency = 0.0
# Using a Lock for thread-safe updates to global stats
stats_lock = threading.Lock()

def send_request(session, request_id):
    """Sends a single GET request and records its latency."""
    global successful_requests, failed_requests, total_latency

    start_time = time.perf_counter()
    try:
        response = session.get(SERVER_URL, timeout=REQUEST_TIMEOUT)
        response.raise_for_status()  # Raise HTTPError for bad responses (4xx or 5xx)
        end_time = time.perf_counter()
        latency = (end_time - start_time) * 1000 # Latency in milliseconds

        with stats_lock:
            successful_requests += 1
            total_latency += latency
        # print(f"Request {request_id}: Success (Latency: {latency:.2f}ms, Status: {response.status_code})")

    except requests.exceptions.Timeout:
        end_time = time.perf_counter()
        latency = (end_time - start_time) * 1000
        with stats_lock:
            failed_requests += 1
        print(f"Request {request_id}: Timeout after {latency:.2f}ms")
    except requests.exceptions.RequestException as e:
        with stats_lock:
            failed_requests += 1
        print(f"Request {request_id}: Error - {e}")
    except Exception as e:
        with stats_lock:
            failed_requests += 1
        print(f"Request {request_id}: Unexpected Error - {e}")

def run_load_test():
    """Runs the main load test with configurable concurrency."""
    global successful_requests, failed_requests, total_latency

    successful_requests = 0
    failed_requests = 0
    total_latency = 0.0

    print(f"Starting load test on {SERVER_URL}")
    print(f"Total requests: {NUM_REQUESTS}, Concurrency: {CONCURRENCY}")

    start_test_time = time.perf_counter()

    # Use ThreadPoolExecutor for efficient concurrent request sending
    # Each worker thread will have its own requests.Session to manage connections
    with ThreadPoolExecutor(max_workers=CONCURRENCY) as executor:
        # Create a list of futures
        # tqdm for progress bar if installed
        futures = []
        with requests.Session() as session: # Use a single session per thread for connection pooling
            for i in range(NUM_REQUESTS):
                futures.append(executor.submit(send_request, session, i + 1))

        # Wait for all futures to complete and show progress
        for _ in futures:
            pass # Wait for completion without progress bar

    end_test_time = time.perf_counter()
    total_test_duration = end_test_time - start_test_time

    print("\n--- Test Results ---")
    print(f"Total test duration: {total_test_duration:.2f} seconds")
    print(f"Successful requests: {successful_requests}")
    print(f"Failed requests: {failed_requests}")

    if successful_requests > 0:
        throughput = successful_requests / total_test_duration
        average_latency = total_latency / successful_requests
        print(f"Throughput: {throughput:.2f} requests/second")
        print(f"Average successful request latency: {average_latency:.2f} ms")
    else:
        print("No successful requests to calculate throughput or latency.")

if __name__ == "__main__":
    run_load_test()