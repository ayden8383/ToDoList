# To-Do List with Automatic Categories

A wxWidgets to-do list app that groups tasks by shared phrases, using a
suffix array (DC3) and an LCP array (Kasai's algorithm).

## Submission
- **Report:** Suffix-Array-Report-AydenTran-24923017.pdf
- **Video walkthrough:** [link or docs/Walkthrough.mp4]
- **Runnable app (Windows x64):** see the latest [Release](../../releases)

## Where the code is
- `ToDoList/LCS.h`, `ToDoList/LCS.cpp`: tokenizer, DC3, Kasai, findPhrases
- `ToDoList/MainFrame.cpp`: the UI, including the Categories tab
- `Benchmark/Benchmark.cpp`: naive vs DC3 timings

## Building
Visual Studio 2022, C++, wxWidgets 3.3.3 (static libraries). The project
expects wxWidgets at `Desktop\tools\wxWidgets-3.3.3`; update the
`wxwidgets.props` import path in `ToDoList.vcxproj` if yours is elsewhere.
