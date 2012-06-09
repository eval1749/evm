@if "%_echo%"=="" echo off
: @(#)$Id$
: Run all Evita VM tests
: Copyright (C) 2010 by Project Vogue.
: Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
set OutDir=%1
@if "%OutDir%"=="" set OutDir=debug
%OutDir%\CommonTest.exe && ^
%OutDir%\IlTest.exe && ^
%OutDir%\CompilerTest.exe && ^
%OutDir%\CgTest.exe && ^
%OutDir%\CgX86Test.exe && ^
echo Done.
