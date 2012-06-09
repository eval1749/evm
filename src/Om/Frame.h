// @(#)$Id$
//
// Evita Om - Frame
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Om_Frame_h)
#define INCLUDE_Om_Frame_h

namespace Om {

enum FrameType {
  FrameType_Catch = 'C',
  FrameType_Finally = 'F',
  FrameType_FromNative = 'N',
  FrameType_ToNative = 'T',
};

struct Frame {
  Frame* outer_;
  FrameType type_;
};

static_assert(
    sizeof(Frame) == sizeof(void*) * 2,
    "sizeof(Frame) must be stack alined");

struct CatchFrame : Frame {
  void* exception_;
  void* jump_to_;
};

static_assert(
    sizeof(CatchFrame) == sizeof(Frame) + sizeof(void*) * 2,
    "sizeof(CatchFrame) must be stack alined");

struct FinallyFrame : Frame {
  void* finfun_;
  int num_params_;
};

static_assert(
    sizeof(FinallyFrame) == sizeof(Frame) + sizeof(void*) * 2,
    "sizeof(FinallyFrame) must be stack alined");

} // Om

#endif // !defined(INCLUDE_Om_Frame_h)
