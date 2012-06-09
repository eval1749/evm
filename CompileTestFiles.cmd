@if "%_echo%"=="" echo off
set OutDir=%1
@if "%OutDir%"=="" set OutDir=debug
for %%x in (evsrc\Sample\Test*.ev) do (
  echo %OutDir%\evc -o %OutDir%\%%~nx.evo %%x
  %OutDir%\evc -o %OutDir%\%%~nx.evo %%x
  echo %errorlevel%
  if errorlevel 1 (
    echo Failed
    exit /b 1
  )
)
echo Succeeded
