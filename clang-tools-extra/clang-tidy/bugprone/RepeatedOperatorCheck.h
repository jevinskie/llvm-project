//===--- RepeatedOperatorCheck.h - clang-tidy -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BUGPRONE_REPEATEDOPERATORCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BUGPRONE_REPEATEDOPERATORCHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::bugprone {

/// Check detects repeated operator[] calls with identical arguments.
class RepeatedOperatorCheck : public ClangTidyCheck {
public:
  RepeatedOperatorCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  // private:
  //   std::string getSourceText(const Expr *E) const;
};

} // namespace clang::tidy::bugprone

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BUGPRONE_REPEATEDOPERATORCHECK_H
