# Distributed Lock with Redlock using Redis and Python

This project implements a **Redlock**-based distributed locking mechanism using multiple Redis nodes. It simulates multiple clients trying to acquire and release a lock on a shared resource with fault tolerance and proper logging.

---

## 🛠 Features

- Implements [Redlock algorithm](https://redis.io/docs/interact/locking/) for distributed locking.
- Uses Lua scripting to safely release locks only by the owner.
- Simulates multiple client processes acquiring/releasing a lock.
- Logs all events to `logger.log` for debugging and analysis.
- Supports retry logic and quorum-based consensus for lock acquisition.

---

## 📦 Setup Instructions

### 🔧 Requirements

- **Docker** and **Docker Compose**
- **Python 3.7+**
- Python library: `redis`
---

### 🐳 Spin Up Redis Nodes Using Docker

Use the included `docker-compose.yml` to launch 5 Redis nodes on different ports:

```bash
docker-compose up -d
```

Make the virtual enviorment and install all the requirements:

### Run the Virtual Enviorment 

```bash
python3 -m venv venv
source venv/bin/activate
```

### 🐍 Install Python Dependencies

```bash
pip install -r requirments
```

### ▶️ Run the Locking Script

```bash
python redlock.py
```

## 🧪 How It Works

- Each client process starts after a defined delay (client_processes_waiting).
- Clients attempt to acquire the lock using a majority of Redis nodes (quorum).
- If a quorum is reached and the TTL hasn't expired, the lock is acquired.
- The owner can release the lock using a Lua script to ensure safety.

## 🧰 Configuration

- **Redis Nodes**: Configured in main.py and exposed via Docker on ports 63791 - 63795.
- **TTL**: Default is 5000ms (5 seconds). You can modify the ttl variable in main.py.
- **Client Delays**: Adjustable using the client_processes_waiting list.

## 📂 Logs
All logging is written to a file named logger.log. It includes:
- Successful lock acquisitions
- Lock releases
- Errors and retries during acquisition/release
- Node-specific debug info

## 📋 Example Output
```plaintext
Client-0: Attempting to acquire lock...
Client-0: Lock acquired! Lock ID: 2f5e...
Client-0: Lock released!

Client-1: Attempting to acquire lock...
Client-1: Failed to acquire lock.
```
