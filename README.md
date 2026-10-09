# C Version Control (CVC) 🚀

CVC is a lightweight, Git-inspired version control system built entirely from scratch in C. Designed with a focus on simplicity, readability, and zero external dependencies, this project demonstrates core operating system concepts including recursive directory traversal, file-system I/O, cryptographic hashing, and linked-list data structures.

## ✨ Features

CVC supports the core pillars of version control:
- **`cvc init`**: Initializes a hidden `.cvc` database structure to store snapshots and branch pointers.
- **`cvc add`**: Uses `<dirent.h>` to recursively traverse your workspace, hashing files with a custom `djb2` algorithm and saving them to an object database. Supports ignoring files via a custom `.cvcignore` file.
- **`cvc commit`**: Takes a snapshot of the staging area, creating a highly readable text-based commit object linked to a parent commit.
- **`cvc status`**: Compares the active working directory to the staging index.
- **`cvc log`**: Traverses the commit history backward like a Linked List.
- **`cvc branch`**: Creates lightweight branch pointers to allow divergent histories.
- **`cvc checkout`**: Safely switches the `HEAD` pointer and dynamically restores the working directory to match the target commit snapshot.
- **`cvc merge`**: Supports fast-forward branch merging.

## 🏗️ Architecture

CVC operates on a simplified object model to maximize code readability:
- **Object Store (`.cvc/objects/`)**: Files and commits are saved here. File contents are hashed directly to avoid the overhead of complex Tree objects.
- **The Index (`.cvc/index`)**: A flat, plain-text file keeping track of what is currently staged for the next commit.
- **Refs (`.cvc/refs/heads/`)**: Branches are simply 1-line text files containing the hash of the latest commit.
- **HEAD (`.cvc/HEAD`)**: A single file indicating the currently active branch.

## 🚀 Getting Started

### 1. Build the Project
The project uses a standard `Makefile`. Simply clone the repository and run:
```bash
make
```
This will compile the source code inside `src/` and output the executable to `bin/cvc`.

### 2. Global Installation (Optional)
To use `cvc` anywhere on your computer (just like real Git), copy it to your local bin:
```bash
sudo cp bin/cvc /usr/local/bin/cvc
```

### 3. Usage Example
```bash
cvc init
cvc add .
cvc commit -m "My first commit!"
cvc branch new-feature
cvc checkout new-feature
```

## 🛠️ Built With
- **C99** standard
- Built-in OS headers (`<dirent.h>`, `<sys/stat.h>`)
- No external libraries

---
*Created as an academic exploration into the internals of distributed version control systems.*
