# 🦅 Crane

Crane 是一个完全从零开始、用纯 C 语言手写的轻量级编程语言实现。

它不依赖任何现成的编译器框架（如 LLVM），也没有使用 Lex/Yacc 等生成工具。从词法分析、语法分析、抽象语法树（AST）、字节码生成，到最终的栈式虚拟机（VM）执行，全部由作者独立手搓完成。

## ✨ 核心特性

- **手写编译器前端**：自行实现词法分析器（Lexer）与基于递归下降的语法分析器（Parser）。
- **多类型支持**：通过带标签的联合体（Tagged Union）实现 `value` 结构，原生支持 `int`、`float`、`string` 等不同类型。
- **常量池架构**：将指令流（Codes）与真实数据（Constants）解耦，大幅精简字节码体积。
- **栈式虚拟机**：纯 C 实现的执行引擎，核心循环由 `while` 和 `switch` 构成，包含操作数栈与局部变量槽位（Slots）管理。

## 🏗️ 架构概览

```text
源码 (.cr) 
  ↓ [词法分析 Lexer]
Token 流 
  ↓ [语法分析 Parser]
抽象语法树 (AST) 
  ↓ [代码生成 Codegen]
字节码 (Bytecode) + 常量池 (Constant Pool) 
  ↓ [虚拟机 VM]
最终结果
```

git clone https://github.com/Xokcal/crane.git
cd crane
make

```cr
int b = 31 * 61 / 91 + 31 - 21 * 21 / 21 + 43;
int a = 11;
```