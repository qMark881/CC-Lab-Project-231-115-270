# Contributing Guidelines


This document outlines the expected development workflow and collaboration practices for all project groups. Following these guidelines will help your team maintain a clean, organized, and professional repository throughout the project.



# Getting Started

Before writing any code, every team member should:

- Accept the invitation to collaborate on the group's repository.
- Clone the group's GitHub repository to their local machine.
- Verify that Git is correctly configured with their own GitHub account.

Example:

```bash
git clone https://github.com/<your-team>/<repository-name>.git
cd <repository-name>
```



# Working as a Team

Compiler development is divided into multiple independent components. Coordinate with your teammates before starting implementation to avoid duplicate work.

Whenever possible, assign clear responsibilities such as:

- Lexical Analyzer
- Parser
- AST Construction
- Symbol Table
- Semantic Analysis
- Intermediate Code Generation
- Documentation
- Testing

Regular communication among team members is strongly encouraged.



# Development Practices

Develop your project incrementally.

Instead of completing large portions of the project before committing, make small logical improvements and commit them regularly.

A typical development cycle is:

1. Pull the latest changes.
2. Implement a single feature or fix.
3. Test your changes.
4. Commit with a descriptive message.
5. Push your changes to GitHub.

Frequent commits provide a clear development history and reduce merge conflicts.



# Commit Message Guidelines

Every commit should clearly describe the work performed.

### Recommended

```text
Add support for floating-point literals

Implement nested scope handling

Detect variable redeclaration

Generate TAC for while loops

Refactor parser grammar

Improve lexical error messages
```

### Avoid

```text
update

changes

work

code

done

misc
```

A meaningful commit history is an important part of professional software development.


# Branching Strategy

Using feature branches is optional but recommended for larger teams.

Suggested branch names:

```text
feature/lexer

feature/parser

feature/ast

feature/semantic

feature/codegen

feature/documentation
```

After verifying that a feature works correctly, merge it into the `main` branch.



# Before Pushing

Before pushing your work:

- Ensure the project builds successfully.
- Verify that your changes do not break existing functionality.
- Remove debugging statements that are no longer needed.
- Check formatting and indentation.
- Review your code for readability.

Never push code that has not been tested.



# Repository Organization

Keep the repository well structured.

Place source files in their appropriate directories and avoid placing unrelated files in the project root.

Generated files, temporary files, executables, and IDE-specific configuration files should not be committed unless explicitly required.



# Coding Practices

Aim for code that is easy to understand and maintain.

Recommended practices include:

- Meaningful identifiers
- Consistent formatting
- Modular design
- Reasonable function sizes
- Clear comments where necessary

Avoid unnecessary complexity whenever a simpler solution is available.



# Documentation

Documentation is part of the project.

Whenever significant functionality is added:

- Update the README if required.
- Document any assumptions.
- Keep build and execution instructions accurate.
- Ensure test cases remain up to date.

Good documentation makes the project easier to understand and evaluate.



# Testing

Each major component should be tested before integration.

Include both:

- Valid test programs
- Invalid test programs

Your repository should demonstrate that the compiler correctly handles successful compilation as well as lexical, syntax, and semantic errors.



# Professional Conduct

Every member is expected to contribute fairly to the project.

Repositories showing little or no participation from some members may receive additional attention during project evaluation.

Treat your teammates respectfully, communicate regularly, and resolve technical disagreements collaboratively.



# Academic Responsibility

You are welcome to consult books, documentation, research papers, and AI-assisted development tools.

However, every team member is responsible for understanding the implementation submitted under the group's name.

Be prepared to explain your design decisions, algorithms, and code during the project demonstration and viva.



# Final Checklist

Before submitting your repository, verify that:

- All required modules have been implemented.
- The project builds successfully.
- The repository structure is organized.
- Documentation is complete.
- Test cases are included.
- Generated files are excluded.
- Every team member has contributed through Git.
- The latest version has been pushed to GitHub.


Following these guidelines will help your team maintain a professional development workflow and produce a well-organized project repository.
