# Installation Guide

This document explains how to set up the development environment for this repository.

The project builds cleanly with a standard C toolchain on Linux or WSL.



# System Requirements

Before you begin, ensure that you have:

- Ubuntu 22.04 LTS or later, or WSL with a Linux distribution
- Internet connection
- A GitHub account



# Required Software

The following software is needed to build and run the current repository.

| Software | Purpose |
|-----------|---------|
| GCC / G++ | C/C++ Compiler |
| Make | Build Automation |
| Git | Version Control |

The compiler front-end in this repository is self-contained, so no additional parser generator is required to build it.



# Step 1: Update Package Information

Open a terminal and run:

```bash
sudo apt update
```



# Step 2: Install Build Tools

Install the GNU compiler and Make.

```bash
sudo apt install build-essential
```



# Step 3: Install Git

```bash
sudo apt install git
```



# Step 4: Verify the Installation

Run the following commands to confirm that everything has been installed correctly.

```bash
gcc --version
```

```bash
g++ --version
```

```bash
make --version
```

```bash
git --version
```



# Step 5: Configure Git

If you are using Git for the first time, configure your identity.

```bash
git config --global user.name "Your Name"
```

```bash
git config --global user.email "your_email@example.com"
```

Verify the configuration.

```bash
git config --list
```



# Step 6: Clone the Repository

Clone your repository.

```bash
git clone https://github.com/<your-username>/<repository-name>.git
```

Move into the project directory.

```bash
cd <repository-name>
```



# Step 7: Build the Project

Build the compiler executable with:

```bash
make
```

If the build is successful, the compiler executable will be generated according to the repository Makefile.



# Step 8: Run the Compiler

Use the executable on a sample input file:

```bash
make run INPUT=tests/valid/arithmetic.md
```

or run it directly:

```bash
./compiler tests/valid/arithmetic.md
```



# Updating Your Repository

Before starting new work, synchronize your local repository with GitHub.

```bash
git pull origin main
```



# Saving Your Work

Stage your changes.

```bash
git add .
```

Create a commit.

```bash
git commit -m "Describe your changes"
```

Push to GitHub.

```bash
git push origin main
```

Commit regularly throughout the project instead of making one large commit near the submission deadline.



# Common Installation Issues

## gcc: command not found

Install the build tools.

```bash
sudo apt install build-essential
```



## make: command not found

Install the build tools.

```bash
sudo apt install build-essential
```



## git: command not found

Install Git.

```bash
sudo apt install git
```



# Recommended Development Environment

The following setup is recommended for this project.

- Operating System: Ubuntu 22.04 LTS or later
- Compiler: GCC/G++
- Build System: Make
- Version Control: Git
- Repository Hosting: GitHub
- Code Editor: Visual Studio Code, CLion, or Vim

If you encounter installation or setup problems that are not covered in this guide, contact the course instructor before the submission deadline.
