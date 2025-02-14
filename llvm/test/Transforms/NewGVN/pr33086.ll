; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 4
; RUN: opt -passes=newgvn -S %s | FileCheck %s
; REQUIRES: asserts


define void @tinkywinky(i1 %arg) {
; CHECK-LABEL: define void @tinkywinky(i1 %arg) {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    br i1 %arg, label [[FOR_COND18:%.*]], label [[FOR_COND_PREHEADER:%.*]]
; CHECK:       for.cond.preheader:
; CHECK-NEXT:    br label [[FOR_COND2THREAD_PRE_SPLIT:%.*]]
; CHECK:       for.cond2thread-pre-split:
; CHECK-NEXT:    [[CONV24:%.*]] = phi i32 [ 0, [[FOR_COND_PREHEADER]] ], [ [[CONV:%.*]], [[FOR_INC_SPLIT:%.*]] ]
; CHECK-NEXT:    br label [[FOR_INC_SPLIT]]
; CHECK:       for.inc.split:
; CHECK-NEXT:    [[ADD:%.*]] = shl nsw i32 [[CONV24]], 16
; CHECK-NEXT:    [[SEXT23:%.*]] = add i32 [[ADD]], 65536
; CHECK-NEXT:    [[CONV]] = ashr exact i32 [[SEXT23]], 16
; CHECK-NEXT:    [[CMP:%.*]] = icmp slt i32 [[SEXT23]], 3604480
; CHECK-NEXT:    br i1 [[CMP]], label [[FOR_COND2THREAD_PRE_SPLIT]], label [[L1_LOOPEXIT:%.*]]
; CHECK:       l1.loopexit:
; CHECK-NEXT:    br label [[L1:%.*]]
; CHECK:       l1:
; CHECK-NEXT:    [[TMP0:%.*]] = load i16, ptr null, align 2
; CHECK-NEXT:    [[G_0_G_0__PR:%.*]] = load i16, ptr null, align 2
; CHECK-NEXT:    ret void
; CHECK:       for.cond18:
; CHECK-NEXT:    br label [[L1]]
;
entry:
  br i1 %arg, label %for.cond18, label %for.cond.preheader

for.cond.preheader:
  br label %for.cond2thread-pre-split

for.cond2thread-pre-split:
  %conv24 = phi i32 [ 0, %for.cond.preheader ], [ %conv, %for.inc.split ]
  br label %for.inc.split

for.inc.split:
  %add = shl nsw i32 %conv24, 16
  %sext23 = add i32 %add, 65536
  %conv = ashr exact i32 %sext23, 16
  %cmp = icmp slt i32 %sext23, 3604480
  br i1 %cmp, label %for.cond2thread-pre-split, label %l1.loopexit

l1.loopexit:
  br label %l1

l1:
  %h.0 = phi ptr [ undef, %for.cond18 ], [ null, %l1.loopexit ]
  %0 = load i16, ptr %h.0, align 2
  store i16 %0, ptr null, align 2
  %g.0.g.0..pr = load i16, ptr null, align 2
  %tobool15 = icmp eq i16 %g.0.g.0..pr, 0
  ret void

for.cond18:
  br label %l1
}
