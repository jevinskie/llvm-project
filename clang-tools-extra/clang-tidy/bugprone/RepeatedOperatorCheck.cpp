//===--- RepeatedOperatorCheck.cpp - clang-tidy ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "RepeatedOperatorCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include <algorithm>

using namespace clang::ast_matchers;
// using namespace clang::as

namespace {
static std::string getSourceText(const MatchFinder::MatchResult &Result,
                                 const clang::Expr *E) {
  return clang::Lexer::getSourceText(
             clang::CharSourceRange::getTokenRange(E->getSourceRange()),
             *Result.SourceManager, clang::LangOptions())
      .str();
}
} // namespace

namespace clang::tidy::bugprone {

void RepeatedOperatorCheck::registerMatchers(MatchFinder *Finder) {
  // Match operator[] calls
  auto OperatorCall = cxxOperatorCallExpr(
      hasOverloadedOperatorName("[]"),
      hasArgument(0, expr().bind("obj")), // The object (e.g., vec)
      hasArgument(1, expr().bind("idx"))  // The index (e.g., i)
  );

  // Look for statements with two such calls
  Finder->addMatcher(stmt(hasDescendant(OperatorCall.bind("obj")),
                          hasDescendant(OperatorCall.bind("idx")))
                         .bind("opCall"),
                     this);
}

void RepeatedOperatorCheck::check(const MatchFinder::MatchResult &Result) {
    static const CXXOperatorCallExpr *PreviousCall = nullptr;
    static SourceLocation PreviousLoc;
    static const CompoundStmt *CurrentCompound = nullptr;
    static const Expr *PrevObj = nullptr;
    static const Expr *PrevIndex = nullptr;
  
    // Get the current operator[] call and its compound statement
    const auto *CurrentCall = Result.Nodes.getNodeAs<CXXOperatorCallExpr>("opCall");
    if (!CurrentCall) return;
  
    const auto *CurrentCompoundStmt = Result.Nodes.getNodeAs<CompoundStmt>("compoundStmt");
    if (!CurrentCompoundStmt) return;
  
    // Reset static variables if we've moved to a new compound statement
    if (CurrentCompound != CurrentCompoundStmt) {
      PreviousCall = nullptr;
      PreviousLoc = SourceLocation();
      CurrentCompound = CurrentCompoundStmt;
    }
  
    // If this is the first call in the compound statement, store it and move on
    if (!PreviousCall) {
      PreviousCall = CurrentCall;
      PreviousLoc = CurrentCall->getBeginLoc();
      return;
    }
  
    // Compare the current call with the previous one
    const auto *CurrObj = Result.Nodes.getNodeAs<Expr>("obj");
    const auto *CurrIndex = Result.Nodes.getNodeAs<Expr>("idx");
  
    std::string PrevObjSrc = getSourceText(Result, PrevObj);
    std::string CurrObjSrc = getSourceText(Result, CurrObj);
    std::string PrevIndexSrc = getSourceText(Result, PrevIndex);
    std::string CurrIndexSrc = getSourceText(Result, CurrIndex);
  
    // Check if the calls match and are in the same file
    if (PrevObjSrc == CurrObjSrc && PrevIndexSrc == CurrIndexSrc &&
        Result.SourceManager->getFileID(PreviousLoc) == Result.SourceManager->getFileID(CurrentCall->getBeginLoc())) {
      diag(CurrentCall->getBeginLoc(), "Repeated use of operator[] with the same object and index");
    }
  
    // Update the previous call for the next iteration
    PreviousCall = CurrentCall;
    PreviousLoc = CurrentCall->getBeginLoc();
}

// std::string RepeatedOperatorCheck::getSourceText(const
// MatchFinder::MatchResult &Result, const clang::Expr *E) const {
//     return clang::Lexer::getSourceText(
//         clang::CharSourceRange::getTokenRange(E->getSourceRange()),
//         Result.SourceManager, clang::LangOptions());
//   }

} // namespace clang::tidy::bugprone

#if 0
namespace {

class RepeatedOperatorCheck : public ClangTidyCheck {
public:
  RepeatedOperatorCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(MatchFinder *Finder) override {
    // Match operator[] calls
    auto OperatorCall = cxxOperatorCallExpr(
        hasOverloadedOperatorName("[]"),
        hasArgument(0, expr().bind("object")),  // The object (e.g., vec)
        hasArgument(1, expr().bind("index"))    // The index (e.g., i)
    );

    // Look for statements with two such calls
    Finder->addMatcher(
        stmt(hasDescendant(OperatorCall.bind("first")),
             hasDescendant(OperatorCall.bind("second")))
            .bind("stmt"),
        this);
  }

  void check(const ast_matchers::MatchFinder::MatchResult &Result) override {
    static const CXXOperatorCallExpr *PreviousCall = nullptr;
    static SourceLocation PreviousLoc;
    static const CompoundStmt *CurrentCompound = nullptr;
  
    // Get the current operator[] call and its compound statement
    const auto *CurrentCall = Result.Nodes.getNodeAs<CXXOperatorCallExpr>("opCall");
    if (!CurrentCall) return;
  
    const auto *CurrentCompoundStmt = Result.Nodes.getNodeAs<CompoundStmt>("compoundStmt");
    if (!CurrentCompoundStmt) return;
  
    // Reset static variables if we've moved to a new compound statement
    if (CurrentCompound != CurrentCompoundStmt) {
      PreviousCall = nullptr;
      PreviousLoc = SourceLocation();
      CurrentCompound = CurrentCompoundStmt;
    }
  
    // If this is the first call in the compound statement, store it and move on
    if (!PreviousCall) {
      PreviousCall = CurrentCall;
      PreviousLoc = CurrentCall->getBeginLoc();
      return;
    }
  
    // Compare the current call with the previous one
    const auto *PrevObj = Result.Nodes.getNodeAs<Expr>("obj");
    const auto *PrevIndex = Result.Nodes.getNodeAs<Expr>("index");
    const auto *CurrObj = Result.Nodes.getNodeAs<Expr>("obj");
    const auto *CurrIndex = Result.Nodes.getNodeAs<Expr>("index");
  
    std::string PrevObjSrc = getSourceText(PrevObj);
    std::string CurrObjSrc = getSourceText(CurrObj);
    std::string PrevIndexSrc = getSourceText(PrevIndex);
    std::string CurrIndexSrc = getSourceText(CurrIndex);
  
    // Check if the calls match and are in the same file
    if (PrevObjSrc == CurrObjSrc && PrevIndexSrc == CurrIndexSrc &&
        Result.SourceManager->getFileID(PreviousLoc) == Result.SourceManager->getFileID(CurrentCall->getBeginLoc())) {
      diag(CurrentCall->getBeginLoc(), "Repeated use of operator[] with the same object and index");
    }
  
    // Update the previous call for the next iteration
    PreviousCall = CurrentCall;
    PreviousLoc = CurrentCall->getBeginLoc();
  }

private:
  std::string getSourceText(const Expr *E) const {
    SourceManager &SM = *getContext()->getSourceManager();
    LangOptions LO;
    return Lexer::getSourceText(CharSourceRange::getTokenRange(E->getSourceRange()), SM, LO).str();
  }
};

} // namespace

// Register the check in a module
class RepeatedOperatorModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<RepeatedOperatorCheck>("custom-repeated-operator");
  }
};

// Register the module with Clang-Tidy
static ClangTidyModuleRegistry::Add<RepeatedOperatorModule>
    X("custom-repeated-operator-module", "Adds check for repeated operator[] calls.");
#endif
