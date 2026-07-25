# Installation Guide

This document explains how to set up the required development environment for the Compiler Construction Lab Project.

The instructions below are intended for **Ubuntu/Debian-based Linux distributions**, which are the recommended platforms for this project.



# System Requirements

Before you begin, ensure that you have:

- Ubuntu 22.04 LTS or later (recommended)
- Internet connection
- A GitHub account



# Required Software

The following software must be installed.

| Software | Purpose |
|-----------|---------|
| GCC / G++ | C/C++ Compiler |
| Make | Build Automation |
| Flex | Lexical Analyzer Generator |
| Bison | Parser Generator |
| Git | Version Control |



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



# Step 3: Install Flex

```bash
sudo apt install flex
```



# Step 4: Install Bison

```bash
sudo apt install bison
```



# Step 5: Install Git

```bash
sudo apt install git
```



# Step 6: Verify the Installation

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
flex --version
```

```bash
bison --version
```

```bash
git --version
```

Each command should display the installed version instead of an error message.



# Step 7: Configure Git

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



# Step 8: Fork the Project Repository

1. Open this GitHub repository.
2. Click **Fork**.
3. Wait until GitHub creates a copy under your own account.



# Step 9: Clone Your Repository

Clone **your fork**, not this repository.

```bash
git clone https://github.com/<your-username>/<repository-name>.git
```

Move into the project directory.

```bash
cd <repository-name>
```



# Step 10: Build the Project

If your project includes a Makefile, build it using:

```bash
make
```

If the build is successful, the compiler executable will be generated according to your project's Makefile.



# Step 11: Run Your Compiler

The exact execution command depends on your implementation.

For example:

```bash
./compiler examples/sample.txt
```

Refer to your project's README for the exact command if it differs.



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

## flex: command not found

Install Flex.

```bash
sudo apt install flex
```



## bison: command not found

Install Bison.

```bash
sudo apt install bison
```



## make: command not found

Install the build tools.

```bash
sudo apt install build-essential
```



## Permission denied

Make sure the generated executable has execute permission.

```bash
chmod +x compiler
```

Replace `compiler` with the actual executable name if different.



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
- Lexer Generator: Flex
- Parser Generator: Bison
- Version Control: Git
- Repository Hosting: GitHub
- Code Editor: Visual Studio Code, CLion, or Vim



If you encounter installation or setup problems that are not covered in this guide, contact the course instructor before the project submission deadline.
